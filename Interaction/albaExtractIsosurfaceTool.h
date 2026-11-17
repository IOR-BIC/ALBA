/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaExtractIsosurfaceTool
 Authors: Gianluigi Crimi, from albaInteractorExtractIsosurface by Paolo Quadrani & Marco Petrone

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaExtractIsosurfaceTool_h
#define __albaExtractIsosurfaceTool_h

#include "albaInteractionTool.h"

/** Tool picking the iso-value of the extract isosurface operation in the render window of its renderer.
  A left click without dragging sends albaEvent(this, VME_PICKED) with the picked point (GetVtkObj, a vtkPoints);
  the drags move the camera. */
class ALBA_EXPORT albaExtractIsosurfaceTool : public albaInteractionTool
{
public:
  albaTypeMacro(albaExtractIsosurfaceTool, albaInteractionTool);

  /** Start a click */
  bool OnLeftButtonDown(albaEventInteraction *e) override;

  /** Pick the iso-value if the mouse has not been dragged */
  bool OnLeftButtonUp(albaEventInteraction *e) override;

  /** A drag cancels the picking; the camera moves */
  bool OnDrag(albaEventInteraction *e) override;

protected:
  albaExtractIsosurfaceTool();
  virtual ~albaExtractIsosurfaceTool() {}

  bool m_PickValue; ///< true if the button up has to pick the iso-value

private:
  albaExtractIsosurfaceTool(const albaExtractIsosurfaceTool&);  // Not implemented.
  void operator=(const albaExtractIsosurfaceTool&);  // Not implemented.

  /** test friend */
  friend class albaExtractIsosurfaceToolTest;
};

#endif
