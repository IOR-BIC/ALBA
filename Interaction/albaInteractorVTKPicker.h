/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaInteractorVTKPicker.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 08:39:52 $ 
  Version: $Revision: 1.1.2.5 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef albaInteractorVTKPicker_h__
#define albaInteractorVTKPicker_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaInteractorCameraMove.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class vtkCellPicker;

/** 
class name: albaInteractorVTKPicker
Implements mouse move of camera in the scene or perform continuous
cell picking when using CTRL modifier.
Unlike albaInteractorSelectPoint and albaInteractorSelectCell, it sends the
vtkCellPicker itself to the listener and supports dragging the picked point.

If CTRL modifier is pressed a pick is performed, which according to the
current context leads to various events sent to the listener

1) CRTL + left mouse button down
If any VTK object is picked using vtkCellPicker, then VME_PICKING event is 
issued with the following parameters:
Bool = false
VtkObj = reference to vtkCellPicker that performed the picking

2) left mouse button up
If any VTK object was picked during CTRL + left mouse down, then 
VME_PICKED event is issued with parameters:
VtkObj = reference to original vtkCellPicker

3) mouse move
If ContinuousPicking is enabled (by default it is disabled) and
any VTK object was picked during CTRL + left mouse down, 
then VME_PICKING event is issued with the following parameters:
Bool = true
VtkObj = reference to vtkPoints containing the new position of
point picked during CTRL + left mouse down  
*/
class ALBA_EXPORT albaInteractorVTKPicker : public albaInteractorCameraMove
{
public:
  /** RTTI macro */
  albaTypeMacro(albaInteractorVTKPicker,albaInteractorCameraMove);

  /** Enables/disables continuous picking */
  inline void SetContinuousPicking(bool bValue) {
    m_ContinuousPicking = bValue;
  }

  /** Enables/disables continuous picking */
  inline bool GetContinuousPicking() {
    return m_ContinuousPicking;
  }

  /** redefined not to move the camera if something has been picked */
  virtual void OnMouseMove();

  /** redefined to pick cell if CTRL modifier is pressed */
  virtual void OnLeftButtonDown(albaEventInteraction *e);

  /** redefined to end pick modality */
  virtual void OnLeftButtonUp();

protected:
  /** constructor */
  albaInteractorVTKPicker() {      
    m_ContinuousPicking = false;
    m_Picker = NULL;
  }
  /** destructor */
  virtual ~albaInteractorVTKPicker();          

protected:
  vtkCellPicker* m_Picker;      //<picker to be used for the picking
  bool m_ContinuousPicking;     //<true, if events are sent during mouse move
};

#endif // albaInteractorVTKPicker_h__