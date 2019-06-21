/*=========================================================================
Program:   AlbaMaster
Module:    albaOpInteractionOp.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in ALBA must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpInteractionOp.h"
#include "albaGui.h"

#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

#include "albaVMEGizmo.h"

#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyData.h"

/******************************************************************************/
/** UNCOMMENT HERE TO DECLARE THE ACTIONS               (PASS 1)              */
/* *
//----------------------------------------------------------------------------
const char *albaOpInteractionOp::m_Actions[] = {"MyCustomAction",NULL};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
const char ** albaOpInteractionOp::GetActions()
//----------------------------------------------------------------------------
{
  return m_Actions;
}
/******************************************************************************/


//----------------------------------------------------------------------------
albaOpInteractionOp::albaOpInteractionOp(wxString label)
: albaOp(label)
//----------------------------------------------------------------------------
{
  
}
//----------------------------------------------------------------------------
albaOpInteractionOp::~albaOpInteractionOp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpInteractionOp::Copy()
//----------------------------------------------------------------------------
{
  return new albaOpInteractionOp(m_Label);
}

//----------------------------------------------------------------------------
void albaOpInteractionOp::OpRun()
//----------------------------------------------------------------------------
{
  // CREATE A SPHERE GIZMO
  albaNEW(m_Gizmo);

  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(10);
  m_Gizmo->SetData(sphere->GetOutput());
  
  // add the VME Gizmo to the VME tree
  GetLogicManager()->VmeAdd(m_Gizmo);



  /******************************************************************************/
  /** UNCOMMENT HERE TO CREATE INTERACTORS TO MOVE THE GIZMO  WITH MOUSE (PASS 2)*/
  /* *
  //----------------------------------------------------------------------------
	// compositor
	//----------------------------------------------------------------------------
  albaNEW(m_MouseInteractor); // mouse compositor 
   
	//----------------------------------------------------------------------------
	// translate
	//----------------------------------------------------------------------------
  albaInteractorGenericMouse *mmi_translate = m_MouseInteractor->CreateBehavior(MOUSE_LEFT);
  
  // set to send transform matrix for the gizmo to the operation
  mmi_translate->SetVME(m_Gizmo);
  mmi_translate->SetListener(this);
  mmi_translate->EnableTranslation(true);
  /******************************************************************************/


  /******************************************************************************/
  /** UNCOMMENT HERE TO ATTACH THE INTERACTOR TO THE ACTION    (PASS 3)         */
	/* *
  albaEventMacro( albaEvent( this, INTERACTOR_ADD, m_MouseInteractor, &wxString(m_Actions[0]) ) );
  /******************************************************************************/

  /******************************************************************************/
  /** UNCOMMENT HERE TO ATTACH THE INTERACTOR TO THE VME       (PASS 4)        */
	/* *
  albaVmeData *toolData = (albaVmeData*)m_Gizmo->GetClientData();
  toolData->m_behavior = m_MouseInteractor;
  /******************************************************************************/
  
  
  // the interface
  m_Gui = new albaGUI(this);
  m_Gui->SetListener(this);
  m_Gui->OkCancel();
	m_Gui->Label("");
	ShowGui(); // display the GUI
}
//----------------------------------------------------------------------------
void albaOpInteractionOp::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpInteractionOp::OpUndo()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOpInteractionOp::OpStop(int result)
{	
  HideGui();
  
  /******************************************************************************/
  /** UNCOMMENT HERE TO DETACH THE INTERACTOR FROM THE ACTION      (PASS 3)     */
	/* *
  albaEventMacro( albaEvent( this, INTERACTOR_REMOVE, m_MouseInteractor, &wxString(m_Actions[0]) ) );
  /******************************************************************************/


  /******************************************************************************/
  /** UNCOMMENT HERE TO DETACH THE INTERACTOR FROM THE VME         (PASS 4)     */
	/* *
  albaVmeData *toolData = (albaVmeData*)m_Gizmo->GetClientData();
  toolData->m_behavior = NULL;
  /******************************************************************************/

  /******************************************************************************/
  /** UNCOMMENT HERE TO DESTROY THE INTERACTOR                     (PASS 2)     */
	/* *
  albaDEL(m_MouseInteractor);
  /******************************************************************************/
  

  // REMOVE VME-GIZMO FROM THE TREE
	GetLogicManager()->VmeRemove(m_Gizmo);
  vtkDEL(m_Gizmo);

  albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
void albaOpInteractionOp::OnEvent(albaEventBase *e) 
//----------------------------------------------------------------------------
{ 

  switch(e->GetId())
	{
	case wxOK:
		OpStop(OP_RUN_OK);
	break;
	case wxCANCEL:
		OpStop(OP_RUN_CANCEL);
  break;
  case ID_TRANSFORM:
  {
    albaEvent *event=albaEvent::SafeDownCast(e);
    assert(event);
    m_Gizmo->ApplyMatrix(*(event->GetMatrix()),POST_MULTIPLY);
		GetLogicManager()->CameraUpdate();
  }    
  break;
	default:
		albaEventMacro(*e); 
  break;
	}
}







