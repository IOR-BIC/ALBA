/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAModificationTracker
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAModificationTracker_h
#define __vtkALBAModificationTracker_h

#include "albaConfigure.h"
#include <string>


class vtkDataObject;
class vtkDataSet;
class vtkFieldData;
class vtkImageData;
class vtkObject;
class vtkPointSet;
class vtkPolyData;
class vtkStructuredGrid;
class vtkUnstructuredGrid;

/**
 * Debugging utility that logs every modification of a VTK data object and of
 * its sub-objects (points, cell arrays, point/cell/field data and every array
 * they contain), together with the call stack of the code that triggered it.
 *
 * Supported data objects: vtkUnstructuredGrid, vtkStructuredGrid,
 * vtkImageData and vtkPolyData. Any other vtkObject can be observed with
 * WatchObject().
 *
 * Observers are attached to the objects that exist when the Watch method is
 * called. If an array or the points are replaced afterwards, call the Watch
 * method again. Calling it twice on unchanged objects logs each modification
 * twice.
 */
class ALBA_EXPORT vtkALBAModificationTracker
{
public:
  /**
   * Dispatches on the runtime type of the data object.
   * Returns false if the pointer is null or the type is not supported.
   */
  static bool Watch(vtkDataObject *dataObject, const std::string &label = "data object");

  static void WatchUnstructuredGrid(vtkUnstructuredGrid *grid,
    const std::string &label = "unstructured grid");
  static void WatchStructuredGrid(vtkStructuredGrid *grid,
    const std::string &label = "structured grid");
  static void WatchImageData(vtkImageData *image, const std::string &label = "image data");
  static void WatchPolyData(vtkPolyData *polyData, const std::string &label = "poly data");

  /**
   * Observes the modifications of a single VTK object.
   */
  static void WatchObject(vtkObject *object, const std::string &label);

private:
  class Observer;

  static void WatchDataSet(vtkDataSet *dataSet, const std::string &label);
  static void WatchPointSetPoints(vtkPointSet *pointSet, const std::string &label);
  static void WatchFieldData(vtkFieldData *fieldData, const std::string &label);

  vtkALBAModificationTracker() = delete;
  vtkALBAModificationTracker(const vtkALBAModificationTracker &) = delete;
  void operator=(const vtkALBAModificationTracker &) = delete;
};

#endif