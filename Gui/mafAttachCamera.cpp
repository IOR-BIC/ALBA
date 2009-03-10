/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAttachCamera.cpp,v $
  Language:  C++
  Date:      $Date: 2009-03-10 14:55:25 $
  Version:   $Revision: 1.20.2.1 $
  Authors:   Paolo Quadrani
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

#include "mafAttachCamera.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"

#include "mafVME.h"
#include "mafEventSource.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
mafAttachCamera::mafAttachCamera(wxWindow* parent, mafRWI *rwi, mafObserver *Listener)
//----------------------------------------------------------------------------
{
  m_Gui = NULL;

  m_RenderWindow = rwi;
	
	m_Listener		= Listener;
	m_ParentPanel = parent;

  m_EnableAttachCamera = 0;
  
  m_AttachedVme       = NULL;
  m_AttachedVmeMatrix = NULL;
  vtkNEW(m_StartingMatrix);

  if (parent)
  {
    CreateGui();
  }
}
//----------------------------------------------------------------------------
mafAttachCamera::~mafAttachCamera() 
//----------------------------------------------------------------------------
{
  vtkDEL(m_AttachedVmeMatrix);
  vtkDEL(m_StartingMatrix);
  if (m_AttachedVme && m_AttachedVme->IsValid())
  {
    m_AttachedVme->GetEventSource()->RemoveObserver(this);
  }

  if(m_Gui)	
	  m_Gui->SetListener(NULL);
	cppDEL(m_Gui);
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum ATTACH_CAMERA_WIDGET_ID
{
	ID_CAMERA_ATTACH = MINID,
};
//----------------------------------------------------------------------------
void mafAttachCamera::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
	m_Gui->Show(true);
  m_Gui->Bool(ID_CAMERA_ATTACH,"attach",&m_EnableAttachCamera,0,"Attach the camera to selected vme");
	m_Gui->Divider();
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafAttachCamera::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case ID_CAMERA_ATTACH:
        if(m_EnableAttachCamera)
        {
          mafVME *sel_vme = mafVME::SafeDownCast(m_RenderWindow->m_Sg->GetSelectedVme());
          if(sel_vme == NULL) 
          {
            m_EnableAttachCamera = 0;
            m_Gui->Update();
            return;
          }
          SetVme(sel_vme);
        }
        else
        {
          SetVme(NULL);
        }
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
  else
  {
    switch(maf_event->GetId())
    {
      case NODE_DETACHED_FROM_TREE:
      case NODE_DESTROYED:
        // the VME observed will be removed on the destructor.
        m_EnableAttachCamera = 0;
        m_Gui->Update();
      break;
      case VME_TIME_SET:
        UpdateCameraMatrix();
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafAttachCamera::SetVme(mafNode *node)
//----------------------------------------------------------------------------
{
  if (mafVME::SafeDownCast(node) == NULL)
  {
    if (m_AttachedVme && m_AttachedVme->GetEventSource()->IsObserver(this))
    {
      m_AttachedVme->GetEventSource()->RemoveObserver(this);
    }
    vtkDEL(m_AttachedVmeMatrix);
    m_AttachedVme = NULL;
    m_EnableAttachCamera = 0;
    if (m_Gui != NULL)
    {
      m_Gui->Update();
    }
    return;
  }
  if (m_AttachedVme && m_AttachedVme->Equals(mafVME::SafeDownCast(node)))
  {
    return;
  }
  if (m_AttachedVmeMatrix == NULL)
  {
    vtkNEW(m_AttachedVmeMatrix);
  }
  m_AttachedVme = mafVME::SafeDownCast(node);
  m_AttachedVmeMatrix->DeepCopy(m_StartingMatrix);

  vtkMAFSmartPointer<vtkTransform> delta;
  delta->PreMultiply();
  delta->Concatenate(m_StartingMatrix);

  if (m_RenderWindow)
  {
    m_RenderWindow->m_Camera->ApplyTransform(delta);
    m_RenderWindow->CameraUpdate();
  }

  m_AttachedVme->GetEventSource()->AddObserver(this);
}
//----------------------------------------------------------------------------
void mafAttachCamera::SetStartingMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{
  if(matrix)
    m_StartingMatrix->DeepCopy(matrix->GetVTKMatrix());
}
//----------------------------------------------------------------------------
void mafAttachCamera::UpdateCameraMatrix()
//----------------------------------------------------------------------------
{
  if (m_EnableAttachCamera == 0)
  {
    return;
  }

  if (m_AttachedVme != NULL)
  {
    vtkMatrix4x4 *new_matrix = m_AttachedVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix();

    m_AttachedVmeMatrix->Invert();

    vtkMAFSmartPointer<vtkTransform> delta;
    delta->PreMultiply();
    delta->Concatenate(new_matrix);
    delta->Concatenate(m_AttachedVmeMatrix);

    m_RenderWindow->m_Camera->ApplyTransform(delta);
    m_AttachedVmeMatrix->DeepCopy(new_matrix);
  }
}

void mafAttachCamera::PrintSelf( ostream& os)
{
  os << "-------------------------------------------------------" << std::endl;
  os << "mafAttachCamera " << this << " PrintSelf:" << std::endl;
  
  os << "Camera attach enabled flag m_EnableAttachCamera: " << m_EnableAttachCamera << std::endl;
  os << "Vme to follow with camera m_AttachedVme: " << (m_AttachedVme ? m_AttachedVme->GetName() : "NULL") << std::endl; 
  os << "m_AttachedVmeMatrix: " << m_AttachedVmeMatrix << std::endl;
  if (m_AttachedVmeMatrix)
  {
    m_AttachedVmeMatrix->PrintSelf(os, NULL);
  }


  os << "m_StartingMatrix: " << m_StartingMatrix << std::endl;
  
  if (m_StartingMatrix)
  {
    m_StartingMatrix->PrintSelf(os,NULL);
  }

  os << "m_Listener: " << m_Listener << std::endl;
  os << "m_Gui: " << m_Gui << std::endl;
  os << "m_RenderWindow: " << m_RenderWindow << std::endl;
  os << "m_ParentPanel: " << m_ParentPanel << std::endl;
  os << "-------------------------------------------------------" << std::endl;
}

mafMatrix mafAttachCamera::GetStartingMatrix()
{
  return mafMatrix(m_StartingMatrix);
}

void mafAttachCamera::SetListener( mafObserver *Listener )
{
  m_Listener = Listener;
}

mafObserver * mafAttachCamera::GetListener()
{
  return m_Listener;
}

void mafAttachCamera::EnableAttachCamera()
{
  m_EnableAttachCamera = TRUE;
}

void mafAttachCamera::DisableAttachCamera()
{
  m_EnableAttachCamera = FALSE;
}

int mafAttachCamera::GetEnableAttachCamera()
{
  return m_EnableAttachCamera;
}

void mafAttachCamera::SetEnableAttachCamera( int enable )
{
  m_EnableAttachCamera = enable;
}

mafNode * mafAttachCamera::GetVme()
{
  return m_AttachedVme;
}