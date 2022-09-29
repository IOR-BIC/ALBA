/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: albaOpCropDeformableROI
 Authors: Matteo Giacomoni - Daniele Giunchi
 
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

#include "albaOpCropDeformableROI.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"

#include "vtkMaskPolydataFilter.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "wx\busyinfo.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCropDeformableROI);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCropDeformableROI::albaOpCropDeformableROI(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_MaskPolydataFilter = NULL;
	m_ResultVme = NULL;

	m_Distance = 0.0;
	m_Modality = 0;
	m_OutsideValue = m_InsideValue = 0.0;
	m_PNode = NULL;
}
//----------------------------------------------------------------------------
albaOpCropDeformableROI::~albaOpCropDeformableROI()
//----------------------------------------------------------------------------
{
	vtkDEL(m_MaskPolydataFilter);
	albaDEL(m_ResultVme);

}
//----------------------------------------------------------------------------
bool albaOpCropDeformableROI::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("albaVMEVolumeGray"));
}
//----------------------------------------------------------------------------
albaOp *albaOpCropDeformableROI::Copy()   
//----------------------------------------------------------------------------
{
	return (new albaOpCropDeformableROI(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_DISTANCE  = MINID,
	ID_INSIDE_VALUE,
	ID_OUTSIDE_VALUE,
	ID_MAX_DISTANCE,
	ID_MODALITY,
};
//----------------------------------------------------------------------------
void albaOpCropDeformableROI::OpRun()   
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
void albaOpCropDeformableROI::OpDo()
//----------------------------------------------------------------------------
{
	m_ResultVme->ReparentTo(m_Input->GetRoot());
}
//----------------------------------------------------------------------------
void albaOpCropDeformableROI::OpUndo()
//----------------------------------------------------------------------------
{
	GetLogicManager()->VmeRemove(m_ResultVme);
}
//----------------------------------------------------------------------------
void albaOpCropDeformableROI::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		albaVME *n = NULL;
		switch (e->GetId())
		{
			case ID_MODALITY:
				EnableDisableGui();
				break;
			case wxOK:
				//Run algorithm and stop operation
				Algorithm(m_PNode);
				OpStop(OP_RUN_OK);
				break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpCropDeformableROI::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpCropDeformableROI::Algorithm(albaVME *vme)
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

		if(albaVMESurface::SafeDownCast(vme))
		{
			maskPolydata=(vtkPolyData *)(albaVMESurface::SafeDownCast(vme)->GetOutput()->GetVTKData());
		}
		if (albaVMESurfaceParametric::SafeDownCast(vme))
		{
			maskPolydata = (vtkPolyData *)(albaVMESurfaceParametric::SafeDownCast(vme)->GetSurfaceOutput()->GetVTKData());
		}
		
		if(!maskPolydata)
			return;


		
		albaNEW(m_ResultVme);
		m_ResultVme->DeepCopy(m_Input);
		albaString resultName = "Masked ";
		resultName+=m_Input->GetName();
		m_ResultVme->SetName(resultName);

		vtkPolyData *Mask;

		albaMatrix identityMatrix;
		albaMatrix maskABSMatrix = vme->GetAbsMatrixPipe()->GetMatrix();
		albaMatrix volumeABSMatrix = m_Input->GetAbsMatrixPipe()->GetMatrix();

		bool isMaskMatrixIdentity = maskABSMatrix.Equals(&identityMatrix);
		bool isVolumeMatrixIdentity = volumeABSMatrix.Equals(&identityMatrix);

		if (isMaskMatrixIdentity && isVolumeMatrixIdentity)
			transformedMaskPolydata=maskPolydata;
		else
		{		
			// if VME matrix is not identity apply it to dataset
			albaMatrix meshVolumeAlignMatrix;

			//Calculate align matrix 
			volumeABSMatrix.Invert();
			albaMatrix::Multiply4x4(volumeABSMatrix,maskABSMatrix,meshVolumeAlignMatrix);

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
		albaVMEVolumeGray *volume = albaVMEVolumeGray::SafeDownCast(m_Input);
		m_MaskPolydataFilter->SetInput(volume->GetOutput()->GetVTKData());
		m_MaskPolydataFilter->SetDistance(m_Distance);
		m_MaskPolydataFilter->SetInsideValue(m_InsideValue);
		m_MaskPolydataFilter->SetOutsideValue(m_OutsideValue);
		if (m_Modality == FILL_OUTSIDE)
		{
			m_MaskPolydataFilter->SetInsideOut(false);
			m_MaskPolydataFilter->SetBinarize(false);
		}
		else if (m_Modality == FILL_INSIDE)
		{
			m_MaskPolydataFilter->SetInsideOut(true);
			m_MaskPolydataFilter->SetBinarize(false);
		}
		else //m_Modality == BINARIZE
		{
			m_MaskPolydataFilter->SetInsideOut(false);
			m_MaskPolydataFilter->SetBinarize(true);
		}

		m_MaskPolydataFilter->SetMask(transformedMaskPolydata);
    albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,m_MaskPolydataFilter));
		m_MaskPolydataFilter->Update();

		if(vtkRectilinearGrid::SafeDownCast(m_MaskPolydataFilter->GetOutput()))
			m_ResultVme->SetData(((vtkRectilinearGrid*)m_MaskPolydataFilter->GetOutput()),m_Input->GetTimeStamp());
		else if(vtkImageData::SafeDownCast(m_MaskPolydataFilter->GetOutput()))
			m_ResultVme->SetData(((vtkImageData*)m_MaskPolydataFilter->GetOutput()),m_Input->GetTimeStamp());
	
		m_ResultVme->Modified();
		m_ResultVme->Update();

		vtkDEL(transform);
		vtkDEL(transformFilter);
		cppDEL(busyInfo);
	}
}

void albaOpCropDeformableROI::CreateGui()
{

	// interface:
	m_Gui = new albaGUI(this);

	m_Gui->Label("");
	m_Gui->Label("");

	//Distance is disabled until a fix
	//m_Gui->Double(ID_DISTANCE, _("Distance"), &m_Distance, 0.0);

	m_Gui->Label("Values:", true); 
	m_Gui->Double(ID_INSIDE_VALUE, _("Inside"), &m_InsideValue);
	m_Gui->Double(ID_OUTSIDE_VALUE, _("Outside"), &m_OutsideValue);
	m_Gui->Label("");

	m_Gui->Label("Modality:", true);
	wxString modality_strs[3] = { _("Mask Outside"), _("Mask Inside"), _("Binarize") };
	m_Gui->Radio(ID_MODALITY, "", &m_Modality, 3, modality_strs);
	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();

	EnableDisableGui();

	m_Gui->Divider();
}

//----------------------------------------------------------------------------
void albaOpCropDeformableROI::EnableDisableGui()
{
	m_Gui->Enable(ID_INSIDE_VALUE, m_Modality == FILL_INSIDE || m_Modality == BINARIZE);
	m_Gui->Enable(ID_OUTSIDE_VALUE, m_Modality == FILL_OUTSIDE || m_Modality == BINARIZE);
}

void albaOpCropDeformableROI::MaskSelection()
{
	albaEvent *e; 
	e = new albaEvent();
	albaString title = _("Choose mask");
	e->SetId(VME_CHOOSE);
	e->SetPointer(&albaOpCropDeformableROI::OutputSurfaceAccept);
	e->SetString(&title);
	albaEventMacro(*e);
	m_PNode = e->GetVme();
	delete e;
}
