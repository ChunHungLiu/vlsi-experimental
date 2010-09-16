#ifndef __VanGinnekenTreeNode_H__
#define __VanGinnekenTreeNode_H__

#include "HSteinerPoint.h"
#include "HNet.h"
class VanGinnekenTree;

enum NodeType
{
    SOURCE = 0, //- source;
    SINK = 1, // - sink;
    BRANCH = 2, // - ����� ���������;
    CANDIDATE = 3, // - �������� �� ������� ������; 
    CANDIDATE_ON_PIN = 4, // - �������� �� ������� ������ � ������������ ������������ � source ��� sink;
    CANDIDATE_INTERNAL = 5, // - �������� �� ������� ������ � ����������� ����������� � ���������� ������� ������ ��������
    SOURCE_AND_SINK = 6 // - ����� ������������ ���� � ���� ���� ����� � ���� �����
};

class VanGinnekenTreeNode
{
public:
  VanGinnekenTreeNode();  
  ~VanGinnekenTreeNode();

  double x,y; //���������� ���� �����
  NodeType type; 
  int index;

  bool HasLeft();
  bool HasRight();
  HSteinerPoint GetSteinerPoint(bool first = true);
  HNet GetNet(bool first = true);

  VanGinnekenTreeNode* GetLeft();
  VanGinnekenTreeNode* GetRight();
  VanGinnekenTree* GetTree(); 

  double GetRAT();//RAT in sinc
  double GetC();//capacity in sinc
  double GetR();//resistance in source

  void SetSteinerPoint(HSteinerPoint sp, bool first = true);  
  void SetNet(HNet n, bool first = true);
  void SetLeft(VanGinnekenTreeNode* node);
  void SetRight(VanGinnekenTreeNode* node);
  void SetTree(VanGinnekenTree* t);

  bool isSource();
  bool isSink();
  bool isBranchPoint();
  bool isCandidate();
  bool isCandidateAndRealPoint();
  bool isInternal();

protected:
  HNet net;
  HSteinerPoint sPoint;
  VanGinnekenTreeNode* left;
  VanGinnekenTreeNode* right;
  VanGinnekenTree* tree;
};

class VanGinnekenTreeNodePathBased: public VanGinnekenTreeNode
{
public:
  VanGinnekenTreeNodePathBased();
  HSteinerPoint GetSteinerPoint(bool first = true);
  HNet GetNet(bool first = true);
  void SetSteinerPoint(HSteinerPoint sp, bool first = true);  
  void SetNet(HNet n, bool first = true);

protected:
  HNet secondNet;
  HSteinerPoint secondSPoint;
};

#endif