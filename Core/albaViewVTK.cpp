/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewVTK
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaViewVTK.h"
#include <wx/dc.h>

#include "albaIndent.h"

#include "albaGUI.h"
#include "albaPipe.h"
#include "albaPipeFactory.h"
#include "albaLightKit.h"
#include "albaAttachCamera.h"
#include "albaTextKit.h"
#include "albaAnimate.h"

#include "albaInteractor.h"
#include "albaAvatar.h"
#include "albaAvatar3D.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"

#include "albaVME.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMERoot.h"

#include "vtkALBASmartPointer.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkALBARayCast3DPicker.h"
#include "vtkCellPicker.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewVTK::albaViewVTK(const wxString &label, int camera_position, bool show_axes, bool show_grid,  int stereo, bool show_orientator, int axesType)
:albaView(label)
{
  m_CameraPositionId= camera_position;
  m_ShowAxes        = show_axes;
  m_ShowGrid        = show_grid;
  m_StereoType      = stereo;
  m_ShowOrientator  = show_orientator;
  m_AxesType = axesType;

  m_Sg        = NULL;
  m_Rwi       = NULL;
  m_LightKit  = NULL;
  m_TextKit   = NULL;
  m_AttachCamera = NULL;
  m_AnimateKit = NULL;
}
//----------------------------------------------------------------------------
albaViewVTK::~albaViewVTK() 
{
  m_PipeMap.clear();

  vtkDEL(m_Picker2D);
  vtkDEL(m_Picker3D);
  cppDEL(m_AttachCamera);
  cppDEL(m_LightKit);
  cppDEL(m_TextKit);
  cppDEL(m_AnimateKit);
  cppDEL(m_Sg);
  cppDEL(m_Rwi);
}

//----------------------------------------------------------------------------
void albaViewVTK::PlugVisualPipe(albaString vme_type, albaString pipe_type, long visibility)
{
  albaVisualPipeInfo plugged_pipe;
  plugged_pipe.m_PipeName=pipe_type;
  plugged_pipe.m_Visibility=visibility;
  m_PipeMap[vme_type] = plugged_pipe;
}

//----------------------------------------------------------------------------
albaView *albaViewVTK::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewVTK *v = new albaViewVTK(m_Label, m_CameraPositionId, m_ShowAxes, m_ShowGrid, m_StereoType, m_ShowOrientator, m_AxesType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaViewVTK::Create()
{
  if(m_LightCopyEnabled == true) return;
  m_Rwi = new albaRWI(albaGetFrame(), ONE_LAYER, m_ShowGrid, m_ShowAxes, m_StereoType, m_ShowOrientator, m_AxesType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new albaSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack,m_Rwi->m_AlwaysVisibleRenderer);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

	m_BackgroundColor = m_Rwi->GetBackgroundColor();

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.001);
  //m_Picker2D->InitializePickList();
}

//----------------------------------------------------------------------------
void albaViewVTK::SetMouse(albaDeviceButtonsPadMouse *mouse)
{
  assert(m_Rwi->m_RwiBase);
  m_Rwi->m_RwiBase->SetMouse(mouse);
}

//----------------------------------------------------------------------------
vtkRenderWindow *albaViewVTK::GetRenderWindow()
{ 
  return m_Rwi->m_RenderWindow;
}

//----------------------------------------------------------------------------
vtkRenderer *albaViewVTK::GetFrontRenderer()
{
  return m_Rwi->m_RenFront;
}
//----------------------------------------------------------------------------
vtkRenderer *albaViewVTK::GetBackRenderer()
{
  return m_Rwi->m_RenBack;
}

//----------------------------------------------------------------------------
void albaViewVTK::VmeAdd(albaVME *vme)
{
  assert(m_Sg); 
  m_Sg->VmeAdd(vme);
  if (m_AnimateKit && vme->IsALBAType(albaVMERoot))
  {
    m_AnimateKit->SetInputVME(vme);
  }
}
//----------------------------------------------------------------------------
void albaViewVTK::VmeShow(albaVME *vme, bool show)												{assert(m_Sg); m_Sg->VmeShow(vme,show);}
void albaViewVTK::VmeUpdateProperty(albaVME *vme, bool fromTag)	        {assert(m_Sg); m_Sg->VmeUpdateProperty(vme,fromTag);}

//----------------------------------------------------------------------------
int  albaViewVTK::GetNodeStatus(albaVME *vme)
{
  int status = m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;

	albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
	if (lm)
	{
		albaVMELandmarkCloud *lmc=albaVMELandmarkCloud::SafeDownCast(lm->GetParent());
		if (lmc)
		{
			if ( (m_Sg->GetNodeStatus(lmc) == NODE_VISIBLE_ON) && lmc->IsLandmarkShow(lm))
				return NODE_VISIBLE_ON;
		}
	}

  albaString vme_type = vme->GetTypeName();
	if (m_PipeMap.count(vme_type))
	{
		if (m_PipeMap[vme_type].m_Visibility == NON_VISIBLE)
		{
			status = NODE_NON_VISIBLE;
		}
		else if (m_PipeMap[vme_type].m_Visibility == MUTEX)
		{
			albaSceneNode *n = m_Sg->Vme2Node(vme);
			if (n != NULL)
			{
				n->SetMutex(true);
			}
			status = m_Sg->GetNodeStatus(vme);
		}
  }
  return status;
}
//----------------------------------------------------------------------------
void albaViewVTK::VmeRemove(albaVME *vme)
{
  assert(m_Sg); 
  m_Sg->VmeRemove(vme);
  if (m_AnimateKit && vme->IsALBAType(albaVMERoot))
  {
    m_AnimateKit->ResetKit();
  }
}
//----------------------------------------------------------------------------
void albaViewVTK::VmeSelect(albaVME *vme, bool select)
{
  assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);
}
//----------------------------------------------------------------------------
void albaViewVTK::CameraSet(int camera_position) 
{
  assert(m_Rwi);
  m_CameraPositionId = camera_position; 
  m_Rwi->CameraSet(camera_position);
}
//----------------------------------------------------------------------------
void albaViewVTK::CameraReset(albaVME *vme)
{
  assert(m_Rwi); 
  m_Rwi->CameraReset(vme);
}
//----------------------------------------------------------------------------
void albaViewVTK::CameraUpdate() 
{
  assert(m_Rwi); 
  m_Rwi->CameraUpdate();
}
//----------------------------------------------------------------------------
albaPipe* albaViewVTK::GetNodePipe(albaVME *vme)
{
   assert(m_Sg);
   albaSceneNode *n = m_Sg->Vme2Node(vme);
   if(!n) return NULL;
   return n->GetPipe();
}
//----------------------------------------------------------------------------
void albaViewVTK::GetVisualPipeName(albaVME *node, albaString &pipe_name)
{
  assert(node);

  node->Modified();
  vtkDataSet *data = node->GetOutput()->GetVTKData();
  albaVMELandmarkCloud *lmc = albaVMELandmarkCloud::SafeDownCast(node);
  albaVMELandmark *lm = albaVMELandmark::SafeDownCast(node);
  if (lmc == NULL && data == NULL && lm == NULL)
  {
    pipe_name = "albaPipeBox";
  }
  else
  {
    // custom visualization for the view should be considered only
    // if we are not in editing mode.
    albaString vme_type = node->GetTypeName();
    if (m_PipeMap.count(vme_type))
    {
      // pick up the visual pipe from the view's visual pipe map
      pipe_name = m_PipeMap[vme_type].m_PipeName;
    }
  }

  if(pipe_name.IsEmpty())
  {
    // pick up the default visual pipe from the vme
    pipe_name = node->GetVisualPipe();
  }
}
//----------------------------------------------------------------------------
void albaViewVTK::VmeCreatePipe(albaVME *vme)
{
  albaString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;
    albaPipeFactory *pipe_factory  = albaPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    albaObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    albaPipe *pipe = (albaPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      albaSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->GetPipe());
      pipe->Create(n);
    }
    else
    {
      albaErrorMessage(_("Cannot create visual pipe object of type \"%s\"!"),pipe_name.GetCStr());
    }
  }
}
//----------------------------------------------------------------------------
void albaViewVTK::VmeDeletePipe(albaVME *vme)
{
  m_NumberOfVisibleVme--;
  albaSceneNode *n = m_Sg->Vme2Node(vme);
	n->DeletePipe();
}
//-------------------------------------------------------------------------
albaGUI *albaViewVTK::CreateGui()
{
  assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

  m_Gui->AddGui(m_Rwi->GetGui());
  
  /////////////////////////////////////////Attach Camera GUI
  m_AttachCamera = new albaAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->RollOut(ID_ROLLOUT_ATTACH_CAMERA, " Attach camera", m_AttachCamera->GetGui(), false);

  /////////////////////////////////////////Text GUI
  m_TextKit = new albaTextKit(m_Gui, m_Rwi->m_RenFront, this);
  m_Gui->RollOut(ID_ROLLOUT_TEXT_KIT, " Text kit", m_TextKit->GetGui(), false);

  /////////////////////////////////////////Light GUI
  m_LightKit = new albaLightKit(m_Gui, m_Rwi->m_RenFront, this);
  m_Gui->RollOut(ID_ROLLOUT_LIGHT_KIT, " Light kit", m_LightKit->GetGui(), false);
  
  // Animate kit
  m_AnimateKit = new albaAnimate(m_Rwi->m_RenFront,m_Sg->GetSelectedVme()->GetRoot(),this);
  m_Gui->RollOut(ID_ROLLOUT_ANIMATE_KIT, " Animate kit", m_AnimateKit->GetGui(), false);

  m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewVTK::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
      case CAMERA_PRE_RESET:
        OnPreResetCamera();
        albaEventMacro(*alba_event);
      break;
      case CAMERA_POST_RESET:
        OnPostResetCamera();
        albaEventMacro(*alba_event);
      break;
      case ID_ROLLOUT_ATTACH_CAMERA:
      case ID_ROLLOUT_TEXT_KIT:
      case ID_ROLLOUT_LIGHT_KIT:
      case ID_ROLLOUT_ANIMATE_KIT:
      break;
      default:
        Superclass::OnEvent(alba_event);
      break;
    }
  }
  else
  {
    //albaEventMacro(*alba_event);
		Superclass::OnEvent(alba_event);
  }
}
//----------------------------------------------------------------------------
void albaViewVTK::OnPreResetCamera()
{
}
//----------------------------------------------------------------------------
void albaViewVTK::OnPostResetCamera()
{
}
//----------------------------------------------------------------------------
void albaViewVTK::SetWindowSize(int w, int h)
{
	GetRenderWindow()->SetSize(w,h);
}
//----------------------------------------------------------------------------
bool albaViewVTK::FindPokedVme(albaDevice *device,albaMatrix &point_pose,vtkProp3D *&picked_prop,albaVME *&picked_vme,albaInteractor *&picked_behavior)
{
  albaDeviceButtonsPadTracker *tracker = albaDeviceButtonsPadTracker::SafeDownCast(device);
  albaDeviceButtonsPadMouse   *mouse   = albaDeviceButtonsPadMouse::SafeDownCast(device);
  int mouse_pos[2];
  bool picked_something = false;

  if (tracker)
  {
    albaAvatar *avatar = tracker->GetAvatar();
    albaMatrix world_pose;
    if (avatar)
    {
      albaAvatar3D *avatar3D = albaAvatar3D::SafeDownCast(avatar);
      if (avatar3D)
        avatar3D->TrackerToWorld(point_pose,world_pose,albaAvatar3D::CANONICAL_TO_WORLD_SCALE);
      else
        world_pose = point_pose;
      picked_something = Pick(world_pose);
    }
  }
  else if (mouse)
  { 
    mouse_pos[1] = (int)point_pose.GetElement(1,3);
    mouse_pos[0] = (int)point_pose.GetElement(0,3);
    picked_something = Pick(mouse_pos[0], mouse_pos[1]);
  }
  if(picked_something)
  {
    picked_vme = GetPickedVme();
    picked_prop = GetPickedProp();
    picked_behavior = picked_vme->GetBehavior();
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------
bool albaViewVTK::Pick(int x, int y)
{
  vtkRendererCollection *rc = m_Rwi->m_RwiBase->GetRenderWindow()->GetRenderers();
  vtkRenderer *r = NULL;
  rc->InitTraversal();
  while(r = rc->GetNextItem())
  {
    if(m_Picker2D->Pick(x,y,0,r))
    {
      m_Picker2D->GetPickPosition(m_PickedPosition);
      return FindPickedVme(m_Picker2D->GetPath());
    }
  }
  return false;
}
//----------------------------------------------------------------------------
bool albaViewVTK::Pick(albaMatrix &m)
{
  // Compute intersection ray:
  double p1[4]={0,0,0.05,1}; // from albaAvatar3DCone (to revise!!)
  double p2[4]={0,0,-0.05,1};

  // points in world coordinates
  double world_p1[4],world_p2[4];
  m.MultiplyPoint(p1,world_p1);
  m.MultiplyPoint(p2,world_p2);

  vtkRendererCollection *rc = m_Rwi->m_RwiBase->GetRenderWindow()->GetRenderers();
  vtkRenderer *r = NULL;
  rc->InitTraversal();
  while(r = rc->GetNextItem())
  {
    if( m_Picker3D->Pick(world_p1,world_p2,r) )
    {
      m_Picker3D->GetPickPosition(m_PickedPosition);
      return FindPickedVme(m_Picker3D->GetPath());
    }
  }
  return false;
}

//----------------------------------------------------------------------------
void albaViewVTK::GetImage(wxBitmap &bmp, int magnification)
{
	wxBitmap* imgPtr = m_Rwi->m_RwiBase->GetImage(magnification);
  bmp = *imgPtr;
	cppDEL(imgPtr);
}

//-------------------------------------------------------------------------
void albaViewVTK::Print(std::ostream& os, const int tabs)// const
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  os << indent << "albaViewVTK " << '\t' << this << "\n";
  os << indent << "Name: " << '\t' << m_Label << "\n";
  os << indent << "View ID: " << '\t' << m_Id << "\n";
  os << indent << "View Mult: " << '\t' << m_Mult << "\n";
  os << indent << "Visible VME counter: " << '\t' << m_NumberOfVisibleVme << "\n";
  
  m_Sg->Print(os, 1);
  os << std::endl;
}

//-------------------------------------------------------------------------
int albaViewVTK::GetCameraPosition()
{
   return m_CameraPositionId;
}

//-------------------------------------------------------------------------
void albaViewVTK::SetBackgroundColor(wxColor color)
{
	assert(m_Rwi);

	m_BackgroundColor = color;
	m_Rwi->SetBackgroundColor(color);
	m_Rwi->CameraUpdate();
}