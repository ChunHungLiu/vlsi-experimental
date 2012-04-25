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
  HCluster clusterIdx;
  HCluster bestNeighborIdx;
  double score;

  MergeCandidate()
  {
    score = 0.0;
    //bestNeighborIdx = HClusters::Null();
    //clusterIdx = HClusters::Null();
  }
};

// �� ����� ������������� ������� ��� ������������� (clusteringLog) ��� ����, ����� ����� 
// �� ���� ���������� ����� �� ���������� ������� �������������.
// ������, �-�� ���������� ������� � ������ ������������� ��� ������������� ���� ���������, 
// ����� ����������� ��������� ����������
struct MergedCluster
{
  HCluster cluster1Idx;
  HCluster cluster2Idx;
  size_t nCellsInCluster1;
};

class ClusteredNet
{
public:
  std::vector<HCluster> clusterIdxs;
  
  double weight;
  double k;

  ClusteredNet()
  {
    weight = 1.0;
    k      = 1;
  }
  ClusteredNet& operator=(const ClusteredNet& cn)
  {
    this->weight      = cn.weight;
    this->k           = cn.k;
    this->clusterIdxs = cn.clusterIdxs;
    return *this;
  }
};

typedef std::list<MergedCluster> ClusteringLog;
typedef std::list<ClusteringLog>::reverse_iterator ClusteringLogIterator;

//typedef std::vector<ClusteredNet> ClusteringNetList0;  //�������� ������������ ���������� ����� ����������
//typedef std::list<NetList>::reverse_iterator NetListIterator;

typedef std::vector<HClusteredNet> ConnectionsVector;// �������� �������� ����������, � ������� ������ ������� �������

typedef double (*pAffinityFunction)(HDesign& hd, HCluster& firstClusterIdx, HCluster& secondClusterIdx, 
                                                   std::vector<Cluster>& clusters, HClusteredNets* netList,
                                                   std::vector<ConnectionsVector>& currTableOfAdjacentNets);

#endif