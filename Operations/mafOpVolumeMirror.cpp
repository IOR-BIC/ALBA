/*=========================================================================

 Program: MAF2
 Module: mafOpVolumeMirror
 Authors: Gianluigi Crimi
 
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

#include "mafOpVolumeMirror.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkPointData.h"
#include "vtkDataSetWriter.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpVolumeMirror);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpVolumeMirror::mafOpVolumeMirror(wxString label) :
mafOp(label)
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
mafOpVolumeMirror::~mafOpVolumeMirror( ) 
//----------------------------------------------------------------------------
{
	vtkDEL(m_InputImageData);
	vtkDEL(m_OutputImageData);
}
//----------------------------------------------------------------------------
mafOp* mafOpVolumeMirror::Copy()   
//----------------------------------------------------------------------------
{
  mafOpVolumeMirror *cp = new mafOpVolumeMirror(m_Label);
  cp->m_Canundo		= m_Canundo;
  cp->m_OpType		= m_OpType;
  cp->m_Listener	= m_Listener;
  cp->m_Next			= NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool mafOpVolumeMirror::Accept(mafVME* node)   
//----------------------------------------------------------------------------
{ return  ( node && node->IsMAFType(mafVMEVolumeGray) && node->GetOutput()->GetVTKData()->IsA("vtkImageData")	);
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
void mafOpVolumeMirror::OpRun()   
//----------------------------------------------------------------------------
{  
    
	vtkNEW(m_InputImageData);
	m_InputImageData->DeepCopy((vtkImageData*)((mafVMEVolumeGray *)m_Input)->GetOutput()->GetVTKData());
	
	vtkNEW(m_OutputImageData);
	m_OutputImageData->DeepCopy((vtkImageData*)((mafVMEVolumeGray *)m_Input)->GetOutput()->GetVTKData());
	

	if(!m_TestMode)
	{
		// interface:
		m_Gui = new mafGUI(this);
		m_Gui->SetListener(this);

		m_Gui->Label("this doesn't work on animated vme");
		m_Gui->Label("");
		
		m_Gui->Bool(ID_MIRRORX,"mirror x coords", &m_MirrorX, 1);
		m_Gui->Bool(ID_MIRRORY,"mirror y coords", &m_MirrorY, 1);
		m_Gui->Bool(ID_MIRRORZ,"mirror z coords", &m_MirrorZ, 1);

		m_Gui->Label("");
		m_Gui->OkCancel();

		ShowGui();
	}

  Preview();
}
//----------------------------------------------------------------------------
void mafOpVolumeMirror::OpDo()
//----------------------------------------------------------------------------
{
  assert(m_OutputImageData);

	((mafVMEVolumeGray *)m_Input)->SetData(m_OutputImageData,m_Input->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpVolumeMirror::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_InputImageData);

	((mafVMEVolumeGray *)m_Input)->SetData(m_InputImageData,m_Input->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpVolumeMirror::OnEvent(mafEventBase *maf_event)
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
void mafOpVolumeMirror::OpStop(int result)
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
void mafOpVolumeMirror::Preview()  
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
	
  ((mafVMEVolumeGray *)m_Input)->SetData(m_OutputImageData,m_Input->GetTimeStamp());

  mafEventMacro(mafEvent(this, CAMERA_UPDATE));

	if (wait)
		delete wait;
}
