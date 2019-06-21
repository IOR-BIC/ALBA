/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeComputeWrapping_H__
#define __albaPipeComputeWrapping_H__

#include "albaPipe.h"
#include "albaDefines.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkALBATubeFilter;
class albaVMEComputeWrapping;
class vtkLookupTable;
class vtkCaptionActor2D;
class albaGUIMaterialButton;

//----------------------------------------------------------------------------
// albaPipeComputeWrapping :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeComputeWrapping : public albaPipe
{
public:
  albaTypeMacro(albaPipeComputeWrapping,albaPipe);

               albaPipeComputeWrapping();
  virtual     ~albaPipeComputeWrapping ();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  virtual void Create(albaSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
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
  albaVMEComputeWrapping       *m_WrappedMeterVME;
  albaGUIMaterialButton *m_MaterialButton;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  virtual albaGUI  *CreateGui();
};
#endif // __albaPipeComputeWrapping_H__
