/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-05 11:58:36 $
  Version:   $Revision: 1.32 $
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
#include "mafVME.h"
#include "mafPipe.h"
#include "mafPipeFactory.h"
#include "mafLightKit.h"

#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"

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

  m_Sg        = NULL;
  m_Rwi       = NULL;
  m_LightKit  = NULL;
  m_TextMapper= NULL;
  m_TextActor = NULL;

  m_TextPosition[0] = m_TextPosition[1] = 3;
  m_TextColor[0] = m_TextColor[1] = m_TextColor[2] = 1;
  m_TextInView = "";

  m_NumberOfVisibleVme = 0;

  m_CameraAttach  = 0;
  m_AttachedVme       = NULL;
  m_AttachedVmeMatrix = NULL;
}
//----------------------------------------------------------------------------
mafViewVTK::~mafViewVTK() 
//----------------------------------------------------------------------------
{
  m_PipeMap.clear();

  m_Rwi->m_RenFront->RemoveActor(m_TextActor);
  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);
  vtkDEL(m_Picker2D);
  vtkDEL(m_Picker3D);
  cppDEL(m_LightKit);
  cppDEL(m_Sg);
  cppDEL(m_Rwi);
  vtkDEL(m_AttachedVmeMatrix);
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
  m_Rwi = new mafRWI(mafGetFrame(), ONE_LAYER, false, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Rwi->SetAxesVisibility(m_ShowAxes != 0);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;
  m_Rwi->m_Camera->SetClippingRange(0.1,1000); 
  this->CameraReset();

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.001);
  m_Picker2D->InitializePickList();

  vtkNEW(m_TextMapper);
  m_TextMapper->SetInput(m_TextInView.GetCStr());
  m_TextMapper->GetTextProperty()->AntiAliasingOff();

  vtkNEW(m_TextActor);
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->SetPosition(m_TextPosition);
  m_TextActor->GetProperty()->SetColor(m_TextColor);

  m_Rwi->m_RenFront->AddActor(m_TextActor);
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
int  mafViewVTK::GetNodeStatus(mafNode *vme)                            {return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;}
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
  if (m_CameraAttach && select)
  {
    m_AttachedVme = mafVME::SafeDownCast(vme);
    if (m_AttachedVme == NULL)
    {
      m_CameraAttach = 0;
      m_Gui->Update();
    }
    else
    {
      if (m_AttachedVmeMatrix == NULL)
        vtkNEW(m_AttachedVmeMatrix);
      m_AttachedVmeMatrix->DeepCopy(m_AttachedVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
    }
  }
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
  if(m_AttachedVme != NULL && m_AttachedVmeMatrix != NULL)
    UpdateCameraMatrix(); 
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
  if (!m_PipeMap.empty() && (m_PipeMap[vme_type].m_Visibility == VISIBLE))
  {
    // pick up the visual pipe from the view's visual pipe map
    pipe_name = m_PipeMap[vme_type].m_PipeName;
  }
  else
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
        mafEventMacro(mafEvent(this,CAMERA_RESET));
      }
      else
      {
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  
  /////////////////////////////////////////Attach Camera GUI
  m_Gui->Label("attach camera to selected object",true);
  m_Gui->Bool(ID_ATTACH_CAMERA,"attach",&m_CameraAttach,1);

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
      case ID_ATTACH_CAMERA:
        if(m_CameraAttach)
        {
          if(!m_Sg->GetSelectedVme()) 
            return;
          m_AttachedVme = mafVME::SafeDownCast(m_Sg->GetSelectedVme());
          if (m_AttachedVme == NULL)
          {
            m_CameraAttach = 0;
            m_Gui->Update();
            return;
          }
          if (m_AttachedVmeMatrix == NULL)
            vtkNEW(m_AttachedVmeMatrix);
          m_AttachedVmeMatrix->DeepCopy(m_AttachedVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
        }
        else
        {
          m_AttachedVme = NULL;
        }
      break;
      case CAMERA_PRE_RESET:
        OnPreResetCamera();
        mafEventMacro(*maf_event);
      break;
      case CAMERA_POST_RESET:
        OnPostResetCamera();
        mafEventMacro(*maf_event);
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
void mafViewVTK::UpdateCameraMatrix()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkTransform> delta;

  vtkMatrix4x4 *new_matrix = m_AttachedVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix();

  m_AttachedVmeMatrix->Invert();

  delta->Concatenate(new_matrix);
  delta->Concatenate(m_AttachedVmeMatrix);

  m_Rwi->m_Camera->ApplyTransform(delta);

  m_AttachedVmeMatrix->DeepCopy(new_matrix);
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
void mafViewVTK::SetText(const char *text, int x, int y)
//----------------------------------------------------------------------------
{
  m_TextInView = text;
  m_TextPosition[0] = x;
  m_TextPosition[1] = y;
}
//----------------------------------------------------------------------------
void mafViewVTK::SetTextColor(double textColor[3])
//----------------------------------------------------------------------------
{
  m_TextColor[0] = textColor[0];
  m_TextColor[1] = textColor[1];
  m_TextColor[2] = textColor[2];

  if(m_TextActor) 
    m_TextActor->GetProperty()->SetColor(m_TextColor);
}
