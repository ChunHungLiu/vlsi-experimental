#ifndef __VanGinnekenTreeNode_H__
#define __VanGinnekenTreeNode_H__

#include "HSteinerPoint.h"
#include "HNet.h"
class VanGinnekenTree;

class VanGinnekenTreeNode
{
protected:
  double x,y; //���������� ���� �����
  int type; // ��� �����: 0 - source; 1 - sink; 2 - ����� ���������; 3 - �������� �� ������� ������; 
  //4 - �������� �� ������� ������ � ������������ ������������ � source ��� sink;
  //5 - �������� �� ������� ������ � ����������� ����������� � ���������� ������� ������ ��������
  // TODO: ��������� ���������� ������������

  int index;
  HSteinerPoint sPoint;
  VanGinnekenTreeNode* left;
  VanGinnekenTreeNode* right;
  VanGinnekenTree* tree;

public:
  HNet net;
  VanGinnekenTreeNode();
  bool isSource();
  bool isSink();
  bool isBranchPoint();
  bool isCandidate();
  bool isCandidateAndRealPoint();
  bool isInternal();
  bool HasLeft();
  bool HasRight();
  HSteinerPoint GetSteinerPoint();  
  double GetX();
  double GetY();

  int GetIndex();
  VanGinnekenTreeNode* GetLeft();
  VanGinnekenTreeNode* GetRight();
  VanGinnekenTree* GetTree(); 

  double GetRAT();//RAT in sinc
  double GetC();//capacity in sinc
  double GetR();//resistance in source

  void SetSteinerPoint(HSteinerPoint sp);  
  void SetX(double value);
  void SetY(double value);
  void SetLeft(VanGinnekenTreeNode* node);
  void SetRight(VanGinnekenTreeNode* node);
  void SetType(int t);
  void SetIndex(int i);
  void SetTree(VanGinnekenTree* t);

  ~VanGinnekenTreeNode();
};

#endif