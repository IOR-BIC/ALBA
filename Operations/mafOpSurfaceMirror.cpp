/*=========================================================================

 Program: MAF2
 Module: mafOpSurfaceMirror
 Authors: Paolo Quadrani - porting  Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpSurfaceMirror.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"


#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"

#include "vtkPolyData.h"
#include "vtkMAFPolyDataMirror.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpSurfaceMirror);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpSurfaceMirror::mafOpSurfaceMirror(wxString label) :
mafOp(label)
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
mafOpSurfaceMirror::~mafOpSurfaceMirror( ) 
//----------------------------------------------------------------------------
{
	vtkDEL(m_InputPolydata);
	vtkDEL(m_OutputPolydata);
	vtkDEL(m_MirrorFilter);
}
//----------------------------------------------------------------------------
mafOp* mafOpSurfaceMirror::Copy()   
//----------------------------------------------------------------------------
{
  mafOpSurfaceMirror *cp = new mafOpSurfaceMirror(m_Label);
  cp->m_Canundo		= m_Canundo;
  cp->m_OpType		= m_OpType;
  cp->m_Listener	= m_Listener;
  cp->m_Next			= NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool mafOpSurfaceMirror::Accept(mafNode* node)   
//----------------------------------------------------------------------------
{ return  ( 
		      node
					
					&& 
					(
							node->IsMAFType(mafVMESurface) 
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
	ID_HELP,
};
//----------------------------------------------------------------------------
void mafOpSurfaceMirror::OpRun()   
//----------------------------------------------------------------------------
{  
    
	vtkNEW(m_InputPolydata);
	m_InputPolydata->DeepCopy((vtkPolyData*)((mafVMESurface *)m_Input)->GetOutput()->GetVTKData());
	
	vtkNEW(m_OutputPolydata);
	m_OutputPolydata->DeepCopy((vtkPolyData*)((mafVMESurface *)m_Input)->GetOutput()->GetVTKData());
	

	if(!m_TestMode)
	{
		// interface:
		m_Gui = new mafGUI(this);
		m_Gui->SetListener(this);
		
		mafEvent buildHelpGui;
		buildHelpGui.SetSender(this);
		buildHelpGui.SetId(GET_BUILD_HELP_GUI);
		mafEventMacro(buildHelpGui);

		if (buildHelpGui.GetArg() == true)
		{
			m_Gui->Button(ID_HELP, "Help","");	
		}

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

  m_MirrorFilter = vtkMAFPolyDataMirror::New();
  m_MirrorFilter->SetInput(m_InputPolydata);

  Preview();
}
//----------------------------------------------------------------------------
void mafOpSurfaceMirror::OpDo()
//----------------------------------------------------------------------------
{
  assert(m_OutputPolydata);

	((mafVMESurface *)m_Input)->SetData(m_OutputPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpSurfaceMirror::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_InputPolydata);

	((mafVMESurface *)m_Input)->SetData(m_InputPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpSurfaceMirror::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
	  switch(e->GetId())
	  {
			case ID_HELP:
			{
				mafEvent helpEvent;
				helpEvent.SetSender(this);
				mafString operationLabel = this->m_Label;
				helpEvent.SetString(&operationLabel);
				helpEvent.SetId(OPEN_HELP_PAGE);
				mafEventMacro(helpEvent);
			}
			break;

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
				mafEventMacro(*e);
			break; 
	  }
	}  
}
//----------------------------------------------------------------------------
void mafOpSurfaceMirror::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(result == OP_RUN_CANCEL) OpUndo();

	if(!m_TestMode)
	{
	  HideGui();
	  delete m_Gui;
	}
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpSurfaceMirror::Preview()  
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
  ((mafVMESurface *)m_Input)->SetData(m_OutputPolydata,((mafVME *)m_Input)->GetTimeStamp());


  mafEventMacro(mafEvent(this, CAMERA_UPDATE));

	if (wait)
		delete wait;
}
