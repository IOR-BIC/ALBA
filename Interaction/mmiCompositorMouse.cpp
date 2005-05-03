/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiCompositorMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:36 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "wx/wxprec.h" 
#pragma hdrstop

#include "mafDecl.h"
#include "mafInteractionDecl.h"

#include "mmdMouse.h"

#include "mmiCompositorMouse.h"

#include "mflDefines.h"
#include "mflEventInteraction.h"
#include "mflVME.h"

#include "vtkDoubleArray.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkDOFMatrix.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"

using namespace std;

vtkStandardNewMacro(mmiCompositorMouse);

//----------------------------------------------------------------------------
mmiCompositorMouse::mmiCompositorMouse() 
//----------------------------------------------------------------------------
{
  StartButton = -1;
  InteractionFlag = 0;

  ActiveMMIGeneric = NULL;  
  MousePose[0] = MousePose[1] = LastMousePose[0] = LastMousePose[1] = 0;
  CurrentCamera = NULL;
  SetLockDevice(false);
}
//----------------------------------------------------------------------------
mmiCompositorMouse::~mmiCompositorMouse() 
//----------------------------------------------------------------------------
{
	// traverse the map and destroy all the behaviors
	typedef map<int, mmiGenericMouse*> typeActivatorMap;
	typeActivatorMap::iterator iter = ActivatorMap.begin();
	typeActivatorMap::iterator iterEnd = ActivatorMap.end();

  //debug 
  //int nisa = ActivatorMap.size();
  //mmiGenericMouse *opi = ((mmiGenericMouse *)((*iter).second));

	while ( iter != iterEnd )
	{
		vtkDEL((*iter).second);
		iter++;
	}
}

//------------------------------------------------------------------------------
int mmiCompositorMouse::StartInteraction(mmdMouse *mouse)
//------------------------------------------------------------------------------
{
  // fill this object Renderer ivar and get the camera
  SetRenderer(mouse->GetRenderer());
  if (Renderer)
  {
    CurrentCamera = Renderer->GetActiveCamera();
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
  
  ActivatorMap[activator] = pISAG;
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
  return ActivatorMap[activator];
}

//----------------------------------------------------------------------------
void mmiCompositorMouse::OnMiddleButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
	InitInteraction(MOUSE_MIDDLE_PRESSED, e);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnMiddleButtonUp(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnLeftButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
 	InitInteraction(MOUSE_LEFT_PRESSED, e);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnLeftButtonUp(mflEventInteraction *e)
//----------------------------------------------------------------------------
{

  if (ActiveMMIGeneric)
  {
    mmdMouse *mouse = (mmdMouse *)e->GetSender();
    if (ActiveMMIGeneric->StopInteraction(mouse)) // stop interaction
    {
      ActiveMMIGeneric->OnButtonUpAction();
      ActiveMMIGeneric = NULL;
    }
  }

  // stop interacting with the mouse when a button is released
  
  //mmdMouse *mouse = (mmdMouse *)e->GetSender(); 
 
  // Set the Renderer and CurrentCamera ivar
  //StopInteraction(mouse);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnRightButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
 	InitInteraction(MOUSE_RIGHT_PRESSED, e);
}
//----------------------------------------------------------------------------
void mmiCompositorMouse::OnRightButtonUp(mflEventInteraction *e)
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
    vtkErrorMacro("Unknown mouse keyboard combination")
    return -1;
  }
}
   
//----------------------------------------------------------------------------
void mmiCompositorMouse::InitInteraction(int buttonPressed, mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  double pos[2];
  e->Get2DPosition(pos);

  // fill this object MousePos ivar
  LastMousePose[0] = MousePose[0] = (int)pos[0];
  LastMousePose[1] = MousePose[1] = (int)pos[1];

  mmdMouse *mouse = (mmdMouse *)e->GetSender(); 

  // get info from picked vme
  mflVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mflMatrix     *point_pose  = mflMatrix::New();

  point_pose->SetElement(0,3,MousePose[0]);
  point_pose->SetElement(1,3,MousePose[1]);

  FindPokedVme(mouse,point_pose,picked_prop,picked_vme,picked_bh);

  // Set the Renderer and CurrentCamera ivar
  StartInteraction(mouse);

  // check for ctrl and shift pressure
  int shiftPressed = e->GetModifier(MAF_SHIFT_KEY) ? 1 : 0;
  int ctrlPressed = e->GetModifier(MAF_CTRL_KEY) ? 1 : 0;

  // get the activator from key mouse combination:
  int activator = MouseToActivator(buttonPressed, shiftPressed, ctrlPressed);

  // get the behavior from the ActivatorMap
  if (ActivatorMap.count(activator))
  {
    ActiveMMIGeneric = ActivatorMap[activator];
    if (ActiveMMIGeneric != NULL) 
    {
      // ActiveMMIGeneric is the current active mouse behavior
     
      ActiveMMIGeneric->SetRenderer(Renderer);
      ActiveMMIGeneric->SetProp(picked_prop);
      ActiveMMIGeneric->SetCurrentCamera(Renderer->GetActiveCamera());
      if (ActiveMMIGeneric->StartInteraction(mouse)) // force 
      {
        ActiveMMIGeneric->OnButtonDownAction(MousePose[0], MousePose[1]);
      }
    }
  }
  else
  // no behavior is associated with current key combo
  {
    ActiveMMIGeneric = NULL;
  }

  point_pose->Delete();
}


//----------------------------------------------------------------------------
void mmiCompositorMouse::OnButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  // if in multi button mode and already interacting simply forward the event...
  if (ActiveMMIGeneric&&ButtonMode==MULTI_BUTTON_MODE)
  {
    ActiveMMIGeneric->ProcessEvent(e,mafDevice::DeviceInputChannel);
    return;
  }

  // get the button pressed
  ButtonPressed = e->GetButton(); 

  double pos[2];
  e->Get2DPosition(pos);
  LastMousePose[0] = MousePose[0] = (int)pos[0];
  LastMousePose[1] = MousePose[1] = (int)pos[1];

  //mmdMouse *mouse = (mmdMouse *)e->GetSender();
  //StartInteraction(mouse);

  switch(ButtonPressed) 
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
void mmiCompositorMouse::OnButtonUp(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  ButtonPressed = e->GetButton();
  if (CurrentButton>=0&&ButtonPressed!=CurrentButton )
  {
    if (ActiveMMIGeneric&&ButtonMode==MULTI_BUTTON_MODE)
      ActiveMMIGeneric->ProcessEvent(e,mafDevice::DeviceInputChannel);
    return;
  }
  
  switch(ButtonPressed) 
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
void mmiCompositorMouse::ProcessEvent(mflEvent *event,mafID channel)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  unsigned int id=event->GetID();

  if (channel == mafDevice::DeviceInputChannel && InteractionFlag)
  {
    mflEventInteraction *e = (mflEventInteraction *)event;
    mmdMouse *mouse = mmdMouse::SafeDownCast(GetDevice());
    
    // if the event comes from tracker which started the interaction continue...
    // Move2DEvent handling
    if (id == mmdMouse::Move2DEvent && mouse)
    {
      
      if (!CurrentCamera)
        return;

	    double pos2d[2];
      e->Get2DPosition(pos2d);

      // get the mouse pose and forward it to the active interactor 
      MousePose[0] = (int)pos2d[0];
      MousePose[1] = (int)pos2d[1];

      // if the position has not changed discard OnMouseMoveAction
      if (!(MousePose[0] == LastMousePose[0] && MousePose[1] == LastMousePose[1]))
      {
        if (ActiveMMIGeneric) 
        {
          ActiveMMIGeneric->OnMouseMoveAction(MousePose[0], MousePose[1]);
          LastMousePose[0] = MousePose[0];
          LastMousePose[1] = MousePose[1];
        }
      }
      return;
    }
  }
    
  Superclass::ProcessEvent(event,channel);
}
//------------------------------------------------------------------------------