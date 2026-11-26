/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolDICOMImporter
 Authors: Gianluigi Crimi, from albaInteractorDICOMImporter by Paolo Quadrani, Stefano Perticoni

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolDICOMImporter_h
#define __albaToolDICOMImporter_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPlaneSource;
class vtkActor;
class albaRWIBase;

/** Tool editing the crop rectangle of the DICOM importer in the render window of its renderer.
  The rectangle is visible while the tool is pushed: the left button draws a new rectangle or drags its sides
  and corners, and moving the mouse updates the cursor; the other drags move the camera. */
class ALBA_EXPORT albaToolDICOMImporter : public albaInteractionTool
{
public:
  albaTypeMacro(albaToolDICOMImporter, albaInteractionTool);

  /** Update the cursor on the sides of the crop rectangle */
  bool OnMoveImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

  /** Start drawing or dragging the crop rectangle */
  bool OnLeftButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

  /** Draw or drag the crop rectangle */
  bool OnDragImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

  /** End drawing or dragging the crop rectangle */
  bool OnLeftButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

  /** Redefined to add the crop rectangle to the renderer */
  virtual void SetRendererAndView(vtkRenderer *ren, albaView *view);

  /** Show the crop rectangle */
  void OnPush() override;

  /** Hide the crop rectangle */
  void OnPop() override;

  void GetPlaneBounds(double *bounds);
  void SetSliceBounds(double *bounds);

protected:
  albaToolDICOMImporter();
  virtual ~albaToolDICOMImporter();

  /** Find the side or corner of the crop rectangle at the given world position */
  void CalculateSideDragged(double *pos);

  /** Move the side or corner being dragged, or resize the new rectangle, to the given display position */
  void MoveCropPlane(double x, double y);

  /** Set the cursor of the render window for the side or corner under the mouse */
  void UpdateCursor(albaRWIBase *rwi);

  void SetPlaneFromSliceSize();

  vtkPlaneSource *m_CropPlane;
  vtkActor *m_CropActor;
  int m_GizmoStatus;
  int m_SideToBeDragged;
  double m_SliceSize[2];
  double m_Delta;
  wxStockCursor m_CurrentArrow;

private:
  albaToolDICOMImporter(const albaToolDICOMImporter&);  // Not implemented.
  void operator=(const albaToolDICOMImporter&);  // Not implemented.
};

#endif
