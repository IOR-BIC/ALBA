/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-28 13:04:44 $
  Version:   $Revision: 1.45 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewVTK.h"
#include <wx/dc.h>
#include "mafVME.h"
#include "mafPipe.h"
#include "mafPipeFactory.h"
#include "mafLightKit.h"
#include "mafAttachCamera.h"
#include "mafTextKit.h"

#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include "mafInteractor.h"
#include "mafAvatar.h"
#include "mafAvatar3D.h"
#include "mmdTracker.h"
#include "mmdMouse.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewVTK::mafViewVTK(wxString label, int camera_position, bool show_axes, int stereo, bool external)
:mafView(label,external)
//----------------------------------------------------------------------------
{
  m_CameraPosition  = camera_position;
  m_ExternalFlag    = external;
  m_ShowAxes        = show_axes;
  m_StereoType      = stereo;
  
  m_ShowGrid        = 0;
  m_GridNormal      = 1;

  m_Sg        = NULL;
  m_Rwi       = NULL;
  m_LightKit  = NULL;
  m_TextKit   = NULL;
  m_AttachCamera = NULL;

  m_NumberOfVisibleVme = 0;
}
//----------------------------------------------------------------------------
mafViewVTK::~mafViewVTK() 
//----------------------------------------------------------------------------
{
  m_PipeMap.clear();

  vtkDEL(m_Picker2D);
  vtkDEL(m_Picker3D);
  cppDEL(m_AttachCamera);
  cppDEL(m_LightKit);
  cppDEL(m_TextKit);
  cppDEL(m_Sg);
  cppDEL(m_Rwi);
}
//----------------------------------------------------------------------------
void mafViewVTK::PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
  mafVisualPipeInfo plugged_pipe;
  plugged_pipe.m_PipeName=pipe_type;
  plugged_pipe.m_Visibility=visibility;
  m_PipeMap[vme_type] = plugged_pipe;
}
//----------------------------------------------------------------------------
mafView *mafViewVTK::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewVTK *v = new mafViewVTK(m_Label, m_CameraPosition, m_ShowAxes, m_StereoType, m_ExternalFlag);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewVTK::Create()
//----------------------------------------------------------------------------
{
  m_Rwi = new mafRWI(mafGetFrame(), ONE_LAYER, m_ShowGrid != 0, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Rwi->SetAxesVisibility(m_ShowAxes != 0);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.001);
  m_Picker2D->InitializePickList();
}
//----------------------------------------------------------------------------
void mafViewVTK::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
  assert(m_Rwi->m_RwiBase);
  m_Rwi->m_RwiBase->SetMouse(mouse);
}

//----------------------------------------------------------------------------
vtkRenderWindow *mafViewVTK::GetRenderWindow()
//----------------------------------------------------------------------------
{ 
  return m_Rwi->m_RenderWindow;
}

//----------------------------------------------------------------------------
vtkRenderer *mafViewVTK::GetFrontRenderer()
//----------------------------------------------------------------------------
{
  return m_Rwi->m_RenFront;
}
//----------------------------------------------------------------------------
vtkRenderer *mafViewVTK::GetBackRenderer()
//----------------------------------------------------------------------------
{
  return m_Rwi->m_RenBack;
}

//----------------------------------------------------------------------------
void mafViewVTK::VmeAdd(mafNode *vme)                                   {assert(m_Sg); m_Sg->VmeAdd(vme);}
void mafViewVTK::VmeShow(mafNode *vme, bool show)												{assert(m_Sg); m_Sg->VmeShow(vme,show);}
void mafViewVTK::VmeUpdateProperty(mafNode *vme, bool fromTag)	        {assert(m_Sg); m_Sg->VmeUpdateProperty(vme,fromTag);}
//----------------------------------------------------------------------------
int  mafViewVTK::GetNodeStatus(mafNode *vme)
//----------------------------------------------------------------------------
{
  int status = m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
  if (!m_PipeMap.empty())
  {
    mafString vme_type = vme->GetTypeName();
    if(m_PipeMap[vme_type].m_Visibility == NON_VISIBLE)
    {
      status = NODE_NON_VISIBLE;
    }
    else if (m_PipeMap[vme_type].m_Visibility == MUTEX)
    {
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      n->m_Mutex = true;
      status = m_Sg->GetNodeStatus(vme);
    }
  }
  return status;
}
//----------------------------------------------------------------------------
void mafViewVTK::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafViewVTK::VmeSelect(mafNode *vme, bool select)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);
}
//----------------------------------------------------------------------------
void mafViewVTK::CameraSet(int camera_position) 
//----------------------------------------------------------------------------
{
  assert(m_Rwi);
  m_CameraPosition = camera_position; 
  m_Rwi->CameraSet(camera_position);
}
//----------------------------------------------------------------------------
void mafViewVTK::CameraReset(mafNode *node)
//----------------------------------------------------------------------------
{
  assert(m_Rwi); 
  m_Rwi->CameraReset(node);
}
//----------------------------------------------------------------------------
void mafViewVTK::CameraUpdate() 
//----------------------------------------------------------------------------
{
  if (m_AttachCamera != NULL)
  {
    m_AttachCamera->UpdateCameraMatrix();
  }
  assert(m_Rwi); 
  m_Rwi->CameraUpdate();
}
//----------------------------------------------------------------------------
mafPipe* mafViewVTK::GetNodePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
   assert(m_Sg);
   mafSceneNode *n = m_Sg->Vme2Node(vme);
   if(!n) return NULL;
   return n->m_Pipe;
}
//----------------------------------------------------------------------------
void mafViewVTK::GetVisualPipeName(mafNode *node, mafString &pipe_name)
//----------------------------------------------------------------------------
{
  assert(node->IsA("mafVME"));
  mafVME *v = ((mafVME*)node);

  mafString vme_type = v->GetTypeName();
  if (!m_PipeMap.empty())
  {
    // pick up the visual pipe from the view's visual pipe map
    pipe_name = m_PipeMap[vme_type].m_PipeName;
  }
  if(pipe_name.IsEmpty())
  {
    // pick up the default visual pipe from the vme
    pipe_name = v->GetVisualPipe();
  }
}
//----------------------------------------------------------------------------
void mafViewVTK::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  if (pipe_name != "")
  {
    if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 1)
    {
      m_NumberOfVisibleVme = 1;
    }
    else
    {
      m_NumberOfVisibleVme++;
    }
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    pipe->SetListener(this);
    if (pipe)
    {
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->m_Pipe);
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
      if (m_NumberOfVisibleVme == 1)
      {
        CameraReset();
      }
      else
      {
        CameraUpdate();
      }
    }
    else
    {
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
    }
  }
}
//----------------------------------------------------------------------------
void mafViewVTK::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 0)
  {
    m_NumberOfVisibleVme = 0;
  }
  else
  {
    m_NumberOfVisibleVme--;
  }
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//-------------------------------------------------------------------------
mmgGui *mafViewVTK::CreateGui()
//-------------------------------------------------------------------------
{
  wxString grid_normal[3] = {"X axes","Y axes","Z axes"};

  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_SHOW_GRID,"grid",&m_ShowGrid,0,"Turn On/Off the grid");
  m_Gui->Combo(ID_GRID_NORMAL,"grid normal",&m_GridNormal,3,grid_normal,"orientation axes for the grid");
  m_Gui->Divider(2);
  
  /////////////////////////////////////////Text GUI
  m_TextKit = new mafTextKit(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_TextKit->GetGui());
  m_Gui->Divider(2);

  /////////////////////////////////////////Attach Camera GUI
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());

  /////////////////////////////////////////Light GUI
  m_Gui->Divider(2);
  m_LightKit = new mafLightKit(m_Gui, this->m_Rwi->m_RenFront, this);
  m_Gui->AddGui(m_LightKit->GetGui());
  
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewVTK::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case CAMERA_PRE_RESET:
        OnPreResetCamera();
        mafEventMacro(*maf_event);
      break;
      case CAMERA_POST_RESET:
        OnPostResetCamera();
        mafEventMacro(*maf_event);
      break;
      case ID_SHOW_GRID:
        m_Rwi->SetGridVisibility(m_ShowGrid != 0);
        CameraUpdate();
      break;
      case ID_GRID_NORMAL:
        m_Rwi->SetGridNormal(m_GridNormal);
        CameraUpdate();
      break;
      default:
        mafEventMacro(*maf_event);
      break;
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafViewVTK::OnPreResetCamera()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafViewVTK::OnPostResetCamera()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafViewVTK::SetWindowSize(int w, int h)
//----------------------------------------------------------------------------
{
	GetRenderWindow()->SetSize(w,h);
}
//----------------------------------------------------------------------------
bool mafViewVTK::FindPokedVme(mafDevice *device,mafMatrix &point_pose,vtkProp3D *&picked_prop,mafVME *&picked_vme,mafInteractor *&picked_behavior)
//----------------------------------------------------------------------------
{
  mmdTracker *tracker = mmdTracker::SafeDownCast(device);
  mmdMouse   *mouse   = mmdMouse::SafeDownCast(device);
  int mouse_pos[2];
  bool picked_something = false;

  if (tracker)
  {
    mafAvatar *avatar = tracker->GetAvatar();
    mafMatrix world_pose;
    if (avatar)
    {
      mafAvatar3D *avatar3D = mafAvatar3D::SafeDownCast(avatar);
      if (avatar3D)
        avatar3D->TrackerToWorld(point_pose,world_pose,mafAvatar3D::CANONICAL_TO_WORLD_SCALE);
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
bool mafViewVTK::Pick(int x, int y)
//----------------------------------------------------------------------------
{
  vtkRendererCollection *rc = m_Rwi->m_RwiBase->GetRenderWindow()->GetRenderers();
  vtkRenderer *r = NULL;
  rc->InitTraversal();
  while(r = rc->GetNextItem())
  {
    if(m_Picker2D->Pick(x,y,0,r))
    {
      m_Picker2D->GetPickPosition(m_PickedPosition);
      FindPickedVme(m_Picker2D->GetPath());
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------------
bool mafViewVTK::Pick(mafMatrix &m)
//----------------------------------------------------------------------------
{
  // Compute intersection ray:
  double p1[4]={0,0,0.05,1}; // from mafAvatar3DCone (to revise!!)
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
      FindPickedVme(m_Picker3D->GetPath());
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------------
void mafViewVTK::Print(wxDC *dc, wxRect margins)
//----------------------------------------------------------------------------
{
  wxBitmap *image = m_Rwi->m_RwiBase->GetImage(2);
  PrintBitmap(dc, margins, image);
  cppDEL(image);
}
