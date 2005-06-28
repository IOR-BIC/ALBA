/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-28 10:21:57 $
  Version:   $Revision: 1.13 $
  Authors:   Silvano Imboden
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

#include "vtkMAFSmartPointer.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewVTK::mafViewVTK(wxString label, bool external)
:mafView(label,external)
//----------------------------------------------------------------------------
{
  m_CameraPosition = CAMERA_PERSPECTIVE;
  m_Sg        = NULL;
  m_Rwi       = NULL;
  m_LightKit  = NULL;
  
  m_CameraAttach  = 0;
  m_AttachedVme       = NULL;
  m_AttachedVmeMatrix = NULL;
}
//----------------------------------------------------------------------------
mafViewVTK::~mafViewVTK() 
//----------------------------------------------------------------------------
{
  cppDEL(m_LightKit);
  cppDEL(m_Sg);
  cppDEL(m_Rwi);
  vtkDEL(m_AttachedVmeMatrix);
}
//----------------------------------------------------------------------------
mafView *mafViewVTK::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewVTK *v = new mafViewVTK(m_Label);
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
  m_Rwi = new mafRWI(mafGetFrame() /*, ONE_LAYER, m_show_grid == 1, m_stereo_type*/);
  m_Rwi->SetListener(this); //SIL. 16-6-2004: 
  m_Rwi->CameraSet(m_CameraPosition);
  //m_Rwi->SetAxesVisibility(m_show_axes != 0);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;
  /*
  m_Sg->SetCreatableFlag(VME_SURFACE);
  m_Sg->SetCreatableFlag(VME_GRAY_VOLUME);
  m_Sg->SetCreatableFlag(VME_VOLUME);
  m_Sg->SetCreatableFlag(VME_IMAGE);
  m_Sg->SetCreatableFlag(VME_POINTSET);
  m_Sg->SetCreatableFlag(VME_TOOL);
  m_Sg->SetCreatableFlag(VME_SCALAR);
  */

  m_Rwi->m_Camera->SetClippingRange(0.1,1000); 
  this->CameraReset();
}

//----------------------------------------------------------------------------
void  mafViewVTK::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
  assert(m_Rwi->m_RwiBase);
  m_Rwi->m_RwiBase->SetMouse(mouse);
}

//----------------------------------------------------------------------------
void mafViewVTK::VmeAdd(mafNode *vme)                                   {assert(m_Sg); m_Sg->VmeAdd(vme);}
void mafViewVTK::VmeRemove(mafNode *vme)                                {assert(m_Sg); m_Sg->VmeRemove(vme);}
void mafViewVTK::VmeShow(mafNode *vme, bool show)												{assert(m_Sg); m_Sg->VmeShow(vme,show);}
void mafViewVTK::VmeUpdateProperty(mafNode *vme, bool fromTag)	        {assert(m_Sg); m_Sg->VmeUpdateProperty(vme,fromTag);}
int  mafViewVTK::GetNodeStatus(mafNode *vme)                            {return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;}
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
void mafViewVTK::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->m_Pipe);
  assert(vme->IsA("mafVME"));
  mafVME *v = ((mafVME*)vme);

  mafObject *obj= NULL;
  mafString pipe_name = "";
  mafString vme_type = v->GetTypeName(); // Paolo 2005-04-23 Just to try PlugVisualPipe 
  // (to be replaced with something that extract the type of the vme)

  mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
  assert(pipe_factory!=NULL);
  if (!m_PipeMap.empty() && (m_PipeMap[vme_type].m_Visibility == NODE_VISIBLE_ON))  // Paolo 2005-04-23
  {
    // keep the visual pipe from the view's visual pipe map
    pipe_name = m_PipeMap[vme_type].m_PipeName;
  }
  else
  {
    // keep the default visual pipe from the vme
    pipe_name = v->GetVisualPipe();
  }

  if (pipe_name != "")
  {
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    pipe->Create(n);
    n->m_Pipe = (mafPipe*)pipe;
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void mafViewVTK::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
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
