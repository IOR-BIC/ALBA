/*=========================================================================

 Program: MAF2
 Module: mafOpInteractionOp
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpInteractionOp.h"
#include "mafGui.h"

#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"

#include "mafVMEGizmo.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyData.h"

/******************************************************************************/
/** UNCOMMENT HERE TO DECLARE THE ACTIONS               (PASS 1)              */
/* *
//----------------------------------------------------------------------------
const char *mafOpInteractionOp::m_Actions[] = {"MyCustomAction",NULL};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
const char ** mafOpInteractionOp::GetActions()
//----------------------------------------------------------------------------
{
  return m_Actions;
}
/******************************************************************************/


//----------------------------------------------------------------------------
mafOpInteractionOp::mafOpInteractionOp(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
  
}
//----------------------------------------------------------------------------
mafOpInteractionOp::~mafOpInteractionOp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpInteractionOp::Copy()
//----------------------------------------------------------------------------
{
  return new mafOpInteractionOp(m_Label);
}

//----------------------------------------------------------------------------
void mafOpInteractionOp::OpRun()
//----------------------------------------------------------------------------
{
  // CREATE A SPHERE GIZMO
  mafNEW(m_Gizmo);

  vtkMAFSmartPointer<vtkSphereSource> sphere;
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
  mafNEW(m_MouseInteractor); // mouse compositor 
   
	//----------------------------------------------------------------------------
	// translate
	//----------------------------------------------------------------------------
  mafInteractorGenericMouse *mmi_translate = m_MouseInteractor->CreateBehavior(MOUSE_LEFT);
  
  // set to send transform matrix for the gizmo to the operation
  mmi_translate->SetVME(m_Gizmo);
  mmi_translate->SetListener(this);
  mmi_translate->EnableTranslation(true);
  /******************************************************************************/


  /******************************************************************************/
  /** UNCOMMENT HERE TO ATTACH THE INTERACTOR TO THE ACTION    (PASS 3)         */
	/* *
  mafEventMacro( mafEvent( this, INTERACTOR_ADD, m_MouseInteractor, &wxString(m_Actions[0]) ) );
  /******************************************************************************/

  /******************************************************************************/
  /** UNCOMMENT HERE TO ATTACH THE INTERACTOR TO THE VME       (PASS 4)        */
	/* *
  mafVmeData *toolData = (mafVmeData*)m_Gizmo->GetClientData();
  toolData->m_behavior = m_MouseInteractor;
  /******************************************************************************/
  
  
  // the interface
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  m_Gui->OkCancel();
	m_Gui->Label("");
	ShowGui(); // display the GUI
}
//----------------------------------------------------------------------------
void mafOpInteractionOp::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpInteractionOp::OpUndo()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpInteractionOp::OpStop(int result)
{	
  HideGui();
  
  /******************************************************************************/
  /** UNCOMMENT HERE TO DETACH THE INTERACTOR FROM THE ACTION      (PASS 3)     */
	/* *
  mafEventMacro( mafEvent( this, INTERACTOR_REMOVE, m_MouseInteractor, &wxString(m_Actions[0]) ) );
  /******************************************************************************/


  /******************************************************************************/
  /** UNCOMMENT HERE TO DETACH THE INTERACTOR FROM THE VME         (PASS 4)     */
	/* *
  mafVmeData *toolData = (mafVmeData*)m_Gizmo->GetClientData();
  toolData->m_behavior = NULL;
  /******************************************************************************/

  /******************************************************************************/
  /** UNCOMMENT HERE TO DESTROY THE INTERACTOR                     (PASS 2)     */
	/* *
  mafDEL(m_MouseInteractor);
  /******************************************************************************/
  

  // REMOVE VME-GIZMO FROM THE TREE
	GetLogicManager()->VmeRemove(m_Gizmo);
  vtkDEL(m_Gizmo);

  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void mafOpInteractionOp::OnEvent(mafEventBase *e) 
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
    mafEvent *event=mafEvent::SafeDownCast(e);
    assert(event);
    m_Gizmo->ApplyMatrix(*(event->GetMatrix()),POST_MULTIPLY);
		GetLogicManager()->CameraUpdate();
  }    
  break;
	default:
		mafEventMacro(*e); 
  break;
	}
}







