/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiCompositorMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-24 16:43:06 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmiCompositorMouse.h"

#include "mmdMouse.h"
#include "mafDecl.h"
#include "mafEventInteraction.h"
#include "mafVME.h"

#include "vtkDoubleArray.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
//#include "vtkDOFMatrix.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"

using namespace std;

mafCxxTypeMacro(mmiCompositorMouse);

//----------------------------------------------------------------------------
mmiCompositorMouse::mmiCompositorMouse() 
//----------------------------------------------------------------------------
{
  m_StartButton = -1;

  m_ActiveMMIGeneric = NULL;  
  m_MousePose[0] = m_MousePose[1] = m_LastMousePose[0] = m_LastMousePose[1] = 0;
  m_CurrentCamera = NULL;
  SetLockDevice(false);
}
//----------------------------------------------------------------------------
mmiCompositorMouse::~mmiCompositorMouse() 
//----------------------------------------------------------------------------
{
	// traverse the map and destroy all the behaviors
	mmuActivatorMap::iterator iter = m_ActivatorMap.begin();
	mmuActivatorMap::iterator iterEnd = m_ActivatorMap.end();

  //debug 
  //int nisa = m_ActivatorMap.size();
  //mmiGenericMouse *opi = ((mmiGenericMouse *)((*iter).second));

   //modified by Marco. 24-5-2005 : removed, changed into an AutoPointer
	//while ( iter != iterEnd )
	//{
	//	vtkDEL((*iter).second);
	//	iter++;
	//}
}

//------------------------------------------------------------------------------
int mmiCompositorMouse::StartInteraction(mmdMouse *mouse)
//------------------------------------------------------------------------------
{
  // fill this object m_Renderer ivar and get the camera
  SetRenderer(mouse->GetRenderer());
  if (m_Renderer)
  {
    m_CurrentCamera = m_Renderer->GetActiveCamera();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
mmiGenericMouse *mmiCompositorMouse::CreateBehavior(MMI_ACTIVATOR activator)
//----------------------------------------------------------------------------
{
  // create the generic behavior and populate the map
  mmiGenericMouse *pISAG = mmiGenericMouse::New();
  
  m_ActivatorMap[activator] = pISAG;
  switch (activator)
  {    
    case MOUSE_LEFT:
      pISAG->SetStartButton(MAF_LEFT_BUTTON);
      break;
    case MOUSE_LEFT_SHIFT:
      pISAG->SetStartButton(MAF_LEFT_BUTTON);
      pISAG->SetModifiers(MAF_SHIFT_KEY);
      break;
    case MOUSE_LEFT_CONTROL:
      pISAG->SetStartButton(MAF_LEFT_BUTTON);
      pISAG->SetModifiers(MAF_CTRL_KEY);
      break;
    case MOUSE_MIDDLE:
      pISAG->SetStartButton(MAF_MIDDLE_BUTTON);
      break;
    case MOUSE_MIDDLE_SHIFT:
      pISAG->SetStartButton(MAF_MIDDLE_BUTTON);
      pISAG->SetModifiers(MAF_SHIFT_KEY);
      break;
    case MOUSE_MIDDLE_CONTROL:
      pISAG->SetStartButton(MAF_MIDDLE_BUTTON);
      pISAG->SetModifiers(MAF_CTRL_KEY);
      break;
    case MOUSE_RIGHT:
      pISAG->SetStartButton(MAF_RIGHT_BUTTON);
      break;
    case MOUSE_RIGHT_SHIFT:
      pISAG->SetStartButton(MAF_RIGHT_BUTTON);
      pISAG->SetModifiers(MAF_SHIFT_KEY);
      break;
    case MOUSE_RIGHT_CONTROL:
      pISAG->SetStartButton(MAF_RIGHT_BUTTON);
      pISAG->SetModifiers(MAF_CTRL_KEY);
      break;
  }
  
  return pISAG;
}

//----------------------------------------------------------------------------
mmiGenericMouse *mmiCompositorMouse::GetBehavior(MMI_ACTIVATOR activator)
//----------------------------------------------------------------------------
{
  // return pointer to the behavior given the map key
  return m_ActivatorMap[activator];
}

//----------------------------------------------------------------------------
void mmiCompositorMouse::OnMiddleButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
	InitInteraction(MOUSE_MIDDLE_PRESSED, e);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnMiddleButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnLeftButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
 	InitInteraction(MOUSE_LEFT_PRESSED, e);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnLeftButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{

  if (m_ActiveMMIGeneric)
  {
    mmdMouse *mouse = (mmdMouse *)e->GetSender();
    if (m_ActiveMMIGeneric->StopInteraction(mouse)) // stop interaction
    {
      m_ActiveMMIGeneric->OnButtonUpAction();
      m_ActiveMMIGeneric = NULL;
    }
  }

  // stop interacting with the mouse when a button is released
  
  //mmdMouse *mouse = (mmdMouse *)e->GetSender(); 
 
  // Set the m_Renderer and m_CurrentCamera ivar
  //StopInteraction(mouse);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnRightButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
 	InitInteraction(MOUSE_RIGHT_PRESSED, e);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnRightButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonUp(e);
}

//----------------------------------------------------------------------------
int mmiCompositorMouse::MouseToActivator(int mouseButton, int shift, int ctrl)
//----------------------------------------------------------------------------
{
  if (mouseButton == MOUSE_LEFT_PRESSED)
  {
    if (shift == 1)
      return MOUSE_LEFT_SHIFT;
    else if (ctrl == 1)
      return MOUSE_LEFT_CONTROL;
    else
      return MOUSE_LEFT;
  }
  else if (mouseButton == MOUSE_MIDDLE_PRESSED)
  {
    if (shift == 1)
      return MOUSE_MIDDLE_SHIFT;
    else if (ctrl == 1)
      return MOUSE_MIDDLE_CONTROL;
    else
      return MOUSE_MIDDLE;
  }
  else if (mouseButton == MOUSE_RIGHT_PRESSED)
  {
    if (shift == 1)
      return MOUSE_RIGHT_SHIFT;
    else if (ctrl == 1)
      return MOUSE_RIGHT_CONTROL;
    else
      return MOUSE_RIGHT;
  }
  else
  {
    mafErrorMacro("Unknown mouse keyboard combination")
    return -1;
  }
}
   
//----------------------------------------------------------------------------
void mmiCompositorMouse::InitInteraction(int buttonPressed, mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  double pos[2];
  e->Get2DPosition(pos);

  // fill this object MousePos ivar
  m_LastMousePose[0] = m_MousePose[0] = (int)pos[0];
  m_LastMousePose[1] = m_MousePose[1] = (int)pos[1];

  mmdMouse *mouse = (mmdMouse *)e->GetSender(); 

  // get info from picked vme
  mafVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mafMatrix     point_pose;

  point_pose.SetElement(0,3,m_MousePose[0]);
  point_pose.SetElement(1,3,m_MousePose[1]);

  FindPokedVme(mouse,point_pose,picked_prop,picked_vme,picked_bh);

  // Set the m_Renderer and m_CurrentCamera ivar
  StartInteraction(mouse);

  // check for ctrl and shift pressure
  int shiftPressed = e->GetModifier(MAF_SHIFT_KEY) ? 1 : 0;
  int ctrlPressed = e->GetModifier(MAF_CTRL_KEY) ? 1 : 0;

  // get the activator from key mouse combination:
  int activator = MouseToActivator(buttonPressed, shiftPressed, ctrlPressed);

  // get the behavior from the m_ActivatorMap
  if (m_ActivatorMap.count(activator))
  {
    m_ActiveMMIGeneric = m_ActivatorMap[activator];
    if (m_ActiveMMIGeneric != NULL) 
    {
      // m_ActiveMMIGeneric is the current active mouse behavior
     
      m_ActiveMMIGeneric->SetRenderer(m_Renderer);
      m_ActiveMMIGeneric->SetProp(picked_prop);
      m_ActiveMMIGeneric->SetCurrentCamera(m_Renderer->GetActiveCamera());
      if (m_ActiveMMIGeneric->StartInteraction(mouse)) // force 
      {
        m_ActiveMMIGeneric->OnButtonDownAction(m_MousePose[0], m_MousePose[1]);
      }
    }
  }
  else
  // no behavior is associated with current key combo
  {
    m_ActiveMMIGeneric = NULL;
  }
}


//----------------------------------------------------------------------------
void mmiCompositorMouse::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  // if in multi button mode and already interacting simply forward the event...
  if (m_ActiveMMIGeneric && m_ButtonMode==MULTI_BUTTON_MODE)
  {
    m_ActiveMMIGeneric->OnEvent(e);
    return;
  }

  // get the button pressed
  m_ButtonPressed = e->GetButton(); 

  double pos[2];
  e->Get2DPosition(pos);
  m_LastMousePose[0] = m_MousePose[0] = (int)pos[0];
  m_LastMousePose[1] = m_MousePose[1] = (int)pos[1];

  //mmdMouse *mouse = (mmdMouse *)e->GetSender();
  //StartInteraction(mouse);

  switch(m_ButtonPressed) 
  {
    case MAF_LEFT_BUTTON:
      OnLeftButtonDown(e);
  	break;
    case MAF_MIDDLE_BUTTON:
      OnMiddleButtonDown(e);
  	break;
    case MAF_RIGHT_BUTTON:
      OnRightButtonDown(e);
  	break;

  }
}

//----------------------------------------------------------------------------
void mmiCompositorMouse::OnButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  m_ButtonPressed = e->GetButton();
  if ( m_CurrentButton>=0 && m_ButtonPressed!=m_CurrentButton )
  {
    if (m_ActiveMMIGeneric && m_ButtonMode==MULTI_BUTTON_MODE)
      m_ActiveMMIGeneric->OnEvent(e);

    return;
  }
  
  switch(m_ButtonPressed) 
  {
    case MAF_LEFT_BUTTON:
      OnLeftButtonUp(e);
  	break;
    case MAF_MIDDLE_BUTTON:
      OnMiddleButtonUp(e);
  	break;
    case MAF_RIGHT_BUTTON:
      OnRightButtonUp(e);
  	break;
  }
}

//------------------------------------------------------------------------------
void mmiCompositorMouse::OnEvent(mafEvent *event)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  mafID id=event->GetId();
  mafID channel=event->GetChannel();

  if (channel == MCH_INPUT && m_InteractionFlag)
  {
    mafEventInteraction *e = (mafEventInteraction *)event;
    mmdMouse *mouse = mmdMouse::SafeDownCast(GetDevice());
    
    // if the event comes from tracker which started the interaction continue...
    // Move2DEvent handling
    if (id == mmdMouse::MOUSE_2D_MOVE && mouse)
    {
      
      if (!m_CurrentCamera)
        return;

	    double pos2d[2];
      e->Get2DPosition(pos2d);

      // get the mouse pose and forward it to the active interactor 
      m_MousePose[0] = (int)pos2d[0];
      m_MousePose[1] = (int)pos2d[1];

      // if the position has not changed discard OnMouseMoveAction
      if (!(m_MousePose[0] == m_LastMousePose[0] && m_MousePose[1] == m_LastMousePose[1]))
      {
        if (m_ActiveMMIGeneric) 
        {
          m_ActiveMMIGeneric->OnMouseMoveAction(m_MousePose[0], m_MousePose[1]);
          m_LastMousePose[0] = m_MousePose[0];
          m_LastMousePose[1] = m_MousePose[1];
        }
      }
      return;
    }
  }
    
  Superclass::OnEvent(event);
}
//------------------------------------------------------------------------------