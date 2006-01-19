/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAttachCamera.cpp,v $
  Language:  C++
  Date:      $Date: 2006-01-19 11:20:42 $
  Version:   $Revision: 1.3 $
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
#include "mmgGui.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"

#include "mafVME.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
mafAttachCamera::mafAttachCamera(wxWindow* parent, mafRWI *rwi, mafObserver *Listener)
//----------------------------------------------------------------------------
{
	m_Rwi = rwi;
	
	m_Listener		= Listener;
	m_ParentPanel = parent;

  m_CameraAttach = 0;
  
  m_AttachedVme       = NULL;
  m_AttachedVmeMatrix = NULL;

	CreateGui();
}
//----------------------------------------------------------------------------
mafAttachCamera::~mafAttachCamera() 
//----------------------------------------------------------------------------
{
  vtkDEL(m_AttachedVmeMatrix);

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
	m_Gui = new mmgGui(this);
	m_Gui->Show(true);
  m_Gui->Bool(ID_CAMERA_ATTACH,"attach",&m_CameraAttach,0,"Attach the camera to selected vme");
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
        if(m_CameraAttach)
        {
          mafVME *sel_vme = mafVME::SafeDownCast(m_Rwi->m_Sg->GetSelectedVme());
          if(sel_vme == NULL) 
          {
            m_CameraAttach = 0;
            m_Gui->Update();
            return;
          }
          SetVme(sel_vme);
        }
        else
        {
          m_AttachedVme = NULL;
        }
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void mafAttachCamera::SetVme(mafNode *node)
//----------------------------------------------------------------------------
{
  m_AttachedVme = mafVME::SafeDownCast(node);
  if (m_AttachedVme == NULL)
  {
    return;
  }
  if (m_AttachedVmeMatrix == NULL)
    vtkNEW(m_AttachedVmeMatrix);
  m_AttachedVmeMatrix->DeepCopy(m_AttachedVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
}
//----------------------------------------------------------------------------
void mafAttachCamera::UpdateCameraMatrix()
//----------------------------------------------------------------------------
{
  if (m_AttachedVme == NULL)
  {
    return;
  }
  //mafVME *sel_vme = mafVME::SafeDownCast(m_Rwi->m_Sg->GetSelectedVme());
  if(m_AttachedVme /*!= sel_vme && sel_vme*/)
  {
    //m_AttachedVme = sel_vme;
    if (m_AttachedVmeMatrix == NULL)
      vtkNEW(m_AttachedVmeMatrix);
    m_AttachedVmeMatrix->DeepCopy(m_AttachedVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
  }
  vtkMAFSmartPointer<vtkTransform> delta;

  vtkMatrix4x4 *new_matrix = m_AttachedVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix();

  m_AttachedVmeMatrix->Invert();

  delta->Concatenate(new_matrix);
  delta->Concatenate(m_AttachedVmeMatrix);

  m_Rwi->m_Camera->ApplyTransform(delta);

  m_AttachedVmeMatrix->DeepCopy(new_matrix);
}
