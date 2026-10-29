#/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAModificationTracker
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAModificationTracker.h"

#include "albaDecl.h"

#include <vtkAbstractArray.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkDataObject.h>
#include <vtkDataSet.h>
#include <vtkFieldData.h>
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkObject.h>
#include <vtkPointData.h>
#include <vtkPointSet.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnsignedCharArray.h>
#include <vtksys/SystemInformation.hxx>

#include <sstream>

//------------------------------------------------------------------------------
class vtkALBAModificationTracker::Observer : public vtkCommand
{
public:
  static Observer *New()
  {
    return new Observer;
  }

  void Execute(vtkObject *caller, unsigned long eventId, void *) override
  {
    if (eventId != vtkCommand::ModifiedEvent)
    {
      return;
    }

    std::stringstream str;
    str << "[Modified] " << this->Label << " (" << caller->GetClassName()
        << " @ " << caller << ") mtime=" << caller->GetMTime() << "\n"
        << vtksys::SystemInformation::GetProgramStack(2, 0) << "\n";

    albaLogMessage("%s", str.str().c_str());
  }

  std::string Label;

protected:
  Observer() = default;
};

//------------------------------------------------------------------------------
void vtkALBAModificationTracker::WatchObject(vtkObject *object, const std::string &label)
{
  if (object == nullptr)
  {
    return;
  }

  vtkNew<Observer> observer;
  observer->Label = label;
  object->AddObserver(vtkCommand::ModifiedEvent, observer);
}

//------------------------------------------------------------------------------
void vtkALBAModificationTracker::WatchFieldData(vtkFieldData *fieldData, const std::string &label)
{
  if (fieldData == nullptr)
  {
    return;
  }

  WatchObject(fieldData, label);

  for (int i = 0; i < fieldData->GetNumberOfArrays(); ++i)
  {
    vtkAbstractArray *array = fieldData->GetAbstractArray(i);
    if (array != nullptr)
    {
      const char *name = array->GetName();
      WatchObject(array, label + " array '" + (name ? name : "<unnamed>") + "'");
    }
  }
}

//------------------------------------------------------------------------------
void vtkALBAModificationTracker::WatchPointSetPoints(vtkPointSet *pointSet, const std::string &label)
{
  vtkPoints *points = pointSet->GetPoints();
  if (points == nullptr)
  {
    return;
  }

  WatchObject(points, label + " points");
  WatchObject(points->GetData(), label + " points array");
}

//------------------------------------------------------------------------------
void vtkALBAModificationTracker::WatchDataSet(vtkDataSet *dataSet, const std::string &label)
{
  WatchObject(dataSet, label);
  WatchFieldData(dataSet->GetPointData(), label + " point data");
  WatchFieldData(dataSet->GetCellData(), label + " cell data");
  WatchFieldData(dataSet->GetFieldData(), label + " field data");
}

//------------------------------------------------------------------------------
void vtkALBAModificationTracker::WatchUnstructuredGrid(vtkUnstructuredGrid *grid,
                                                       const std::string &label)
{
  if (grid == nullptr)
  {
    return;
  }

  WatchDataSet(grid, label);
  WatchPointSetPoints(grid, label);
  WatchObject(grid->GetCells(), label + " cell connectivity");
  WatchObject(grid->GetCellTypesArray(), label + " cell types array");
}

//------------------------------------------------------------------------------
void vtkALBAModificationTracker::WatchStructuredGrid(vtkStructuredGrid *grid,
                                                     const std::string &label)
{
  if (grid == nullptr)
  {
    return;
  }

  WatchDataSet(grid, label);
  WatchPointSetPoints(grid, label);
}

//------------------------------------------------------------------------------
void vtkALBAModificationTracker::WatchImageData(vtkImageData *image, const std::string &label)
{
  if (image == nullptr)
  {
    return;
  }

  WatchDataSet(image, label);
}

//------------------------------------------------------------------------------
void vtkALBAModificationTracker::WatchPolyData(vtkPolyData *polyData, const std::string &label)
{
  if (polyData == nullptr)
  {
    return;
  }

  WatchDataSet(polyData, label);
  WatchPointSetPoints(polyData, label);
  WatchObject(polyData->GetVerts(), label + " vertices");
  WatchObject(polyData->GetLines(), label + " lines");
  WatchObject(polyData->GetPolys(), label + " polygons");
  WatchObject(polyData->GetStrips(), label + " triangle strips");
}

//------------------------------------------------------------------------------
bool vtkALBAModificationTracker::Watch(vtkDataObject *dataObject, const std::string &label)
{
  if (dataObject == nullptr)
  {
    return false;
  }

  if (vtkUnstructuredGrid *unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(dataObject))
  {
    WatchUnstructuredGrid(unstructuredGrid, label);
    return true;
  }
  if (vtkStructuredGrid *structuredGrid = vtkStructuredGrid::SafeDownCast(dataObject))
  {
    WatchStructuredGrid(structuredGrid, label);
    return true;
  }
  if (vtkImageData *imageData = vtkImageData::SafeDownCast(dataObject))
  {
    WatchImageData(imageData, label);
    return true;
  }
  if (vtkPolyData *polyData = vtkPolyData::SafeDownCast(dataObject))
  {
    WatchPolyData(polyData, label);
    return true;
  }

  albaLogMessage("[ModificationTracker] Unsupported data object type: %s", dataObject->GetClassName());
  return false;
}