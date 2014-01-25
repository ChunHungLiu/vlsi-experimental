#pragma once

#include <vector>

class HCell;
class HCluster;
class HClusteredNets;
class HClusteredNet;

struct LClusterization
{
    //���������� � ���������
    IDType nClusterEnd;
    IDType nClusterUnreal;
    IDType nClusterLimit;
    std::vector<HCell>** cells;
    std::vector<HClusteredNet>** tableOfAdjacentNets;
    double* xCoord;  // ��� ����������
    double* yCoord;
    double* area;    // ���������� ������� (= ����� �������� ���� ��������� ��������)
    double* potentialMultiplier; 
    bool*   isValid; //����������� � ��������� �������������. ���� true, �� ������������� score 
    //��� ������� �������� �� �����, � ��������� ������ score ���������������, 
    //� ������� ���������� � ������� (Heap) ��� ������������� �� ��
    //�����, �-�� ������������� ��� ����� score. ��� ����� ������������ �-�� InsertInHeap
    bool*   isFake;  //��� ������� ���� ��������� ��������� ���������� � ������ �� ���, 
    //� ������ ���������� ��� Fake
    bool* isTerminals;
    bool* isPrimary;
    int* id;
    int* clusterIdx2solutionIdxLUT;
    bool* isCPoint;

	//��������� � ���������� ��������� HClusteredNet
	IDType* nClusteredNetEnd;
	IDType* nClusteredNetLimit;
	std::vector<HCluster>*** clusterIdxs;
	double** weight;
	double** k;
    int** idNet;

	int nNetLevelsEnd;
	int nNetLevelsLimit;
    std::vector<HClusteredNets*>* netLevels;
    //HClusteredNets** netLevels;
    int maxNClusteredNetLimit;

    LClusterization()
    {
        nClusterEnd = 0;
        nClusterUnreal = 0;
        nClusterLimit = 0;
        cells = 0;
        tableOfAdjacentNets = 0;
        xCoord = 0;
        yCoord = 0;
        area = 0; 
        potentialMultiplier = 0;
        isValid = 0;
        isFake = 0;
        isTerminals = 0;
        isPrimary = 0;
        id = 0;
        clusterIdx2solutionIdxLUT = 0;
        isCPoint = 0;

		nClusteredNetEnd = 0;
		nClusteredNetLimit = 0;
		clusterIdxs = 0;
	    weight = 0;
	    k = 0;
        idNet = 0;

		nNetLevelsEnd = 0;
		nNetLevelsLimit = 0;
        netLevels = 0;
        maxNClusteredNetLimit= 1;

    };	
};