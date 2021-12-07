/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDecimateSurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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

#include "albaOpDecimateSurface.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMESurface.h"
#include "mmaMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCleanPolyData.h"

#include "vtkDecimatePro.h"

#include "vtkTriangleFilter.h"
#include "vtkProperty.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpDecimateSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpDecimateSurface::albaOpDecimateSurface(const wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

  m_PreviewResultFlag	= false;
	m_ClearInterfaceFlag= false;
  
	m_TopologyFlag= 0;
	m_Reduction		= 50;

  m_WireFrame = 0;
	
	m_ResultPolydata	  = NULL;
	m_OriginalPolydata  = NULL;

}
//----------------------------------------------------------------------------
albaOpDecimateSurface::~albaOpDecimateSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool albaOpDecimateSurface::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVMESurface));
}
//----------------------------------------------------------------------------
albaOp *albaOpDecimateSurface::Copy()   
//----------------------------------------------------------------------------
{
  return (new albaOpDecimateSurface(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_CLEAN = MINID,
  ID_WIREFRAME,
	ID_DECIMATE,
	ID_PRESERVE_TOPOLOGY,
	ID_REDUCTION,
	ID_PREVIEW,
	ID_RESET_ALL,
};
//----------------------------------------------------------------------------
void albaOpDecimateSurface::OpRun()   
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

	  
		m_Gui->Bool(ID_WIREFRAME,"wireframe",&m_WireFrame, 0);

		m_Gui->Divider(2);
		m_Gui->Label("decimate",true);
		m_Gui->Bool(ID_PRESERVE_TOPOLOGY,"preserve topology",&m_TopologyFlag, 1);
		m_Gui->Slider(ID_REDUCTION,"reduc.to %",&m_Reduction,1, 100);
		m_Gui->Button(ID_DECIMATE,"apply decimate");

		//albaGUIMaterialButton *m_MaterialButton = new albaGUIMaterialButton(m_Input,this);
		
		m_Gui->Divider(2);
		m_Gui->Label("");
		m_Gui->Button(ID_PREVIEW,"preview");
		m_Gui->Button(ID_RESET_ALL,"undo");
		m_Gui->OkCancel();
		m_Gui->Enable(wxOK,false);

		m_Gui->Enable(ID_PREVIEW,false);
		m_Gui->Enable(ID_RESET_ALL,false);

		m_Gui->Divider();

		m_Gui->Update();
		ShowGui();
	}
}
//----------------------------------------------------------------------------
void albaOpDecimateSurface::OpDo()
//----------------------------------------------------------------------------
{
	((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpDecimateSurface::OpUndo()
//----------------------------------------------------------------------------
{
  ((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpDecimateSurface::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	   
      case ID_WIREFRAME:
        {
          albaVMESurface *surface;
          surface = albaVMESurface::SafeDownCast(m_Input);

          if(m_WireFrame == 1)
            surface->GetMaterial()->m_Prop->SetRepresentationToWireframe();
          else
            surface->GetMaterial()->m_Prop->SetRepresentationToSurface();

          albaEventMacro(albaEvent(this,UPDATE_PROPERTY,m_Input, true));
					GetLogicManager()->CameraUpdate();
        }
      break;
      case ID_DECIMATE:
        OnDecimate();
      break; 
      case ID_PREVIEW:
        OnPreview(); 
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
void albaOpDecimateSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
  albaVMESurface *surface;
  surface = albaVMESurface::SafeDownCast(m_Input);
  surface->GetMaterial()->m_Prop->SetRepresentationToSurface();
  albaEventMacro(albaEvent(this,UPDATE_PROPERTY,m_Input, true));

	HideGui();
	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpDecimateSurface::OnDecimate()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
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

	if(!m_TestMode)
	{
		m_Gui->Enable(ID_DECIMATE,true);
		m_Gui->Enable(ID_PRESERVE_TOPOLOGY,true);
		m_Gui->Enable(ID_REDUCTION,true);

		m_Gui->Enable(ID_PREVIEW,true);
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
		m_Gui->Update();
	}

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void albaOpDecimateSurface::OnPreview()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
		wxBusyCursor wait;
	
  ((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());

	if(!m_TestMode)
	{
		m_Gui->Enable(ID_PREVIEW,false);
		m_Gui->Enable(ID_RESET_ALL,true);
		m_Gui->Enable(wxOK,true);
		m_Gui->Update();
	}
	

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	if(!m_TestMode)
		GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpDecimateSurface::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

  ((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	
	m_ResultPolydata->DeepCopy(m_OriginalPolydata);

	m_TopologyFlag= 0;
	m_Reduction		= 50;
	

	m_Gui->Enable(ID_CLEAN,true);
	
	m_Gui->Enable(ID_DECIMATE,true);
	m_Gui->Enable(ID_PRESERVE_TOPOLOGY,true);
	m_Gui->Enable(ID_REDUCTION,true);


	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_RESET_ALL,false);
	m_Gui->Enable(wxOK,false);
	

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;
  
  m_Gui->Update();

	GetLogicManager()->CameraUpdate();
}
