#ifndef __VanGinneken_H__
#define __VanGinneken_H__


#include "HDesign.h"
#include "HDPGrid.h"
#include "AdaptiveRoute.h"
#include "DelayCalculation.h"
#include "Timing.h" 
#include "Legalization.h"
#include <crtdbg.h>
#include <fstream>
#include "VanGinnekenNode.h"
#include "PlacementQualityAnalyzer.h"


class HDesign;

#define EPS	0.000000000000000000000001 //!!!
#define MAXBUFFERTYPE 64
#define nullSP m_hd.SteinerPoints.Null()
#define QM 1
#define PREDICT 1 //NOTE: predictive pruning 
#define TREATZERO 1

class VanGinneken
{
private:
  struct BufferInfo
  {
    HMacroType BufferMacroType;
    double TIntrinsic;
    double Capacitance;
    double Resistance;
  };

  //NOTE: ������ ������� 
  typedef struct comp
  { 
    struct comp *left;  
    struct comp *right; 
    int buffertype; 
    int x;  //(x, y) means the buffer is inserted at point with index y with the direction x->y  
    int y;  
    int counter;  //NOTE: use for comp deletion and comp chare 
  } Comp;

  //NOTE: Ordered list used in Van Ginneken algorithm 
  typedef struct rlnode
  {
    struct rlnode *next;
    double cap;   //NOTE: downstream capacitance 
    double time;  //NOTE: required arrival time at the node 
    Comp *com;    //NOTE: buffer location 
  } RLnode;

public:
  VanGinneken(HDesign& design);

  //NOTE: ������������ �����������, ������� �������� "�������" ����, ����� �� ����������
  int Buffering2Pass(HDPGrid& DPGrid, int argc, char** argv, const char* exportFileName);
  int BufferingTillDegradation(); //NOTE: ���������� �� ���������

  int NetBuffering(HNet &net);    //NOTE: ����������� ����

  int BufferingOfMostCriticalPaths(int nPaths = 0); //NOTE: ������������ ��� ����������� ����
  int CriticalPathBuffering(HCriticalPath aPath);   //NOTE: ����������� ������������ ����
  
  HWirePhysicalParams GetPhysical();
  BufferInfo* GetBufferInfo();
  VGNode GetVGTree();
  int NetBufferNotDegradation(HNet &net);

private:
  typedef TemplateTypes<BufferInfo>::vector BuffersVector;

  RLnode *create_list(VGNode *t); //NOTE: ������� RLnode
  void list_delete(RLnode *l);  //NOTE: ������� RLnode
  void comp_delete(Comp *com);   //NOTE: ������� comp
  RLnode *redundent(RLnode *list);
  RLnode *add_buffer(double distance, RLnode *list, int px, int py);
  double distance(VGNode *t, VGNode *t1);
  RLnode* MergeLists(RLnode* sleft, RLnode* sright, VGNode *t);

  //NOTE: ����� ����������� �������� ��� ���������
  RLnode *van(VGNode *t, double& van_answer, double rd);

  void LoadAvailableBuffers();

  void ParsingFinalLocationVan(VanGinneken::Comp *com);

  //NOTE: ������� � �������������� VGnode
  int RunVG(HNet& net);

  void InitializeBuffersIdxs();

  //NOTE: ������� ���������� ����� � ����� ����
  void PinsCountCalculation(VGNode& startNode, int startNodeIdx, int& nPins, bool doIndexesClear = false);

  //NOTE: ������� ����� ���� � ����
  void CreateNetsAndCells(HNet& net);  
  void CreateCells(string bufferName, HCell* insertedBuffers);
  void CreateNets(HNet& net, HCell* insertedBuffers, HNet* newNet);
  void AddSinks2Net(HCell* insertedBuffers, HNet& net, VGNode& startNode, int startNodeIdx, 
    HNetWrapper::PinsEnumeratorW& pinEnum, bool doIndexesClear = false);

  void print(RLnode* x);
  void printbuffer(Comp *x, int *i);

  double CalculationOptimumNumberBuffers(HNet net);

  BuffersVector       m_AvailableBuffers;  //NOTE: ���������� ��������� ������� ��� �������
  HWirePhysicalParams m_WirePhisics;
  HDesign&            m_hd;
  VGNode              m_vgNetSplitted;
  int                 m_nCandidatesForBuffering;  //���������� ����� ������� ������ ������������
  int*                m_buffersIdxsAtNetSplitted; //������� ������� � ������� ������������� �������� ������,
                                                  //m_buffersIdxsAtNetSplitted[i][0] ������ ����� �������� ������� 

  RLnode* m_VGOutput;                     //�������������� RLNode ���������� ������� ��� ���������
  int     m_nCriticalPaths;     //���������� ����������� �����, ������� �������� ������������
  int     m_RATidx;             //������� ������ � ������� rAT
  double  m_freeSpace;          //���������� ����������� ���������� ������������ �� �����
  bool    m_isFreeSpaceEnded;   //��������� �� ��������� ������������

  bool    m_doReportBuffering;
  Comp*   m_finalLocationVan;   //NOTE: ��������� ��������� Comp �������� ��� ����������
  double  bestTNS, bestWNS;
  PlacementQualityAnalyzer* m_QualityAnalyzer; 
};

#endif //__VanGinneken_H__