#ifndef __CLUSTERING__
#define __CLUSTERING__

#include "HDesign.h"
#include "Auxiliary.h"

const int OK = 0;

const int SHIFT_NUMBER_FOR_TERMINALS = 1000000; 
const int SHIFT_NUMBER_FOR_PRIMARY_PINS = 2000000;

bool IsMovableCell(int idx);
bool IsTerminal(int idx);
bool IsPrimaryPin(int idx);

struct Cluster
{
  //std::vector<std::string> cells;  // ������� �������� cell, ������� ������ � ������ �������
  std::vector<HCell> cells;
  double xCoord;  // ��� ����������
  double yCoord;
  double area;    // ���������� ������� (= ����� �������� ���� ��������� ��������)
  double potentialMultiplier; 
  bool   isValid; //����������� � ��������� �������������. ���� true, �� ������������� score 
                  //��� ������� �������� �� �����, � ��������� ������ score ���������������, 
                  //� ������� ���������� � ������� (Heap) ��� ������������� �� ��
                  //�����, �-�� ������������� ��� ����� score. ��� ����� ������������ �-�� InsertInHeap
  bool   isFake;  //��� ������� ���� ��������� ��������� ���������� � ������ �� ���, 
                  //� ������ ���������� ��� Fake
  Cluster()
  {
    xCoord = yCoord = 0.0;
    area = 0.0;
    isFake = false;
    isValid = true;
  }
};

struct MergeCandidate
{
  int clusterIdx;
  int bestNeighborIdx;
  double score;

  MergeCandidate()
  {
    score = 0.0;
    bestNeighborIdx = -1;
    clusterIdx = -1;
  }
};

// �� ����� ������������� ������� ��� ������������� (clusteringLog) ��� ����, ����� ����� 
// �� ���� ���������� ����� �� ���������� ������� �������������.
// ������, �-�� ���������� ������� � ������ ������������� ��� ������������� ���� ���������, 
// ����� ����������� ��������� ����������
struct MergedCluster
{
  int cluster1Idx;
  int cluster2Idx;
  size_t nCellsInCluster1;
};

struct ClusteredNet
{
  std::vector<int> clusterIdxs;
  double weight;
  double Lnet;
  double k;
  double braces;

  ClusteredNet()
  {
    weight = 1.0;
    k = 0.0;
  }
};

typedef std::list<MergedCluster> ClusteringInfoAtEachLevel;

//�������� ������������ ���������� ����� ����������
typedef std::vector<ClusteredNet> NetList;

// �������� �������� ����������, � ������� ������ ������� �������
typedef std::vector<int> ConnectionsVector;

typedef std::list<ClusteringInfoAtEachLevel>::reverse_iterator ClusteringLogIterator;

typedef std::list<NetList>::reverse_iterator NetLevelsIterator;

typedef double (*pAffinityFunction)(HDesign& hd, const int& firstClusterIdx, const int& secondClusterIdx,
                                    std::vector<Cluster>& clusters, NetList& netList, int* netListSizes,
                                    std::vector<ConnectionsVector>& currTableOfAdjacentNets);

class ClusteringInformation
{
public:
  int    mClusterRatio; // decrease ratio of the number of clusters after each iteration
  int    mDesiredFinalNumberOfClusters;
  double mClustersAreaTolerance;
  int    mCurrentNumberOfClusters;
  std::vector<Cluster> clusters;
  std::vector<HPin> primaryPins;     //PP and terminal also indexed as usual cells for LSE calculation
  std::vector<HCell> terminalCells;  //they aren't clustered, but  they has their own cluster index

  std::list<ClusteringInfoAtEachLevel> clusteringLog; // ���������� ��� �������� ����� �������� �������������

  NetList netList;
  std::list<NetList> netLevels;  // ������ ��������� �� ������ ������ �������������

  std::vector<ConnectionsVector> tableOfAdjacentNets;
  pAffinityFunction affinityFunction;

  ClusteringInformation(HDesign& hd)
  {
    mClusterRatio                 = hd.cfg.ValueOf("Clustering.clusterRatio", 5);
    mDesiredFinalNumberOfClusters = hd.cfg.ValueOf("Clustering.desiredFinalNumberOfClusters", 2000);
    mClustersAreaTolerance        = hd.cfg.ValueOf("Clustering.clustersAreaTolerance", 1.5);
  }

  void SaveToFile(const char* fileName, const char* benchName, HDesign& hd);
  bool LoadFromFile(const char* fileName, const char* benchName, HDesign& hd);

private:
  void SaveClustersToFile(FILE* rf, HDesign& hd);
  void SaveClusteringLogToFile(FILE* rf);
  void SaveNetListToFile(FILE* rf, NetList& netList);
  void SaveNetLevelsToFile(FILE* rf);
  void SaveCurrTableOfAdjacentNetsToFile(FILE* rf);

  void LoadClustersFromFile(FILE* rf, HDesign& hd);
  void LoadClusteringLogFromFile(FILE* rf);
  void LoadNetListFromFile(FILE* rf, NetList& netList);
  void LoadNetLevelsFromFile(FILE* rf);
  void LoadCurrTableOfAdjacentNetsFromFile(FILE* rf);
};

int Clustering(HDesign& hd, ClusteringInformation& ci);

void UnclusterLevelUp(HDesign& hd, ClusteringInformation& ci, ClusteringLogIterator& iter);

void CalculateNetListSizes(NetList& netList, int* netListSizes);

bool PredicateMergePairGreater(MergeCandidate elem1, MergeCandidate elem2);

//double AffinityInterp(const int& firstClusterIdx, const int& secondClusterIdx,
//                   std::vector<Cluster>& clusters, NetList& netList, int* netListSizes,
//                   std::vector<ConnectionsVector>& currTableOfAdjacentNets);
double Affinity(HDesign& hd, const int& firstClusterIdx, const int& secondClusterIdx, 
             std::vector<Cluster>& clusters, NetList& netList, int* netListSizes,
             std::vector<ConnectionsVector>& currTableOfAdjacentNets);
// affinity function that considers euclidean distance
//double AffinitySP(const int& firstClusterIdx, const int& secondClusterIdx, 
//               std::vector<Cluster>& clusters, NetList& netList, int* netListSizes,
//               std::vector<ConnectionsVector>& currTableOfAdjacentNets);

int FindBestNeighbour(HDesign& hd, ClusteringInformation& ci, int* netListSizes, 
                      MergeCandidate& mergeCandidate);

void CreateTableOfAdjacentNets(HDesign& hd, ClusteringInformation& ci);

//returns next not fake cluster index
bool GetNextActiveClusterIdx(ClusteringInformation* ci, int& clusterIdx);

void CalculateScores(HDesign& hd, ClusteringInformation& ci, 
                     std::list<MergeCandidate>& mergeCandidates, int*& netListSizes);
void CalculateScores(HDesign& hd, ClusteringInformation& ci, 
                     std::vector<MergeCandidate>& clustersData, int*& netListSizes);

int CalculateNumberOfActiveClusters(ClusteringInformation& ci);

#endif