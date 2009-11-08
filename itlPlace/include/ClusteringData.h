#ifndef __CLUSTERING_DATA__
#define __CLUSTERING_DATA__

struct Cluster
{
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

  //SumOfDelays data
  double Lnet;
  double k;
  double braces;

  //Lagrangian Relaxation data
  double sourceAFactor;
  std::vector<double> sinkLoad;
  std::vector<double> muNetVector;

  ClusteredNet()
  {
    weight = 1.0;
    k = 0.0;
    sourceAFactor = 0.0;
  }
};

typedef std::list<MergedCluster> ClusteringLog;
typedef std::list<ClusteringLog>::reverse_iterator ClusteringLogIterator;

typedef std::vector<ClusteredNet> NetList;  //�������� ������������ ���������� ����� ����������
typedef std::list<NetList>::reverse_iterator NetListIterator;

typedef std::vector<int> ConnectionsVector;// �������� �������� ����������, � ������� ������ ������� �������

typedef double (*pAffinityFunction)(HDesign& hd, 
                                    const int& firstClusterIdx, const int& secondClusterIdx,
                                    std::vector<Cluster>& clusters, NetList& netList, int* netListSizes,
                                    std::vector<ConnectionsVector>& currTableOfAdjacentNets);

#endif