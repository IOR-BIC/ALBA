/*=========================================================================
Program:   Alba
Module:    albaOpNearestLandmark.cpp
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

#include "albaOpNearestLandmark.h"
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


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpNearestLandmark);

//----------------------------------------------------------------------------
albaOpNearestLandmark::albaOpNearestLandmark(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}

//----------------------------------------------------------------------------
albaOpNearestLandmark::~albaOpNearestLandmark()
{
}

//----------------------------------------------------------------------------
bool albaOpNearestLandmark::InternalAccept(albaVME *node)
{
	return node->IsA("albaVMELandmark");
}

//----------------------------------------------------------------------------
bool albaOpNearestLandmark::SurfaceAccept(albaVME* node)
{
	return(node && (node->IsA("albaVMESurface") || node->IsA("albaVMESurfaceParametric")));
}

//----------------------------------------------------------------------------
albaOp* albaOpNearestLandmark::Copy()
{
	albaOpNearestLandmark *cp = new albaOpNearestLandmark(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpNearestLandmark::OpRun()
{
	albaString title = albaString("Select a surface:");
	albaEvent e(this, VME_CHOOSE);
	e.SetString(&title);
	e.SetPointer(&SurfaceAccept);
	albaEventMacro(e);
	albaVME *surface = (albaVME *)e.GetVme();
	
	if (surface == NULL)
		OpStop(OP_RUN_CANCEL);
	
	surface->GetOutput()->Update();
	surface->GetOutput()->GetVTKData()->Update();

	albaVMELandmark *lm = albaVMELandmark::SafeDownCast(m_Input);
	

	vtkALBASmartPointer <vtkTransform> tra;
	tra->SetMatrix(surface->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

	vtkALBASmartPointer<vtkTransformPolyDataFilter> v_tpdf;
	v_tpdf->SetInput((vtkPolyData *)surface->GetOutput()->GetVTKData());
	v_tpdf->SetTransform(tra);
	v_tpdf->Update();
	vtkPolyData *tranPoly=v_tpdf->GetOutput();


	albaVMELandmarkCloud *lmc=CreateClosestPoint(tranPoly, lm, surface);
	GetLogicManager()->VmeShow(lmc->GetLastChild(), true);

	OpStop(OP_RUN_OK);
}

//----------------------------------------------------------------------------
albaVMELandmarkCloud *albaOpNearestLandmark::CreateClosestPoint(vtkPolyData * poly, albaVMELandmark * lm, albaVME * surface)
{
	vtkALBASmartPointer <vtkCellLocator> cellLocator;
	cellLocator->SetDataSet(poly);
	cellLocator->BuildLocator();

	double closestPoint[3];
	double closestPointDist2;
	vtkIdType cellId;
	int subId;
	double point[3];
	lm->GetPoint(point);

	cellLocator->FindClosestPoint(point, closestPoint, cellId, subId, closestPointDist2);

	albaVMELandmarkCloud *lmc = albaVMELandmarkCloud::SafeDownCast(lm->GetParent());

	albaString lmName;
	lmName.Printf("Closest %s on %s", lm->GetName(), surface->GetName());

	lmc->AppendAbsoluteLandmark(closestPoint, lmName.GetCStr());

	albaLogMessage("%s distance to %s: %f", lm->GetName(), surface->GetName(), sqrt(closestPointDist2));

	return lmc;
}

//----------------------------------------------------------------------------
void albaOpNearestLandmark::OpStop(int result)
{

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpNearestLandmark::OpDo()
{
}

//----------------------------------------------------------------------------
void albaOpNearestLandmark::OnEvent(albaEventBase *alba_event)
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
