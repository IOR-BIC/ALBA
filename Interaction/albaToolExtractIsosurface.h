/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolExtractIsosurface
 Authors: Gianluigi Crimi, from albaInteractorExtractIsosurface by Paolo Quadrani & Marco Petrone

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolExtractIsosurface_h
#define __albaToolExtractIsosurface_h

#include "albaInteractionTool.h"

/** Tool picking the iso-value of the extract isosurface operation in the render window of its renderer.
  A left click without dragging sends albaEvent(this, VME_PICKED) with the picked point (GetVtkObj, a vtkPoints);
  the drags move the camera. */
class ALBA_EXPORT albaToolExtractIsosurface : public albaInteractionTool
{
public:
  albaTypeMacro(albaToolExtractIsosurface, albaInteractionTool);

  /** Start a click */
  bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) override;

  /** Pick the iso-value if the mouse has not been dragged */
  bool OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) override;

  /** A drag cancels the picking; the camera moves */
  bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]) override;

protected:
  albaToolExtractIsosurface();
  virtual ~albaToolExtractIsosurface() {}

  bool m_PickValue; ///< true if the button up has to pick the iso-value

private:
  albaToolExtractIsosurface(const albaToolExtractIsosurface&);  // Not implemented.
  void operator=(const albaToolExtractIsosurface&);  // Not implemented.

  /** test friend */
  friend class albaToolExtractIsosurfaceTest;
};

#endif
