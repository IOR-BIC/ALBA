/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttachCamera
 Authors: Paolo Quadrani
 
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

#include "albaAttachCamera.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"

#include "albaVME.h"

#include "vtkALBASmartPointer.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
albaAttachCamera::albaAttachCamera(wxWindow* parent, albaRWI *rwi, albaObserver *Listener)
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
albaAttachCamera::~albaAttachCamera() 
//----------------------------------------------------------------------------
{
  vtkDEL(m_AttachedVmeMatrix);
  vtkDEL(m_StartingMatrix);
  if (m_AttachedVme && m_AttachedVme->IsValid())
  {
    m_AttachedVme->RemoveObserver(this);
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
void albaAttachCamera::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);
	m_Gui->Show(true);
  m_Gui->Bool(ID_CAMERA_ATTACH,"attach",&m_EnableAttachCamera,0,"Attach the camera to selected vme");
	m_Gui->Divider();
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaAttachCamera::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case ID_CAMERA_ATTACH:
        if(m_EnableAttachCamera)
        {
          albaVME *sel_vme = m_RenderWindow->m_Sg->GetSelectedVme();
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
		GetLogicManager()->CameraUpdate();
	}
  else
  {
    switch(alba_event->GetId())
    {
      case NODE_DETACHED_FROM_TREE:
      case NODE_DESTROYED:
        // the VME observed will be removed on the destructor.
        m_EnableAttachCamera = 0;
        if (m_Gui)
        {
        	m_Gui->Update();
        }
      break;
      case VME_TIME_SET:
        UpdateCameraMatrix();
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaAttachCamera::SetVme(albaVME *node)
//----------------------------------------------------------------------------
{
	if (m_AttachedVme && m_AttachedVme==node)
	{
		return;
	}
	
	if (m_AttachedVme && m_AttachedVme->IsObserver(this))
	{
		m_AttachedVme->RemoveObserver(this);
	}
  
	if (node == NULL)
  {
   
    vtkDEL(m_AttachedVmeMatrix);
    m_AttachedVme = NULL;
    m_EnableAttachCamera = 0;
    if (m_Gui != NULL)
    {
      m_Gui->Update();
    }
    return;
  }
  
  if (m_AttachedVmeMatrix == NULL)
  {
    vtkNEW(m_AttachedVmeMatrix);
  }
  m_AttachedVme = node;
  m_AttachedVmeMatrix->DeepCopy(m_StartingMatrix);

  vtkALBASmartPointer<vtkTransform> delta;
  delta->PreMultiply();
  delta->Concatenate(m_StartingMatrix);

  if (m_RenderWindow)
  {
    m_RenderWindow->m_Camera->ApplyTransform(delta);
    m_RenderWindow->CameraUpdate();
  }

  m_AttachedVme->AddObserver(this);
}
//----------------------------------------------------------------------------
void albaAttachCamera::SetStartingMatrix(albaMatrix *matrix)
//----------------------------------------------------------------------------
{
  if(matrix)
    m_StartingMatrix->DeepCopy(matrix->GetVTKMatrix());
}
//----------------------------------------------------------------------------
void albaAttachCamera::UpdateCameraMatrix()
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

    vtkALBASmartPointer<vtkTransform> delta;
    delta->PreMultiply();
    delta->Concatenate(new_matrix);
    delta->Concatenate(m_AttachedVmeMatrix);

    m_RenderWindow->m_Camera->ApplyTransform(delta);
    m_AttachedVmeMatrix->DeepCopy(new_matrix);
  }
}

void albaAttachCamera::PrintSelf( ostream& os)
{
  os << "-------------------------------------------------------" << std::endl;
  os << "albaAttachCamera " << this << " PrintSelf:" << std::endl;
  
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

albaMatrix albaAttachCamera::GetStartingMatrix()
{
  return albaMatrix(m_StartingMatrix);
}

void albaAttachCamera::SetListener( albaObserver *Listener )
{
  m_Listener = Listener;
}

albaObserver * albaAttachCamera::GetListener()
{
  return m_Listener;
}

void albaAttachCamera::EnableAttachCamera()
{
  m_EnableAttachCamera = TRUE;
}

void albaAttachCamera::DisableAttachCamera()
{
  m_EnableAttachCamera = FALSE;
}

int albaAttachCamera::GetEnableAttachCamera()
{
  return m_EnableAttachCamera;
}

void albaAttachCamera::SetEnableAttachCamera( int enable )
{
  m_EnableAttachCamera = enable;
}

albaVME * albaAttachCamera::GetVme()
{
  return m_AttachedVme;
}

albaGUI * albaAttachCamera::GetGui()
{
  return m_Gui;
}
