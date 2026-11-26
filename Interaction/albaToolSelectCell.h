/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSelectCell
 Authors: Gianluigi Crimi, from albaInteractorSelectCell by Stefano Perticoni

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolSelectCell_h
#define __albaToolSelectCell_h

#include "albaInteractionTool.h"

/** Tool picking the cells of the render window of its renderer.
  CTRL + left button on a cell starts picking, and keeps picking while dragging; for each picked cell it sends
  albaEvent(this, VME_PICKED) with the picked point (GetVtkObj, a vtkPoints) and the cell id (GetArg).
  Without CTRL, or with CTRL outside the cells, the tool does not use the button: the camera moves.
  @sa albaOpRemoveCells for a usage example */
class ALBA_EXPORT albaToolSelectCell : public albaInteractionTool
{
public:
  albaTypeMacro(albaToolSelectCell, albaInteractionTool);

  /** With CTRL on a cell start picking */
  bool OnLeftButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

  /** End picking */
  bool OnLeftButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

  /** Keep picking while dragging: the camera does not move */
  bool OnDrag(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

protected:
  albaToolSelectCell();
  virtual ~albaToolSelectCell() {}

  /** Pick the cell under the mouse and send it to the listener; return true if a cell was picked */
  bool PickCell(vtkRenderer *ren, double pos[2]);

  bool m_IsPicking; ///< true while CTRL + left button is down

private:
  albaToolSelectCell(const albaToolSelectCell&);  // Not implemented.
  void operator=(const albaToolSelectCell&);  // Not implemented.

  /** test friend */
  friend class albaToolSelectCellTest;
};

#endif
