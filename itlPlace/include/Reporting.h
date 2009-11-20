#ifndef __REPORTING_H__
#define __REPORTING_H__

#include "HDesign.h"
#include "VanGinneken.h"

//macro types reports
void ReportCellsByMacroFunction(HDesign& design);
//���������� cell ������� ��� MacroType
void ReportCellByMacroType(HDesign& design);
//����� ������� ��� ������� MacroType: ���, ������, ���, ���������� �����, 
//��� ������� ����: ���, �����������, �������������� ����������, �������� ������������ 
//�������� �������� �����������
void ReportMacroTypeInfo(HDesign& design);

//circuit reports
void ReportPlacementArea(HDesign& design);
void ReportWithTheCharacteristicsOfCircuits(HDesign& design);
void ReportBenchmarkStatistics(HDesign& design);

//nets reports
//���������� ���������� ����������, ���������� � �.�. �����.
void ReportNetsInfo(HDesign& design);

//���������� � ���������� ����� ������� ���� (all, skipped, buffered, removed, Active).
void ReportCountNetsWithTypes(HDesign& design);
void ReportNetPins(HDesign& hd, HNet net);
void ReportNetPinsCoordinates(HDesign& hd, HNet net);
void ReportNetTiming(HDesign& design, HNet net);

void ReportNegativeSlacks(HDesign&);

void PlotMostCriticalPaths(HDesign& design, int n = -1);
void PrintTimingReport(HDesign& hd, int nPaths = -1);

#endif //__REPORTING_H__