/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpSurfaceMirror.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-21 12:01:37 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpSurfaceMirror.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"


#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"

#include "vtkPolyData.h"
#include "vtkMEDPolyDataMirror.h"

//----------------------------------------------------------------------------
medOpSurfaceMirror::medOpSurfaceMirror(wxString label) :
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
medOpSurfaceMirror::~medOpSurfaceMirror( ) 
//----------------------------------------------------------------------------
{
	vtkDEL(m_InputPolydata);
	vtkDEL(m_OutputPolydata);
}
//----------------------------------------------------------------------------
mafOp* medOpSurfaceMirror::Copy()   
//----------------------------------------------------------------------------
{
  medOpSurfaceMirror *cp = new medOpSurfaceMirror(m_Label);
  cp->m_Canundo		= m_Canundo;
  cp->m_OpType		= m_OpType;
  cp->m_Listener	= m_Listener;
  cp->m_Next			= NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool medOpSurfaceMirror::Accept(mafNode* node)   
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
{
	ID_MIRRORX = MINID,
	ID_MIRRORY,
	ID_MIRRORZ,
	ID_FLIPNORMALS,
};
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OpRun()   
//----------------------------------------------------------------------------
{  
    
	vtkNEW(m_InputPolydata);
	m_InputPolydata->DeepCopy((vtkPolyData*)((mafVMESurface *)m_Input)->GetOutput()->GetVTKData());
	
	vtkNEW(m_OutputPolydata);
	m_OutputPolydata->DeepCopy((vtkPolyData*)((mafVMESurface *)m_Input)->GetOutput()->GetVTKData());
	

	// interface:
	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);
	m_Gui->Label("this doesn't work on animated vme");
	m_Gui->Label("");
	
	m_Gui->Bool(ID_MIRRORX,"mirror x coords", &m_MirrorX, 1);
	m_Gui->Bool(ID_MIRRORY,"mirror y coords", &m_MirrorY, 1);
	m_Gui->Bool(ID_MIRRORZ,"mirror z coords", &m_MirrorZ, 1);
	//m_Gui->Bool(ID_FLIPNORMALS,"flip normals",&m_flip_normals,1);
	m_Gui->Label("");
	m_Gui->OkCancel();

	ShowGui();


  m_MirrorFilter = vtkMEDPolyDataMirror::New();
  //mafProgressMacro(mirrorFilter,"mirroring surface");
  m_MirrorFilter->SetInput(m_InputPolydata);
  m_MirrorFilter->SetMirrorXCoordinate(m_MirrorX);
  m_MirrorFilter->SetMirrorYCoordinate(m_MirrorY);
  m_MirrorFilter->SetMirrorZCoordinate(m_MirrorZ);
  //mirrorFilter->SetFlipNormals(m_FlipNormals);
  m_MirrorFilter->Update();
	
  m_OutputPolydata->DeepCopy(m_MirrorFilter->GetOutput());
  m_OutputPolydata->Update();

  ((mafVMESurface *)m_Input)->SetData(m_OutputPolydata,((mafVME *)m_Input)->GetTimeStamp());

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OpDo()
//----------------------------------------------------------------------------
{
  assert(m_OutputPolydata);


	((mafVMESurface *)m_Input)->SetData(m_OutputPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_InputPolydata);


	((mafVMESurface *)m_Input)->SetData(m_InputPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
				mafEventMacro(*e);
			break; 
	  }
	}  
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(result == OP_RUN_CANCEL) OpUndo();

  vtkDEL(m_MirrorFilter);
	HideGui();
	delete m_Gui;
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::Preview()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

  m_MirrorFilter->SetMirrorXCoordinate(m_MirrorX);
  m_MirrorFilter->SetMirrorYCoordinate(m_MirrorY);
  m_MirrorFilter->SetMirrorZCoordinate(m_MirrorZ);
  //mirrorFilter->SetFlipNormals(m_FlipNormals);
  m_MirrorFilter->Update();
  

  m_OutputPolydata->DeepCopy(m_MirrorFilter->GetOutput());
  m_OutputPolydata->Update();
  ((mafVMESurface *)m_Input)->SetData(m_OutputPolydata,((mafVME *)m_Input)->GetTimeStamp());


  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
