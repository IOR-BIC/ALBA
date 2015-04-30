/*=========================================================================

 Program: MAF2Medical
 Module: mafOpCropDeformableROI
 Authors: Matteo Giacomoni - Daniele Giunchi
 
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

#include "mafOpCropDeformableROI.h"
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
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "wx\busyinfo.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCropDeformableROI);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCropDeformableROI::mafOpCropDeformableROI(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_MaskPolydataFilter = NULL;
	m_ResultVme = NULL;

	m_Distance = 0.0;
	m_InsideOut = 0;
	m_FillValue = 0.0;
	m_PNode = NULL;
}
//----------------------------------------------------------------------------
mafOpCropDeformableROI::~mafOpCropDeformableROI()
//----------------------------------------------------------------------------
{
	vtkDEL(m_MaskPolydataFilter);
	mafDEL(m_ResultVme);

}
//----------------------------------------------------------------------------
bool mafOpCropDeformableROI::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("mafVMEVolumeGray"));
}
//----------------------------------------------------------------------------
mafOp *mafOpCropDeformableROI::Copy()   
//----------------------------------------------------------------------------
{
	return (new mafOpCropDeformableROI(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_DISTANCE  = MINID,
	ID_FILL_VALUE,
	ID_MAX_DISTANCE,
	ID_INSIDE_OUT,
};
//----------------------------------------------------------------------------
void mafOpCropDeformableROI::OpRun()   
//----------------------------------------------------------------------------
{
	//Create GUI 
	CreateGui();

	//Select mask surface for the operation
	MaskSelection();

	//If a surface is selected show GUI 
	//else stop operation (cannot run without surface)
	if (m_PNode)
		ShowGui();
	else 
		OpStop(OP_RUN_CANCEL);
}
//----------------------------------------------------------------------------
void mafOpCropDeformableROI::OpDo()
//----------------------------------------------------------------------------
{
	m_ResultVme->ReparentTo(m_Input->GetRoot());
}
//----------------------------------------------------------------------------
void mafOpCropDeformableROI::OpUndo()
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_REMOVE,m_ResultVme));
}
//----------------------------------------------------------------------------
void mafOpCropDeformableROI::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		mafNode *n=NULL;
		switch(e->GetId())
		{	
		case wxOK:
			//Run algorithm and stop operation
			Algorithm(mafVME::SafeDownCast(m_PNode));
			OpStop(OP_RUN_OK);        
			break;
		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);        
			break;
		}
	}
}
//----------------------------------------------------------------------------
void mafOpCropDeformableROI::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpCropDeformableROI::Algorithm(mafVME *vme)
//----------------------------------------------------------------------------
{
	if(vme)
	{
		wxBusyInfo *busyInfo=NULL;;
		if(GetTestMode() == false)
		{
				busyInfo = new wxBusyInfo("Please Wait...");
		}
		vtkPolyData *maskPolydata=NULL;
		vtkPolyData *transformedMaskPolydata=NULL;
		vtkTransform *transform = NULL;
		vtkTransformFilter *transformFilter = NULL;


		vme->Update();

		if(mafVMESurface::SafeDownCast(vme))
		{
			maskPolydata=(vtkPolyData *)(mafVMESurface::SafeDownCast(vme)->GetOutput()->GetVTKData());
		}
		if (mafVMESurfaceParametric::SafeDownCast(vme))
		{
			maskPolydata = (vtkPolyData *)(mafVMESurfaceParametric::SafeDownCast(vme)->GetSurfaceOutput()->GetVTKData());
		}
		
		if(!maskPolydata)
			return;


		
		mafNEW(m_ResultVme);
		m_ResultVme->DeepCopy(m_Input);
		mafString resultName = "Masked ";
		resultName+=m_Input->GetName();
		m_ResultVme->SetName(resultName);

		vtkPolyData *Mask;

		mafMatrix identityMatrix;
		mafMatrix maskABSMatrix = mafVME::SafeDownCast(vme)->GetAbsMatrixPipe()->GetMatrix();
		mafMatrix volumeABSMatrix = mafVME::SafeDownCast(m_Input)->GetAbsMatrixPipe()->GetMatrix();

		bool isMaskMatrixIdentity = maskABSMatrix.Equals(&identityMatrix);
		bool isVolumeMatrixIdentity = volumeABSMatrix.Equals(&identityMatrix);

		if (isMaskMatrixIdentity && isVolumeMatrixIdentity)
			transformedMaskPolydata=maskPolydata;
		else
		{		
			// if VME matrix is not identity apply it to dataset
			mafMatrix meshVolumeAlignMatrix;

			//Calculate align matrix 
			volumeABSMatrix.Invert();
			mafMatrix::Multiply4x4(volumeABSMatrix,maskABSMatrix,meshVolumeAlignMatrix);

			// apply abs matrix to geometry
			transform = vtkTransform::New();
			transform->SetMatrix(meshVolumeAlignMatrix.GetVTKMatrix());

			// to delete
			transformFilter = vtkTransformFilter::New();
			
			transformFilter->SetInput(maskPolydata);
			transformFilter->SetTransform(transform);
			transformFilter->Update();

			transformedMaskPolydata=transformFilter->GetPolyDataOutput();
		}


		vtkNEW(m_MaskPolydataFilter);
		mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(m_Input);
		m_MaskPolydataFilter->SetInput(volume->GetOutput()->GetVTKData());
		m_MaskPolydataFilter->SetDistance(m_Distance);
		m_MaskPolydataFilter->SetFillValue(m_FillValue);
		m_MaskPolydataFilter->SetInsideOut(m_InsideOut);
		m_MaskPolydataFilter->SetMask(transformedMaskPolydata);
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,m_MaskPolydataFilter));
		m_MaskPolydataFilter->Update();

		if(vtkRectilinearGrid::SafeDownCast(m_MaskPolydataFilter->GetOutput()))
			m_ResultVme->SetData(((vtkRectilinearGrid*)m_MaskPolydataFilter->GetOutput()),((mafVME*)m_Input)->GetTimeStamp());
		else if(vtkImageData::SafeDownCast(m_MaskPolydataFilter->GetOutput()))
			m_ResultVme->SetData(((vtkImageData*)m_MaskPolydataFilter->GetOutput()),((mafVME*)m_Input)->GetTimeStamp());
	
		m_ResultVme->Modified();
		m_ResultVme->Update();

		vtkDEL(transform);
		vtkDEL(transformFilter);
		cppDEL(busyInfo);
	}
}

void mafOpCropDeformableROI::CreateGui()
{

	// interface:
	m_Gui = new mafGUI(this);

	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Double(ID_DISTANCE,_("Distance"),&m_Distance,0.0);
	m_Gui->Double(ID_FILL_VALUE,_("Fill value"),&m_FillValue);
	m_Gui->Bool(ID_INSIDE_OUT,_("Mask inside"),&m_InsideOut);
	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();

	m_Gui->Divider();
}

void mafOpCropDeformableROI::MaskSelection()
{
	mafEvent *e; 
	e = new mafEvent();
	mafString title = _("Choose mask");
	e->SetId(VME_CHOOSE);
	e->SetArg((long)&mafOpCropDeformableROI::OutputSurfaceAccept);
	e->SetString(&title);
	mafEventMacro(*e);
	m_PNode = e->GetVme();
	delete e;
}
