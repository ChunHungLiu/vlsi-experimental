/*  
 * multilevel_framework.h
 * this is a part of itlDragon
 * Copyright (C) 2007, ITLab, Zhivoderov Artem
 * email: zhivoderov.a@gmail.com
 */

#ifndef _MULTILEVEL_FRAMEWORK_H_
#define _MULTILEVEL_FRAMEWORK_H_

#include "..\include\errors.h"
#include "..\include\global.h"
#include "..\include\random_numbers_generation.h"
#include "tao.h"
#include <vector>
#include <list>

#define IsNotTerminal(nodeIdx) ((nodeIdx < nNodes) ? true : false)
#define MARK_NEIGHBORS_INVALID true
#define DONT_MARK_NEIGHBORS_INVALID false

using namespace std;

namespace MultilevelFramework
{
  const int CLUSTER_RATIO = 5; // decrease of the number of clusters after each iteration
  const int FINAL_NCLUSTERS = 2000;
  const double CLUSTERS_AREA_TOLERANCE = 1.5;

  struct Cluster
  {
    vector<int> cellIdxs;  // ������� �������� node'��, ������� ������ � ������ �������
    double xCoord;         // ��� ����������
    double yCoord;
    double area;           // ���������� ������� (= ����� �������� ���� ����� �� cellIdxs)
    bool   isValid;        // ����������� � ��������� �������������. ���� true, �� ������������ score ��� ������� ��������
                           // �� �����, � ��������� ������ score ���������������, � ������� ���������� � ������� (Heap)
                           // ��� ������������� �� ��
                           // �����, �-�� ������������� ��� ����� score. ��� ����� ������������ �-�� InsertInHeap
    bool   isFake;         // ��� ������� ���� ��������� ��������� ���������� � ������ �� ���, � ������ ���������� ��� Fake
    Cluster()
    {
      xCoord  = yCoord = area = 0.0;
      isFake  = false;
      isValid = true;
    }
  };
  
  struct ClusterData
  {
    double score;
    int bestNeighborIdx;
    int clusterIdx;
    ClusterData()
    {
      score = 0.0;
      bestNeighborIdx = 0;
      clusterIdx = 0;
    }
  };

  // �� ����� ������������� ������� ��� ������������� (clusteringLog) ��� ����, ����� ����� 
  // �� ���� ���������� ����� �� ���������� ������� �������������.
  // ������, �-�� ���������� ������� � ������ ������������� ��� ������������� ���� ���������, ����� �����������
  // ��������� ����������
  struct ClusteredNodes
  {
    int firstClusterIdx;
    int secondClusterIdx;
    size_t size;  // end index of clusters in "vector<int> cellIdxs" of the result cluster
  };
  
  typedef list<ClusteredNodes> ClusteringInfoAtEachLevel;

  // �������� ������������ ���������� ����� ����������
  typedef vector<vector<int>> NetList;
  
  // �������� �������� ����������, � ������� ������ ������� �������
  typedef vector<int> ConnectionsList;
  
  typedef list<ClusteringInfoAtEachLevel>::reverse_iterator ClusteringLogIterator;

  typedef MULTIPLACER_ERROR (*affinityFunc)(const int& firstClusterIdx, const int& secondClusterIdx,
                                            vector<Cluster>& clusters, NetList& netList, 
                                            vector<ConnectionsList>& currTableOfConnections,
                                            double* netAreas, double& result);
  
  void Merge(vector<int>& a, vector<int>& b, int result[]);

  MULTIPLACER_ERROR Clusterize(Circuit& circuit, vector<Cluster>& clusters, NetList& netList,
                             vector<ConnectionsList>& currTableOfConnections, list<NetList>& netLevels,
                             affinityFunc Affinity, list<ClusteringInfoAtEachLevel>& clusteringLog, int& currNClustrers);

  void InitializeDataStructures(Circuit& circuit, vector<Cluster>& clusters, NetList& netList, const int& nNodes);

  MULTIPLACER_ERROR Relaxation(Circuit& circuit, vector<Cluster>& clusters, NetList& netList);
  
  MULTIPLACER_ERROR Interpolation(Circuit& circuit, vector<Cluster>& clusters, 
                                  vector<ConnectionsList>& currTableOfConnections, NetList& netList);

  void UnclusterLevelUp(vector<Cluster>& clusters, vector<ConnectionsList>& currTableOfConnections,
                        NetList& netList, ClusteringLogIterator clusteringLogIterator, const int& nNodes);

  void UpdateCoords(Circuit& circuit, vector<Cluster>& clusters);

  MULTIPLACER_ERROR MergeClusters(const int& firstClusterIdx, const int& secondClusterIdx,
                                  vector<Cluster>& clusters, NetList& netList,
                                  vector<ConnectionsList>& currTableOfConnections,
                                  double* netAreas);
  MULTIPLACER_ERROR AffinityInterp(const int& firstClusterIdx, const int& secondClusterIdx,
                                   vector<Cluster>& clusters, NetList& netList, 
                                   vector<ConnectionsList>& currTableOfConnections,
                                   double* netAreas, double& result);
  MULTIPLACER_ERROR Affinity(const int& firstClusterIdx, const int& secondClusterIdx, 
                             vector<Cluster>& clusters, NetList& netList, 
                             vector<ConnectionsList>& currTableOfConnections, double* netAreas, double& result);
  // affinity function that considers spacial proximity
  MULTIPLACER_ERROR AffinitySP(const int& firstClusterIdx, const int& secondClusterIdx, 
                               vector<Cluster>& clusters, NetList& netList, 
                               vector<ConnectionsList>& currTableOfConnections, double* netAreas, double& result);
  MULTIPLACER_ERROR CalculateScore(const int& currClusterIdx, vector<ConnectionsList>& currTableOfConnections,
                                   NetList& netList, double* netAreas, vector<Cluster>& clusters,
                                   double& score, int& bestNeighborIdx, bool isToMark,
                                   affinityFunc Affinity);
  MULTIPLACER_ERROR InsertInHeap(list<ClusterData>& clustersDataList);

  void CreateTableOfConnections(vector<Cluster>& clusters, vector<ConnectionsList>& currTableOfConnections,
                                NetList& netList, const int& nNodes);

  double LogSumExpForClusters(PetscScalar *coordinates, void* data);
  void SetInitialState(vector<Cluster>& clusters, Circuit& circuit, const int& numOfClusters);
}

#endif