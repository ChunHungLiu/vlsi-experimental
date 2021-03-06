#include "GlobalPlacement.h"
#include "Utils.h"

void RandomPlacement(HDesign& design)
{
  WRITELINE("");
  ALERT("RANDOM PLACEMENT STARTED");

  double minXcoordinate = design.Circuit.PlacementMinX();
  double minYcoordinate = design.Circuit.PlacementMinY();
  double width  = design.Circuit.PlacementWidth();
  double height = design.Circuit.PlacementHeight();
  double rndX, rndY;
  srand(design.cfg.ValueOf(".randSeed", 0));

  for (HCells::PlaceableCellsEnumeratorW i = design.Cells.GetPlaceableCellsEnumeratorW(); i.MoveNext(); )
  {
    rndX = minXcoordinate + ((double)rand()/(double)RAND_MAX)*(width - i.Width());
    rndY = minYcoordinate + ((double)rand()/(double)RAND_MAX)*(height - i.Height());
    i.SetX(rndX);
    i.SetY(rndY);
  }

  ALERT("HPWL after random placement: %f", Utils::CalculateHPWL(design, true));
  ALERT("RANDOM PLACEMENT FINISHED");
}