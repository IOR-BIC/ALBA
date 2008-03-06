/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpDecimateSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi - Matteo Giacomoni
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

#include "mafOpDecimateSurface.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafVMESurface.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCleanPolyData.h"

#include "vtkDecimatePro.h"

#include "vtkTriangleFilter.h"
#include "vtkProperty.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpDecimateSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpDecimateSurface::mafOpDecimateSurface(const wxString label) :
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

  m_WireFrame = 0;
	
	m_ResultPolydata	  = NULL;
	m_OriginalPolydata  = NULL;

}
//----------------------------------------------------------------------------
mafOpDecimateSurface::~mafOpDecimateSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool mafOpDecimateSurface::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *mafOpDecimateSurface::Copy()   
//----------------------------------------------------------------------------
{
  return (new mafOpDecimateSurface(m_Label));
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
	ID_CLEAR,
};
//----------------------------------------------------------------------------
void mafOpDecimateSurface::OpRun()   
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

	  
		m_Gui->Bool(ID_WIREFRAME,"wireframe",&m_WireFrame, 0);

		m_Gui->Divider(2);
		m_Gui->Label("decimate",true);
		m_Gui->Bool(ID_PRESERVE_TOPOLOGY,"preserve topology",&m_TopologyFlag, 1);
		m_Gui->Slider(ID_REDUCTION,"reduc.to %",&m_Reduction,1, 100);
		m_Gui->Button(ID_DECIMATE,"apply decimate");

		//mmgMaterialButton *m_MaterialButton = new mmgMaterialButton(m_Input,this);
		
		m_Gui->Divider(2);
		m_Gui->Label("");
		m_Gui->Button(ID_PREVIEW,"preview");
		m_Gui->Button(ID_CLEAR,"undo");
		m_Gui->OkCancel();
		m_Gui->Enable(wxOK,false);

		m_Gui->Enable(ID_PREVIEW,false);
		m_Gui->Enable(ID_CLEAR,false);

		m_Gui->Divider();

		m_Gui->Update();
		ShowGui();
	}
}
//----------------------------------------------------------------------------
void mafOpDecimateSurface::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpDecimateSurface::OpUndo()
//----------------------------------------------------------------------------
{
  ((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpDecimateSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {	   
      case ID_WIREFRAME:
        {
          mafVMESurface *surface;
          surface = mafVMESurface::SafeDownCast(m_Input);

          if(m_WireFrame == 1)
            surface->GetMaterial()->m_Prop->SetRepresentationToWireframe();
          else
            surface->GetMaterial()->m_Prop->SetRepresentationToSurface();

          mafEventMacro(mafEvent(this,UPDATE_PROPERTY,m_Input, true));
					mafEventMacro(mafEvent(this,CAMERA_UPDATE));

        }
      break;
      case ID_DECIMATE:
        OnDecimate();
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
void mafOpDecimateSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
  mafVMESurface *surface;
  surface = mafVMESurface::SafeDownCast(m_Input);
  surface->GetMaterial()->m_Prop->SetRepresentationToSurface();
  mafEventMacro(mafEvent(this,UPDATE_PROPERTY,m_Input, true));

	HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpDecimateSurface::OnDecimate()
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

	vtkMAFSmartPointer<vtkDecimatePro> decimate;
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
		m_Gui->Enable(ID_CLEAR,true);
		m_Gui->Enable(wxOK,true);
		m_Gui->Update();
	}

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mafOpDecimateSurface::OnPreview()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
		wxBusyCursor wait;
	
  ((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());

	if(!m_TestMode)
	{
		m_Gui->Enable(ID_PREVIEW,false);
		m_Gui->Enable(ID_CLEAR,true);
		m_Gui->Enable(wxOK,true);
		m_Gui->Update();
	}
	

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	if(!m_TestMode)
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpDecimateSurface::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

  ((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());
	
	m_ResultPolydata->DeepCopy(m_OriginalPolydata);

	m_TopologyFlag= 0;
	m_Reduction		= 50;
	

	m_Gui->Enable(ID_CLEAN,true);
	
	m_Gui->Enable(ID_DECIMATE,true);
	m_Gui->Enable(ID_PRESERVE_TOPOLOGY,true);
	m_Gui->Enable(ID_REDUCTION,true);


	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,false);
	m_Gui->Enable(wxOK,false);
	

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;
  
  m_Gui->Update();

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
