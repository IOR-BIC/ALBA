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

#ifndef __albaPickHelper_h
#define __albaPickHelper_h

#include "albaDefines.h"
#include "vtkType.h"

class albaView;
class albaVME;
class albaMatrix;
class vtkRenderWindow;

/** Picking functions shared by the interactors.
  PickCell performs a cell picking at a 2D display position on the renderers of a render window,
  stopping at the first renderer picking something, and returns the picked position together with
  the VME owning the picked prop, so that position and VME always come from the same picking.
  GetPointScalar returns the point of the VME data nearest to a position and its scalar value. */
class ALBA_EXPORT albaPickHelper
{
public:
  /** Pick at the display position (x, y) on the renderers of the view render window
    (compound views use their selected sub view). Return true if something has been picked.
    pickedPos is set to the picked position, or to (0, 0, 0) if nothing has been picked.
    pickedVme (optional) is set to the VME owning the picked prop, NULL if the prop does not belong to a VME.
    cellId (optional) is set to the picked cell id, -1 if nothing has been picked. */
  static bool PickCell(albaView *view, double x, double y, double pickedPos[3], albaVME **pickedVme = NULL, vtkIdType *cellId = NULL);

  /** Same as above, on the renderers of the given render window. */
  static bool PickCell(vtkRenderWindow *renderWindow, double x, double y, double pickedPos[3], albaVME **pickedVme = NULL, vtkIdType *cellId = NULL);

  /** Pick the VME at the display position mousePos, or along the tracker pose trackerPose when mousePos is NULL.
    Return true if a VME has been picked, setting the picked position, the id of the nearest point of the
    VME data and its scalar value (see GetPointScalar). */
  static bool PickPoint(albaView *view, double *mousePos, albaMatrix *trackerPose, double pickedPos[3], albaVME *&pickedVme, vtkIdType &pointId, double &scalar);

  /** Return the id of the point of the VME data nearest to pos (-1 if not found) and set scalar to its
    scalar value (0 if the data has no scalars). If clampToBounds is true pos is first clamped into the
    data bounds, to compensate the approximation of the picked position on the data boundary. */
  static vtkIdType GetPointScalar(albaVME *vme, const double pos[3], double &scalar, bool clampToBounds = false);
};

#endif
