/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: mmiVTKPicker.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-05 11:50:42 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef mmiVTKPicker_h__
#define mmiVTKPicker_h__

#include "mmiCameraMove.h"

class vtkCellPicker;

/** Implements mouse move of camera in the scene or perform continuous
cell picking when using CTRL modifier.

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
class mmiVTKPicker : public mmiCameraMove
{
public:
  mafTypeMacro(mmiVTKPicker,mmiCameraMove);

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
  virtual void OnLeftButtonDown(mafEventInteraction *e);

  /** redefined to end pick modality */
  virtual void OnLeftButtonUp();

protected:
  mmiVTKPicker() {      
    m_ContinuousPicking = false;
    m_Picker = NULL;
  }
  virtual ~mmiVTKPicker();          

protected:
  vtkCellPicker* m_Picker;      //<picker to be used for the picking
  bool m_ContinuousPicking;     //<true, if events are sent during mouse move
};

#endif // mmiVTKPicker_h__