/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreatePolyline
 Authors: Nicola Vanella
 
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

#include "albaOpCreatePolyline.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaInteractor2DMeasure.h"
#include "albaInteractor2DMeasure_Point.h"
#include "albaLogicWithManagers.h"
#include "albaVMEPolyline.h"
#include "albaView.h"
#include "albaViewManager.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreatePolyline);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreatePolyline::albaOpCreatePolyline(const wxString &label) :
albaOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

	m_PointInteractor = NULL;

	m_Polyline = NULL;
	m_PolylineName = "New Polyline";
	m_IsLoading = false;
}
//----------------------------------------------------------------------------
albaOpCreatePolyline::~albaOpCreatePolyline( ) 
{
	albaDEL(m_Polyline);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreatePolyline::Copy()   
{
	return new albaOpCreatePolyline(m_Label);
}
//----------------------------------------------------------------------------
char** albaOpCreatePolyline::GetIcon()
{
 #include "pic/MENU_OP_CREATE_POLYLINE.xpm"
 	return MENU_OP_CREATE_POLYLINE_xpm;
}
//----------------------------------------------------------------------------
bool albaOpCreatePolyline::InternalAccept(albaVME*node)
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void albaOpCreatePolyline::CreateGui()
{
	// Setup Gui
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpCreatePolyline::UpdateGui()
{
	if (m_Gui && !GetTestMode())
	{
	}
}

//----------------------------------------------------------------------------
void albaOpCreatePolyline::OpRun()
{
	if (!GetTestMode())
	{
		CreateGui();
		UpdateGui();
	}

	albaView *view = ((albaLogicWithManagers*)GetLogicManager())->GetViewManager()->GetSelectedView();

	if (view)
	{
		// Init Point Interactor
		m_PointInteractor = albaInteractor2DMeasure_Point::New();
		m_PointInteractor->SetListener(this);
		m_PointInteractor->Enable(false);
		m_PointInteractor->SetRendererByView(view);
		m_PointInteractor->SetColorText(1, 0, 0);
		m_PointInteractor->ShowText(false);

		albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_PointInteractor));
		m_PointInteractor->Enable(true);
		m_PointInteractor->SetListener(this);

		if (m_Input->IsA("albaVMEPolyline"))
		{
			m_IsLoading = true;

			albaVMEPolyline *polyline = albaVMEPolyline::SafeDownCast(m_Input);

			vtkPolyData *in_data = polyline->GetPolylineOutput()->GetPolylineData();

			double point[3];
			for (int i = 0; i < in_data->GetNumberOfPoints(); i++)
			{
				in_data->GetPoint(i, point);
				m_PointInteractor->AddMeasure(point);
			}

			m_PolylineName = "New ";
			m_PolylineName += m_Input->GetName();

			m_IsLoading = false;
		}

	}
}
//----------------------------------------------------------------------------
void albaOpCreatePolyline::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	if (result == OP_RUN_OK)
	{
		CreatePolyline();
	}
	else
	{
		if (m_Polyline)
			m_Polyline->ReparentTo(NULL);
	}

	if (m_PointInteractor)
	{
		// Remove ExtraInteractor
		albaEventMacro(albaEvent(this, PER_POP));

		m_PointInteractor->RemoveAllMeasures();
		albaDEL(m_PointInteractor);

		GetLogicManager()->CameraUpdate();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpCreatePolyline::OpDo()
{

}
//----------------------------------------------------------------------------
void albaOpCreatePolyline::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case wxOK: OpStop(OP_RUN_OK); break;
			case wxCANCEL: OpStop(OP_RUN_CANCEL); break;

			default:
				albaEventMacro(*e);
				break;
			}
		}
		else // From Interactor
		{
			if (!m_IsLoading)
				switch (e->GetId())
				{
				case albaInteractor2DMeasure::ID_MEASURE_SELECTED: break;
				case albaInteractor2DMeasure::ID_MEASURE_ADDED: break;
				case albaInteractor2DMeasure::ID_MEASURE_CHANGED: break;
				case albaInteractor2DMeasure::ID_MEASURE_FINISHED:
				{
					if (m_PointInteractor && m_PointInteractor->GetMeasureCount() > 2)
					{
						CreatePolyline();

						GetLogicManager()->VmeShow(m_Polyline, true);
					}
				}
				break;
				}
		}
	}
	else
	{
		Superclass::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void albaOpCreatePolyline::CreatePolyline()
{
	if (m_Polyline == NULL)
	{
		albaNEW(m_Polyline);
		m_Polyline->SetName(m_PolylineName);
		m_Polyline->ReparentTo(m_Input);
	}

	// Create PolyData
	vtkALBASmartPointer<vtkPolyData> polydata;
	vtkALBASmartPointer<vtkPoints> points;
	vtkALBASmartPointer<vtkCellArray> cells;
	vtkIdType pointId[2];

	double point[3];

	for (int i = 0; i < m_PointInteractor->GetMeasureCount(); i++)
	{
		m_PointInteractor->GetMeasurePoint(i, point);
		points->InsertNextPoint(point[0], point[1], point[2]);

		if (i > 0)
		{
			pointId[0] = i - 1;
			pointId[1] = i;
			cells->InsertNextCell(2, pointId);
		}
	}

	points->Modified();

	polydata->SetPoints(points);
	polydata->SetLines(cells);
	polydata->Modified();
	polydata->Update();
	
	// Update Polyline
	m_Polyline->SetData(vtkPolyData::SafeDownCast(polydata), 0.0, albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
	m_Polyline->Modified();
	m_Polyline->GetOutput()->Update();

	m_Output = m_Polyline;
}
