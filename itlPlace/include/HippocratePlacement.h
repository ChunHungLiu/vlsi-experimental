#ifndef HippocratePlacement_h
#define HippocratePlacement_h

//#include "TDDPAlgorithms.h"

#include "HDesign.h"
#include "Utils.h"
#include <algorithm>
#include <hdpgrid.h>
#include <STA.h>
#include <stack>

#define RADIUS_OF_WINDOW  5
#define MIN_ROW	      0
#define MIN_COLUMN	  0
#define CH_MOVE				0
#define CH_SWAP				1
#define CH_COMPACT		2
#define CH_CENTER			3

#define CONST_A				1.0
#define CONST_B				2.0

#define CONST_Kd			1.0 //constant based on transition of 10% to 90% VDD


#define SPECIAL_WINDOW_SIZE 10

typedef HNet* pHNet;
typedef HCell* pHCell;

struct PseudoCell{
	int row, col; //���������� ������ ������� ����
	int GapSize;  
};

struct PossiblePos{
	int row, col; //���������� ������ ������� ����
};
struct SpecWindow
{
	int StartRow, StartCol, FinishRow, FinishCol; //��������� ������ �������� � ������� �������
};
class LWindow
{
public:
	int R;					// ������
	int count;			// ���������� ���������
	HCell* cells;   // ������ ���������
	//��� ������������ �����������
	LWindow();
	~LWindow(); 
};

enum HippocrateMethods{SWAP, MOVE, LOCALMOVE, COMPACT, CENTER};

struct LPoint
{
	int col;
	int row;
};

struct BBox 
{
	int col, row, width, height;
	std::vector<HCell> cells;
	int colCenter,rowCenter;
};


class StatisticsAnalyser{
private:
	int numOfSWAPAttepmts;                   //���������� ������� ����������� ��������
	int numOfSWAPSuccessfulAttempts;         //� ���������� ������� �������
	int numOfMOVEAttepmts;
	int numOfMOVESuccessfulAttempts;
	int numOfLOCALMOVEAttepmts;
	int numOfLOCALMOVESuccessfulAttempts;
	int numOfCOMPACTAttepmts;
	int numOfCOMPACTSuccessfulAttempts;
	int numOfCENTERAttepmts;
	int numOfCENTERSuccessfulAttempts;

	bool reportSuccessful;

public:
	StatisticsAnalyser();
	void incrementAttempts(HippocrateMethods method);
	void incrementAttemptsSuccessful(HippocrateMethods method);
	void doReport();
};

class TimingHPWLWatcher{
private:
	double oldTNS;
	double oldWNS;
	double oldHPWL;
	double newTNS;
	double newWNS;
	double newHPWL;
	double  TNSImprovement;
	double  WNSImprovement;
	double HPWLImprovement;

	int callCounter;

	HDesign& hd;
	bool watch;

	char filename[100];
public:
	TimingHPWLWatcher(HDesign&, bool _watch);
	void doReport();
};

bool isCellExists(HDPGrid& hdpp, int row, int col);

double CalculateAlpha(HDesign& hd, HNet changedNet); //Alpha ����������� ��� �������� �����
double CalculateBetta(HDesign& hd, HPinWrapper sink, double oldMDist);  //Betta ����������� ��� ������� ����� � PO
inline double CalculateGamma(HDesign& hd);  //Gamma ����������� ��� ��������


double CalculateDeltaArrivalTime(HDesign& hd, HPinWrapper sink);
double GetNetSlack(HDesign& hd, HNet& net);
__inline double GetNetWeight(HDesign& hd, HNet& net);

double CalculateDiffWTWL(HDesign& hd, std::vector<HNet>& netvector, bool updateCachedValues);

std::vector<HCell> GetCurrentWindow(HDPGrid& hdpp, int R, HCell curCell);// ����� ��� MOVE, SWAP
BBox GetCurrentBBox(HDPGrid& hdpp, HDesign& hd, const HCriticalPath::PointsEnumeratorW&);// ����� ��� CENTER, COMPACT

bool ControlOfConstraints(HDPGrid& hdpp, HDesign& hd, HNet net, int oldRow, int oldCol, HCell& curCell); //�������� �����������

std::vector<PseudoCell> getPseudoCellsFromWindow(HDPGrid& hdpp, HDesign& hd, std::vector<HCell>& VCellsWindow);//�� ���� LWindow ���������� ������ ����� � ���// moded
std::vector<PossiblePos> getVPossibleHorizontalPos(HDPGrid& hdpp, HCell curCell);
std::vector<HCell> getSpecialWindow(HDPGrid& hdpp, HDesign& hd, HCell& CellFromNet, BBox& curBBox);//window for COMPACT
std::vector<PseudoCell> getPseudoCellsFromSpecWindow(HDPGrid& hdpp, HDesign& hd, std::vector<HCell>& VCellInSpWind);//���� - ������������� ������ ���������(���� �� ������ �������� � ������� �������), ����� - ������ ����� � ���
PossiblePos GetCenterNearestPos (HCell& NetCell, std::vector<PseudoCell>& VGap);

SpecWindow getSpecWindForCell(HDPGrid& hdpp, HDesign& hd, HCell& CellFromNet, BBox& curBBox );
std::vector<PseudoCell> getGapsForCell(HDPGrid& hdpp, HDesign& hd, HCell& CellFromNet, BBox& curBBox, bool InsideGaps );
std::vector<HPinWrapper> CellsInPath(HDesign& hd, HCriticalPath path);
BBox GetCurrentBBox3(HDPGrid& hdpp, HDesign& hd, HPinWrapper& myPin);
std::vector<HCell> GetCurrentWindow(HDPGrid& hdpp, int R, HCell curCell);	// ������� ���� �������� // moded


//void DoAllMethodsWithPath(HDesign&, HCriticalPath, int);

std::vector<HCell> GetCellsConnectedWithCurrent(HDPGrid& hdpp, HDesign& hd, HCell curCell);

struct PathCallback;
void DoAllMethodsWithPath(HDesign&, HCriticalPath, int);

//public:


//============= �������������� ===================
void HippocratePlacementMOVE(HDPGrid& hdpp, HDesign& hd, /*int numOfPath,int numOfPoint, */HCell& curCell, std::vector<HCell>& currentWnd, StatisticsAnalyser& stat);
void HippocratePlacementCOMPACT(HDPGrid& hdpp, HDesign& hd, HCell& curCell, BBox curBBox, StatisticsAnalyser& stat);
void HippocratePlacementLOCALMOVE(HDPGrid& hdpp, HDesign& hd, HCell& curCell, StatisticsAnalyser& stat);
void HippocratePlacementSWAP(HDPGrid& hdpp, HDesign& hd, HCell& curCell, std::vector<HCell>& currentWnd, StatisticsAnalyser& stat);
void HippocratePlacementCENTER(HDPGrid& hdpp, HDesign& hd, HCell& curCell, std::vector<HCell>& currentWnd, StatisticsAnalyser& stat);
//================================================

void DoHippocratePlacement(HDPGrid& hdpp, HDesign& hd, StatisticsAnalyser& stat, TimingHPWLWatcher& thpwlWatcher);
void HippocratePlacementInit();
void HippocratePlacementFinalize();

//};

//������-����������� ��� ���������� ��������� � ���� 
//(����� ��� getPseudoCellsFromWindow ��� ���������� �����)
class CellRowComparator{
public:
	HDPGrid& hdpp;
	CellRowComparator(HDPGrid& _hdpp):hdpp(_hdpp){}
	bool operator() (const HCell &a, const HCell &b){
		return hdpp.CellRow(a)<hdpp.CellRow(b);
	}
};
class CellColComparator{
public:
	HDPGrid& hdpp;
	CellColComparator(HDPGrid& _hdpp):hdpp(_hdpp){}
	bool operator() (const HCell &a, const HCell &b){
		return hdpp.CellColumn(a)<hdpp.CellColumn(b);
	}
};

struct PathCallback
{
	HDPGrid& hdpp;
	StatisticsAnalyser& stat;
	TimingHPWLWatcher& thpwlWatcher;

	PathCallback(HDPGrid& _hdpp, StatisticsAnalyser& _stat, TimingHPWLWatcher& _thpwlWatcher): hdpp(_hdpp),stat(_stat),thpwlWatcher(_thpwlWatcher){}

	/*bool ProcessPath(HDesign& design, HCriticalPath path, int pathNumber)
	{
		if (pathNumber > 0)
			return false;//stop pathes processing

		//do smth

		return true;
	}*/

	void ProcessPath2(HDesign& hd, HCriticalPath path, int pathNumber);
};

#endif