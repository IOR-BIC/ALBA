/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2008-05-07 13:48:38 $
  Version:   $Revision: 1.77 $
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

#include "mafIndent.h"

#include "mafPipe.h"
#include "mafPipeFactory.h"
#include "mafLightKit.h"
#include "mafAttachCamera.h"
#include "mafTextKit.h"
#include "mafAnimate.h"

#include "mafInteractor.h"
#include "mafAvatar.h"
#include "mafAvatar3D.h"
#include "mmdTracker.h"
#include "mmdMouse.h"

#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMERoot.h"

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
mafViewVTK::mafViewVTK(const wxString &label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo, bool show_orientator)
:mafView(label)
//----------------------------------------------------------------------------
{
  m_CameraPositionId= camera_position;
  m_ShowAxes        = show_axes;
  m_ShowGrid        = show_grid;
  m_StereoType      = stereo;
  m_ShowRuler       = show_ruler;
	m_ShowOrientator  = show_orientator;
  
  m_Sg        = NULL;
  m_Rwi       = NULL;
  m_LightKit  = NULL;
  m_TextKit   = NULL;
  m_AttachCamera = NULL;
  m_AnimateKit = NULL;
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
  cppDEL(m_AnimateKit);
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
  mafViewVTK *v = new mafViewVTK(m_Label, m_CameraPositionId, m_ShowAxes, m_ShowGrid, m_ShowRuler, m_StereoType, m_ShowOrientator);
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
  m_Rwi = new mafRWI(mafGetFrame(), ONE_LAYER, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType, m_ShowOrientator);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.001);
  //m_Picker2D->InitializePickList();
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
void mafViewVTK::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeAdd(vme);
  if (m_AnimateKit && vme->IsMAFType(mafVMERoot))
  {
    m_AnimateKit->SetInputVME(vme);
  }
}
//----------------------------------------------------------------------------
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
      if (n != NULL)
      {
      	n->m_Mutex = true;
      }
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
  if (m_AnimateKit && vme->IsMAFType(mafVMERoot))
  {
    m_AnimateKit->ResetKit();
  }
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
  m_CameraPositionId = camera_position; 
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
  mafVME *v = mafVME::SafeDownCast(node);
  assert(v);

  v->Modified();
  vtkDataSet *data = v->GetOutput()->GetVTKData();
  mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(v);
  mafVMELandmark *lm = mafVMELandmark::SafeDownCast(v);
  if (lmc == NULL && data == NULL && lm == NULL)
  {
    pipe_name = "mafPipeBox";
  }
  else
  {
    // custom visualization for the view should be considered only
    // if we are not in editing mode.
    mafString vme_type = v->GetTypeName();
    if (!m_PipeMap.empty())
    {
      // pick up the visual pipe from the view's visual pipe map
      pipe_name = m_PipeMap[vme_type].m_PipeName;
    }
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
    m_NumberOfVisibleVme++;
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->m_Pipe);
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
    }
    else
    {
      mafErrorMessage(_("Cannot create visual pipe object of type \"%s\"!"),pipe_name.GetCStr());
    }
  }
}
//----------------------------------------------------------------------------
void mafViewVTK::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_NumberOfVisibleVme--;
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//-------------------------------------------------------------------------
mmgGui *mafViewVTK::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui(); //new mmgGui(this);
  m_Gui->AddGui(m_Rwi->GetGui());
  
  /////////////////////////////////////////Attach Camera GUI
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->RollOut(ID_ROLLOUT_ATTACH_CAMERA, " Attach camera", m_AttachCamera->GetGui(), false);

  /////////////////////////////////////////Text GUI
  m_TextKit = new mafTextKit(m_Gui, m_Rwi->m_RenFront, this);
  m_Gui->RollOut(ID_ROLLOUT_TEXT_KIT, " Text kit", m_TextKit->GetGui(), false);

  /////////////////////////////////////////Light GUI
  m_LightKit = new mafLightKit(m_Gui, m_Rwi->m_RenFront, this);
  m_Gui->RollOut(ID_ROLLOUT_LIGHT_KIT, " Light kit", m_LightKit->GetGui(), false);
  
  // Animate kit
  m_AnimateKit = new mafAnimate(m_Rwi->m_RenFront,m_Sg->GetSelectedVme()->GetRoot(),this);
  m_Gui->RollOut(ID_ROLLOUT_ANIMATE_KIT, " Animate kit", m_AnimateKit->GetGui(), false);

  m_Gui->Divider();

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
      case ID_ROLLOUT_ATTACH_CAMERA:
      case ID_ROLLOUT_TEXT_KIT:
      case ID_ROLLOUT_LIGHT_KIT:
      case ID_ROLLOUT_ANIMATE_KIT:
      break;
      default:
        Superclass::OnEvent(maf_event);
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
      return FindPickedVme(m_Picker2D->GetPath());
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
      return FindPickedVme(m_Picker3D->GetPath());
    }
  }
  return false;
}
//----------------------------------------------------------------------------
void mafViewVTK::Print(wxDC *dc, wxRect margins)
//----------------------------------------------------------------------------
{
  wxBitmap image;
  GetImage(image/*, 2*/);
  PrintBitmap(dc, margins, &image);
}
//----------------------------------------------------------------------------
void mafViewVTK::GetImage(wxBitmap &bmp, int magnification)
//----------------------------------------------------------------------------
{
  bmp = *m_Rwi->m_RwiBase->GetImage(magnification);
}
//----------------------------------------------------------------------------
void mafViewVTK::LinkView(bool link_camera)
//----------------------------------------------------------------------------
{
  m_Rwi->LinkCamera(link_camera);
}
//----------------------------------------------------------------------------
void mafViewVTK::OptionsUpdate()
//----------------------------------------------------------------------------
{
  m_Rwi->UpdateRulerUnit();
}

//-------------------------------------------------------------------------
void mafViewVTK::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "mafViewVTK " << '\t' << this << "\n";
  os << indent << "Name: " << '\t' << m_Label << "\n";
  os << indent << "View ID: " << '\t' << m_Id << "\n";
  os << indent << "View Mult: " << '\t' << m_Mult << "\n";
  os << indent << "Visible VME counter: " << '\t' << m_NumberOfVisibleVme << "\n";
  
  m_Sg->Print(os, 1);
  os << std::endl;
}
