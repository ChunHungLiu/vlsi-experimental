#include "TimingUtils.h"
#include "TimingPointUtils.h"
#include "Timing.h"

using namespace Utils;

double Utils::GetNetLoad(HDesign& design, HNet net, KSignalDirection ph)
{
  double load = 0.0;
  for (HNet::SinksEnumeratorW sink = design[net].GetSinksEnumeratorW(); sink.MoveNext(); )
    load += Utils::GetSinkCapacitance(design, sink, ph);

  return load;
}

SinkPhisics Utils::GetSinkCapacitance(HDesign& design, HPinType sink, KSignalDirection ph)
{
  switch (ph)
  {
  case SignalDirection_Rise:
    return design.GetDouble<HPinType::RiseCapacitance>(sink);
  case SignalDirection_Fall:
    return design.GetDouble<HPinType::FallCapacitance>(sink);
  case SignalDirection_None:
    return design.GetDouble<HPinType::Capacitance>(sink);
  case SignalDirection_Average:
    return 0.5 * (design.GetDouble<HPinType::RiseCapacitance>(sink) 
      + design.GetDouble<HPinType::FallCapacitance>(sink));
  default:
    GLOGCRITICAL(LOGINPLACE, "Unknown signal direction: %d", ph);
    return 0.0;
  };
}

SinkPhisics Utils::GetSinkCapacitance(HDesign& design, HPin sink, KSignalDirection ph)
{
  return GetSinkCapacitance(design, design.Get<HPin::Type, HPinType>(sink), ph);
}

DriverPhisics Utils::GetArcPhisics(HDesign& design, HTimingArcType timingArc, KSignalDirection ph)
{
  DriverPhisics result;
  result.C = result.T = result.R = 0.0;

  if (::IsNull(timingArc)) return result;
  HTimingArcTypeWrapper arc = design[timingArc];
  if (arc.TimingType() == TimingType_Combinational
    || arc.TimingType() == TimingType_RisingEdge
    || arc.TimingType() == TimingType_FallingEdge
    || arc.TimingType() == TimingType_HoldFalling
    || arc.TimingType() == TimingType_HoldRising
    || arc.TimingType() == TimingType_SetupFalling
    || arc.TimingType() == TimingType_SetupRising
    )
  {
    switch (ph)
    {
    case SignalDirection_Rise:
    case SignalDirection_None:
      result.R = arc.ResistanceRise();
      result.T = arc.TIntrinsicRise();
    case SignalDirection_Fall:
      result.R = arc.ResistanceFall();
      result.T = arc.TIntrinsicFall();
      break;
    case SignalDirection_Average:
      result.R = std::max(arc.ResistanceFall(), arc.ResistanceRise());
      result.T = std::max(arc.TIntrinsicFall(), arc.TIntrinsicRise());
      break;
    default:
      GLOGCRITICAL(LOGINPLACE, "Unknown signal direction: %d", ph);
      break;
    };
  }

  return result;
}

DriverPhisics Utils::GetDriverWorstPhisics(HDesign& design, HPinType driver, KSignalDirection ph)
{
  DriverPhisics result;
  result.C = result.T = result.R = 0.0;

  if (::IsNull(driver)) return result;

  ASSERT((design.Get<HPinType::Direction, PinDirection>(driver) == PinDirection_OUTPUT));

  result.C = GetSinkCapacitance(design, driver, ph);

  for (HPinType::ArcsEnumeratorW arc = design[driver].GetArcsEnumeratorW(); arc.MoveNext(); )
  {
    DriverPhisics arcph = GetArcPhisics(design, arc, ph);
    result.R = std::max(result.R, arcph.R);
    result.T = std::max(result.T, arcph.T);
  }

  return result;
}

DriverPhisics Utils::GetDriverAveragePhisics(HDesign& design, HPinType driver, KSignalDirection ph)
{
  ASSERT((design.Get<HPinType::Direction, PinDirection>(driver) == PinDirection_OUTPUT));

  DriverPhisics result;
  result.T = result.R = 0.0;

  result.C = GetSinkCapacitance(design, driver, ph);

  int arcsCount = 0;
  for (HPinType::ArcsEnumeratorW arc = design[driver].GetArcsEnumeratorW(); arc.MoveNext(); )
  {
    DriverPhisics arcph = GetArcPhisics(design, arc, ph);
    result.R += arcph.R;
    result.T += arcph.T;

    if (arcph.R != 0.0 || arcph.T != 0.0)
      ++arcsCount;
  }

  if (arcsCount > 1)
  {
    result.R /= arcsCount;
    result.T /= arcsCount;
  }

  return result;
}

DriverPhisics Utils::GetDriverTimingPhisics(HDesign& design, HPin driver, KSignalDirection ph)
{
  ASSERT((design.Get<HPin::Direction, PinDirection>(driver) == PinDirection_OUTPUT));
  HPinType driverType = design.Get<HPin::Type, HPinType>(driver);

  DriverPhisics result;
  result.T = result.R = 0.0;

  result.C = GetSinkCapacitance(design, driverType, ph);

  switch (ph)
  {
  case SignalDirection_Rise:
  case SignalDirection_None:
    {
      bool inverted;
      double arcTime;
      HTimingArcType arc = FindArrivalArc(design, design.TimingPoints[driver], ph, arcTime, inverted);
      result.R = design.GetDouble<HTimingArcType::ResistanceRise>(arc);
      result.T = design.GetDouble<HTimingArcType::TIntrinsicRise>(arc);
    }
    break;
  case SignalDirection_Fall:
    {
      bool inverted;
      double arcTime;
      HTimingArcType arc = FindArrivalArc(design, design.TimingPoints[driver], ph, arcTime, inverted);
      result.R = design.GetDouble<HTimingArcType::ResistanceFall>(arc);
      result.T = design.GetDouble<HTimingArcType::TIntrinsicFall>(arc);
    }
    break;
  case SignalDirection_Average:
    {
      bool inverted;
      double arcTime;
      HTimingArcType arc1 = FindArrivalArc(design, design.TimingPoints[driver], SignalDirection_Fall, arcTime, inverted);
      HTimingArcType arc2 = FindArrivalArc(design, design.TimingPoints[driver], SignalDirection_Rise, arcTime, inverted);
      result.R = std::max(design.GetDouble<HTimingArcType::ResistanceRise>(arc2)
        , design.GetDouble<HTimingArcType::ResistanceFall>(arc1));
      result.T = std::max(design.GetDouble<HTimingArcType::TIntrinsicRise>(arc2)
        , design.GetDouble<HTimingArcType::TIntrinsicFall>(arc1));
    }
    break;
  default:
    GLOGCRITICAL(LOGINPLACE, "Unknown signal direction: %d", ph);
    break;
  };

  return result;
}

DriverPhisics Utils::GetDriverWorstPhisics(HDesign& design, HPin driver, KSignalDirection ph)
{
  return GetDriverWorstPhisics(design, design.Get<HPin::Type, HPinType>(driver), ph);
}

DriverPhisics Utils::GetDriverAveragePhisics(HDesign& design, HPin driver, KSignalDirection ph)
{
  return GetDriverAveragePhisics(design, design.Get<HPin::Type, HPinType>(driver), ph);
}

DriverPhisics Utils::GetElementWorstPhisics(HDesign& design, HCell element, KSignalDirection ph)
{
  DriverPhisics result;
  result.T = result.R = result.C = 0.0;

  for(HCell::PinsEnumeratorW pin = design.Get<HCell::Pins, HCell::PinsEnumeratorW>(element); pin.MoveNext(); )
  {
    if (::IsNull(pin.Net())) continue;

    if (pin.Direction() == PinDirection_INPUT)
    {
      result.C = std::max(result.C, GetSinkCapacitance(design, pin.Type(), ph));
    }
    else if (pin.Direction() == PinDirection_OUTPUT)
    {
      DriverPhisics pinPh = GetDriverWorstPhisics(design, pin.Type(), ph);
      result.R = std::max(result.R, pinPh.R);
      result.T = std::max(result.T, pinPh.T);
    }
    else if (PinFunction_Signal == design.Get<HPinType::Function, PinFunction>(pin.Type()))
    {
      GLOGERROR(LOGINPLACE, "Unsupported pin direction: %d", pin.Direction());
    }
    else
    {
      GLOGWARNING(LOGINPLACE, "Special pin has name \"%s\"", pin.Name().c_str()); //KNOTE: shall probably print net too
    }
  }
  return result;
}

DriverPhisics Utils::GetElementWorstPhisics(HDesign& design, HMacroType elementType, KSignalDirection ph)
{
  DriverPhisics result;
  result.T = result.R = result.C = 0.0;

  for(HMacroType::PinsEnumeratorW pin = design.Get<HMacroType::PinTypesEnumerator, HMacroType::PinsEnumeratorW>(elementType); pin.MoveNext(); )
  {
    if (pin.Direction() == PinDirection_INPUT)
    {
      result.C = std::max(result.C, GetSinkCapacitance(design, pin, ph));
    }
    else if (pin.Direction() == PinDirection_OUTPUT)
    {
      DriverPhisics pinPh = GetDriverWorstPhisics(design, pin, ph);
      result.R = std::max(result.R, pinPh.R);
      result.T = std::max(result.T, pinPh.T);
    }
    else if (PinFunction_Signal == pin.Function())
    {
      GLOGERROR(LOGINPLACE, "Unsupported pin direction: %d", pin.Direction());
    }
  }
  return result;
}

DriverPhisics Utils::GetElementAveragePhisics(HDesign& design, HCell element, KSignalDirection ph)
{
  DriverPhisics result;
  result.T = result.R = result.C = 0.0;
  int inputs = 0;
  int outputs = 0;

  for(HCell::PinsEnumeratorW pin = design.Get<HCell::Pins, HCell::PinsEnumeratorW>(element); pin.MoveNext(); )
  {
    if (::IsNull(pin.Net())) continue;
    if (pin.Direction() == PinDirection_INPUT)
    {
      result.C += GetSinkCapacitance(design, pin.Type(), ph);
      inputs++;
    }
    else if (pin.Direction() == PinDirection_OUTPUT)
    {
      DriverPhisics pinPh = GetDriverWorstPhisics(design, pin.Type(), ph);
      result.R += pinPh.R;
      result.T += pinPh.T;
      outputs++;
    }
    else if (PinFunction_Signal == design.Get<HPinType::Function, PinFunction>(pin.Type()))
    {
      GLOGERROR(LOGINPLACE, "Unsupported pin direction: %d", pin.Direction());
    }
    else
    {
      GLOGWARNING(LOGINPLACE, "Special pin has name \"%s\"", pin.Name().c_str()); //KNOTE: shall probably print net too
    }
  }

  if (inputs > 1)
    result.C /= inputs;
  if (outputs > 1)
  {
    result.R /= outputs;
    result.T /= outputs;
  }
  return result;
}

DriverPhisics Utils::GetElementAveragePhisics(HDesign& design, HMacroType elementType, KSignalDirection ph)
{
  DriverPhisics result;
  result.T = result.R = result.C = 0.0;
  int inputs = 0;
  int outputs = 0;

  for(HMacroType::PinsEnumeratorW pin = design.Get<HMacroType::PinTypesEnumerator, HMacroType::PinsEnumeratorW>(elementType); pin.MoveNext(); )
  {
    if (pin.Direction() == PinDirection_INPUT)
    {
      result.C += GetSinkCapacitance(design, pin, ph);
      inputs++;
    }
    else if (pin.Direction() == PinDirection_OUTPUT)
    {
      DriverPhisics pinPh = GetDriverWorstPhisics(design, pin, ph);
      result.R += pinPh.R;
      result.T += pinPh.T;
      outputs++;
    }
    else if (PinFunction_Signal == pin.Function())
    {
      GLOGERROR(LOGINPLACE, "Unsupported pin direction: %d", pin.Direction());
    }
  }

  if (inputs > 1)
    result.C /= inputs;
  if (outputs > 1)
  {
    result.R /= outputs;
    result.T /= outputs;
  }
  return result;
}

static bool VerifyPin(HDesign& design, HPin pin)
{
  HTimingPointWrapper point = design[design.TimingPoints[pin]];
  if (::IsNull(point.Next()) && ::IsNull(point.Previous()))
    return true;

  if (design.Get<HTimingPoint::PreviousPoint, HTimingPoint>(point.Next()) != point)
  {
    GLOGWARNING(LOGINPLACE, "Point [%s] breaks topological order. Previous point [%s] has [%s] as next.",
      Utils::MakePointFullName(design, point).c_str(),
      Utils::MakePointFullName(design, point.Next()).c_str(),
      Utils::MakePointFullName(design, design.Get<HTimingPoint::PreviousPoint, HTimingPoint>(point.Next())).c_str()
      );
    return false;
  }
  if (design.Get<HTimingPoint::NextPoint, HTimingPoint>(point.Previous()) != point)
  {
    GLOGWARNING(LOGINPLACE, "Point [%s] breaks topological order. Next point [%s] has [%s] as previous.",
      Utils::MakePointFullName(design, point).c_str(),
      Utils::MakePointFullName(design, point.Previous()).c_str(),
      Utils::MakePointFullName(design, design.Get<HTimingPoint::NextPoint, HTimingPoint>(point.Previous())).c_str()
      );
    return false;
  }
  return true;
}

bool Utils::VerifyTimingCalculationOrder(HDesign& design)
{
  bool verified = true;
  int pointscount = 0;

  //1. Verify topologically ordered list integrity
  for (HCells::CellsEnumeratorW cell = design.Cells.GetEnumeratorW(); cell.MoveNext(); )
  {
    if (cell.PlacementStatus() == PlacementStatus_Fictive) continue;

    for (HCell::PinsEnumerator pin = cell.GetPinsEnumerator(); pin.MoveNext(); )
    {
      verified = VerifyPin(design, pin);
      pointscount++;
    }
  }

  for (HPins::PrimariesEnumerator pad = design.Pins.GetEnumerator(); pad.MoveNext(); )
  {
    verified = VerifyPin(design, pad);
    pointscount++;
  }

  if (!verified) return verified;

  //2. Verify/detect cycles
  int idx = 0;
  for (HTimingPointWrapper point = design[design.TimingPoints.TopologicalOrderRoot()]; !::IsNull(point.GoNext()); )
  {
    idx++;
    if (idx > pointscount)
    {
      verified = false;
      LOGWARNING("Detected cycle in topological order.");
      break;
    }
  }

  idx = 0;
  for (HTimingPointWrapper point = design[design.TimingPoints.TopologicalOrderRoot()]; !::IsNull(point.GoPrevious()); )
  {
    idx++;
    if (idx > pointscount)
    {
      verified = false;
      LOGWARNING("Detected cycle in topological backward order.");
      break;
    }
  }

  if (!verified) return verified;

  //3. Verify timing start and end points
  HTimingPoint root = design.TimingPoints.TopologicalOrderRoot();

  //start points
  HTimingPointWrapper sp = design[root];
  int spCount = 0;

  while (sp.GoNext().IsTimingStartPoint())
    spCount++;

  if (sp != design.TimingPoints.FirstInternalPoint())
  {
    GLOGWARNING(LOGINPLACE, "Timing start points are calculated incorrectly. First internal is [%s] instead of [%s].",
      Utils::MakePointFullName(design, sp).c_str(),
      Utils::MakePointFullName(design, design.TimingPoints.FirstInternalPoint()).c_str()
      );
    verified = false;
  }
  if (spCount != design.TimingPoints.StartPointsCount())
  {
    GLOGWARNING(LOGINPLACE, "Timing start points are calculated incorrectly. There are %d points instead of %d.",
      spCount,
      design.TimingPoints.StartPointsCount()
      );
    verified = false;
  }

  //end points
  HTimingPointWrapper ep = design[root];
  int epCount = 0;

  while (ep.GoPrevious().IsTimingEndPoint())
    epCount++;

  if (ep != design.TimingPoints.LastInternalPoint())
  {
    GLOGWARNING(LOGINPLACE, "Timing end points are calculated incorrectly. Last internal is [%s] instead of [%s].",
      Utils::MakePointFullName(design, ep).c_str(),
      Utils::MakePointFullName(design, design.TimingPoints.LastInternalPoint()).c_str()
      );
    verified = false;
  }
  if (epCount != design.TimingPoints.EndPointsCount())
  {
    GLOGWARNING(LOGINPLACE, "Timing end points are calculated incorrectly. There are %d points instead of %d.",
      epCount,
      design.TimingPoints.EndPointsCount()
      );
    verified = false;
  }

  //internal points
  while (sp.GoNext() != ep)
    if (sp.IsTimingStartPoint() || sp.IsTimingEndPoint())
    {
      GLOGWARNING(LOGINPLACE, "Timing start/end point is placed inside internal points. Point name is [%s].",
        Utils::MakePointFullName(design, sp).c_str()
      );
      verified = false;
    }

  return verified;
}
