/*=========================================================================

 Program: MAF2Medical
 Module: medPipeWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medPipeWrappedMeter_H__
#define __medPipeWrappedMeter_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkTubeFilter;
class medVMEWrappedMeter;
class vtkLookupTable;
class vtkCaptionActor2D;
class mafGUIMaterialButton;

/**
className: medPipeWrappedMeter
Pipe for visualizing wrapped meters. Gives the possibility to change representation just like classical meter pipe.
*/
class MED_VME_EXPORT medPipeWrappedMeter : public mafPipe
{
public:
  /** RTTI macro */
  mafTypeMacro(medPipeWrappedMeter,mafPipe);
  /** constructor */
               medPipeWrappedMeter();
  /** destructor */
  virtual     ~medPipeWrappedMeter ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);
  /** create the pipe */
  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
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
  vtkTubeFilter          *m_Tube;
  vtkLookupTable         *m_Lut;
  vtkPolyDataMapper	     *m_DataMapper;
  vtkActor               *m_DataActor;
  vtkOutlineCornerFilter *m_SelectionBox;
	vtkPolyDataMapper      *m_SelectionMapper;
  vtkProperty            *m_SelectionProperty;
  vtkActor               *m_SelectionActor;

protected:
  medVMEWrappedMeter       *m_WrappedMeterVME;
  mafGUIMaterialButton *m_MaterialButton;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  /** create the gui widget associated to the pipe */
  virtual mafGUI  *CreateGui();
};
#endif // __medPipeWrappedMeter_H__
