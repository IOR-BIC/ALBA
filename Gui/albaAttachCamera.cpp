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
{
  m_Gui = NULL;

  m_RenderWindow = rwi;
	
	m_Listener		= Listener;
	m_ParentPanel = parent;

  m_EnableAttachCamera = 0;
  
  m_AttachedVme       = NULL;
	m_AttachedMatrix = NULL;
  m_CurrentMatrix = NULL;
	vtkNEW(m_CurrentMatrix);

  if (parent)
  {
    CreateGui();
  }
}
//----------------------------------------------------------------------------
albaAttachCamera::~albaAttachCamera() 
{
  vtkDEL(m_CurrentMatrix);
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
enum ATTACH_CAMERA_WIDGET_ID
{
	ID_CAMERA_ATTACH = MINID,
};
//----------------------------------------------------------------------------
void albaAttachCamera::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->Show(true);
  m_Gui->Bool(ID_CAMERA_ATTACH,"Attach",&m_EnableAttachCamera,0,"Attach the camera to selected vme");
	m_Gui->Divider();
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaAttachCamera::OnEvent(albaEventBase *alba_event)
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
{
	if (m_AttachedVme && m_AttachedVme==node)
		return;
	
	if (m_AttachedVme && m_AttachedVme->IsObserver(this))
		m_AttachedVme->RemoveObserver(this);
  
	if (node == NULL)
  {
    m_AttachedVme = NULL;
    m_EnableAttachCamera = 0;
    if (m_Gui != NULL)
      m_Gui->Update();
  }
	else
	{
		m_AttachedMatrix = NULL;
		m_AttachedVme = node;
		m_AttachedVme->AddObserver(this);

		UpdateCameraMatrix();

		if (m_RenderWindow)
			m_RenderWindow->CameraUpdate();
	}
}

//----------------------------------------------------------------------------
void albaAttachCamera::UpdateCameraMatrix()
{
  if (m_EnableAttachCamera == 0)
    return;

  if (m_AttachedVme || m_AttachedMatrix)
  {
		
    vtkMatrix4x4 *new_matrix = m_AttachedVme ? m_AttachedVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix() : m_AttachedMatrix;

    m_CurrentMatrix->Invert();

    vtkALBASmartPointer<vtkTransform> delta;
    delta->PreMultiply();
    delta->Concatenate(new_matrix);
    delta->Concatenate(m_CurrentMatrix);
		delta->Update();

    m_RenderWindow->m_Camera->ApplyTransform(delta);
    m_CurrentMatrix->DeepCopy(new_matrix);
  }
}
//----------------------------------------------------------------------------
void albaAttachCamera::SetListener( albaObserver *Listener )
{
  m_Listener = Listener;
}
//----------------------------------------------------------------------------
albaObserver * albaAttachCamera::GetListener()
{
  return m_Listener;
}
//----------------------------------------------------------------------------
void albaAttachCamera::EnableAttachCamera()
{
  m_EnableAttachCamera = TRUE;
}
//----------------------------------------------------------------------------
void albaAttachCamera::DisableAttachCamera()
{
  m_EnableAttachCamera = FALSE;
}
//----------------------------------------------------------------------------
int albaAttachCamera::GetEnableAttachCamera()
{
  return m_EnableAttachCamera;
}
//----------------------------------------------------------------------------
void albaAttachCamera::SetEnableAttachCamera( int enable )
{
  m_EnableAttachCamera = enable;
}
//----------------------------------------------------------------------------
albaVME * albaAttachCamera::GetVme()
{
  return m_AttachedVme;
}

//----------------------------------------------------------------------------
void albaAttachCamera::SetStartingMatrix(vtkMatrix4x4 *matrix)
{
	m_CurrentMatrix->DeepCopy(matrix);
}

//----------------------------------------------------------------------------
albaGUI * albaAttachCamera::GetGui()
{
  return m_Gui;
}

//----------------------------------------------------------------------------
void albaAttachCamera::SetAttachedMatrix(vtkMatrix4x4 * attachedMatrix)
{
	bool attachedCameraEnabled = m_EnableAttachCamera;
	m_AttachedVme = NULL;
	m_EnableAttachCamera = attachedCameraEnabled;
	m_AttachedMatrix = attachedMatrix;

	UpdateCameraMatrix();

	if (m_RenderWindow)
		m_RenderWindow->CameraUpdate();
}
