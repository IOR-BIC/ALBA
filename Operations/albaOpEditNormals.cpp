/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditNormals
 Authors: Paolo Quadrani
 
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

#include "albaOpEditNormals.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMESurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkStripper.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpEditNormals);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpEditNormals::albaOpEditNormals(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

	m_PreviewResultFlag	= false;
	m_ClearInterfaceFlag= false;

	m_Angle				= 30;
	m_EdgeSplit	  = 0;
	m_FlipNormals	= 0;

	m_ResultPolydata	  = NULL;
	m_OriginalPolydata  = NULL;
}
//----------------------------------------------------------------------------
albaOpEditNormals::~albaOpEditNormals()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool albaOpEditNormals::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node && node->IsALBAType(albaVMESurface));
}
//----------------------------------------------------------------------------
albaOp *albaOpEditNormals::Copy()   
//----------------------------------------------------------------------------
{
	return (new albaOpEditNormals(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_NORMALS = MINID,
	ID_NORMALS_ANGLE,
	ID_EDGE_SPLITTING,
	ID_FLIP_NORMALS,
	ID_PREVIEW,
	ID_RESET_ALL,
	ID_RESET_NORMALS,
};
//----------------------------------------------------------------------------
void albaOpEditNormals::OpRun()   
//----------------------------------------------------------------------------
{  
	vtkNEW(m_ResultPolydata);
	m_ResultPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

	if(!m_TestMode)
	{
		// interface:
		m_Gui = new albaGUI(this);

		m_Gui->Divider(2);
		m_Gui->Label("normals",true);
		m_Gui->Slider(ID_NORMALS_ANGLE,"angle",&m_Angle, 0, 90);
		m_Gui->Bool(ID_EDGE_SPLITTING,"edge splitting",&m_EdgeSplit, 1);
		m_Gui->Bool(ID_FLIP_NORMALS,"flip normals",&m_FlipNormals, 1);
		m_Gui->Button(ID_NORMALS,"apply normals");
		m_Gui->Button(ID_RESET_NORMALS,"reset normals");

		m_Gui->Divider(2);
		m_Gui->Label("");
		m_Gui->Button(ID_PREVIEW,"preview");
		m_Gui->Button(ID_RESET_ALL,"clear");
		m_Gui->OkCancel();
		m_Gui->Enable(wxOK,false);

		m_Gui->Enable(ID_PREVIEW,false);
		m_Gui->Enable(ID_RESET_ALL,false);

		m_Gui->Divider();

		ShowGui();
	}
}
//----------------------------------------------------------------------------
void albaOpEditNormals::OpDo()
//----------------------------------------------------------------------------
{
	((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	if(!m_TestMode)
		GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpEditNormals::OpUndo()
//----------------------------------------------------------------------------
{
	((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	if(!m_TestMode)
		GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpEditNormals::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{	
		case ID_NORMALS:
			OnGenerateNormals();
			break;
		case ID_PREVIEW:
			OnPreview(); 
			break;
		case ID_RESET_ALL:
			OnClear(); 
			break;
		case ID_RESET_NORMALS:
			OnResetNormals();
			break;
		case wxOK:
			if(m_PreviewResultFlag)
				OnPreview();
			OpStop(OP_RUN_OK);        
			break;
		case wxCANCEL:
			if(m_ClearInterfaceFlag)
				OnClear();
			OpStop(OP_RUN_CANCEL);        
			break;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpEditNormals::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpEditNormals::OnGenerateNormals()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
		wxBusyCursor wait;
		m_Gui->Enable(ID_NORMALS_ANGLE,false);
		m_Gui->Enable(ID_EDGE_SPLITTING,false);
		m_Gui->Enable(ID_FLIP_NORMALS,false);
		m_Gui->Enable(ID_NORMALS,false);
		m_Gui->Update();
	}

	vtkALBASmartPointer<vtkPolyDataNormals> normalFilter;
	normalFilter->SetInput(m_ResultPolydata);

	normalFilter->ComputeCellNormalsOn();

	if (m_FlipNormals) 
		normalFilter->FlipNormalsOn(); 
	else 
		normalFilter->FlipNormalsOff();
	normalFilter->SetFeatureAngle(m_Angle);

	if(m_EdgeSplit) 
		normalFilter->SplittingOn(); 
	else 
		normalFilter->SplittingOff();
	normalFilter->Update();

	m_ResultPolydata->DeepCopy(normalFilter->GetOutput());

	if(!m_TestMode)
	{
		m_Gui->Enable(ID_NORMALS_ANGLE,true);
		m_Gui->Enable(ID_EDGE_SPLITTING,true);
		m_Gui->Enable(ID_FLIP_NORMALS,true);
		m_Gui->Enable(ID_NORMALS,true);

		m_Gui->Enable(ID_PREVIEW,true);
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
	}

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpEditNormals::OnResetNormals()
//----------------------------------------------------------------------------
{
	m_Gui->Enable(ID_RESET_NORMALS,false);

	vtkDataArray *normals = m_ResultPolydata->GetPointData()->GetNormals();

	vtkIdType number_tuple;

	if(normals)
		number_tuple = normals->GetNumberOfTuples();

	if(number_tuple > 0)
		normals->Reset();


	m_Gui->Enable(ID_RESET_NORMALS,true);

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_RESET_ALL,true);
	m_Gui->Enable(wxOK,true);
}
//----------------------------------------------------------------------------
void albaOpEditNormals::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_RESET_ALL,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpEditNormals::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());

	m_ResultPolydata->DeepCopy(m_OriginalPolydata);

	m_Angle				= 30;
	m_EdgeSplit	  = 0;
	m_FlipNormals	= 0;

	m_Gui->Enable(ID_NORMALS,true);
	m_Gui->Enable(ID_NORMALS_ANGLE,true);
	m_Gui->Enable(ID_EDGE_SPLITTING,true);
	m_Gui->Enable(ID_FLIP_NORMALS,true);

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_RESET_ALL,false);
	m_Gui->Enable(wxOK,false);
	m_Gui->Update();

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;

	GetLogicManager()->CameraUpdate();
}
