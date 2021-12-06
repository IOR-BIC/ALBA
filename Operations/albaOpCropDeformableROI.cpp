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
	m_InsideOut = 0;
	m_FillValue = 0.0;
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
	ID_FILL_VALUE,
	ID_MAX_DISTANCE,
	ID_INSIDE_OUT,
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
		albaVME *n=NULL;
		switch(e->GetId())
		{	
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
		m_MaskPolydataFilter->SetFillValue(m_FillValue);
		m_MaskPolydataFilter->SetInsideOut(m_InsideOut);
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
	m_Gui->Double(ID_DISTANCE,_("Distance"),&m_Distance,0.0);
	m_Gui->Double(ID_FILL_VALUE,_("Fill value"),&m_FillValue);
	m_Gui->Bool(ID_INSIDE_OUT,_("Mask inside"),&m_InsideOut);
	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();

	m_Gui->Divider();
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
