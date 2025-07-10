/*=========================================================================
Program:   Alba
Module:    albaOpCreateCentroid.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2024 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the alba must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpCreateCentroid.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMELandmark.h"
#include "vtkALBASmartPointer.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCellLocator.h"
#include "vtkPolyData.h"
#include "albaVMELandmarkCloud.h"
#include "albaOpNearestLandmark.h"
#include "albaVect3d.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateCentroid);

//----------------------------------------------------------------------------
albaOpCreateCentroid::albaOpCreateCentroid(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}

//----------------------------------------------------------------------------
albaOpCreateCentroid::~albaOpCreateCentroid()
{
}

//----------------------------------------------------------------------------
bool albaOpCreateCentroid::InternalAccept(albaVME *node)
{
	return(node && (node->IsA("albaVMESurface") || node->IsA("albaVMESurfaceParametric")));
}


//----------------------------------------------------------------------------
albaOp* albaOpCreateCentroid::Copy()
{
	albaOpCreateCentroid *cp = new albaOpCreateCentroid(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpCreateCentroid::OpRun()
{
	
	m_Input->GetOutput()->Update();
	m_Input->GetOutput()->GetVTKData()->Update();

	albaVect3d centroid = { 0,0,0 };
	
	vtkALBASmartPointer <vtkTransform> tra;
	tra->SetMatrix(m_Input->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

	vtkALBASmartPointer<vtkTransformPolyDataFilter> v_tpdf;
	v_tpdf->SetInput((vtkPolyData *)m_Input->GetOutput()->GetVTKData());
	v_tpdf->SetTransform(tra);
	v_tpdf->Update();
	vtkPolyData *tranPoly=v_tpdf->GetOutput();

	vtkIdType nPoints = tranPoly->GetNumberOfPoints();
	for (int i = 0; i < nPoints; i++)
		centroid += tranPoly->GetPoint(i);
	centroid /= nPoints;

	albaString lmName;
	lmName.Printf("Centroid of %s", m_Input->GetName());

	albaVMELandmarkCloud *lmc;
	albaNEW(lmc);
	lmc->SetName("Centroid Cloud");
	lmc->ReparentTo(m_Input);

	lmc->AppendAbsoluteLandmark(centroid.GetVect(),lmName.GetCStr());
	GetLogicManager()->VmeShow(lmc->GetLastChild(), true);
	
	albaVMELandmark * lm = lmc->GetLandmark(lmc->GetNumberOfLandmarks() - 1);
	albaOpNearestLandmark::CreateClosestPoint(tranPoly, lm, m_Input);
	GetLogicManager()->VmeShow(lmc->GetLastChild(), true);


	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void albaOpCreateCentroid::OpStop(int result)
{

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpCreateCentroid::OpDo()
{
}

//----------------------------------------------------------------------------
void albaOpCreateCentroid::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		m_Gui->Update();
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			default:
				Superclass::OnEvent(alba_event);
				break;
			}
		}
// 		else
// 		{
// 			Superclass::OnEvent(alba_event);
// 		}
	}
}
