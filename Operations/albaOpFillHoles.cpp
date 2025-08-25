/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFillHoles
 Authors: Matteo Giacomoni, Josef Kohout
 
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

#include "albaOpFillHoles.h"
#include "albaGUIBusyInfo.h"

#include "albaGUIDialog.h"
#include "albaRWIBase.h"
#include "albaRWI.h"
#include "mmiSelectPoint.h"
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
#include "vtkHoleConnectivity.h"
#include "vtkFeatureEdges.h"
#include "vtkTubeFilter.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkAppendPolyData.h"
#include "vtkALBAFillingHole.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpFillHoles);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpFillHoles::albaOpFillHoles(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;
	m_InputPreserving = false;

	m_OriginalPolydata	= NULL;

	m_ActorSurface						= NULL;
	m_ActorSelectedHole				= NULL;
	m_MapperSelectedHole			= NULL;
	m_MapperSurface						= NULL;
	m_ExctractHole						= NULL;
	m_ExctractFreeEdges				= NULL;
	m_ExctractHole						= NULL;
  m_Glyph                   = NULL;

	m_Picker = NULL;

	m_SelectedPoint = false;

	m_AllHoles	= 0;
	m_Smooth		= 0;
	m_Diameter	= 100.0;
  m_PointID = -1;

  m_SmoothType = 1;
  m_ThinPlateSmoothingSteps = 500;
}
//----------------------------------------------------------------------------
albaOpFillHoles::~albaOpFillHoles()
//----------------------------------------------------------------------------
{
  for(int i=0;i<m_VTKResult.size();i++)
  {
    vtkDEL(m_VTKResult[i]);
  }
  m_VTKResult.clear();

	vtkDEL(m_OriginalPolydata);

  if (m_ExctractHole)
  {
    vtkDEL(m_ExctractHole);
  }
}
//----------------------------------------------------------------------------
bool albaOpFillHoles::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("albaVMESurface"));
}
//----------------------------------------------------------------------------
albaOp *albaOpFillHoles::Copy()   
//----------------------------------------------------------------------------
{
	return (new albaOpFillHoles(m_Label));
}
//----------------------------------------------------------------------------
void albaOpFillHoles::OpRun()   
//----------------------------------------------------------------------------
{	
	albaVMESurface *surface=albaVMESurface::SafeDownCast(m_Input);
	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
	m_OriginalPolydata->Update();

  vtkPolyData *newPoly;
  vtkNEW(newPoly);
  newPoly->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
  newPoly->Update();
  m_VTKResult.push_back(newPoly);

	vtkNEW(m_ExctractHole);

	// interface:
	int result = OP_RUN_CANCEL;
  if (!m_TestMode)
  {
	  CreateOpDialog();
	  int ret_dlg = m_Dialog->ShowModal();
	  if( ret_dlg == wxID_OK )
	  {
		  result = OP_RUN_OK;
	  }
	  DeleteOpDialog();
  }

	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpFillHoles::OpDo()
//----------------------------------------------------------------------------
{
  ((albaVMESurface*)m_Input)->SetData(m_VTKResult[m_VTKResult.size()-1],m_Input->GetTimeStamp());
  ((albaVMESurface*)m_Input)->Modified();

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpFillHoles::OpUndo()
//----------------------------------------------------------------------------
{
	((albaVMESurface*)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	((albaVMESurface*)m_Input)->Modified();

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpFillHoles::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
    case ID_UNDO:
      {
        m_MapperSurface->SetInput(m_VTKResult[m_VTKResult.size()-2]);
        m_MapperSurface->Update();

        m_ExctractFreeEdges->SetInput(m_VTKResult[m_VTKResult.size()-2]);
        m_ExctractFreeEdges->Update();

        vtkPolyData *poly = m_VTKResult[m_VTKResult.size()-1];
        poly->Update();

        vtkDEL(m_VTKResult[m_VTKResult.size()-1]);
        m_VTKResult.pop_back();
        
        m_ButtonUndo->Enable(m_VTKResult.size()>1);

        this->m_Rwi->CameraUpdate();
      }
      break;
		case VME_PICKED:
			{
				//Get the picked point
				int pointID = e->GetArg();
				if(pointID>=0)
					SelectHole(pointID);
			}
			break;
		case ID_FILL:
			{
				if(m_SelectedPoint || m_AllHoles)
					Fill();
			}
			break;
		case ID_ALL:
			{
				m_ButtonFill->Enable((m_AllHoles && m_ExctractFreeEdges->GetOutput()->GetNumberOfPoints()!=0) || m_SelectedPoint);
			}
		break;

    case ID_SMOOTH:
      m_SmoothTypeCtrl->Enable(m_Smooth != 0);    //throw down
    case ID_SMOOTH_TYPE:
      m_SmoothingStepsCtrl->Enable(m_Smooth != 0 && m_SmoothType == 1);
      break;

		case ID_DIAMETER:
			{
				m_Sphere->SetRadius(m_Diameter);
				m_Sphere->Modified();
				m_Rwi->CameraUpdate();
			}
			break;
		case ID_OK:
			m_Dialog->EndModal(wxID_OK);
			break;
		case ID_CANCEL:
			m_Dialog->EndModal(wxID_CANCEL);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpFillHoles::OpStop(int result)
//----------------------------------------------------------------------------
{
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpFillHoles::DeleteOpDialog()
//----------------------------------------------------------------------------
{
	m_Mouse->RemoveObserver(m_Picker);

	if(m_ActorSurface)
		m_Rwi->m_RenFront->RemoveActor(m_ActorSurface);
	
	if(m_ActorSelectedHole)
		m_Rwi->m_RenFront->RemoveActor(m_ActorSelectedHole);

	if(m_ActorHoles)
		m_Rwi->m_RenFront->RemoveActor(m_ActorHoles);

	vtkDEL(m_ActorSelectedHole);
	vtkDEL(m_MapperSelectedHole);
	vtkDEL(m_ActorSurface);
	vtkDEL(m_MapperSurface);
	vtkDEL(m_ActorHoles);
	vtkDEL(m_MapperHoles);
  vtkDEL(m_Glyph);
	vtkDEL(m_Sphere);
	vtkDEL(m_ExctractFreeEdges);
  vtkDEL(m_ExctractHole);
	albaDEL(m_Picker);
	cppDEL(m_Rwi); 
	cppDEL(m_Dialog);

}
//----------------------------------------------------------------------------
void albaOpFillHoles::CreateOpDialog()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	//===== setup interface ====
	m_Dialog = new albaGUIDialog("Fill Holes", albaCLOSEWINDOW | albaRESIZABLE);

	m_Rwi = new albaRWI(m_Dialog,ONE_LAYER,false);
	m_Rwi->SetListener(this);
	m_Rwi->CameraSet(CAMERA_PERSPECTIVE);
	m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
	m_Rwi->SetSize(0,0,500,500);
	m_Rwi->Show(true);
	m_Rwi->m_RwiBase->SetMouse(m_Mouse);

	albaNEW(m_Picker);
	m_Picker->SetListener(this);
	m_Mouse->AddObserver(m_Picker, MCH_INPUT);

	wxPoint p = wxDefaultPosition;

  //BES: 23.6.2008 - added to avoid confusion of DIAMETER text box with smoothing parameters
  wxStaticText* diam_label = new wxStaticText(m_Dialog, ID_DIAMETER_LABEL, _("  sphere size: "));
	wxTextCtrl *diameter = new wxTextCtrl(m_Dialog,ID_DIAMETER, _("sphere dim."),p,wxSize(50, 16 ), wxNO_BORDER );

	m_ButtonOk			= new albaGUIButton(m_Dialog, ID_OK,_("ok"), p, wxSize(80,20));
	albaGUIButton  *b_cancel	= new albaGUIButton(m_Dialog, ID_CANCEL,_("cancel"), p, wxSize(80,20));
	m_ButtonFill		= new albaGUIButton(m_Dialog, ID_FILL,_("fill"), p, wxSize(80,20));
  m_ButtonUndo    = new albaGUIButton(m_Dialog,ID_UNDO,_("undo"),p,wxSize(80,20));

	wxCheckBox *c_all		= new wxCheckBox(m_Dialog, ID_ALL,_("all holes"), p, wxSize(80,20));
	wxCheckBox *c_smooth	= new wxCheckBox(m_Dialog, ID_SMOOTH,_("smooth"), p, wxSize(70,20));

  m_SmoothTypeCtrl = new wxComboBox( m_Dialog, ID_SMOOTH_TYPE, wxEmptyString, 
    p, wxSize(90, 20 ), 0, NULL, wxCB_READONLY );
  m_SmoothTypeCtrl->Append( wxT("membrane") );
  m_SmoothTypeCtrl->Append( wxT("thin plate") );
  m_SmoothTypeCtrl->SetToolTip( 
    wxT("Membrane smoothing does not quarantee C1 connectivity of the patch and the "
    "surrounding mesh and, therefore, it is suitable for filling holes at the ends of "
    "blood vessels.\n\nThin plate smoothing guarantees C1 connectivity and, therefore, "
    "it is suitable for filling holes in curved parts.") );
  

  wxStaticText* stepsLabel = new wxStaticText( m_Dialog, wxID_ANY, wxT("  steps:"));     
  m_SmoothingStepsCtrl = new wxTextCtrl( m_Dialog, ID_SMOOTH_STEPS, wxEmptyString, 
    p, wxSize(50, 20 ), 0 );
  m_SmoothingStepsCtrl->SetToolTip( 
    wxT("Number of steps for thin plate smoothing. Larger value means smoother patch.") );


	m_ButtonOk->SetValidator(albaGUIValidator(this,ID_OK,m_ButtonOk));
	m_ButtonOk->Enable(false);
	b_cancel->SetValidator(albaGUIValidator(this,ID_CANCEL,b_cancel));
	m_ButtonFill->SetValidator(albaGUIValidator(this,ID_FILL,m_ButtonFill));
	m_ButtonFill->Enable(false);
  m_ButtonUndo->SetValidator(albaGUIValidator(this,ID_UNDO,m_ButtonUndo));
  m_ButtonUndo->Enable(false);

	diameter->SetValidator(albaGUIValidator(this,ID_DIAMETER,diameter,&m_Diameter,0.0,9999.0));

	c_all->SetValidator(albaGUIValidator(this,ID_ALL,c_all,&m_AllHoles));
	c_smooth->SetValidator(albaGUIValidator(this,ID_SMOOTH,c_smooth,&m_Smooth));
  m_SmoothTypeCtrl->SetValidator(albaGUIValidator(this, ID_SMOOTH_TYPE, m_SmoothTypeCtrl, &m_SmoothType));
  m_SmoothTypeCtrl->Enable(m_Smooth != 0);
  m_SmoothingStepsCtrl->SetValidator(albaGUIValidator(this, ID_SMOOTH_STEPS, 
    m_SmoothingStepsCtrl, &m_ThinPlateSmoothingSteps, 1, 1000000));
  m_SmoothingStepsCtrl->Enable(m_Smooth != 0 && m_SmoothType == 1);

	wxBoxSizer *h_sizer2 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer2->Add(c_all,0,wxRIGHT);
	h_sizer2->Add(c_smooth,0,wxRIGHT);
  h_sizer2->Add(m_SmoothTypeCtrl,0,wxRIGHT);
  h_sizer2->Add(stepsLabel,0,wxRIGHT);
  h_sizer2->Add(m_SmoothingStepsCtrl,0,wxRIGHT);
  h_sizer2->Add(diam_label,0, wxRIGHT);
	h_sizer2->Add(diameter,0,wxRIGHT);

	wxBoxSizer *h_sizer3 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer3->Add(m_ButtonFill,0,wxRIGHT);
  h_sizer3->Add(m_ButtonUndo,0,wxRIGHT);
	h_sizer3->Add(m_ButtonOk,0,wxRIGHT);
	h_sizer3->Add(b_cancel,0,wxRIGHT);

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

#include "vtkUnstructuredGrid.h"
//----------------------------------------------------------------------------
void albaOpFillHoles::Fill()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyCursor busy;
  }

	//fill the boundary polydata
	vtkALBAFillingHole *fillingHoleFilter;
	vtkNEW(fillingHoleFilter);
	//fillingHoleFilter->SetInput(m_ResultPolydata);
  fillingHoleFilter->SetInput(m_VTKResult[m_VTKResult.size()-1]);
	if(m_AllHoles)
		fillingHoleFilter->SetFillAllHole();  
	else
	{    
    //BES: 31.7.2008 - vtkPolyData has FindPoint, however, the VTK 4.2 
    //implementation has a serious drawback: it gets bounding box for
    //its cells and then it buckets points into a 3D grid of this box, 
    //so, if the data set contain some points not assigned to any cell,
    //there might be points lying outside the 3D grid => their 
    //grid cell index might be invalid and the application might crash

    //Simone Brazzale: 06/12/2010 - Implemented test case
    if (m_PointID==-1)
    {
      // Real case
      vtkUnstructuredGrid* pTmp = vtkUnstructuredGrid::New();
      pTmp->SetPoints(m_VTKResult[m_VTKResult.size()-1]->GetPoints());
      int pointID = pTmp->FindPoint(m_ExctractHole->GetOutput()->GetPoint(0));
      pTmp->Delete();
		  fillingHoleFilter->SetFillAHole(pointID);  
    }
    else
    {
      // Test case
      fillingHoleFilter->SetFillAHole(m_PointID);  
    }
	}
	
	if (!m_Smooth)
    fillingHoleFilter->SetFlatFill();		
	else
  {    
    fillingHoleFilter->SetSmoothFill(m_SmoothType != 0);
    fillingHoleFilter->SetSmoothThinPlateSteps(m_ThinPlateSmoothingSteps);
  }		

	fillingHoleFilter->Update();

	/*m_ResultPolydata->DeepCopy(fillingHoleFilter->GetOutput());
	m_ResultPolydata->Modified();
	m_ResultPolydata->Update();*/
  vtkPolyData *newPoly;
  vtkNEW(newPoly);
  newPoly->DeepCopy(fillingHoleFilter->GetOutput());
  newPoly->Modified();
  newPoly->Update();
  m_VTKResult.push_back(newPoly);

  if (!m_TestMode)
  {
    m_ButtonUndo->Enable(m_VTKResult.size()>1);

    m_MapperSurface->SetInput(m_VTKResult[m_VTKResult.size()-1]);
    m_MapperSurface->Update();

    m_ExctractFreeEdges->SetInput(m_VTKResult[m_VTKResult.size()-1]);
    m_ExctractFreeEdges->Update();

	  m_Rwi->m_RenFront->RemoveActor(m_ActorSelectedHole);

	  this->m_Rwi->CameraUpdate();

	  m_ButtonOk->Enable(true);
	  m_ButtonFill->Enable(m_SelectedPoint);
  }
  m_SelectedPoint = false;
  m_PointID = -1;

	vtkDEL(fillingHoleFilter);
}
//----------------------------------------------------------------------------
void albaOpFillHoles::SelectHole(int pointID)
//----------------------------------------------------------------------------
{
	/*m_ResultPolydata->Update();
	m_ResultPolydata->GetPoint(pointID,m_CoordPointSelected);*/
  m_VTKResult[m_VTKResult.size()-1]->Update();
  m_VTKResult[m_VTKResult.size()-1]->GetPoint(pointID,m_CoordPointSelected);

	m_ExctractHole->SetInput(m_ExctractFreeEdges->GetOutput()); 
	m_ExctractHole->SetPoint(m_CoordPointSelected);

	m_Rwi->m_RenFront->RemoveActor(m_ActorSelectedHole);

	vtkGlyph3D *glyph;
	vtkNEW(glyph);
	glyph->SetInput(m_ExctractHole->GetOutput());
	glyph->SetSource(m_Sphere->GetOutput());

	vtkNEW(m_MapperSelectedHole);
	m_MapperSelectedHole->SetInput(glyph->GetOutput());

	//Show the selected hole - selected hole is red
	vtkNEW(m_ActorSelectedHole);
	m_ActorSelectedHole->SetMapper(m_MapperSelectedHole);
	m_ActorSelectedHole->GetProperty()->SetColor(1,0,0);
	m_ActorSelectedHole->PickableOff();

	m_Rwi->m_RenFront->AddActor(m_ActorSelectedHole);

	m_Rwi->m_RenderWindow->Render();

	vtkDEL(glyph);

	m_SelectedPoint = m_ExctractHole->GetOutput()->GetNumberOfPoints()!=0;
	m_ButtonFill->Enable(m_SelectedPoint);  
}
//----------------------------------------------------------------------------
void albaOpFillHoles::CreatePolydataPipeline()
//----------------------------------------------------------------------------
{
	double bounds[6];
	//m_ResultPolydata->GetBounds(bounds);
  m_VTKResult[m_VTKResult.size()-1]->GetBounds(bounds);

	double dimX, dimY, dimZ;
	dimX = (bounds[1] - bounds[0]);
	dimY = (bounds[3] - bounds[2]);
	dimZ = (bounds[5] - bounds[4]);

	//Compute Max bound
	double maxBounds = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 
	m_Diameter = maxBounds / 100;
	
	//Exctract Holes from the input surface
	vtkNEW(m_ExctractFreeEdges);
	m_ExctractFreeEdges->SetInput(m_VTKResult[m_VTKResult.size()-1]);
	m_ExctractFreeEdges->SetBoundaryEdges(1);
	m_ExctractFreeEdges->SetFeatureEdges(0);
	m_ExctractFreeEdges->SetNonManifoldEdges(0);
	m_ExctractFreeEdges->SetManifoldEdges(0);
	m_ExctractFreeEdges->Update();
	
	vtkNEW(m_Sphere);
	m_Sphere->SetRadius(m_Diameter);

	//Create a m_Glyph to highlight the holes
	vtkNEW(m_Glyph);
	m_Glyph->SetInput(m_ExctractFreeEdges->GetOutput());
	m_Glyph->SetSource(m_Sphere->GetOutput());

	vtkNEW(m_MapperHoles);
	m_MapperHoles->SetInput(m_Glyph->GetOutput());
	
	vtkNEW(m_ActorHoles);
	m_ActorHoles->SetMapper(m_MapperHoles);
	m_ActorHoles->GetProperty()->SetColor(0.0,1.0,0.0);
	m_ActorHoles->PickableOff();

	vtkNEW(m_MapperSurface);
	m_MapperSurface->SetInput(m_VTKResult[m_VTKResult.size()-1]);

	vtkNEW(m_ActorSurface);
	m_ActorSurface->SetMapper(m_MapperSurface);

	m_Rwi->m_RenFront->AddActor(m_ActorSurface);
	m_Rwi->m_RenFront->AddActor(m_ActorHoles);

	m_Rwi->m_RenFront->ResetCamera(bounds);
}