/*=========================================================================

 Program: MAF2
 Module: mafOpSmoothSurface
 Authors: Matteo Giacomoni
 
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

#include "mafOpSmoothSurface.h"
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
mafCxxTypeMacro(mafOpSmoothSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpSmoothSurface::mafOpSmoothSurface(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

	m_PreviewResultFlag	= false;
	m_ClearInterfaceFlag= false;

	m_Iterations	= 50;

	m_ResultPolydata	  = NULL;
	m_OriginalPolydata  = NULL;
	m_BoundarySmoothing = 0;
}
//----------------------------------------------------------------------------
mafOpSmoothSurface::~mafOpSmoothSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool mafOpSmoothSurface::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *mafOpSmoothSurface::Copy()   
//----------------------------------------------------------------------------
{
	return (new mafOpSmoothSurface(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum SMOOTH_SURFACE_ID
{
	ID_SMOOTH = MINID,
	ID_PREVIEW,
	ID_CLEAR,
	ID_ITERACTION,
	ID_BOUNDARY,
};
//----------------------------------------------------------------------------
void mafOpSmoothSurface::OpRun()   
//----------------------------------------------------------------------------
{  
	vtkNEW(m_ResultPolydata);
	m_ResultPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

  if(!m_TestMode)
  {
    CreateGui();
  }

}
//----------------------------------------------------------------------------
void mafOpSmoothSurface::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mafGUI(this);

  m_Gui->Label("");
  m_Gui->Label("smooth",true);
  m_Gui->Slider(ID_ITERACTION,_("n.iteraction: "),&m_Iterations,0,500);
  m_Gui->Button(ID_SMOOTH,_("apply smooth"));
  m_Gui->Bool(ID_BOUNDARY,_("Boundary"),&m_BoundarySmoothing,0,_("Boundary Smoothing On/Off"));

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
void mafOpSmoothSurface::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpSmoothSurface::OpUndo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpSmoothSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{	
		case ID_SMOOTH:
			OnSmooth();
			break;
		case ID_PREVIEW:
			OnPreview(); 
			break;
		case ID_CLEAR:
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
void mafOpSmoothSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpSmoothSurface::OnSmooth()
//----------------------------------------------------------------------------
{
  if(!m_TestMode)
  {
	  wxBusyCursor wait;
	  m_Gui->Enable(ID_SMOOTH,false);
	  m_Gui->Enable(ID_ITERACTION,false);
	  m_Gui->Enable(ID_BOUNDARY, false);
	  m_Gui->Update();
  }

	vtkMAFSmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
	smoothFilter->SetInput(m_ResultPolydata);
	smoothFilter->SetNumberOfIterations(m_Iterations);
	smoothFilter->FeatureEdgeSmoothingOn();
	m_BoundarySmoothing?smoothFilter->BoundarySmoothingOn():smoothFilter->BoundarySmoothingOff();
	smoothFilter->Update();

	m_ResultPolydata->DeepCopy(smoothFilter->GetOutput());

  if(!m_TestMode)
  {
	  m_Gui->Enable(ID_SMOOTH,true);
	  m_Gui->Enable(ID_ITERACTION,true);
	  m_Gui->Enable(ID_BOUNDARY,true);

	  m_Gui->Enable(ID_PREVIEW,true);
	  m_Gui->Enable(ID_CLEAR,true);
	  m_Gui->Enable(wxOK,true);
  }

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpSmoothSurface::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpSmoothSurface::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());

	m_ResultPolydata->DeepCopy(m_OriginalPolydata);

	m_Iterations	= 50;
	m_BoundarySmoothing = false;

	m_Gui->Enable(ID_SMOOTH,true);
	m_Gui->Enable(ID_ITERACTION,true);
	m_Gui->Enable(ID_BOUNDARY,true);

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,false);
	m_Gui->Enable(wxOK,false);
	m_Gui->Update();

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
