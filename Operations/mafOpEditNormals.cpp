/*=========================================================================

 Program: MAF2
 Module: mafOpEditNormals
 Authors: Paolo Quadrani
 
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

#include "mafOpEditNormals.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
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
mafCxxTypeMacro(mafOpEditNormals);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpEditNormals::mafOpEditNormals(const wxString &label) :
mafOp(label)
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
mafOpEditNormals::~mafOpEditNormals()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool mafOpEditNormals::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *mafOpEditNormals::Copy()   
//----------------------------------------------------------------------------
{
	return (new mafOpEditNormals(m_Label));
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
void mafOpEditNormals::OpRun()   
//----------------------------------------------------------------------------
{  
	vtkNEW(m_ResultPolydata);
	m_ResultPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

	if(!m_TestMode)
	{
		// interface:
		m_Gui = new mafGUI(this);

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
void mafOpEditNormals::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	if(!m_TestMode)
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpEditNormals::OpUndo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	if(!m_TestMode)
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpEditNormals::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
void mafOpEditNormals::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpEditNormals::OnGenerateNormals()
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

	vtkMAFSmartPointer<vtkPolyDataNormals> normalFilter;
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
void mafOpEditNormals::OnResetNormals()
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
void mafOpEditNormals::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_RESET_ALL,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpEditNormals::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());

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

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
