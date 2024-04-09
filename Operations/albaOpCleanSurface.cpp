/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCleanSurface
 Authors: Alessandro Chiarini
 
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

#include "albaOpCleanSurface.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMESurface.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

#include "vtkALBASmartPointer.h"
#include "vtkCleanPolyData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCleanSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCleanSurface::albaOpCleanSurface(const wxString &label) :
albaOp(label)
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
albaOpCleanSurface::~albaOpCleanSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool albaOpCleanSurface::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node && node->IsALBAType(albaVMESurface));
}
//----------------------------------------------------------------------------
albaOp *albaOpCleanSurface::Copy()   
//----------------------------------------------------------------------------
{
	return (new albaOpCleanSurface(m_Label));
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
void albaOpCleanSurface::OpRun()   
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
void albaOpCleanSurface::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new albaGUI(this);

  m_Gui->Label("");
  m_Gui->Label(_("Clean Surface"),true);
  m_Gui->Button(ID_SMOOTH,_("Apply Clean"));

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
void albaOpCleanSurface::OpDo()
//----------------------------------------------------------------------------
{
	((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpCleanSurface::OpUndo()
//----------------------------------------------------------------------------
{
	((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpCleanSurface::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{	
		case ID_SMOOTH:
			OnClean();
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
void albaOpCleanSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpCleanSurface::OnClean()
//----------------------------------------------------------------------------
{
  if(!m_TestMode)
  {
	  wxBusyCursor wait;
	  m_Gui->Enable(ID_SMOOTH,false);
	  m_Gui->Enable(ID_ITERACTION,false);
	  m_Gui->Update();
  }

	vtkALBASmartPointer<vtkCleanPolyData> cleanFilter;
	cleanFilter->SetTolerance(0.0); // aacc expose from GUI?
	cleanFilter->SetInputData(m_ResultPolydata);
	cleanFilter->Update();

	m_ResultPolydata->DeepCopy(cleanFilter->GetOutput());


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
void albaOpCleanSurface::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpCleanSurface::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());

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
