/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVTKPickerTool
 Authors: Gianluigi Crimi, from albaInteractorVTKPicker by Josef Kohout

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVTKPickerTool_h
#define __albaVTKPickerTool_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCellPicker;

/** Tool picking the VTK objects of the render window of its renderer.
  CTRL + left button picks: if something is picked it sends albaEvent(this, VME_PICKING, picker) with GetBool false;
  with continuous picking, dragging sends albaEvent(this, VME_PICKING, points) with the new position and GetBool true;
  the button up sends albaEvent(this, VME_PICKED, picker). While the tool uses the button the camera does not move. */
class ALBA_EXPORT albaVTKPickerTool : public albaInteractionTool
{
public:
  albaTypeMacro(albaVTKPickerTool, albaInteractionTool);

  /** Enables/disables continuous picking */
  void SetContinuousPicking(bool value) {m_ContinuousPicking = value;}

  /** Return true if continuous picking is enabled */
  bool GetContinuousPicking() {return m_ContinuousPicking;}

  /** With CTRL pick a VTK object */
  bool OnLeftButtonDown(albaEventInteraction *e) override;

  /** Send the picked object */
  bool OnLeftButtonUp(albaEventInteraction *e) override;

  /** With continuous picking send the new position of the picked point */
  bool OnDrag(albaEventInteraction *e) override;

protected:
  albaVTKPickerTool();
  virtual ~albaVTKPickerTool();

  vtkCellPicker *m_Picker;       ///< picker of the current picking, NULL if nothing has been picked
  vtkRenderer *m_PickRenderer;   ///< renderer of the current picking
  bool m_ContinuousPicking;      ///< true, if events are sent during mouse move

private:
  albaVTKPickerTool(const albaVTKPickerTool&);  // Not implemented.
  void operator=(const albaVTKPickerTool&);  // Not implemented.
};

#endif
