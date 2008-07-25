/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpFilterSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.2 $
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

#include "mafOpFilterSurface.h"
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

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpFilterSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpFilterSurface::mafOpFilterSurface(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

  m_PreviewResultFlag	= false;
	m_ClearInterfaceFlag= false;
  
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
mafOpFilterSurface::~mafOpFilterSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool mafOpFilterSurface::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *mafOpFilterSurface::Copy()   
//----------------------------------------------------------------------------
{
  return (new mafOpFilterSurface(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_CLEAN = MINID,
	ID_VTK_CONNECT,
	ID_SMOOTH,
	ID_ITERACTION,
	ID_DECIMATE,
	ID_PRESERVE_TOPOLOGY,
	ID_REDUCTION,
	ID_STRIPPER,
	ID_TRIANGLE,
	ID_NORMALS,
	ID_NORMALS_ANGLE,
	ID_EDGE_SPLITTING,
	ID_FLIP_NORMALS,
	ID_PREVIEW,
	ID_CLEAR,
	ID_RESET_NORMALS,
};
//----------------------------------------------------------------------------
void mafOpFilterSurface::OpRun()   
//----------------------------------------------------------------------------
{  
	vtkNEW(m_ResultPolydata);
	m_ResultPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());
	
	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());
	
	// interface:
	m_Gui = new mafGUI(this);

	m_Gui->Label("");
	m_Gui->Label("smooth",true);
	m_Gui->Slider(ID_ITERACTION,"n.iteraction: ",&m_Iterations,0,500);
	m_Gui->Button(ID_SMOOTH,"apply smooth");

	m_Gui->Divider(2);
	m_Gui->Label("decimate",true);
	m_Gui->Bool(ID_PRESERVE_TOPOLOGY,"preserve topology",&m_TopologyFlag, 1);
	m_Gui->Slider(ID_REDUCTION,"reduc. to %: ",&m_Reduction,1, 100);
	m_Gui->Button(ID_DECIMATE,"apply decimate");

  m_Gui->Divider(2);
	m_Gui->Label("normals",true);
	m_Gui->Slider(ID_NORMALS_ANGLE,"angle",&m_Angle, 0, 90);
	m_Gui->Bool(ID_EDGE_SPLITTING,"edge splitting",&m_EdgeSplit, 1);
	m_Gui->Bool(ID_FLIP_NORMALS,"flip normals",&m_FlipNormals, 1);
	m_Gui->Button(ID_NORMALS,"apply normals");
	m_Gui->Button(ID_RESET_NORMALS,"reset normals");

  m_Gui->Divider(2);
	m_Gui->Label("other filters",true);
	m_Gui->Button(ID_STRIPPER,"strip");
	m_Gui->Button(ID_TRIANGLE,"triangulate");
	m_Gui->Button(ID_CLEAN,"clean");
	m_Gui->Button(ID_VTK_CONNECT,"connectivity");
	
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
//----------------------------------------------------------------------------
void mafOpFilterSurface::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OpUndo()
//----------------------------------------------------------------------------
{
  ((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
void mafOpFilterSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnClean()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	m_Gui->Enable(ID_CLEAN,false);
	m_Gui->Update();

	vtkMAFSmartPointer<vtkCleanPolyData> cleanPolydata;
	cleanPolydata->SetInput(m_ResultPolydata);
	cleanPolydata->PointMergingOff(); 
	cleanPolydata->ConvertLinesToPointsOn();  
	cleanPolydata->ConvertPolysToLinesOn();
	cleanPolydata->ConvertStripsToPolysOn();     
	cleanPolydata->Update();

	m_ResultPolydata->DeepCopy(cleanPolydata->GetOutput());

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnVtkConnect()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	m_Gui->Enable(ID_VTK_CONNECT,false);
	m_Gui->Update();

	vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
	connectivityFilter->SetInput(m_ResultPolydata);
	connectivityFilter->Update();

	m_ResultPolydata->DeepCopy((vtkPolyData*)(connectivityFilter->GetOutput()));

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnSmooth()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	m_Gui->Enable(ID_SMOOTH,false);
	m_Gui->Enable(ID_ITERACTION,false);
	m_Gui->Update();

	vtkMAFSmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
	smoothFilter->SetInput(m_ResultPolydata);
	smoothFilter->SetNumberOfIterations(m_Iterations);
	smoothFilter->FeatureEdgeSmoothingOn();
	smoothFilter->Update();

	m_ResultPolydata->DeepCopy(smoothFilter->GetOutput());

	m_Gui->Enable(ID_SMOOTH,true);
	m_Gui->Enable(ID_ITERACTION,true);

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnDecimate()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	m_Gui->Enable(ID_DECIMATE,false);
	m_Gui->Enable(ID_PRESERVE_TOPOLOGY,false);
	m_Gui->Enable(ID_REDUCTION,false);
	m_Gui->Update();

	vtkMAFSmartPointer<vtkDecimatePro> decimate;
	decimate->SetInput(m_ResultPolydata);
	decimate->SetPreserveTopology(m_TopologyFlag); 
	decimate->SetTargetReduction(m_Reduction/100.0);
	decimate->Update();

	m_ResultPolydata->DeepCopy(decimate->GetOutput());

	m_Gui->Enable(ID_DECIMATE,true);
	m_Gui->Enable(ID_PRESERVE_TOPOLOGY,true);
	m_Gui->Enable(ID_REDUCTION,true);

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);
	m_Gui->Update();

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnStripper()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	m_Gui->Enable(ID_STRIPPER,false);
	m_Gui->Update();

  vtkMAFSmartPointer<vtkStripper> stripper;
	stripper->SetInput(m_ResultPolydata);
	stripper->Update();

	m_ResultPolydata->DeepCopy(stripper->GetOutput());

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnTriangulate()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	m_Gui->Enable(ID_TRIANGLE,false);
	m_Gui->Update();

	vtkMAFSmartPointer<vtkTriangleFilter> triangleFilter;
	triangleFilter->SetInput(m_ResultPolydata);
	triangleFilter->Update();

	m_ResultPolydata->DeepCopy(triangleFilter->GetOutput());

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnGenerateNormals()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	m_Gui->Enable(ID_NORMALS_ANGLE,false);
	m_Gui->Enable(ID_EDGE_SPLITTING,false);
	m_Gui->Enable(ID_FLIP_NORMALS,false);
	m_Gui->Enable(ID_NORMALS,false);
	m_Gui->Update();

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

	m_Gui->Enable(ID_NORMALS_ANGLE,true);
	m_Gui->Enable(ID_EDGE_SPLITTING,true);
	m_Gui->Enable(ID_FLIP_NORMALS,true);
	m_Gui->Enable(ID_NORMALS,true);

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpFilterSurface::OnResetNormals()
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
void mafOpFilterSurface::OnPreview()
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
void mafOpFilterSurface::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

  ((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());
	
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
	m_Gui->Enable(ID_ITERACTION,true);
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

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,false);
	m_Gui->Enable(wxOK,false);
	m_Gui->Update();

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
