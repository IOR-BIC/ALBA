/*=========================================================================

 Program: MAF2
 Module: mafOpCreateEditSkeleton
 Authors: Matteo Giacomoni
 
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

#include "mafOpCreateEditSkeleton.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEPolylineGraph.h"
#include "mafGeometryEditorPolylineGraph.h"
#include "mafTagArray.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateEditSkeleton);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateEditSkeleton::mafOpCreateEditSkeleton(wxString label) :
mafOp(label)
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
mafOpCreateEditSkeleton::~mafOpCreateEditSkeleton()
//----------------------------------------------------------------------------
{
	cppDEL(m_Editor);
	mafDEL(m_Skeleton);
	vtkDEL(m_ResultPolydata);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateEditSkeleton::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new mafOpCreateEditSkeleton(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCreateEditSkeleton::Accept(mafVME* vme)
//----------------------------------------------------------------------------
{
	return (vme != NULL && (vme->IsMAFType(mafVMEVolumeGray)||(vme->IsMAFType(mafVMEPolylineGraph)&&vme->GetParent()->IsMAFType(mafVMEVolumeGray))));
}
//----------------------------------------------------------------------------
void mafOpCreateEditSkeleton::OpRun()
//----------------------------------------------------------------------------
{
	mafNEW(m_Skeleton);

	if(m_Input->IsMAFType(mafVMEVolumeGray))
    m_Editor = new mafGeometryEditorPolylineGraph(m_Input, this, 0, this->m_TestMode);
	else if(m_Input->IsMAFType(mafVMEPolylineGraph) && m_Input->GetParent()->IsMAFType(mafVMEVolumeGray))
    m_Editor = new mafGeometryEditorPolylineGraph(m_Input->GetParent(), this,mafVMEPolylineGraph::SafeDownCast(m_Input),this->m_TestMode);
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
void mafOpCreateEditSkeleton::OpDo()
//----------------------------------------------------------------------------
{
	if(m_Input->IsMAFType(mafVMEVolumeGray))
	{
		m_Skeleton->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
		m_Skeleton->SetName("VME Skeleton");

    mafTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("SYNTHETIC");

    m_Skeleton->GetTagArray()->SetTag(tag_Nature);

		m_Skeleton->ReparentTo(m_Input);
	}
	else if(m_Input->IsMAFType(mafVMEPolylineGraph))
	{
		mafVMEPolylineGraph::SafeDownCast(m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	}
	
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpCreateEditSkeleton::OpUndo()
//----------------------------------------------------------------------------
{
	m_Skeleton->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void mafOpCreateEditSkeleton::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
	m_Gui->AddGui(m_Editor->GetGui());
	m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
void mafOpCreateEditSkeleton::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
			mafEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void mafOpCreateEditSkeleton::OpStop(int result)
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
	mafEventMacro(mafEvent(this,result));
}