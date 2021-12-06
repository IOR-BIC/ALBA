/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeMirror
 Authors: Gianluigi Crimi
 
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

#include "albaOpVolumeMirror.h"
#include <wx/busyinfo.h>

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkPointData.h"
#include "vtkDataSetWriter.h"
#include "vtkALBASmartPointer.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpVolumeMirror);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpVolumeMirror::albaOpVolumeMirror(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType			 		= OPTYPE_OP;
	m_Canundo			 		= true;
	m_InputPreserving = false; //Natural_preserving
	
	m_OutputImageData = NULL;
	m_InputImageData		= NULL;
  
  m_MirrorX      = 1;
  m_MirrorY      = 0;
  m_MirrorZ      = 0;
  m_FlipNormals = 0;
}
//----------------------------------------------------------------------------
albaOpVolumeMirror::~albaOpVolumeMirror( ) 
//----------------------------------------------------------------------------
{
	vtkDEL(m_InputImageData);
	vtkDEL(m_OutputImageData);
}
//----------------------------------------------------------------------------
albaOp* albaOpVolumeMirror::Copy()   
//----------------------------------------------------------------------------
{
  albaOpVolumeMirror *cp = new albaOpVolumeMirror(m_Label);
  cp->m_Canundo		= m_Canundo;
  cp->m_OpType		= m_OpType;
  cp->m_Listener	= m_Listener;
  cp->m_Next			= NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool albaOpVolumeMirror::InternalAccept(albaVME* node)   
//----------------------------------------------------------------------------
{ return  ( node && node->IsALBAType(albaVMEVolumeGray) && node->GetOutput()->GetVTKData()->IsA("vtkImageData")	);
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
void albaOpVolumeMirror::OpRun()   
//----------------------------------------------------------------------------
{  
    
	vtkNEW(m_InputImageData);
	m_InputImageData->DeepCopy((vtkImageData*)((albaVMEVolumeGray *)m_Input)->GetOutput()->GetVTKData());
	
	vtkNEW(m_OutputImageData);
	m_OutputImageData->DeepCopy((vtkImageData*)((albaVMEVolumeGray *)m_Input)->GetOutput()->GetVTKData());
	

	if(!m_TestMode)
	{
		// interface:
		m_Gui = new albaGUI(this);
		m_Gui->SetListener(this);

		m_Gui->HintBox(NULL, "This doesn't work on animated vme");
// 		m_Gui->Label("this doesn't work on animated vme");
// 		m_Gui->Label("");
		
		m_Gui->Bool(ID_MIRRORX,"Mirror x coords", &m_MirrorX, 1);
		m_Gui->Bool(ID_MIRRORY,"Mirror y coords", &m_MirrorY, 1);
		m_Gui->Bool(ID_MIRRORZ,"Mirror z coords", &m_MirrorZ, 1);

		//////////////////////////////////////////////////////////////////////////
		m_Gui->Label("");
		m_Gui->Divider(1);
		m_Gui->OkCancel();
		m_Gui->Label("");

		ShowGui();
	}

  Preview();
}
//----------------------------------------------------------------------------
void albaOpVolumeMirror::OpDo()
//----------------------------------------------------------------------------
{
  assert(m_OutputImageData);

	((albaVMEVolumeGray *)m_Input)->SetData(m_OutputImageData,m_Input->GetTimeStamp());
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void albaOpVolumeMirror::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_InputImageData);

	((albaVMEVolumeGray *)m_Input)->SetData(m_InputImageData,m_Input->GetTimeStamp());
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void albaOpVolumeMirror::OnEvent(albaEventBase *alba_event)
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
void albaOpVolumeMirror::OpStop(int result)
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
void albaOpVolumeMirror::Preview()  
//----------------------------------------------------------------------------
{
	wxBusyCursor *wait=NULL;
	if(!m_TestMode)
	{
		wait=new wxBusyCursor();
	}
	
	//creating a copy of the input
  m_OutputImageData->DeepCopy(m_InputImageData);
	vtkDataArray *inScalars = m_InputImageData->GetPointData()->GetScalars();
	vtkDataArray *outScalars = m_OutputImageData->GetPointData()->GetScalars();

	int outX, outY, outZ;
	int *dims = m_InputImageData->GetDimensions();

	//invert out values if m_Mirror is selected
	outX = m_MirrorX ? dims[0] - 1 : 0;
	for (int inX = 0; inX < dims[0]; inX++)
	{
		outY = m_MirrorY ? dims[1] - 1 : 0;
		for (int inY = 0; inY < dims[1]; inY++)
		{
			outZ = m_MirrorZ ? dims[2] - 1 : 0;
			for (int inZ = 0; inZ < dims[2]; inZ++)
			{
				//assign
				int inP = inX + inY*dims[0] + inZ*dims[0] * dims[1];
				int outP = outX + outY*dims[0] + outZ*dims[0] * dims[1];
				outScalars->SetTuple1(outP, inScalars->GetTuple1(inP));
				
				m_MirrorZ ? outZ-- : outZ++;
			}
			m_MirrorY ? outY-- : outY++;
		}
		m_MirrorX ? outX-- : outX++;
	}
	outScalars->Modified();

	m_OutputImageData->Update();
	
  ((albaVMEVolumeGray *)m_Input)->SetData(m_OutputImageData,m_Input->GetTimeStamp());

  albaEventMacro(albaEvent(this, CAMERA_UPDATE));

	if (wait)
		delete wait;
}
