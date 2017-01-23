/*=========================================================================

 Program: MAF2
 Module: mafOpTriangulateSurface
 Authors: Alessandro Chiarini
 
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

#include "mafOpTriangulateSurface.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpTriangulateSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpTriangulateSurface::mafOpTriangulateSurface(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

	m_PreviewResultFlag	= false;
	m_ClearInterfaceFlag= false;


	m_ResultPolydata	  = NULL;
	m_OriginalPolydata  = NULL;
}
//----------------------------------------------------------------------------
mafOpTriangulateSurface::~mafOpTriangulateSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool mafOpTriangulateSurface::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *mafOpTriangulateSurface::Copy()   
//----------------------------------------------------------------------------
{
	return (new mafOpTriangulateSurface(m_Label));
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
};
//----------------------------------------------------------------------------
void mafOpTriangulateSurface::OpRun()   
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
void mafOpTriangulateSurface::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mafGUI(this);

  m_Gui->Label("");
  m_Gui->Label(_("Triangulate Surface"),true);
  m_Gui->Button(ID_SMOOTH,_("Triangulate"));

  m_Gui->Divider(2);
  m_Gui->Label("");
  m_Gui->Button(ID_PREVIEW,_("preview"));
  m_Gui->Button(ID_CLEAR,_("clear"));
  m_Gui->OkCancel();
  m_Gui->Enable(wxOK,false);

  m_Gui->Enable(ID_PREVIEW,false);
  m_Gui->Enable(ID_CLEAR,false);

  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void mafOpTriangulateSurface::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafOpTriangulateSurface::OpUndo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafOpTriangulateSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{	
		case ID_SMOOTH:
			OnTriangle();
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
void mafOpTriangulateSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpTriangulateSurface::OnTriangle()
//----------------------------------------------------------------------------
{
  if(!m_TestMode)
  {
	  wxBusyCursor wait;
	  m_Gui->Enable(ID_SMOOTH,false);
	  m_Gui->Enable(ID_ITERACTION,false);
	  m_Gui->Update();
  }

	vtkMAFSmartPointer<vtkTriangleFilter> smoothFilter;
	smoothFilter->SetInput(m_ResultPolydata);
	smoothFilter->Update();

	m_ResultPolydata->DeepCopy(smoothFilter->GetOutput());

  if(!m_TestMode)
  {
	  m_Gui->Enable(ID_SMOOTH,true);
	  m_Gui->Enable(ID_ITERACTION,true);

	  m_Gui->Enable(ID_PREVIEW,true);
	  m_Gui->Enable(ID_CLEAR,true);
	  m_Gui->Enable(wxOK,true);
  }

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpTriangulateSurface::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafOpTriangulateSurface::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());

	m_ResultPolydata->DeepCopy(m_OriginalPolydata);


	m_Gui->Enable(ID_SMOOTH,true);
	m_Gui->Enable(ID_ITERACTION,true);

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,false);
	m_Gui->Enable(wxOK,false);
	m_Gui->Update();

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;

	GetLogicManager()->CameraUpdate();
}
