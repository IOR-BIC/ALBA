/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeWrappedMeter.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:19:42 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeWrappedMeter_H__
#define __medPipeWrappedMeter_H__

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

//----------------------------------------------------------------------------
// medPipeWrappedMeter :
//----------------------------------------------------------------------------
class medPipeWrappedMeter : public mafPipe
{
public:
  mafTypeMacro(medPipeWrappedMeter,mafPipe);

               medPipeWrappedMeter();
  virtual     ~medPipeWrappedMeter ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
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
  virtual mafGUI  *CreateGui();
};
#endif // __medPipeWrappedMeter_H__
