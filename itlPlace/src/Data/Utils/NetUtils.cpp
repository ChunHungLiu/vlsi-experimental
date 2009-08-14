#include "NetUtils.h"
#include "math.h"
#include <vector>
#include <algorithm>
#include "Reporting.h"

namespace Utils
{
  double CalcNetHPWL(HDesign& hd, HNet net)
  {
    LNetList& netlist = hd._Design.NetList;
    IDType pinsStart = netlist.netPinStart[::ToID(net)];
    IDType pinsEnd = netlist.netPinEnd[::ToID(net)];

    if(pinsEnd == pinsStart + 2)
      return fabs(hd.GetDouble<HPin::Y>(::__ConstructPtr<HPin>(netlist.netPins[pinsStart]))
        - hd.GetDouble<HPin::Y>(::__ConstructPtr<HPin>(netlist.netPins[pinsStart + 1])))
        + fabs(hd.GetDouble<HPin::X>(::__ConstructPtr<HPin>(netlist.netPins[pinsStart]))
        - hd.GetDouble<HPin::X>(::__ConstructPtr<HPin>(netlist.netPins[pinsStart + 1])));

    double top = hd.GetDouble<HPin::Y>(::__ConstructPtr<HPin>(netlist.netPins[pinsStart]));
    double bottom = top;
    double left = hd.GetDouble<HPin::X>(::__ConstructPtr<HPin>(netlist.netPins[pinsStart]));
    double right = left;

    for (IDType idx = pinsStart + 1; idx < pinsEnd; idx++)
    {
      double x = hd.GetDouble<HPin::X>(::__ConstructPtr<HPin>(netlist.netPins[idx]));
      if (left > x)
        left = x;
      else if (right < x)
        right = x;

      double y = hd.GetDouble<HPin::Y>(::__ConstructPtr<HPin>(netlist.netPins[idx]));
      if (top < y)
        top = y;
      else
        if (bottom > y)
          bottom = y;
    }
    
    return right - left + top - bottom;
  }

  double CalculateHPWL(HDesign& hd, bool updateCachedValues)
  {
    double hpwl = 0.0;
    int nNetsEnd = hd._Design.NetList.nNetsEnd;

    if (updateCachedValues)
    {
      for (HNets::NetsEnumerator nIter = hd.Nets.GetFullEnumerator(); nIter.MoveNext(); )
        if (hd.Get<HNet::Kind, NetKind>(nIter) == NetKind_Active)
          hd.Set<HNet::HPWL>(nIter, CalcNetHPWL(hd, nIter));
        else
          hd.Set<HNet::HPWL>(nIter, 0.0);
      for (int i = 1; i < nNetsEnd; i++)
        hpwl += hd._Design.NetList.netHPWL[i];
    }
    else
    {
      for (HNets::NetsEnumerator nIter = hd.Nets.GetFullEnumerator(); nIter.MoveNext(); )
        if (hd.Get<HNet::Kind, NetKind>(nIter) == NetKind_Active)
          hpwl += CalcNetHPWL(hd, nIter);
    }
    
    return hpwl;
  }

  double CalculateWeightedHPWL(HDesign& hd, bool updateCachedValues)
  {
    double hpwl = 0.0;
    int nNetsEnd = hd._Design.NetList.nNetsEnd;

    if (updateCachedValues)
    {
      for (HNets::NetsEnumerator nIter = hd.Nets.GetFullEnumerator(); nIter.MoveNext(); )
        if (hd.Get<HNet::Kind, NetKind>(nIter) == NetKind_Active)
          hd.Set<HNet::HPWL>(nIter, CalcNetHPWL(hd, nIter));
        else
          hd.Set<HNet::HPWL>(nIter, 0.0);
      for (int i = 1; i < nNetsEnd; i++)
        hpwl += hd._Design.NetList.netWeight[i] * hd._Design.NetList.netHPWL[i];
    }
    else
    {
      HNets::NetsEnumeratorW netW = hd.Nets.GetFullEnumeratorW();
      for (HNets::NetsEnumerator nIter = hd.Nets.GetFullEnumerator(); nIter.MoveNext(), netW.MoveNext(); )
      {
        if (hd.Get<HNet::Kind, NetKind>(nIter) == NetKind_Active)
          hpwl += netW.Weight() * CalcNetHPWL(hd, nIter);
      }
    }

    return hpwl;
  }

  double CalculateHPWLDiff(HDesign& hd, const HNet* nets, int nNets, bool updateCachedValues)
  {
    double dhpwl = 0.0;
    for (int i = 0; i < nNets; i++)
    {
      double netWL = CalcNetHPWL(hd, nets[i]);
      dhpwl += netWL - hd.GetDouble<HNet::HPWL>(nets[i]);
      if (updateCachedValues)
        hd.Set<HNet::HPWL>(nets[i], netWL);
    }
    return dhpwl;
  }

  double CalculateHPWLDiff(HDesign& hd, const HCell* cells, int nCells, bool updateCachedValues)
  {
    double dhpwl = 0.0;
    std::vector<HNet> nets;
    nets.reserve(nCells * 2);

    for (int i = 0; i < nCells; i++)
    {
      for (HCell::PinsEnumerator pe = hd.Get<HCell::Pins, HCell::PinsEnumerator>(cells[i]); pe.MoveNext(); )
      {
        HNet current_net = hd.Get<HPin::Net, HNet>(pe);
        if (!::IsNull(current_net))
          if (find(nets.begin(), nets.end(), current_net) == nets.end())
          {
            nets.push_back(current_net);
            double netWL = CalcNetHPWL(hd, current_net);
            dhpwl += netWL - hd.GetDouble<HNet::HPWL>(current_net);
            if (updateCachedValues)
              hd.Set<HNet::HPWL>(current_net, netWL);
          }
      }
    }

    return dhpwl;
  }

  static void UnbindNet(HDesign& hd, HNet net, NetKind unbind_kind)
  {
    if (hd.Get<HNet::Kind, NetKind>(net) == NetKind_Active)
    {
      hd.Set<HNet::Kind>(net, unbind_kind);
      for (HNet::PinsEnumeratorW pin = hd.Get<HNet::Pins, HNet::PinsEnumeratorW>(net); pin.MoveNext(); )
      {
        if (pin.Net() == net)
          pin.SetNet(hd.Nets.Null());
      }
    }
  }

  void SkipNet(HDesign& hd, HNet net)
  {
    UnbindNet(hd, net, NetKind_Skipped);
  }

  void RemoveNet(HDesign& hd, HNet net)
  {
    UnbindNet(hd, net, NetKind_Removed);
  }

  static bool IsClockNet(HNetWrapper& net, HDesign& hd)
  {
    for (HNet::PinsEnumeratorW pin = net.GetPinsEnumeratorW(); pin.MoveNext(); )
    {
      //MacroType mtype = hd.Get<HMacroType::Type, MacroType>(hd.Get<HCell::MacroType, HMacroType>(pin.Cell()));
      //if (mtype >= MacroType_FirstSpecial && mtype <= MacroType_LastSpecial)
      //  return true;
      PinFunction func = hd.Get<HPinType::Function, PinFunction>(pin.Type());
      if (func == PinFunction_Clock)
        return true;
    }
    return false;
  }

  static bool IsPureClearPresetNet(HNetWrapper& net, HDesign& hd)
  {
    for (HNet::SinksEnumeratorW pin = net.GetSinksEnumeratorW(); pin.MoveNext(); )
    {
      PinFunction func = hd.Get<HPinType::Function, PinFunction>(pin.Type());
      if (func != PinFunction_Clear && func != PinFunction_Preset)
        return false;
    }
    return true;
  }

  void SkipSpecialNets(HDesign& hd)
  {
    int nSkipped = 0;
    ALERT("NETS SKIPPING STARTED...");
    for (HNets::ActiveNetsEnumeratorW net = hd.Nets.GetNetsEnumeratorW(); net.MoveNext(); )
    {
      if (IsClockNet(net, hd) || IsPureClearPresetNet(net, hd))
      {
        if (hd.cfg.ValueOf("Timing.reportSkippedNets", false))
          ReportNetPins(hd, net);
        SkipNet(hd, net);
        nSkipped++;
      }
    }
    ALERTFORMAT(("\tskipped %d nets", nSkipped));
    ALERT("NETS SKIPPING FINISHED...");
  }

  void CalculateLNets(HDesign& hd)
  {
    for (HNets::NetsEnumeratorW net = hd.Nets.GetFullEnumeratorW(); net.MoveNext(); )
    {
      double lnet = 0.0;

      for (HNet::SinksEnumeratorW sink = net.GetSinksEnumeratorW(); sink.MoveNext(); )
      {
        //ALERTFORMAT(("Sink: %s\tCap: %f", hd.GetString<HPin::Name>(sink).c_str(), 
        //  hd.GetDouble<HPinType::Capacitance>(sink.Type())));
        lnet += hd.GetDouble<HPinType::Capacitance>(sink.Type());
      }

      lnet /= hd.RoutingLayers.Physics.CPerDist;

      double R = 0;
      int nArcs = 0;

      //HPin pin = net.Source();
      //HPinType pt = hd.Get<HPin::Type, HPinType>(net.Source());
      //bool isEntered = false;
      for (HPinType::ArcsEnumeratorW arc = hd.Get<HPinType::ArcTypesEnumerator, HPinType::ArcsEnumeratorW>(hd.Get<HPin::Type, HPinType>(net.Source()));
        arc.MoveNext(); )
      {
        //isEntered = true;
        if (arc.TimingType() == TimingType_Combinational)
        {
          R += arc.ResistanceFall() + arc.ResistanceRise();
          nArcs += 2;
        }
      }
      //if (!isEntered)
      //{
      //  ALERT(hd.GetString<HPin::Name>(net.Source()).c_str());
      //}
      //isEntered = false;
      
      if (nArcs > 0) 
        lnet += R / nArcs / hd.RoutingLayers.Physics.RPerDist;
      net.SetLNet(lnet);
    }
  }
}