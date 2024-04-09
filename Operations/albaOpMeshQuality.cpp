/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMeshQuality
 Authors: Matteo Giacomoni - Daniele Giunchi
 
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

#include "albaOpMeshQuality.h"
#include "wx/busyinfo.h"

#include "albaGUIDialog.h"
#include "albaRWIBase.h"
#include "albaRWI.h"
#include "albaInteractorCameraMove.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaGUIButton.h"
#include "albaGUIValidator.h"
#include "albaVMESurface.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVME.h"

#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkTriangleQualityRatio.h"
#include "vtkLookupTable.h"
#include "vtkCell.h"
#include "vtkScalarBarActor.h"
#include "vtkFeatureEdges.h"
#include "vtkTubeFilter.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpMeshQuality);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpMeshQuality::albaOpMeshQuality(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= false;

	m_CheckMeshQuality = NULL;
	m_LabelAverageAspectRatio = NULL;
	m_LabelMaxAspectRatio = NULL;
	m_LabelMinAspectRatio = NULL;

	m_Actor = NULL;
	m_BarActor = NULL;
  m_FeatureEdgeFilter = NULL;
  m_Angle = 40.0;
}
//----------------------------------------------------------------------------
albaOpMeshQuality::~albaOpMeshQuality()
//----------------------------------------------------------------------------
{
	vtkDEL(m_CheckMeshQuality);
}
//----------------------------------------------------------------------------
bool albaOpMeshQuality::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("albaVMESurface"));
}
//----------------------------------------------------------------------------
albaOp *albaOpMeshQuality::Copy()   
//----------------------------------------------------------------------------
{
	return (new albaOpMeshQuality(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum MESH_QUALITY_ID
{
	ID_OK = MINID,
  ID_RADIUS,
};
//----------------------------------------------------------------------------
void albaOpMeshQuality::OpRun()   
//----------------------------------------------------------------------------
{  
	vtkNEW(m_CheckMeshQuality);

	// interface:
	int result = OP_RUN_CANCEL;
	CreateOpDialog();
	int ret_dlg = m_Dialog->ShowModal();
	if( ret_dlg == wxID_OK )
	{
		result = OP_RUN_OK;
	}
	DeleteOpDialog();

	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpMeshQuality::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
    case ID_RADIUS:
      m_FeatureEdgeFilter->SetFeatureAngle(m_Angle);
      m_FeatureEdgeFilter->Update();
      this->m_Rwi->CameraUpdate();
      break;
		case ID_OK:
			m_Dialog->EndModal(wxID_OK);
			break;
		case wxOK:
			OpStop(OP_RUN_OK);        
			break;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpMeshQuality::OpStop(int result)
//----------------------------------------------------------------------------
{
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpMeshQuality::DeleteOpDialog()
//----------------------------------------------------------------------------
{
	m_Mouse->RemoveObserver(m_Picker);

	m_Rwi->m_RenFront->RemoveActor(m_Actor);
	m_Rwi->m_RenFront->RemoveActor(m_BarActor);
  m_Rwi->m_RenFront->RemoveActor(m_ActorFeatureEdge);

	vtkDEL(m_Actor);
	vtkDEL(m_BarActor);
	vtkDEL(m_Mapper);

  vtkDEL(m_ActorFeatureEdge);
  vtkDEL(m_MapperFeatureEdge);
  vtkDEL(m_TubeFilter);
  vtkDEL(m_FeatureEdgeFilter);

	albaDEL(m_Picker);

	cppDEL(m_Rwi); 
	cppDEL(m_Dialog);
}
//----------------------------------------------------------------------------
void albaOpMeshQuality::CreateOpDialog()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	//===== setup interface ====
	m_Dialog = new albaGUIDialog("Check Mesh Quality", albaCLOSEWINDOW | albaRESIZABLE);

	m_Rwi = new albaRWI(m_Dialog,ONE_LAYER,false);
	m_Rwi->SetListener(this);//SIL. 16-6-2004: 
	m_Rwi->CameraSet(CAMERA_PERSPECTIVE);
	m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
	m_Rwi->SetSize(0,0,500,500);
	//m_Rwi->m_RenderWindow->AddRenderer(m_PIPRen);
	m_Rwi->Show(true);
	m_Rwi->m_RwiBase->SetMouse(m_Mouse);
	albaNEW(m_Picker);
	m_Picker->SetListener(this);
	m_Mouse->AddObserver(m_Picker, MCH_INPUT);

	wxPoint p = wxDefaultPosition;

	wxStaticText *labelAspectRatio = new wxStaticText(m_Dialog,   -1, _("Aspect ratio: "));
	m_LabelAverageAspectRatio  = new wxStaticText(m_Dialog,   -1, _(" average : "));
	m_LabelMaxAspectRatio  = new wxStaticText(m_Dialog,   -1, _(" max : "));
	m_LabelMinAspectRatio  = new wxStaticText(m_Dialog,   -1, _(" min : "));

  wxStaticText *labelAngle = new wxStaticText(m_Dialog, -1, _("Angle"),p, wxSize(80, 16 ));
  wxTextCtrl *angleText		= new wxTextCtrl(m_Dialog,ID_RADIUS, _("Angle"),p,wxSize(50, 16 ), wxNO_BORDER );

  angleText->SetValidator(albaGUIValidator(this,ID_RADIUS,angleText,&m_Angle,0.0,999.0));

	albaGUIButton  *b_ok =     new albaGUIButton(m_Dialog, ID_OK,     "ok", p, wxSize(80,20));

	b_ok->SetValidator(albaGUIValidator(this,ID_OK,b_ok));

	wxBoxSizer *h_sizer2= new wxBoxSizer(wxHORIZONTAL);
	h_sizer2->Add(m_LabelAverageAspectRatio,     0, wxLEFT);
	h_sizer2->Add(m_LabelMaxAspectRatio,     0, wxLEFT);
	h_sizer2->Add(m_LabelMinAspectRatio,     0, wxLEFT);
  h_sizer2->Add(labelAngle,     0, wxLEFT);
  h_sizer2->Add(angleText,     0, wxLEFT);

	wxBoxSizer *h_sizer3 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer3->Add(b_ok,      0,wxRIGHT);

	wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
	v_sizer->Add(m_Rwi->m_RwiBase, 1,wxEXPAND);
	v_sizer->Add(h_sizer2,     0,wxEXPAND | wxALL,5);
	v_sizer->Add(h_sizer3,     0,wxEXPAND | wxALL,5);

	m_Dialog->Add(v_sizer, 1, wxEXPAND);

	int x_pos,y_pos,w,h;
	albaGetFrame()->GetPosition(&x_pos,&y_pos);
	m_Dialog->GetSize(&w,&h);
	m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

	CreatePolydataPipeline();
	this->m_Rwi->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpMeshQuality::CreatePolydataPipeline()
//----------------------------------------------------------------------------
{
	albaVMESurface *surface = albaVMESurface::SafeDownCast(m_Input);
	vtkPolyData *dataset = vtkPolyData::SafeDownCast(surface->GetSurfaceOutput()->GetVTKData());

	for(int i=0;i<dataset->GetNumberOfCells();i++)
	{
		if(dataset->GetCell(i)->GetNumberOfPoints()!=3)
		{
			wxMessageBox(_("The mesh should be triangolarized"));
			return;
		}
	}

	m_CheckMeshQuality->SetInput(dataset);
	m_CheckMeshQuality->Update();

	double averageRatio=m_CheckMeshQuality->GetMeanRatio();
	double maxRatio=m_CheckMeshQuality->GetMaxRatio();
	double minRatio=m_CheckMeshQuality->GetMinRatio();

  if(!m_TestMode)
  {
	  m_LabelAverageAspectRatio->SetLabel(albaString::Format(" average : %.3lf",averageRatio));
	  m_LabelMaxAspectRatio->SetLabel(albaString::Format(" max : %.3lf",maxRatio));
	  m_LabelMinAspectRatio->SetLabel(albaString::Format(" min : %.3lf",minRatio));
  

	  double BoundingBox[6];
	  dataset->GetBounds(BoundingBox);
	  vtkNEW(m_Mapper);
	  m_Mapper->SetInputData(((vtkPolyData*)m_CheckMeshQuality->GetOutput()));

	  //vtkDataArray *scalars=((vtkPolyData*)m_CheckMeshQuality->GetOutput())->GetCellData()->GetArray("quality");

	  double sr[2];
	  m_CheckMeshQuality->GetOutput()->GetCellData()->GetScalars()->GetRange(sr);
	  vtkLookupTable *lut;
	  vtkNEW(lut);
	  lut->SetTableRange(sr);
	  lut->Build();
	  m_Mapper->SetScalarRange(sr);
	  m_Mapper->SetLookupTable(lut);
	  //m_Mapper->SetColorModeToMapScalars();
	  m_Mapper->ScalarVisibilityOn();

	  vtkNEW(m_Actor);
	  m_Actor->SetMapper(m_Mapper);
	  //m_Actor->PickableOff();

	  m_Mapper->Modified();
	  m_Mapper->Update();

	  vtkNEW(m_BarActor);
	  m_BarActor->SetLookupTable(lut);

	  m_Rwi->m_RenFront->AddActor(m_Actor);
	  m_Rwi->m_RenFront->AddActor(m_BarActor);

    vtkNEW(m_FeatureEdgeFilter);
    m_FeatureEdgeFilter->SetInputData(dataset);
    m_FeatureEdgeFilter->FeatureEdgesOn();
    m_FeatureEdgeFilter->NonManifoldEdgesOn();
    m_FeatureEdgeFilter->BoundaryEdgesOff();
    m_FeatureEdgeFilter->ManifoldEdgesOff();
    m_FeatureEdgeFilter->SetFeatureAngle(m_Angle);
    m_FeatureEdgeFilter->Update();

    double boundingBox[6];
    dataset->GetBounds(boundingBox);

    //Compute Max bound
    double dimX, dimY, dimZ;
    dimX = (boundingBox[1] - boundingBox[0]);
    dimY = (boundingBox[3] - boundingBox[2]);
    dimZ = (boundingBox[5] - boundingBox[4]);
    double maxBounds = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 
    double radius = maxBounds / 100;

    vtkNEW(m_TubeFilter);
    m_TubeFilter->SetInputConnection(m_FeatureEdgeFilter->GetOutputPort());
    m_TubeFilter->SetNumberOfSides(10);
    m_TubeFilter->SetRadius(radius);
    m_TubeFilter->Update();

    vtkNEW(m_MapperFeatureEdge);
    m_MapperFeatureEdge->SetInputConnection(m_TubeFilter->GetOutputPort());
    m_MapperFeatureEdge->Update();

    vtkNEW(m_ActorFeatureEdge);
    m_ActorFeatureEdge->SetMapper(m_MapperFeatureEdge);

    m_Rwi->m_RenFront->AddActor(m_ActorFeatureEdge);

	  m_Rwi->m_RenFront->ResetCamera(BoundingBox);
  
  }
}
