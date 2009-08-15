#ifndef __VanGinnekenNode_H__
#define __VanGinnekenNode_H__

#include "HDesign.h"

#define	INFINITY	20000000000.0

const double FBI_WIRE_RESISTANCE_SCALING  = 1000.0;
const double FBI_WIRE_CAPACITANCE_SCALING = 1.0;
const double FBI_CELL_RESISTANCE_SCALING  = 1000.0;
const double FBI_CELL_CAPACITANCE_SCALING = 1.0;
const double FBI_TIME_SCALING             = 1000.0;
const double FBI_LENGTH_SCALING           = 0.001;


//NOTE: ��������� ������ ����� VGNode �� �������
class VGItem
{
public:
  VGItem()
  {
    x = 0;
    y = 0;
  }
  HSteinerPoint steinerPoint;
  double x, y;
};

//NOTE: ����� �������� ���������:
//
//     �
//     |
//     �
//     |
//     o
//
//���
//     o - x
//     |   |
//     x   0
//     |
//     0
//���
// o
//
// "O" - ����, "o" - �����, "0" - ��������� ����� ������������ ������, "x" - ����������� �����, 
// "|" - �������(�����) �����, " - " - �������(�����) � �����(������������� ����������)
class VGNode
{
public:
  VGNode(HSteinerPoint start, HSteinerPoint lEnd, HSteinerPoint rEnd, double capacitance, 
    double requiredArrivalTime, int steps, int step_num, int typePoint, 
    HDesign& hd, int act, int index);

  ~VGNode();
  //���������� �������.
  void UpdatingTree();


  //NOTE: ������������� ������ ������� � �����, �� ������������ ������
  int InitializeTree(HSteinerPoint start,  double capacitance, double requiredArrivalTime, int steps, 
    int typePoint, int act, int index);

  //NOTE: ������ ���������� x � y, ���� isNew = 0-������, 1-����� 
  double X(int isNew = 0) const ;
  double Y(int isNew = 0) const ;

  double Cap() const ;  //NOTE: node capacitance 
  double Time() const ; //NOTE: required arrival time at the node 

  //NOTE: �������� ������, ���������, ������
  bool IsSource() const ;
  bool IsInternal() const ;
  bool IsSink() const ;

  bool IsRightNodeExist();
  bool IsLeftNodeExist();

  VGNode* LeftStep() ;
  VGNode* RightStep() ;
  void    BackStep();

  VGNode* GetLeft() ;
  VGNode* GetRight() ;

  int Index() const ; //NOTE: ������ �������� �����

  //NOTE: ���������� �� �������: ����������� ����� ���� ��� ����, ���������� �����, 
  //����� �������� �� ����� ���������, �������, VGNode � ���� �����
  VGNode& GetSteinerPoint(int index, VGNode &node, VGItem& result, bool doIndexesClear = false);

  void Destroy(); //NOTE: ���������� ������
  void NULLPointer(); //NOTE: �������� ����� � ������ ���������
  bool IsReal();  //NOTE: �� ����� ����� �������� ����������� �����
  void IndexesClear();  //NOTE: ��������� �������� � ������

  double LengthTree(bool doIndexesClear = false);

protected:
  HDesign&      m_hd;

  VGNode* left; //NOTE: ��������� �� ����� ���������
  VGNode* right; //NOTE:  ��������� �� ������ ���������

  int    m_typePoint;//NOTE: ��� ����� 0 - Source; 1 - Internal; 2 - Sink;
  double m_Capacitance; //NOTE: node capacitance 
  double m_RAT; //NOTE: required arrival time at the node  

  HSteinerPoint m_SubtreeRoot; //NOTE: ������ ���������
  HSteinerPoint m_LeftEnd; //NOTE: ����� ������� ���������
  HSteinerPoint m_RightEnd; //NOTE: ������ ������� ���������

  int           m_steps;//NOTE:���������� ����� �� ������� �� ������ �� �����
  int           m_indexStart;//NOTE: ����� � �������� ���������
  int           m_RelativePosition; //������� ������� 0 - �����, 1 - ������, 2 - ���� (��� ������)
  int           m_nLeftSteps, m_nRightSteps; //NOTE: �������, ����������� �� ����� � ��������� (���� = 0 �� ������, ���� = m_steps �� �����)
  bool          isTransitionL, isTransitionR; //NOTE: ��� ������� � �����, �����
  //NOTE:���������� ����� ������ � �����
  double m_xs, m_xle, m_xre;
  double m_ys, m_yle, m_yre;

  int m_leftCount; //NOTE: ����� ������� � ����� ���������
  bool isRightNodeExist;



};

#endif //__VanGinnekenNode_H__