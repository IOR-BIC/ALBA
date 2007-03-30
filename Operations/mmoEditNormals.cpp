/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoEditNormals.cpp,v $
Language:  C++
Date:      $Date: 2007-03-30 08:48:31 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoEditNormals.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

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

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoEditNormals);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoEditNormals::mmoEditNormals(const wxString &label) :
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
mmoEditNormals::~mmoEditNormals()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool mmoEditNormals::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *mmoEditNormals::Copy()   
//----------------------------------------------------------------------------
{
	return (new mmoEditNormals(m_Label));
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
	ID_CLEAR,
	ID_RESET_NORMALS,
};
//----------------------------------------------------------------------------
void mmoEditNormals::OpRun()   
//----------------------------------------------------------------------------
{  
	vtkNEW(m_ResultPolydata);
	m_ResultPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

	if(!m_TestMode)
	{
		// interface:
		m_Gui = new mmgGui(this);

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
		m_Gui->Button(ID_CLEAR,"clear");
		m_Gui->OkCancel();
		m_Gui->Enable(wxOK,false);

		m_Gui->Enable(ID_PREVIEW,false);
		m_Gui->Enable(ID_CLEAR,false);

		m_Gui->Divider();

		ShowGui();
	}
}
//----------------------------------------------------------------------------
void mmoEditNormals::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());
	if(!m_TestMode)
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoEditNormals::OpUndo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());
	if(!m_TestMode)
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoEditNormals::OnEvent(mafEventBase *maf_event)
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
		case ID_CLEAR:
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
void mmoEditNormals::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mmoEditNormals::OnGenerateNormals()
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
		m_Gui->Enable(ID_CLEAR,true);
		m_Gui->Enable(wxOK,true);
	}

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mmoEditNormals::OnResetNormals()
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
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);
}
//----------------------------------------------------------------------------
void mmoEditNormals::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoEditNormals::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());

	m_ResultPolydata->DeepCopy(m_OriginalPolydata);

	m_Angle				= 30;
	m_EdgeSplit	  = 0;
	m_FlipNormals	= 0;

	m_Gui->Enable(ID_NORMALS,true);
	m_Gui->Enable(ID_NORMALS_ANGLE,true);
	m_Gui->Enable(ID_EDGE_SPLITTING,true);
	m_Gui->Enable(ID_FLIP_NORMALS,true);

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,false);
	m_Gui->Enable(wxOK,false);
	m_Gui->Update();

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
