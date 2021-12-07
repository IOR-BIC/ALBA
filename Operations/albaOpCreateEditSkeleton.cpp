/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateEditSkeleton
 Authors: Matteo Giacomoni
 
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

#include "albaOpCreateEditSkeleton.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEPolylineGraph.h"
#include "albaGeometryEditorPolylineGraph.h"
#include "albaTagArray.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateEditSkeleton);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateEditSkeleton::albaOpCreateEditSkeleton(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = true;

	m_Editor  = NULL;
	m_Skeleton = NULL;
	m_ResultPolydata = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateEditSkeleton::~albaOpCreateEditSkeleton()
//----------------------------------------------------------------------------
{
	cppDEL(m_Editor);
	albaDEL(m_Skeleton);
	vtkDEL(m_ResultPolydata);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateEditSkeleton::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new albaOpCreateEditSkeleton(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateEditSkeleton::InternalAccept(albaVME* vme)
//----------------------------------------------------------------------------
{
	return (vme != NULL && (vme->IsALBAType(albaVMEVolumeGray)||(vme->IsALBAType(albaVMEPolylineGraph)&&vme->GetParent()->IsALBAType(albaVMEVolumeGray))));
}
//----------------------------------------------------------------------------
void albaOpCreateEditSkeleton::OpRun()
//----------------------------------------------------------------------------
{
	albaNEW(m_Skeleton);

	if(m_Input->IsALBAType(albaVMEVolumeGray))
    m_Editor = new albaGeometryEditorPolylineGraph(m_Input, this, 0, this->m_TestMode);
	else if(m_Input->IsALBAType(albaVMEPolylineGraph) && m_Input->GetParent()->IsALBAType(albaVMEVolumeGray))
    m_Editor = new albaGeometryEditorPolylineGraph(m_Input->GetParent(), this,albaVMEPolylineGraph::SafeDownCast(m_Input),this->m_TestMode);
	else
    OpStop(OP_RUN_CANCEL);

  m_Editor->Show(true);

	// Added test compatibility
  if (!m_TestMode)
  {
    // no test case
    CreateGui();
	  ShowGui();
  }
  else
  {
    // test case
    OpStop(OP_RUN_OK);
  }
}
//----------------------------------------------------------------------------
void albaOpCreateEditSkeleton::OpDo()
//----------------------------------------------------------------------------
{
	if(m_Input->IsALBAType(albaVMEVolumeGray))
	{
		m_Skeleton->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
		m_Skeleton->SetName("VME Skeleton");

    albaTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("SYNTHETIC");

    m_Skeleton->GetTagArray()->SetTag(tag_Nature);

		m_Skeleton->ReparentTo(m_Input);
	}
	else if(m_Input->IsALBAType(albaVMEPolylineGraph))
	{
		albaVMEPolylineGraph::SafeDownCast(m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	}
	
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpCreateEditSkeleton::OpUndo()
//----------------------------------------------------------------------------
{
	m_Skeleton->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void albaOpCreateEditSkeleton::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);
	m_Gui->AddGui(m_Editor->GetGui());

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");
}
//----------------------------------------------------------------------------
void albaOpCreateEditSkeleton::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
		case wxOK:
			{
				OpStop(OP_RUN_OK);
			}
			break;
		case wxCANCEL:
			{
				OpStop(OP_RUN_CANCEL);
			}
			break;
		default:
			albaEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void albaOpCreateEditSkeleton::OpStop(int result)
//----------------------------------------------------------------------------
{
	m_Editor->Show(false);
	if(result==OP_RUN_OK)
	{
		m_ResultPolydata = m_Editor->GetOutput();
	}

	cppDEL(m_Editor);

	if (!m_TestMode)
  {
    HideGui();
  }
	albaEventMacro(albaEvent(this,result));
}