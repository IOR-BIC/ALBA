/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpCropDeformableROI.cpp,v $
Language:  C++
Date:      $Date: 2008-07-25 10:31:18 $
Version:   $Revision: 1.3 $
Authors:   Matteo Giacomoni - Daniele Giunchi
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

#include "medOpCropDeformableROI.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafNode.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"

#include "vtkMaskPolydataFilter.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpCropDeformableROI);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpCropDeformableROI::medOpCropDeformableROI(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_MaskPolydataFilter = NULL;
	m_ResultVme = NULL;

	m_Distance = 0.0;
	m_InsideOut = 0;
	m_MaxDistance = sqrt(1.0e29)/3.0;
	m_FillValue = 0.0;
	m_PNode = NULL;
  m_Surface = NULL;
}
//----------------------------------------------------------------------------
medOpCropDeformableROI::~medOpCropDeformableROI()
//----------------------------------------------------------------------------
{
	vtkDEL(m_MaskPolydataFilter);
	mafDEL(m_ResultVme);

  if(mafVMESurfaceParametric::SafeDownCast(m_Input))
    mafDEL(m_Surface);
}
//----------------------------------------------------------------------------
bool medOpCropDeformableROI::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("mafVMEVolumeGray"));
}
//----------------------------------------------------------------------------
mafOp *medOpCropDeformableROI::Copy()   
//----------------------------------------------------------------------------
{
	return (new medOpCropDeformableROI(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_CHOOSE_MASK = MINID,
	ID_DISTANCE,
	ID_FILL_VALUE,
	ID_MAX_DISTANCE,
	ID_INSIDE_OUT,
};
//----------------------------------------------------------------------------
void medOpCropDeformableROI::OpRun()   
//----------------------------------------------------------------------------
{
	mafNEW(m_ResultVme);
	m_ResultVme->DeepCopy(m_Input);

	vtkNEW(m_MaskPolydataFilter);

	// interface:
	m_Gui = new mafGUI(this);

	m_Gui->Button(ID_CHOOSE_MASK,_("Choose mask"));

	m_Gui->Double(ID_DISTANCE,_("distance"),&m_Distance,0.0);
	m_Gui->Double(ID_FILL_VALUE,_("fill value"),&m_FillValue);
	m_Gui->Double(ID_MAX_DISTANCE,_("max dist."),&m_MaxDistance,0.0);
	m_Gui->Bool(ID_INSIDE_OUT,_("mask inside out"),&m_InsideOut);

	m_Gui->OkCancel();

	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void medOpCropDeformableROI::OpDo()
//----------------------------------------------------------------------------
{
	m_ResultVme->ReparentTo(m_Input->GetRoot());
}
//----------------------------------------------------------------------------
void medOpCropDeformableROI::OpUndo()
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_REMOVE,m_ResultVme));
}
//----------------------------------------------------------------------------
void medOpCropDeformableROI::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		mafNode *n=NULL;
		switch(e->GetId())
		{	
		case ID_CHOOSE_MASK:
			{
				mafString title = _("Choose mask");
				e->SetId(VME_CHOOSE);
        e->SetArg((long)&medOpCropDeformableROI::OutputSurfaceAccept);
				e->SetString(&title);
				mafEventMacro(*e);
				m_PNode = e->GetVme();
			}
			break;
		case wxOK:
			if(m_PNode!=NULL)
				{
					Algorithm(mafVME::SafeDownCast(m_PNode));
				}

			OpStop(OP_RUN_OK);        
			break;
		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);        
			break;
		}
	}
}
//----------------------------------------------------------------------------
void medOpCropDeformableROI::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void medOpCropDeformableROI::Algorithm(mafVME *vme)
//----------------------------------------------------------------------------
{
	if(vme)
	{
		mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(m_Input);
		m_MaskPolydataFilter->SetInput(volume->GetOutput()->GetVTKData());
		m_MaskPolydataFilter->SetDistance(m_Distance);
		m_MaskPolydataFilter->SetFillValue(m_FillValue);
		m_MaskPolydataFilter->SetMaximumDistance(m_MaxDistance);
		m_MaskPolydataFilter->SetFillValue(m_FillValue);
		m_MaskPolydataFilter->SetInsideOut(m_InsideOut);
		
    if(m_Surface = mafVMESurface::SafeDownCast(vme));
    else
    {
      mafNEW(m_Surface);
      vtkPolyData *poly = (vtkPolyData *)(mafVMESurfaceParametric::SafeDownCast(vme)->GetSurfaceOutput()->GetVTKData());
      m_Surface->SetData(poly,0);
    }
		if(!m_Surface)
			return;
    	  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,m_MaskPolydataFilter));

    m_Surface->Update();
		m_MaskPolydataFilter->SetMask(vtkPolyData::SafeDownCast(m_Surface->GetOutput()->GetVTKData()));
		m_MaskPolydataFilter->Update();

		if(vtkRectilinearGrid::SafeDownCast(m_MaskPolydataFilter->GetOutput()))
			m_ResultVme->SetData(((vtkRectilinearGrid*)m_MaskPolydataFilter->GetOutput()),((mafVME*)m_Input)->GetTimeStamp());
		else if(vtkImageData::SafeDownCast(m_MaskPolydataFilter->GetOutput()))
			m_ResultVme->SetData(((vtkImageData*)m_MaskPolydataFilter->GetOutput()),((mafVME*)m_Input)->GetTimeStamp());
	
		m_ResultVme->Modified();
		m_ResultVme->Update();
	}
}
