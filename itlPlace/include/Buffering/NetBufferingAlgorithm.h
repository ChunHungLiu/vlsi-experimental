#pragma once

#include "VanGinnekenData.h"
#include "BufferPositions.h"
#include "AdditionNewElement.h"
#include "ModificationVanGinnekenList.h"
#include "CreateVGList.h"
#include "HCell.h"
#include "HPin.h"
#include "HPinType.h"
#include "HNet.h"

#define	INFINITY 20000000000.0
#define ZERO 0.00000000001
#define MAXBUFFERTYPE 64

using namespace Utils;

class NetBufferingAlgorithm
{
public:
  VGAlgorithmData* data;
  AbstractCreateVGListAlgorithm* createVGListAlgorithm;
  AbstractModificationVanGinnekenList* modificationVanGinnekenList;
  AbstractAdditionNewElement* additionNewElement;
  CalculateVGBranchPoint* calculateBranchPoint;
  CalculateVGCandidatePoint* calculateCandidatePoint;

  NetBufferingAlgorithm();
  NetBufferingAlgorithm(HDesign& hd);
  ~NetBufferingAlgorithm();
  virtual void Initialize(bool isAgainInitialize = false);
  virtual VGVariantsListElement BufferingNet(HNet& net, bool isRealBuffering = true);

protected:
  bool isInitialize;
  virtual VGVariantsListElement Algorithm(VanGinnekenTree* vGTree);
};
