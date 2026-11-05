/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPickHelper
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaPickHelper.h"

#include "albaView.h"
#include "albaVME.h"
#include "albaMatrix.h"
#include "albaRWIBase.h"
#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"

#include "vtkCellPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkAssemblyNode.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

//----------------------------------------------------------------------------
bool albaPickHelper::PickCell(albaView *view, double x, double y, double pickedPos[3], albaVME **pickedVme, vtkIdType *cellId)
//----------------------------------------------------------------------------
{
  // compound views return the render window interactor of their selected sub view
  albaRWIBase *rwi = view ? view->GetRWI() : NULL;
  return PickCell(rwi ? rwi->GetRenderWindow() : NULL, x, y, pickedPos, pickedVme, cellId);
}

//----------------------------------------------------------------------------
bool albaPickHelper::PickCell(vtkRenderWindow *renderWindow, double x, double y, double pickedPos[3], albaVME **pickedVme, vtkIdType *cellId)
//----------------------------------------------------------------------------
{
  pickedPos[0] = pickedPos[1] = pickedPos[2] = 0.0;
  if (pickedVme)
    *pickedVme = NULL;
  if (cellId)
    *cellId = -1;

  if (renderWindow == NULL)
    return false;

  vtkALBASmartPointer<vtkCellPicker> cellPicker;
  cellPicker->SetTolerance(0.001);

  // stop at the first renderer picking something
  bool picked = false;
  vtkRendererCollection *rc = renderWindow->GetRenderers();
  vtkRenderer *r = NULL;
  rc->InitTraversal();
  while (!picked && (r = rc->GetNextItem()))
    picked = cellPicker->Pick(x, y, 0, r) != 0;

  if (!picked)
    return false;

  cellPicker->GetPickPosition(pickedPos);
  if (cellId)
    *cellId = cellPicker->GetCellId();

  // scan the path from the leaf to find the assembly knowing the picked VME
  vtkAssemblyPath *path = cellPicker->GetPath();
  if (pickedVme && path)
  {
    for (int i = path->GetNumberOfItems() - 1; i >= 0; i--)
    {
      vtkAssemblyNode *node = (vtkAssemblyNode *)path->GetItemAsObject(i);
      vtkProp *prop = node ? node->GetViewProp() : NULL;
      if (prop && prop->IsA("vtkALBAAssembly"))
      {
        *pickedVme = ((vtkALBAAssembly *)prop)->GetVme();
        break;
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------
bool albaPickHelper::PickPoint(albaView *view, double *mousePos, albaMatrix *trackerPose, double pickedPos[3], albaVME *&pickedVme, vtkIdType &pointId, double &scalar)
//----------------------------------------------------------------------------
{
  pickedVme = NULL;
  pointId = -1;
  scalar = 0;
  if (view == NULL)
    return false;

  bool picked = false;
  if (mousePos)
    picked = PickCell(view, mousePos[0], mousePos[1], pickedPos, &pickedVme);
  else if (trackerPose)
  {
    picked = view->Pick(*trackerPose);
    view->GetPickedPosition(pickedPos);
    pickedVme = picked ? view->GetPickedVme() : NULL;
  }

  if (!picked || pickedVme == NULL)
    return false;

  pointId = GetPointScalar(pickedVme, pickedPos, scalar);
  return true;
}

//----------------------------------------------------------------------------
vtkIdType albaPickHelper::GetPointScalar(albaVME *vme, const double pos[3], double &scalar, bool clampToBounds)
//----------------------------------------------------------------------------
{
  scalar = 0;
  vtkDataSet *data = (vme && vme->GetOutput()) ? vme->GetOutput()->GetVTKData() : NULL;
  if (data == NULL)
    return -1;

  double point[3] = {pos[0], pos[1], pos[2]};
  if (clampToBounds)
  {
    double bounds[6];
    data->GetBounds(bounds);
    for (int i = 0; i < 3; i++)
    {
      if (point[i] < bounds[2 * i]) point[i] = bounds[2 * i];
      if (point[i] > bounds[2 * i + 1]) point[i] = bounds[2 * i + 1];
    }
  }

  vtkIdType pointId = data->FindPoint(point);
  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  if (scalars && pointId >= 0)
    scalar = scalars->GetComponent(pointId, 0);

  return pointId;
}
