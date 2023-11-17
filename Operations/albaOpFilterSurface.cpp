/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterSurface
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

#include "albaOpFilterSurface.h"
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

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpFilterSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpFilterSurface::albaOpFilterSurface(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = true;

  m_PreviewResultFlag	 = false;
	m_ClearInterfaceFlag = false;
  m_StripFlag          = false;
  m_TriangulateFlag    = false;
  m_CleanFlag          = false;
  m_ConnectivityFlag   = false;
  
	m_TopologyFlag= 0;
	m_Reduction		= 50;
	m_Angle				= 30;
	m_EdgeSplit	  = 0;
	m_FlipNormals	= 0;
	m_Iterations	= 50;
	
	m_ResultPolydata	  = NULL;
	m_OriginalPolydata  = NULL;
}
//----------------------------------------------------------------------------
albaOpFilterSurface::~albaOpFilterSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool albaOpFilterSurface::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVMESurface));
}
//----------------------------------------------------------------------------
albaOp *albaOpFilterSurface::Copy()   
//----------------------------------------------------------------------------
{
  return (new albaOpFilterSurface(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_CLEAN = MINID,
	ID_VTK_CONNECT,
	ID_SMOOTH,
	ID_ITERATION,
	ID_DECIMATE,
	ID_PRESERVE_TOPOLOGY,
	ID_REDUCTION,
	ID_STRIPPER,
	ID_TRIANGLE,
	ID_NORMALS,
	ID_NORMALS_ANGLE,
	ID_EDGE_SPLITTING,
	ID_FLIP_NORMALS,
	ID_RESET_ALL,
};
//----------------------------------------------------------------------------
void albaOpFilterSurface::OpRun()   
//----------------------------------------------------------------------------
{  
	vtkNEW(m_ResultPolydata);
	m_ResultPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());
	
	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

  if (!m_TestMode)
  {
  	CreateGui();
  }	
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new albaGUI(this);

  m_Gui->Label("");
  m_Gui->Label("Smooth",true);
  m_Gui->Slider(ID_ITERATION,"Iterations:",&m_Iterations,0,500);
  m_Gui->Button(ID_SMOOTH,"Apply Smooth");

  m_Gui->Divider(2);
  m_Gui->Label("Decimate",true);
  m_Gui->Bool(ID_PRESERVE_TOPOLOGY,"Preserve topology",&m_TopologyFlag, 1);
  m_Gui->Slider(ID_REDUCTION,"Reduction:",&m_Reduction,1, 100);
  m_Gui->Button(ID_DECIMATE,"Apply Decimate");

  m_Gui->Divider(2);
  m_Gui->Label("Normals",true);
  m_Gui->Slider(ID_NORMALS_ANGLE,"Angle",&m_Angle, 0, 90);
  m_Gui->Bool(ID_EDGE_SPLITTING,"Edge splitting",&m_EdgeSplit, 1);
  m_Gui->Bool(ID_FLIP_NORMALS,"Flip normals",&m_FlipNormals, 1);
  m_Gui->Button(ID_NORMALS,"Apply Normals");

  m_Gui->Divider(2);
  m_Gui->Label("Other filters",true);
  m_Gui->Button(ID_STRIPPER,"Apply Strip");
  m_Gui->Button(ID_TRIANGLE,"Apply Triangulate");
  m_Gui->Button(ID_CLEAN,"Apply Clean");
  m_Gui->Button(ID_VTK_CONNECT,"Apply Connectivity");

  m_Gui->Divider(2);
  m_Gui->Label("");
  m_Gui->Button(ID_RESET_ALL,"Reset");

  m_Gui->OkCancel();
  m_Gui->Enable(wxOK,false);

  m_Gui->Enable(ID_RESET_ALL,false);

  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OpDo()
//----------------------------------------------------------------------------
{
	((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OpUndo()
//----------------------------------------------------------------------------
{
  ((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	   
	  case ID_CLEAN:
        OnClean();
      break;
      case ID_VTK_CONNECT:
        OnVtkConnect();
      break;
      case ID_SMOOTH:
        OnSmooth();
      break;
      case ID_DECIMATE:
        OnDecimate();
      break;
      case ID_STRIPPER:
        OnStripper();        
      break;
      case ID_TRIANGLE:
        OnTriangulate();
      break;
      case ID_NORMALS:
        OnGenerateNormals();
      break;
      break;
      case ID_RESET_ALL:
        OnClear(); 
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
void albaOpFilterSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnClean()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		wxBusyCursor wait;
	}

	vtkALBASmartPointer<vtkCleanPolyData> cleanPolydata;
  cleanPolydata->SetTolerance(0.0);
	cleanPolydata->SetInput(m_ResultPolydata);
	cleanPolydata->Update();

	m_ResultPolydata->DeepCopy(cleanPolydata->GetOutput());

	if (!m_TestMode)
	{
		OnPreview();
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
	}

  m_CleanFlag = true;
	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnVtkConnect()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		wxBusyCursor wait;
	}

	vtkALBASmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
	connectivityFilter->SetInput(m_ResultPolydata);
	connectivityFilter->Update();

	m_ResultPolydata->DeepCopy((vtkPolyData*)(connectivityFilter->GetOutput()));

	if (!m_TestMode)
	{
		OnPreview();
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
	}

  m_ConnectivityFlag = true;
	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnSmooth()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		wxBusyCursor wait;
		m_Gui->Enable(ID_SMOOTH,false);
		m_Gui->Enable(ID_ITERATION,false);
		m_Gui->Update();
	}

	vtkALBASmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
	smoothFilter->SetInput(m_ResultPolydata);
	smoothFilter->SetNumberOfIterations(m_Iterations);
	smoothFilter->FeatureEdgeSmoothingOn();
	smoothFilter->Update();

	m_ResultPolydata->DeepCopy(smoothFilter->GetOutput());

	if (!m_TestMode)
	{
		m_Gui->Enable(ID_SMOOTH,true);
		m_Gui->Enable(ID_ITERATION,true);
	
		OnPreview();
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
	}

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnDecimate()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		wxBusyCursor wait;
		m_Gui->Enable(ID_DECIMATE,false);
		m_Gui->Enable(ID_PRESERVE_TOPOLOGY,false);
		m_Gui->Enable(ID_REDUCTION,false);
		m_Gui->Update();
	}

	vtkALBASmartPointer<vtkDecimatePro> decimate;
	decimate->SetInput(m_ResultPolydata);
	decimate->SetPreserveTopology(m_TopologyFlag); 
	decimate->SetTargetReduction(m_Reduction/100.0);
	decimate->Update();

	m_ResultPolydata->DeepCopy(decimate->GetOutput());

	if (!m_TestMode)
	{
		m_Gui->Enable(ID_DECIMATE,true);
		m_Gui->Enable(ID_PRESERVE_TOPOLOGY,true);
		m_Gui->Enable(ID_REDUCTION,true);
	
		OnPreview();
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
		m_Gui->Update();
	}

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnStripper()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		wxBusyCursor wait;
	}

  vtkALBASmartPointer<vtkStripper> stripper;
	stripper->SetInput(m_ResultPolydata);
	stripper->Update();

	m_ResultPolydata->DeepCopy(stripper->GetOutput());

	if (!m_TestMode)
	{
		OnPreview();
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
	}

  m_StripFlag = true;
	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnTriangulate()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		wxBusyCursor wait;
	}

	vtkALBASmartPointer<vtkTriangleFilter> triangleFilter;
	triangleFilter->SetInput(m_ResultPolydata);
	triangleFilter->Update();

	m_ResultPolydata->DeepCopy(triangleFilter->GetOutput());

	if (!m_TestMode)
	{
		OnPreview();
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
	}

  m_TriangulateFlag = true;
	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnGenerateNormals()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
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

	if (!m_TestMode)
	{
		m_Gui->Enable(ID_NORMALS_ANGLE,true);
		m_Gui->Enable(ID_EDGE_SPLITTING,true);
		m_Gui->Enable(ID_FLIP_NORMALS,true);
		m_Gui->Enable(ID_NORMALS,true);
	
		OnPreview();
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
	}

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	
  ((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	
	m_Gui->Enable(ID_RESET_ALL,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpFilterSurface::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

  ((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	
	m_ResultPolydata->DeepCopy(m_OriginalPolydata);

	m_TopologyFlag= 0;
	m_Reduction		= 50;
	m_Angle				= 30;
	m_EdgeSplit	  = 0;
	m_FlipNormals	= 0;
	m_Iterations	= 50;

	m_Gui->Enable(ID_CLEAN,true);
	m_Gui->Enable(ID_VTK_CONNECT,true);
	m_Gui->Enable(ID_SMOOTH,true);
	m_Gui->Enable(ID_ITERATION,true);
	m_Gui->Enable(ID_DECIMATE,true);
	m_Gui->Enable(ID_PRESERVE_TOPOLOGY,true);
	m_Gui->Enable(ID_REDUCTION,true);
	m_Gui->Enable(ID_STRIPPER,true);
	m_Gui->Enable(ID_TRIANGLE,true);
	m_Gui->Enable(ID_NORMALS,true);

	m_Gui->Enable(ID_PRESERVE_TOPOLOGY,true);
	m_Gui->Enable(ID_REDUCTION,true);
	m_Gui->Enable(ID_NORMALS_ANGLE,true);
	m_Gui->Enable(ID_EDGE_SPLITTING,true);
	m_Gui->Enable(ID_FLIP_NORMALS,true);

	m_Gui->Enable(ID_RESET_ALL,false);
	m_Gui->Enable(wxOK,false);
	m_Gui->Update();

  m_StripFlag          = false;
  m_TriangulateFlag    = false;
  m_CleanFlag          = false;
  m_ConnectivityFlag   = false;
	m_PreviewResultFlag  = false;
	m_ClearInterfaceFlag = false;

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
albaString albaOpFilterSurface::GetParameters()
//----------------------------------------------------------------------------
{
  wxString parameter;

  parameter.Append("n.iteraction = ");
  parameter.Append(albaString::Format("%i", m_Iterations));
  parameter.Append(", ");
  parameter.Append("preserve topology = ");
  parameter.Append(albaString::Format("%i", m_TopologyFlag));

  parameter.Append(", ");
  parameter.Append("reduc. to % = ");
  parameter.Append(albaString::Format("%i", m_Reduction));
  parameter.Append(", ");
  parameter.Append("angle = ");
  parameter.Append(albaString::Format("%i", m_Angle));
  parameter.Append(", ");
  parameter.Append("edge splitting = ");
  parameter.Append(albaString::Format("%i", m_EdgeSplit));
  parameter.Append(", ");
  parameter.Append("flip normals = ");
  parameter.Append(albaString::Format("%i", m_FlipNormals));
  parameter.Append(", ");
  parameter.Append("strip = ");
  parameter.Append(albaString::Format("%i", m_StripFlag));
  parameter.Append(", ");
  parameter.Append("triangulate = ");
  parameter.Append(albaString::Format("%i", m_TriangulateFlag));
  parameter.Append(", ");
  parameter.Append("clean = ");
  parameter.Append(albaString::Format("%i", m_CleanFlag));
  parameter.Append(", ");
  parameter.Append("connectivity = ");
  parameter.Append(albaString::Format("%i", m_ConnectivityFlag));

  return parameter;
}