/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoSurfaceReconstruction.cpp,v $
Language:  C++
Date:      $Date: 2007-05-09 08:41:12 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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

#include "mmoSurfaceReconstruction.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafNode.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"

#include "vtkPowerCrustSurfaceReconstruction.h"
//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoSurfaceReconstruction);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoSurfaceReconstruction::mmoSurfaceReconstruction(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_ReconstructionFilter	= NULL;
	m_OutputSurface					= NULL;
	m_OutputMedial					= NULL;

	m_LabelRecostrution = _("");
	m_LabelMedial				= _("");
}
//----------------------------------------------------------------------------
mmoSurfaceReconstruction::~mmoSurfaceReconstruction()
//----------------------------------------------------------------------------
{
	mafDEL(m_OutputMedial);
	mafDEL(m_OutputSurface);
	vtkDEL(m_ReconstructionFilter);
}
//----------------------------------------------------------------------------
bool mmoSurfaceReconstruction::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	//return (node && node->IsA("mafVMELandmarkCloud"));
	return (node && node->IsA("mafVMESurface"));
}
//----------------------------------------------------------------------------
mafOp *mmoSurfaceReconstruction::Copy()   
//----------------------------------------------------------------------------
{
	return (new mmoSurfaceReconstruction(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_CREATE = MINID,
};
//----------------------------------------------------------------------------
void mmoSurfaceReconstruction::OpRun()   
//----------------------------------------------------------------------------
{  
	//interface:
	m_Gui = new mmgGui(this);

	m_Gui->Button(ID_CREATE,_("Create Surfaces"));

	m_Gui->Label(_("recostr."),&m_LabelRecostrution);
	m_Gui->Label(_("medial"),&m_LabelMedial);
	
	m_Gui->OkCancel();
	m_Gui->Enable(wxOK,false);

	ShowGui();
}
//----------------------------------------------------------------------------
void mmoSurfaceReconstruction::CreateSurfaces()
//----------------------------------------------------------------------------
{
	vtkPolyData *data=vtkPolyData::SafeDownCast(((mafVMESurface*)m_Input)->GetOutput()->GetVTKData());

	vtkPoints *points=data->GetPoints();
	vtkCellArray *polys;
	vtkNEW(polys);
	for(int j=0; j < data->GetNumberOfPoints(); j++)
	{
		polys->InsertNextCell(1);
		polys->InsertCellPoint(j);
	}
	vtkPolyData *inputFilter;
	vtkNEW(inputFilter);
	inputFilter->SetPoints(points);
	inputFilter->SetVerts(polys);
	inputFilter->Update();

	vtkNEW(m_ReconstructionFilter);
	m_ReconstructionFilter->SetInput(inputFilter);
	m_ReconstructionFilter->Update();

	int result;

	mafNEW(m_OutputSurface);
	m_OutputSurface->SetName("New_Surf");
	result = m_OutputSurface->SetData(vtkPolyData::SafeDownCast(m_ReconstructionFilter->GetOutput()),0);
	if(result == MAF_ERROR)
	{
		m_LabelRecostrution = _(" NO");
		mafDEL(m_OutputSurface);
	}
	else
	{
		m_LabelRecostrution = _(" OK");
		m_OutputSurface->Update();
	}

	mafNEW(m_OutputMedial);
	m_OutputMedial->SetName("New_Surf_Medial");
	result = m_OutputMedial->SetData(vtkPolyData::SafeDownCast(m_ReconstructionFilter->GetMedialSurface()),0);
	if(result == MAF_ERROR)
	{
		m_LabelMedial = _(" NO");
		mafDEL(m_OutputMedial);
	}
	else
	{
		m_LabelMedial = _(" OK");
		m_OutputMedial->Update();
	}

	m_Gui->Enable(wxOK,true);

	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmoSurfaceReconstruction::OpDo()
//----------------------------------------------------------------------------
{
	if(m_OutputSurface)
		m_OutputSurface->ReparentTo(m_Input->GetParent());
	if(m_OutputMedial)
		m_OutputMedial->ReparentTo(m_Input->GetParent());
}
//----------------------------------------------------------------------------
void mmoSurfaceReconstruction::OpUndo()
//----------------------------------------------------------------------------
{
	if(m_OutputSurface)
		m_OutputSurface->ReparentTo(NULL);
	if(m_OutputMedial)
		m_OutputMedial->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void mmoSurfaceReconstruction::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ID_CREATE:
			{
				CreateSurfaces();
			}
			break;
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
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mmoSurfaceReconstruction::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	vtkDEL(m_ReconstructionFilter);

	mafEventMacro(mafEvent(this,result));        
}