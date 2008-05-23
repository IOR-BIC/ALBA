/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpCreateEditSkeleton.cpp,v $
Language:  C++
Date:      $Date: 2008-05-23 09:25:42 $
Version:   $Revision: 1.7 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpCreateEditSkeleton.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "medVMEPolylineGraph.h"
#include "medGeometryEditorPolylineGraph.h"
#include "mafTagArray.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpCreateEditSkeleton);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpCreateEditSkeleton::medOpCreateEditSkeleton(wxString label) :
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
medOpCreateEditSkeleton::~medOpCreateEditSkeleton()
//----------------------------------------------------------------------------
{
	cppDEL(m_Editor);
	mafDEL(m_Skeleton);
	vtkDEL(m_ResultPolydata);
}
//----------------------------------------------------------------------------
mafOp* medOpCreateEditSkeleton::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new medOpCreateEditSkeleton(m_Label);
}
//----------------------------------------------------------------------------
bool medOpCreateEditSkeleton::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return (vme != NULL && (vme->IsMAFType(mafVMEVolumeGray)||(vme->IsMAFType(medVMEPolylineGraph)&&vme->GetParent()->IsMAFType(mafVMEVolumeGray))));
}
//----------------------------------------------------------------------------
void medOpCreateEditSkeleton::OpRun()
//----------------------------------------------------------------------------
{
	mafNEW(m_Skeleton);

	if(m_Input->IsMAFType(mafVMEVolumeGray))
		m_Editor = new medGeometryEditorPolylineGraph(mafVME::SafeDownCast(m_Input), this);
	else if(m_Input->IsMAFType(medVMEPolylineGraph) && m_Input->GetParent()->IsMAFType(mafVMEVolumeGray))
		m_Editor = new medGeometryEditorPolylineGraph(mafVME::SafeDownCast(m_Input->GetParent()), this,medVMEPolylineGraph::SafeDownCast(m_Input));
	else
    OpStop(OP_RUN_CANCEL);

  m_Editor->Show(true);

	CreateGui();
	ShowGui();
}
//----------------------------------------------------------------------------
void medOpCreateEditSkeleton::OpDo()
//----------------------------------------------------------------------------
{
	if(m_Input->IsMAFType(mafVMEVolumeGray))
	{
		m_Skeleton->SetData(m_ResultPolydata,((mafVME*)m_Input)->GetTimeStamp());
		m_Skeleton->SetName("VME Skeleton");

    mafTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("SYNTHETIC");

    m_Skeleton->GetTagArray()->SetTag(tag_Nature);

		m_Skeleton->ReparentTo(m_Input);
	}
	else if(m_Input->IsMAFType(medVMEPolylineGraph))
	{
		medVMEPolylineGraph::SafeDownCast(m_Input)->SetData(m_ResultPolydata,((mafVME*)m_Input)->GetTimeStamp());
	}
	
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpCreateEditSkeleton::OpUndo()
//----------------------------------------------------------------------------
{
	m_Skeleton->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void medOpCreateEditSkeleton::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);
	m_Gui->AddGui(m_Editor->GetGui());
	m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
void medOpCreateEditSkeleton::OnEvent(mafEventBase *maf_event)
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
void medOpCreateEditSkeleton::OpStop(int result)
//----------------------------------------------------------------------------
{
	m_Editor->Show(false);
	if(result==OP_RUN_OK)
	{
		m_ResultPolydata = m_Editor->GetOutput();
	}

	cppDEL(m_Editor);

	HideGui();
	mafEventMacro(mafEvent(this,result));
}