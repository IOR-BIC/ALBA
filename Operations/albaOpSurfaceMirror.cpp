/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSurfaceMirror
 Authors: Paolo Quadrani - porting  Daniele Giunchi
 
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

#include "albaOpSurfaceMirror.h"
#include <wx/busyinfo.h>

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"


#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaVMELandmarkCloud.h"

#include "vtkPolyData.h"
#include "vtkALBAPolyDataMirror.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpSurfaceMirror);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpSurfaceMirror::albaOpSurfaceMirror(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType			 		= OPTYPE_OP;
	m_Canundo			 		= true;
	m_InputPreserving = false; //Natural_preserving
	
	m_OutputPolydata = NULL;
	m_InputPolydata		= NULL;
  m_MirrorFilter     = NULL;

  m_MirrorX      = 1;
  m_MirrorY      = 0;
  m_MirrorZ      = 0;
  m_FlipNormals = 0;
}
//----------------------------------------------------------------------------
albaOpSurfaceMirror::~albaOpSurfaceMirror( ) 
//----------------------------------------------------------------------------
{
	vtkDEL(m_InputPolydata);
	vtkDEL(m_OutputPolydata);
	vtkDEL(m_MirrorFilter);
}
//----------------------------------------------------------------------------
albaOp* albaOpSurfaceMirror::Copy()   
//----------------------------------------------------------------------------
{
  albaOpSurfaceMirror *cp = new albaOpSurfaceMirror(m_Label);
  cp->m_Canundo		= m_Canundo;
  cp->m_OpType		= m_OpType;
  cp->m_Listener	= m_Listener;
  cp->m_Next			= NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool albaOpSurfaceMirror::InternalAccept(albaVME* node)   
//----------------------------------------------------------------------------
{ return  ( 
		      node
					
					&& 
					(
							node->IsALBAType(albaVMESurface) 
							/*
							|| 
							( 
								 vme->IsA("mflVMELandmarkCloud") 
								 && 
								 !((mflVMELandmarkCloud*)vme)->IsOpen()
							) 
							*/
					)
					//&& node->GetNumberOfItems()==1
					//&& node->GetItem(0)
					//&& node->GetItem(0)->GetData() 	
					);
};   
//----------------------------------------------------------------------------
enum SURFACE_MIRROR_ID
//----------------------------------------------------------------------------
{
	ID_MIRRORX = MINID,
	ID_MIRRORY,
	ID_MIRRORZ,
	ID_FLIPNORMALS,
};
//----------------------------------------------------------------------------
void albaOpSurfaceMirror::OpRun()   
//----------------------------------------------------------------------------
{  
    
	vtkNEW(m_InputPolydata);
	m_InputPolydata->DeepCopy((vtkPolyData*)((albaVMESurface *)m_Input)->GetOutput()->GetVTKData());
	
	vtkNEW(m_OutputPolydata);
	m_OutputPolydata->DeepCopy((vtkPolyData*)((albaVMESurface *)m_Input)->GetOutput()->GetVTKData());
	

	if(!m_TestMode)
	{
		// interface:
		m_Gui = new albaGUI(this);
		m_Gui->SetListener(this);

		m_Gui->Label("this doesn't work on animated vme");
		m_Gui->Label("");
		
		m_Gui->Bool(ID_MIRRORX,"mirror x coords", &m_MirrorX, 1);
		m_Gui->Bool(ID_MIRRORY,"mirror y coords", &m_MirrorY, 1);
		m_Gui->Bool(ID_MIRRORZ,"mirror z coords", &m_MirrorZ, 1);
		//m_Gui->Bool(ID_FLIPNORMALS,"flip normals",&m_FlipNormals,1);
		m_Gui->Label("");
		m_Gui->OkCancel();

		ShowGui();
	}

  m_MirrorFilter = vtkALBAPolyDataMirror::New();
  m_MirrorFilter->SetInput(m_InputPolydata);

  Preview();
}
//----------------------------------------------------------------------------
void albaOpSurfaceMirror::OpDo()
//----------------------------------------------------------------------------
{
  assert(m_OutputPolydata);

	((albaVMESurface *)m_Input)->SetData(m_OutputPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpSurfaceMirror::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_InputPolydata);

	((albaVMESurface *)m_Input)->SetData(m_InputPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpSurfaceMirror::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
	  switch(e->GetId())
	  {
			case ID_MIRRORX:
			case ID_MIRRORY:
			case ID_MIRRORZ:
			case ID_FLIPNORMALS:
	         Preview();
			break;
			case wxOK:
				OpStop(OP_RUN_OK);        
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);        
			break;
				default:
				albaEventMacro(*e);
			break; 
	  }
	}  
}
//----------------------------------------------------------------------------
void albaOpSurfaceMirror::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(result == OP_RUN_CANCEL) OpUndo();

	if(!m_TestMode)
	{
	  HideGui();
	  delete m_Gui;
	}
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpSurfaceMirror::Preview()  
//----------------------------------------------------------------------------
{
	wxBusyCursor *wait=NULL;
	if(!m_TestMode)
	{
		wait=new wxBusyCursor();
	}

  m_MirrorFilter->SetMirrorXCoordinate(m_MirrorX);
  m_MirrorFilter->SetMirrorYCoordinate(m_MirrorY);
  m_MirrorFilter->SetMirrorZCoordinate(m_MirrorZ);
  //m_MirrorFilter->SetFlipNormals(m_FlipNormals);
  m_MirrorFilter->Update();
  

  m_OutputPolydata->DeepCopy(m_MirrorFilter->GetOutput());
  m_OutputPolydata->Update();
  ((albaVMESurface *)m_Input)->SetData(m_OutputPolydata,m_Input->GetTimeStamp());

	GetLogicManager()->CameraUpdate();

	if (wait)
		delete wait;
}
