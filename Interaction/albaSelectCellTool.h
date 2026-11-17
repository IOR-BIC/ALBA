/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSelectCellTool
 Authors: Gianluigi Crimi, from albaInteractorSelectCell by Stefano Perticoni

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaSelectCellTool_h
#define __albaSelectCellTool_h

#include "albaInteractionTool.h"

/** Tool picking the cells of the render window of its renderer.
  CTRL + left button picks a cell, and keeps picking while dragging; for each picked cell it sends
  albaEvent(this, VME_PICKED) with the picked point (GetVtkObj, a vtkPoints) and the cell id (GetArg).
  Without CTRL the tool does not use the button: the camera moves.
  @sa albaOpRemoveCells for a usage example */
class ALBA_EXPORT albaSelectCellTool : public albaInteractionTool
{
public:
  albaTypeMacro(albaSelectCellTool, albaInteractionTool);

  /** With CTRL start picking */
  bool OnLeftButtonDown(albaEventInteraction *e) override;

  /** End picking */
  bool OnLeftButtonUp(albaEventInteraction *e) override;

  /** Keep picking while dragging: the camera does not move */
  bool OnDrag(albaEventInteraction *e) override;

protected:
  albaSelectCellTool();
  virtual ~albaSelectCellTool() {}

  /** Pick the cell under the mouse and send it to the listener */
  void PickCell(albaEventInteraction *e);

  bool m_IsPicking; ///< true while CTRL + left button is down

private:
  albaSelectCellTool(const albaSelectCellTool&);  // Not implemented.
  void operator=(const albaSelectCellTool&);  // Not implemented.

  /** test friend */
  friend class albaSelectCellToolTest;
};

#endif
