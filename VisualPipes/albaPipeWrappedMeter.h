/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeWrappedMeter_H__
#define __albaPipeWrappedMeter_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkALBATubeFilter;
class albaVMEWrappedMeter;
class vtkLookupTable;
class vtkCaptionActor2D;
class albaGUIMaterialButton;

/**
className: albaPipeWrappedMeter
Pipe for visualizing wrapped meters. Gives the possibility to change representation just like classical meter pipe.
*/
class ALBA_EXPORT albaPipeWrappedMeter : public albaPipe
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeWrappedMeter,albaPipe);
  /** constructor */
               albaPipeWrappedMeter();
  /** destructor */
  virtual     ~albaPipeWrappedMeter ();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);
  /** create the pipe */
  virtual void Create(albaSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  /** called when the associated vme has been selected */
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_METER_WIDGET_ID
  {
    ID_SHOW_LABEL = Superclass::ID_LAST,
    ID_COLOR_MODE,
    ID_DISTANCE_RANGE,
    ID_METER_REPRESENTATION,
    ID_TUBE_RADIUS,
    ID_TUBE_CAPPING,
    ID_METER_MEASURE_TYPE,
    ID_INIT_MEASURE,
    ID_GENERATE_EVENT,
    ID_DELTA_PERCENT,
    ID_LAST
  };

  vtkCaptionActor2D      *m_Caption;
	vtkALBATubeFilter       *m_Tube;
  vtkLookupTable         *m_Lut;
  vtkPolyDataMapper	     *m_DataMapper;
  vtkActor               *m_DataActor;
  vtkOutlineCornerFilter *m_SelectionBox;
	vtkPolyDataMapper      *m_SelectionMapper;
  vtkProperty            *m_SelectionProperty;
  vtkActor               *m_SelectionActor;

protected:
  albaVMEWrappedMeter       *m_WrappedMeterVME;
  albaGUIMaterialButton *m_MaterialButton;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  /** create the gui widget associated to the pipe */
  virtual albaGUI  *CreateGui();
};
#endif // __albaPipeWrappedMeter_H__
