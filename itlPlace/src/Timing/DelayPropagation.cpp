#include <float.h>
#include <vector>

#include "Timing.h"
#include "DelayPropagation.h"
#include "TimingPointsEnumerator.h"

enum PropagationDirection
{
  PropagationDirection_Arrival = 0,
  PropagationDirection_Required = 1
};

template<int sdNum>
void CalcArrivalTimeOnOutputPin(HDesign& design, HPin pin, DelayPropagation<sdNum>& dp)
{//search for worst signal from cell inputs
  HPinType ptype = design.Get<HPin::Type, HPinType>(pin);
  if (design.GetInt<HPinType::TimingArcsCount>(ptype) == 0) return;

  HSteinerPoint this_point = design.SteinerPoints[pin];
  typename DelayPropagation<sdNum>::CapacitanceType observedC = dp.GetObservedC(this_point);

  typename DelayPropagation<sdNum>::TimeType worstTime = -DBL_MAX;
  typename DelayPropagation<sdNum>::BoolType worstInversed = false;
  typename DelayPropagation<sdNum>::TimingPoint worstRelatedPoint = design.TimingPoints.Null();

  for (HPinType::ArcsEnumeratorW arc = design.Get<HPinType::ArcTypesEnumerator, HPinType::ArcsEnumeratorW>(ptype);
    arc.MoveNext(); )
  {
    if (arc.TimingType() != TimingType_Combinational) continue;//skip constraint arcs

    HTimingPoint startPoint = design.TimingPoints[arc.GetStartPin(pin)];

    //arc delays
    typename DelayPropagation<sdNum>::BoolType inversed;
    typename DelayPropagation<sdNum>::TimeType time = dp.GetArcOutputTimeArrival(arc, observedC, dp.GetArrivalTime(startPoint), inversed);
    //remember results
    dp.AccumulateWorstArrivalInFirst(worstTime, worstInversed, worstRelatedPoint, time, inversed, startPoint);
  }//for (HPinType::ArcsEnumeratorW arc

  //save results
  HTimingPoint pt = design.TimingPoints[pin];
  dp.SetArrivalTime(pt, worstTime);
  dp.SetArrivalAncestor(pt, worstRelatedPoint);
  dp.SetArrivalInversion(pt, worstInversed);
}

template<int sdNum>
void PropagateArrivals(HDesign& design)
{
  DelayPropagation<sdNum> dp(design);

  for(TopologicalOrder::IntTepEnumerator pt = design; pt.MoveNext(); )
  {
    HPin pin = pt.Pin();
    if (design.Get<HPin::Direction, PinDirection>(pin) == PinDirection_INPUT)
    {//single arc from net source
      HNet net = design.Get<HPin::Net, HNet>(pin);
      HPin sourcePin = design.Get<HNet::Source, HPin>(net);
      HTimingPoint source = design.TimingPoints[sourcePin];
      dp.SetArrivalTime(pt, dp.GetSinkArrivalTime(source, design.SteinerPoints[pin]));
      dp.SetArrivalAncestor(pt, source);
    }
    else
    {//here can be several cell's arcs
      ASSERT((design.Get<HPin::Direction, PinDirection>(pin) == PinDirection_OUTPUT));
      CalcArrivalTimeOnOutputPin(design, pin, dp);
    }
  }//for (TopologicalOrder::IntTepEnumerator pt
}

template<int sdNum>
void CalcRequiredTimeOnInputPins(HDesign& design, HPin outPin, DelayPropagation<sdNum>& dp)
{
  HPinType ptype = design.Get<HPin::Type, HPinType>(outPin);
  if (design.GetInt<HPinType::TimingArcsCount>(ptype) == 0) return;

  HSteinerPoint this_point = design.SteinerPoints[outPin];
  HTimingPoint tp = design.TimingPoints[outPin];

  typename DelayPropagation<sdNum>::CapacitanceType observedC = dp.GetObservedC(this_point);
  typename DelayPropagation<sdNum>::TimeType ptTime = dp.GetRequiredTime(tp);

  //apply each arc that gives worst known solution
  for (HPinType::ArcsEnumeratorW arc = design.Get<HPinType::ArcTypesEnumerator, HPinType::ArcsEnumeratorW>(ptype);
    arc.MoveNext(); )
  {
    if (arc.TimingType() != TimingType_Combinational) continue;

    HTimingPoint startPoint = design.TimingPoints[arc.GetStartPin(outPin)];

    typename DelayPropagation<sdNum>::BoolType inversed;
    typename DelayPropagation<sdNum>::TimeType time = dp.GetArcOutputTimeRequired(arc, observedC, ptTime, inversed);

    dp.AccumulateWorstRequiredOnPin(startPoint, time, inversed, tp);
  }//for (HPinType::ArcsEnumeratorW arc
}

template<int sdNum>
void PropagateRequires(HDesign& design)
{
  DelayPropagation<sdNum> dp(design);
  for (ReverseTopologicalOrder::TepIntEnumerator pt = design; pt.MoveNext(); )
  {
    CRITICAL_ASSERT(!::IsNull(pt));
    HPin pin = pt.Pin();
    if (design.Get<HPin::Direction, PinDirection>(pin) == PinDirection_INPUT)
    {//single arc to net source
      HTimingPoint source = design.TimingPoints[design.Get<HNet::Source, HPin>(design.Get<HPin::Net, HNet>(pin))];
      dp.AccumulateSourceRequiredTime(source, pt, dp.GetSourceRequiredTime(pt));
    }
    else
    {
      ASSERT((design.Get<HPin::Direction, PinDirection>(pin) == PinDirection_OUTPUT));
      CalcRequiredTimeOnInputPins(design, pin, dp);
    }
  }//for (ReverseTopologicalOrder::TepIntEnumerator pt
}

template<int sdNum>
void SetStartPointsArrivals(HDesign& design, double clock_cycle)
{
  DelayPropagation<sdNum> dp(design);

  for(TopologicalOrder::TspEnumerator sp = design; sp.MoveNext(); )
  {
    if (design.GetBool<HPin::IsPrimary>(sp.Pin()))
      dp.SetArrivalTime(sp, 0.0);
    else 
    {//flip-flops and latches
      switch (design.Get<HMacroType::Type, MacroType>(
        design.Get<HCell::MacroType, HMacroType>(
        design.Get<HPin::Cell, HCell>(sp.Pin()))))
      {
      case MacroType_REFF:
      case MacroType_HLSL:
        dp.SetArrivalTime(sp, 0.0);
        break;
      case MacroType_FEFF:
        dp.SetArrivalTime(sp, -clock_cycle * 0.5);
        break;
      case MacroType_LLSL:
        dp.SetArrivalTime(sp, clock_cycle * 0.5);
        break;
      default:
        dp.SetArrivalTime(sp, 0.0);
        break;
      }

      //apply edge arcs
      HSteinerPoint this_point = design.SteinerPoints[sp.Pin()];
      for (HPinType::ArcsEnumeratorW arc
        = design.Get<HPinType::ArcTypesEnumerator, HPinType::ArcsEnumeratorW>(
        design.Get<HPin::Type, HPinType>(sp.Pin()));
      arc.MoveNext(); )
      {
        if (arc.TimingType() == TimingType_RisingEdge || arc.TimingType() == TimingType_FallingEdge)
        {
          dp.SetArrivalTime(sp, dp.GetEdgeArcOutputTime(arc, sp, this_point));
          HPin startPin = design.TimingArcTypes.GetStartPin(arc, sp.Pin());
          dp.SetArrivalAncestor(sp, design.TimingPoints[startPin]);
        }
      }
    }
  }//for (TopologicalOrder::TspEnumerator sp

  for (TopologicalOrder::IntTepEnumerator p = design; p.MoveNext(); )
    dp.SetArrivalTime(p, -DBL_MAX);
}

template<int sdNum>
void SetEndPointsRequires(HDesign& design, double clock_cycle)
{
  DelayPropagation<sdNum> dp(design);

  for(ReverseTopologicalOrder::TepEnumerator ep = design; ep.MoveNext(); )
  {
    if (design.GetBool<HPin::IsPrimary>(ep.Pin()))
      dp.SetRequiredTime(ep, clock_cycle);
    else 
    {
      switch (design.Get<HMacroType::Type, MacroType>(
        design.Get<HCell::MacroType, HMacroType>(
        design.Get<HPin::Cell, HCell>(ep.Pin()))))
      {
      case MacroType_REFF:
      case MacroType_LLSL:
        dp.SetRequiredTime(ep, clock_cycle);
        break;
      case MacroType_HLSL:
      case MacroType_FEFF:
        dp.SetRequiredTime(ep, clock_cycle * 0.5);
        break;
      default:
        dp.SetRequiredTime(ep, clock_cycle);
        break;
      }

      //apply setup constraints
      for (HPinType::ArcsEnumeratorW arc
        = design.Get<HPinType::ArcTypesEnumerator, HPinType::ArcsEnumeratorW>(
        design.Get<HPin::Type, HPinType>(ep.Pin()));
      arc.MoveNext(); )
      {
        if (arc.TimingType() == TimingType_SetupRising || arc.TimingType() == TimingType_SetupFalling)
          dp.SetRequiredTime(ep, dp.GetSetupArcOutputTime(arc, ep));
      }
    }
  }//for (ReverseTopologicalOrder::TepEnumerator ep

  //prefill other points
  for (ReverseTopologicalOrder::IntTspEnumerator p = design; p.MoveNext(); )
    dp.SetRequiredTime(p, DBL_MAX);
}

template<int SignalDirectionsNum>
void PropagateDelaysInternal(HDesign& design, double clock_cycle)
{
  SetStartPointsArrivals<SignalDirectionsNum>(design, clock_cycle);
  SetEndPointsRequires<SignalDirectionsNum>(design, clock_cycle);
  PropagateArrivals<SignalDirectionsNum>(design);
  PropagateRequires<SignalDirectionsNum>(design);
}

void PropagateDelays(HDesign& design, double clock_cycle)
{
  design.TimingPoints.ClearTimingResults();

  if (design.cfg.ValueOf("Timing.SignalDirectionsUsed", 2) == 1)
    PropagateDelaysInternal<1>(design, clock_cycle);
  else
    PropagateDelaysInternal<2>(design, clock_cycle);
}

void PropagateDelays(HDesign& design)
{
  PropagateDelays(design, design.cfg.ValueOf("benchmark.target_clock_cycle", 0.0));
}

template<int sdNum, class ArcSelector>
HTimingArcType FindArc(HDesign& design,
                       HTimingPoint basePoint,
                       const ArcSelector& arcSelector,
                       typename DelayPropagation<sdNum>::TimeType& arcTime,
                       typename DelayPropagation<sdNum>::BoolType& inversed)
{
  HTimingPoint startPoint = arcSelector.GetStartPoint(design, basePoint);
  HTimingPoint endPoint = arcSelector.GetEndPoint(design, basePoint);

  HPin endPin = design.Get<HTimingPoint::Pin, HPin>(endPoint);
  HPinType endPinType = design.Get<HPin::Type, HPinType>(endPin);

  if (design.GetInt<HPinType::TimingArcsCount>(endPinType) > 0)
  {
    DelayPropagation<sdNum> dp(design);
    HPin startPin = design.Get<HTimingPoint::Pin, HPin>(startPoint);
    HSteinerPoint endStPoint = design.SteinerPoints[endPin];

    typename DelayPropagation<sdNum>::CapacitanceType observedC = dp.GetObservedC(endStPoint);
    bool found = false;
    arcTime = arcSelector.GetBestPossibleTime();
    HTimingArcType wArc = design.TimingArcTypes.Null();

    for (HPinType::ArcsEnumeratorW arc = design.Get<HPinType::ArcTypesEnumerator, HPinType::ArcsEnumeratorW>(endPinType);
      arc.MoveNext(); )
    {
      if ((arc.TimingType() == TimingType_Combinational 
        || arc.TimingType() == TimingType_RisingEdge
        || arc.TimingType() == TimingType_FallingEdge)
        && arc.GetStartPin(endPin) == startPin)
      {
        found = true;
        typename DelayPropagation<sdNum>::BoolType timeInversed;
        typename DelayPropagation<sdNum>::TimeType time = arcSelector.GetArcOutputTime(dp, arc, observedC, startPoint, endPoint, timeInversed);

        if (arcSelector.FirstTimeIsWorse(time, arcTime))
        {
          arcTime = time;
          inversed = timeInversed;
          wArc = arc;
        }
      }
    }

    if (found)
    {
      arcSelector.AdjustArcTime(design, basePoint == startPoint ? endPoint : startPoint, inversed, arcTime);
      return wArc;
    }
  }//if (hd.GetInt<HPinType::TimingArcsCount>(ptype) > 0)

  arcTime = 0.0;
  inversed = false;
  return design.TimingArcTypes.Null();
}

template<int sdNum>
struct ArcSelectorBase
{
  typedef DelayPropagation<sdNum> DP1;
  typedef typename DP1::CapacitanceType CapacitanceType1;
  typedef typename DP1::TimeType TimeType1;
  typedef typename DP1::BoolType BoolType1;
};

template<int sdNum>
struct ArrivalArcSelectorBase: public ArcSelectorBase<sdNum>
{
  typedef typename ArcSelectorBase<sdNum>::TimeType1 TimeType;
  typedef typename ArcSelectorBase<sdNum>::DP1 DP;
  typedef typename ArcSelectorBase<sdNum>::CapacitanceType1 CapacitanceType;
  typedef typename ArcSelectorBase<sdNum>::BoolType1 BoolType;

  HTimingPoint GetEndPoint(HDesign& design, HTimingPoint basePoint) const { return basePoint; }
  double GetBestPossibleTime() const {return -DBL_MAX;}
  TimeType GetArcOutputTime(DP& dp, const HTimingArcTypeWrapper& arc,
    CapacitanceType observedC, HTimingPoint startPoint, HTimingPoint endPoint, BoolType& timeInversed) const
  {
    return dp.GetArcOutputTimeArrival(arc, observedC, dp.GetArrivalTime(startPoint), timeInversed);
  }
};

struct ArrivalArcSelector1D: public ArrivalArcSelectorBase<1>
{
  HTimingPoint GetStartPoint(HDesign& design, HTimingPoint basePoint) const { return design.Get<HTimingPoint::ArrivalAncestor, HTimingPoint>(basePoint); }
  bool FirstTimeIsWorse(TimeType time1, TimeType time2) const { return time1 > time2; }
  void AdjustArcTime(HDesign& design, HTimingPoint contrPoint, BoolType& isInversed, TimeType& arcTime) const
  {
    arcTime -= design.GetDouble<HTimingPoint::ArrivalTime>(contrPoint);
  }
};

struct ArrivalArcSelector2DRise: public ArrivalArcSelectorBase<2>
{
  HTimingPoint GetStartPoint(HDesign& design, HTimingPoint basePoint) const { return design.Get<HTimingPoint::RiseArrivalAncestor, HTimingPoint>(basePoint); }
  bool FirstTimeIsWorse(TimeType time1, TimeType time2) const { return time1.rise > time2.rise; }
  void AdjustArcTime(HDesign& design, HTimingPoint contrPoint, BoolType& isInversed, TimeType& arcTime) const
  {
    arcTime.rise -= isInversed.rise ? design.GetDouble<HTimingPoint::FallArrivalTime>(contrPoint) : design.GetDouble<HTimingPoint::RiseArrivalTime>(contrPoint);
  }
};

struct ArrivalArcSelector2DFall: public ArrivalArcSelectorBase<2>
{
  HTimingPoint GetStartPoint(HDesign& design, HTimingPoint basePoint) const { return design.Get<HTimingPoint::FallArrivalAncestor, HTimingPoint>(basePoint); }
  bool FirstTimeIsWorse(TimeType time1, TimeType time2) const { return time1.fall > time2.fall; }
  void AdjustArcTime(HDesign& design, HTimingPoint contrPoint, BoolType& isInversed, TimeType& arcTime) const
  {
    arcTime.fall -= isInversed.fall ? design.GetDouble<HTimingPoint::RiseArrivalTime>(contrPoint) : design.GetDouble<HTimingPoint::FallArrivalTime>(contrPoint);
  }
};

template<int sdNum>
struct RequiredArcSelectorBase: public ArcSelectorBase<sdNum>
{
  typedef typename ArcSelectorBase<sdNum>::TimeType1 TimeType;
  typedef typename ArcSelectorBase<sdNum>::DP1 DP;
  typedef typename ArcSelectorBase<sdNum>::CapacitanceType1 CapacitanceType;
  typedef typename ArcSelectorBase<sdNum>::BoolType1 BoolType;

  HTimingPoint GetStartPoint(HDesign& design, HTimingPoint basePoint) const { return basePoint; }
  double GetBestPossibleTime() const {return DBL_MAX;}
  TimeType GetArcOutputTime(DP& dp, const HTimingArcTypeWrapper& arc,
    CapacitanceType observedC, HTimingPoint startPoint, HTimingPoint endPoint, BoolType& timeInversed) const
  {
    return dp.GetArcOutputTimeRequired(arc, observedC, dp.GetRequiredTime(endPoint), timeInversed);
  }
};

struct RequiredArcSelector1D: public RequiredArcSelectorBase<1>
{
  HTimingPoint GetEndPoint(HDesign& design, HTimingPoint basePoint) const { return design.Get<HTimingPoint::RequiredAncestor, HTimingPoint>(basePoint); }
  bool FirstTimeIsWorse(TimeType time1, TimeType time2) const { return time1 < time2; }
  void AdjustArcTime(HDesign& design, HTimingPoint contrPoint, BoolType& isInversed, TimeType& arcTime) const
  {
    arcTime -= design.GetDouble<HTimingPoint::RequiredTime>(contrPoint);
    arcTime = -arcTime;
  }
};

struct RequiredArcSelector2DRise: public RequiredArcSelectorBase<2>
{
  HTimingPoint GetEndPoint(HDesign& design, HTimingPoint basePoint) const { return design.Get<HTimingPoint::RiseRequiredAncestor, HTimingPoint>(basePoint); }
  bool FirstTimeIsWorse(TimeType time1, TimeType time2) const { return time1.rise < time2.rise; }
  void AdjustArcTime(HDesign& design, HTimingPoint contrPoint, BoolType& isInversed, TimeType& arcTime) const
  {
    arcTime.rise -= isInversed.rise ? design.GetDouble<HTimingPoint::FallRequiredTime>(contrPoint) : design.GetDouble<HTimingPoint::RiseRequiredTime>(contrPoint);
    arcTime.rise = -arcTime.rise;
  }
};

struct RequiredArcSelector2DFall: public RequiredArcSelectorBase<2>
{
  HTimingPoint GetEndPoint(HDesign& design, HTimingPoint basePoint) const { return design.Get<HTimingPoint::FallRequiredAncestor, HTimingPoint>(basePoint); }
  bool FirstTimeIsWorse(TimeType time1, TimeType time2) const { return time1.fall < time2.fall; }
  void AdjustArcTime(HDesign& design, HTimingPoint contrPoint, BoolType& isInversed, TimeType& arcTime) const
  {
    arcTime.fall -= isInversed.fall ? design.GetDouble<HTimingPoint::RiseRequiredTime>(contrPoint) : design.GetDouble<HTimingPoint::FallRequiredTime>(contrPoint);
    arcTime.fall = -arcTime.fall;
  }
};

void GetArrivalRisingArc(HDesign& hd,
                         HTimingPoint startPoint,
                         HTimingPoint endPoint,
                         HTimingArcType* risingArc,
                         double* arcRiseTime,
                         bool* isInversed)
{
  DelayPropagation<2>::TimeType time;
  DelayPropagation<2>::BoolType inv;
  *risingArc = FindArc<2>(hd, endPoint, ArrivalArcSelector2DRise(), time, inv);
  *arcRiseTime = time.rise;
  *isInversed = inv.rise;
}

void GetArrivalFallingArc(HDesign& hd,
                          HTimingPoint startPoint,
                          HTimingPoint endPoint,
                          HTimingArcType* fallingArc,
                          double* arcFallTime,
                          bool* isInversed)
{
  DelayPropagation<2>::TimeType time;
  DelayPropagation<2>::BoolType inv;
  *fallingArc = FindArc<2>(hd, endPoint, ArrivalArcSelector2DFall(), time, inv);
  *arcFallTime = time.fall;
  *isInversed = inv.fall;
}

void GetArrivalArc(HDesign& hd,
                   HTimingPoint startPoint,
                   HTimingPoint endPoint,
                   HTimingArcType* arc,
                   double* arcTime,
                   bool* isInversed)
{
  *arc = FindArc<1>(hd, endPoint, ArrivalArcSelector1D(), *arcTime, *isInversed);
}

void GetRequiredFallingArc(HDesign& hd,
                           HTimingPoint startPoint,
                           HTimingPoint endPoint,
                           HTimingArcType* fallingArc,
                           double* arcFallTime,
                           bool* isInversed)
{
  DelayPropagation<2>::TimeType time;
  DelayPropagation<2>::BoolType inv;
  *fallingArc = FindArc<2>(hd, endPoint, RequiredArcSelector2DFall(), time, inv);
  *arcFallTime = time.fall;
  *isInversed = inv.fall;
}

void GetRequiredRisingArc(HDesign& hd,
                          HTimingPoint startPoint,
                          HTimingPoint endPoint,
                          HTimingArcType* risingArc,
                          double* arcRiseTime,
                          bool* isInversed)
{
  DelayPropagation<2>::TimeType time;
  DelayPropagation<2>::BoolType inv;
  *risingArc = FindArc<2>(hd, endPoint, RequiredArcSelector2DRise(), time, inv);
  *arcRiseTime = time.rise;
  *isInversed = inv.rise;
}

void GetRequiredArc(HDesign& hd,
                    HTimingPoint startPoint,
                    HTimingPoint endPoint,
                    HTimingArcType* arc,
                    double* arcTime,
                    bool* isInversed)
{
  *arc = FindArc<1>(hd, endPoint, RequiredArcSelector1D(), *arcTime, *isInversed);
}

HTimingArcType FindArrivalArc(HDesign& hd,
                              HTimingPoint arcEndPoint,
                              KSignalDirection dir,
                              double& arcTime,
                              bool& isInversed)
{
  switch (dir)
  {
  case SignalDirection_Fall:
    {
      DelayPropagation<2>::TimeType time;
      DelayPropagation<2>::BoolType inv;
      HTimingArcType ret = FindArc<2>(hd, arcEndPoint, ArrivalArcSelector2DFall(), time, inv);
      arcTime = time.fall;
      isInversed = inv.fall;
      return ret;
    }
  case SignalDirection_Rise:
    {
      DelayPropagation<2>::TimeType time;
      DelayPropagation<2>::BoolType inv;
      HTimingArcType ret = FindArc<2>(hd, arcEndPoint, ArrivalArcSelector2DRise(), time, inv);
      arcTime = time.rise;
      isInversed = inv.rise;
      return ret;
    }
  case SignalDirection_None:
    return FindArc<1>(hd, arcEndPoint, ArrivalArcSelector1D(), arcTime, isInversed);
  default:
    GLOGCRITICAL(LOGINPLACE, "Unsupported signal direction: %d", dir);
    return hd.TimingArcTypes.Null();
  };
}

HTimingArcType FindRequiredArc(HDesign& hd,
                               HTimingPoint arcStartPoint,
                               KSignalDirection dir,
                               double& arcTime,
                               bool& isInversed)
{
  switch (dir)
  {
  case SignalDirection_Fall:
    {
      DelayPropagation<2>::TimeType time;
      DelayPropagation<2>::BoolType inv;
      HTimingArcType ret = FindArc<2>(hd, arcStartPoint, RequiredArcSelector2DFall(), time, inv);
      arcTime = time.fall;
      isInversed = inv.fall;
      return ret;
    }
  case SignalDirection_Rise:
    {
      DelayPropagation<2>::TimeType time;
      DelayPropagation<2>::BoolType inv;
      HTimingArcType ret = FindArc<2>(hd, arcStartPoint, RequiredArcSelector2DRise(), time, inv);
      arcTime = time.rise;
      isInversed = inv.rise;
      return ret;
    }
  case SignalDirection_None:
    return FindArc<1>(hd, arcStartPoint, RequiredArcSelector1D(), arcTime, isInversed);
  default:
    GLOGCRITICAL(LOGINPLACE, "Unsupported signal direction: %d", dir);
    return hd.TimingArcTypes.Null();
  };
}
