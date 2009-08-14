/* 
* main.cpp
* this is a part of itlAnalyticalPlacer
* Copyright (C) 2005-2006, ITLab, Kornyakov, Kurina, Zhivoderov
* email: kirillkornyakov@yandex.ru
* email: zhivoderov.a@gmail.com
* email: nina.kurina@gmail.com
*/

#include <iostream>
#include <conio.h>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>
#include <windows.h>
#include <direct.h>
#include "..\include\parser.h"
#include "..\include\output.h"
#include "..\include\check_legality.h"
#include "..\include\errors.h"
#include "..\include\global.h"
#include "..\include\abs_detailed_placement.h"

GeneralWindow *slidingWindow;
double *x_array;
Circuit  circuit;

int ReadBounds(char fileName[], double *&boundsArray, char *&auxName, int &firstRow ) {
  FILE *f = fopen(fileName, "r");
  int stringSize = 100;
  int k1 = 0;
  int k2 = 0;

  if (f == NULL) {
    cout<<"File with bounds not found!\n";
    return 0;
  }
  
  char *str = new char [stringSize];
	auxName = new char [stringSize];
  char *num = new char [stringSize];

  for (int i = 0; i < stringSize; i++) {
    str[i] = '\0';
    num[i] = '\0';
		auxName[i] = '\0';
  }

  fgets(str, stringSize - 1, f); //getting aux file name
	for(int i = 0; str[i] != '\n'; i++)
		auxName[i] = str[i];
	
	fgets(str, stringSize - 1, f); //getting number of rows

  while((str[k1] != ':') && (k1 < stringSize))
    k1++;

  k1 += 2;

  while ((k1 < stringSize) && (isdigit(str[k1]) ))
    num[k2++] = str[k1++];
  
  int rowsNum = atoi(num);
  //cout<<"num: "<<rowsNum<<endl; ///
  boundsArray = new double [rowsNum * 2];

  fgets(str, stringSize - 1, f); //getting index of first row
  for(int j = 0; j < k2 + 1; j++)
		num[j] = '\0';

  k1 = 0;
  k2 = 0;

  while((str[k1] != ':') && (k1 < stringSize))
	  k1++;

  k1 += 2;

  while ((k1 < stringSize) && (isdigit(str[k1]) ))
    num[k2++] = str[k1++];
  
  firstRow = atoi(num);
  cout<<"firstRow: "<<firstRow<<endl;

  for(int i = 0; i < rowsNum * 2;) { //getting bounds of rows
    for(int j = 0; j < k2 + 1; j++)
		num[j] = '\0';

    k1 = 0;
    k2 = 0;
    fgets(str, stringSize - 1, f);

    if(str[0] == 'e') {
      cout<<"incorrect file\n";
      delete [] str;
      delete [] num;
      return 0;
    }

    k1 = 0;
    k2 = 0;
    
    while((k1 < stringSize) && (str[k1] != ':'))
      k1++;

    k1 += 3;

    while ((k1 < stringSize) && (isdigit(str[k1]) ))
      num[k2++] = str[k1++];

    //cout<<num<<endl;
    boundsArray[i++] = atof(num); //first bound for this row
    //cout<<boundsArray[i - 1]<<endl; ///

		for(int j = 0; j < k2 + 1; j++)
			num[j] = '\0';

    k2 = 0;
    k1 += 2;

    while ((k1 < stringSize) && (isdigit(str[k1]) ))
      num[k2++] = str[k1++];

    boundsArray[i++] = atoi(num); //second bound for this row
     // cout<<boundsArray[i - 1]<<endl; ///
  }

  /*if(k1 >= stringSize) {
    cout<<"incorrect file\n";
    return 0;
  }*/

  
  delete [] str;
  delete [] num;
  fclose(f);
  return rowsNum;
}

int Initialize(int task_number)
{

  Statistics statistics;
  int _argc = 3;
  char **_argv = new char* [_argc];
  _argv[0] = "detailed_placement";
  _argv[1] = "-f";
  int firstRow = 0;
  //_argv[2] = "abt01.aux";

  char abtBoundsFile[256]; // = "benchmark_info.txt";
  sprintf_s(abtBoundsFile, "abt0%d\\benchmark_info.txt", task_number);
  int rows_num = ReadBounds(abtBoundsFile, x_array, _argv[2], firstRow);
  MULTIPLACER_ERROR errorCode = OK;
  CMDParse(_argc, _argv);

  if (gOptions.calcTimingFileName[0] != '\0')
  {
    //if (gOptions.isLEFDEFinput)
    //ParseDEF(gOptions.calcTimingFileName, circuit);
    //else
    if (gOptions.defName[0] != '\0')
    {
      //ParseLEFDEF(gOptions.lefName, gOptions.defName, circuit);
      ParseAux(gOptions.benchmarkName, circuit);
      ParsePl(gOptions.calcTimingFileName, circuit);
      MakeTableOfConnections(circuit);
    }

    exit(1);
  }

  // initialize all data structures, parse benchmark and so on
  errorCode = Initialization(circuit, statistics);
  CheckCode(errorCode);

  // we shift point of origin to the bottom left corner of placement area
  ShiftCoords(circuit);
  statistics.currentWL = cf_recalc_all(UPDATE_NETS_WLS, circuit.nNets, circuit.nets, circuit.placement);
	cout<<"WL: "<<statistics.currentWL<<endl;

  PrintCircuitInfo(circuit);

  if (gOptions.doCheckLegality)
  {// if itlAnalyticalPlacer is run with -check key
    cout << "Pin-to-Pin HPWL: " << statistics.currentWL << "\n\n";  
    errorCode = CheckLegalityOfPlacement(circuit);
    CheckCode(errorCode);
    Exit();
  }

  if (gOptions.doConvertToRouter == true)
  {
    cout << "Save nets to router format..." << endl;
    PrintNetsToRouterFormat(circuit);
    cout << "File " << gOptions.GRFileName << " written successful" << endl;
    Exit();
  }

  if (gOptions.isLEFDEFinput && gOptions.convert2BookshelfName[0] != '\0')
  {
    LEFDEF2Bookshelf(gOptions.convert2BookshelfName, circuit);
    cout << "Writing bookshelf from LEFDEF to " << gOptions.convert2BookshelfName << endl;
    Exit();
  }

  PrintToTmpPL(circuit, statistics);

  //*************************** O U T P U T ************************//
  int *all_elements = new int [circuit.nNodes];

  for(int i = 0; i < circuit.nNodes; i++)
    all_elements[i] = i;

  //circuit.placement[4].xCoord -= 1;
  slidingWindow = new GeneralWindow( rows_num, firstRow, x_array, circuit.nNodes, circuit, all_elements);
  //slidingWindow->elements[4].XCoord -= 0.5;
  //cout<<endl<<slidingWindow->g1WindowBounds(circuit)<<endl;
  //cout<<slidingWindow->g2Overlaps(circuit)<<endl;
  //cout<<slidingWindow->CalcWL(circuit, x_array)<<endl;
  return 0;
}

int main() {
  Initialize(1);
  //system("PAUSE");
  return 0;
}





///////////////////////////////////////////////////////////////////////////////
////                                     //
////          NIZHNY NOVGOROD STATE UNIVERSITY           //
////                                     //
////         Copyright (c) 1999-2006 by A. Sysoyev.          //
////              All Rights Reserved.               //
////                                     //
////  File:    ${DLL_NAME}.cpp                       //
////                                     //
////  Purpose:   implementation of all involved functionals          //
////                                     //
////  Author(s): Vinogradov                          //
////                                     //
///////////////////////////////////////////////////////////////////////////////

#define PREFIX_EXPORT extern "C" __declspec(dllexport)
#define M_PI 3.1415926535897932384626433832795

int DIMENSION = circuit.nNodes;
bool first_time = true;

#define MAX_PATH 260
static char DllWorkingDir[MAX_PATH];
static char ExternalWorkingDir[MAX_PATH];

// ---------------------------------------------------------------------------
PREFIX_EXPORT int __cdecl getDimension()
{
  DIMENSION = circuit.nNodes;
  return DIMENSION;
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT bool __cdecl setDimension(int dimension)
{
  if (dimension > 1)
  {
    DIMENSION = dimension;
    return true;
  }
  else
    return false;
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT int __cdecl getNumberOfFunctions()
{
	return 3;
}

//----------------------------------------------------------------------------
PREFIX_EXPORT int __cdecl getNumberOfConstraints()
{
  return 2;
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT bool __cdecl getOrderOfCriteria(int* order)
{
  if (order == NULL)
    return false;
  order[0] = 2; ///2
  return true;
}

PREFIX_EXPORT bool __cdecl getOrderOfConstraints(int* order)
{
  if (order == NULL)
    return false;
  order[0] = 0;
  order[1] = 1;
  return true;
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT bool __cdecl getDomain(double* a, double* b)
{
  if (a != NULL && b != NULL)
  {
    for (int i = 0; i < DIMENSION; ++i)
    {
      a[i] = 0; b[i] = slidingWindow->boundX[slidingWindow->nRows];
    }
    return true;
  }
  else
  {
    return false;
  }
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT void __cdecl 
getDescription(char* name, char* desc, int num_of_function)
{
  switch (num_of_function)
  {
  case 0:
    if (name != NULL)
      strcpy(name, "WindowBounds");
    if (desc != NULL)
      strcpy(desc, "constraint 1");
    break;  
  case 1:
    if (name != NULL)
      strcpy(name, "Overlaps");
    if (desc != NULL)
      strcpy(desc, "constraint 2");
    break;
	case 2:
    if (name != NULL)
      strcpy(name, "HPWL");
    if (desc != NULL)
      strcpy(desc, "criterion");
    break;
  }
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT double __cdecl function(const double * y, int num_of_function)
{
  _getcwd(ExternalWorkingDir, 256);
  _chdir(DllWorkingDir);
  double res = 0.0;

  for(int i = 0; i < slidingWindow->elementsNum; i++)
  {
	  slidingWindow->elements[i].XCoord = y[i]; //elments[i].XCoord
  }

  switch (num_of_function) {
  case 0: // constraint 1 - geometry
    res = slidingWindow->g1WindowBounds(circuit); break; ///g1WindowBounds(circuit)

  case 1: // constraint 2 - overlaps
    res = slidingWindow->g2Overlaps(circuit); break; ///g2Overlaps(circuit)

  case 2: // criterion - HPWL
    res = slidingWindow->CalcWL(circuit); break;

  default:
    res = 0; break;
  }

  _chdir(ExternalWorkingDir);
  return res;
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT void _cdecl init_task_dll()
{
  _getcwd(ExternalWorkingDir, 256);
  _chdir(DllWorkingDir);

  /*if(first_time)
  {
    Initialize(1);
    first_time = false;
  }*/
  Initialize(1);

  _chdir(ExternalWorkingDir);
}

//----------------------------------------------------------------------------
PREFIX_EXPORT void __cdecl setDllWorkingDir(const char* work_dir)
{
  strcpy(DllWorkingDir, work_dir);
}

//----------------------------------------------------------------------------
PREFIX_EXPORT int __cdecl getNumberOfInputFiles()
{
  return 28;
}

//----------------------------------------------------------------------------
PREFIX_EXPORT bool __cdecl getInputFiles(char** names)
{
  if (names == NULL)
  return false;
  strcpy(names[0], "abt01\\abt01.aux");
  strcpy(names[1], "abt01\\abt01.nets");
  strcpy(names[2], "abt01\\abt01.nodes");
  strcpy(names[3], "abt01\\abt01.pl");
  strcpy(names[4], "abt01\\benchmark_info.txt");
  strcpy(names[5], "abt01\\abt01.scl");
  strcpy(names[6], "abt01\\abt01.wts");

  strcpy(names[7], "abt02\\abt02.aux");
  strcpy(names[8], "abt02\\abt02.nets");
  strcpy(names[9], "abt02\\abt02.nodes");
  strcpy(names[10], "abt02\\abt02.pl");
  strcpy(names[11], "abt02\\benchmark_info.txt");
  strcpy(names[12], "abt02\\abt02.scl");
  strcpy(names[13], "abt02\\abt02.wts");

  strcpy(names[14], "abt03\\abt03.aux");
  strcpy(names[15], "abt03\\abt03.nets");
  strcpy(names[16], "abt03\\abt03.nodes");
  strcpy(names[17], "abt03\\abt03.pl");
  strcpy(names[18], "abt03\\benchmark_info.txt");
  strcpy(names[19], "abt03\\abt03.scl");
  strcpy(names[20], "abt03\\abt03.wts");

  strcpy(names[21], "abt04\\abt04.aux");
  strcpy(names[22], "abt04\\abt04.nets");
  strcpy(names[23], "abt04\\abt04.nodes");
  strcpy(names[24], "abt04\\abt04.pl");
  strcpy(names[25], "abt04\\benchmark_info.txt");
  strcpy(names[26], "abt04\\abt04.scl");
  strcpy(names[27], "abt04\\abt04.wts");

  return true;
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT int __cdecl getNumberOfTasks()
{
  return 4;
}

// ---------------------------------------------------------------------------
PREFIX_EXPORT void __cdecl setTask(const int task_number)
{
  _getcwd(ExternalWorkingDir, 256);
  _chdir(DllWorkingDir);

  first_time = false;
  Initialize(task_number + 1);

  _chdir(ExternalWorkingDir);
}

// ---------------------------------------------------------------------------