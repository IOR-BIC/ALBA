/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: mmoFillHoles.cpp,v $
Language:  C++
Date:      $Date: 2009-05-12 08:44:17 $
Version:   $Revision: 1.2.2.1 $
Authors:   Matteo Giacomoni, Josef Kohout
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoFillHoles.h"
#include "wx/busyinfo.h"

#include "mafGUIDialog.h"
#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mmiSelectPoint.h"
#include "mmdMouse.h"
#include "mafGUIButton.h"
#include "mafGUIValidator.h"
#include "mafVMESurface.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafNode.h"
#include "mafVME.h"

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
#include "vtkFillingHole.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoFillHoles);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoFillHoles::mmoFillHoles(const wxString &label) :
mafOp(label)
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

  m_SmoothType = 1;
  m_ThinPlateSmoothingSteps = 500;
}
//----------------------------------------------------------------------------
mmoFillHoles::~mmoFillHoles()
//----------------------------------------------------------------------------
{
  for(int i=0;i<m_VTKResult.size();i++)
  {
    vtkDEL(m_VTKResult[i]);
  }
  m_VTKResult.clear();

	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool mmoFillHoles::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("mafVMESurface"));
}
//----------------------------------------------------------------------------
mafOp *mmoFillHoles::Copy()   
//----------------------------------------------------------------------------
{
	return (new mmoFillHoles(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_OK = MINID,
	ID_CANCEL,
	ID_FILL,
	ID_ALL,
	ID_SMOOTH,
  ID_SMOOTH_TYPE,
  ID_SMOOTH_STEPS,
  ID_DIAMETER_LABEL,
	ID_DIAMETER,
  ID_UNDO,
};
//----------------------------------------------------------------------------
void mmoFillHoles::OpRun()   
//----------------------------------------------------------------------------
{	
	mafVMESurface *surface=mafVMESurface::SafeDownCast(m_Input);
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
	CreateOpDialog();
	int ret_dlg = m_Dialog->ShowModal();
	if( ret_dlg == wxID_OK )
	{
		result = OP_RUN_OK;
	}
	DeleteOpDialog();

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoFillHoles::OpDo()
//----------------------------------------------------------------------------
{
	/*((mafVMESurface*)m_Input)->SetData(m_ResultPolydata,((mafVME*)m_Input)->GetTimeStamp());
	((mafVMESurface*)m_Input)->Modified();*/

  ((mafVMESurface*)m_Input)->SetData(m_VTKResult[m_VTKResult.size()-1],((mafVME*)m_Input)->GetTimeStamp());
  ((mafVMESurface*)m_Input)->Modified();

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoFillHoles::OpUndo()
//----------------------------------------------------------------------------
{
	((mafVMESurface*)m_Input)->SetData(m_OriginalPolydata,((mafVME*)m_Input)->GetTimeStamp());
	((mafVMESurface*)m_Input)->Modified();

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoFillHoles::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        
        b_undo->Enable(m_VTKResult.size()>1);

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
				b_fill->Enable((m_AllHoles && m_ExctractFreeEdges->GetOutput()->GetNumberOfPoints()!=0) || m_SelectedPoint);
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
void mmoFillHoles::OpStop(int result)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mmoFillHoles::DeleteOpDialog()
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
	mafDEL(m_Picker);
	cppDEL(m_Rwi); 
	cppDEL(m_Dialog);

}
//----------------------------------------------------------------------------
void mmoFillHoles::CreateOpDialog()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	//===== setup interface ====
	m_Dialog = new mafGUIDialog("Fill Holes", mafCLOSEWINDOW | mafRESIZABLE);

	m_Rwi = new mafRWI(m_Dialog,ONE_LAYER,false);
	m_Rwi->SetListener(this);
	m_Rwi->CameraSet(CAMERA_PERSPECTIVE);
	m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
	m_Rwi->SetSize(0,0,500,500);
	m_Rwi->Show(true);
	m_Rwi->m_RwiBase->SetMouse(m_Mouse);

	mafNEW(m_Picker);
	m_Picker->SetListener(this);
	m_Mouse->AddObserver(m_Picker, MCH_INPUT);

	wxPoint p = wxDefaultPosition;

  //BES: 23.6.2008 - added to avoid confusion of DIAMETER text box with smoothing parameters
  wxStaticText* diam_label = new wxStaticText(m_Dialog, ID_DIAMETER_LABEL, _("  sphere size: "));
	wxTextCtrl *diameter = new wxTextCtrl(m_Dialog,ID_DIAMETER, _("sphere dim."),p,wxSize(50, 16 ), wxNO_BORDER );

	b_ok			= new mafGUIButton(m_Dialog, ID_OK,_("ok"), p, wxSize(80,20));
	mafGUIButton  *b_cancel	= new mafGUIButton(m_Dialog, ID_CANCEL,_("cancel"), p, wxSize(80,20));
	b_fill		= new mafGUIButton(m_Dialog, ID_FILL,_("fill"), p, wxSize(80,20));
  b_undo    = new mafGUIButton(m_Dialog,ID_UNDO,_("undo"),p,wxSize(80,20));

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


	b_ok->SetValidator(mafGUIValidator(this,ID_OK,b_ok));
	b_ok->Enable(false);
	b_cancel->SetValidator(mafGUIValidator(this,ID_CANCEL,b_cancel));
	b_fill->SetValidator(mafGUIValidator(this,ID_FILL,b_fill));
	b_fill->Enable(false);
  b_undo->SetValidator(mafGUIValidator(this,ID_UNDO,b_undo));
  b_undo->Enable(false);

	diameter->SetValidator(mafGUIValidator(this,ID_DIAMETER,diameter,&m_Diameter,0.0,9999.0));

	c_all->SetValidator(mafGUIValidator(this,ID_ALL,c_all,&m_AllHoles));
	c_smooth->SetValidator(mafGUIValidator(this,ID_SMOOTH,c_smooth,&m_Smooth));
  m_SmoothTypeCtrl->SetValidator(mafGUIValidator(this, ID_SMOOTH_TYPE, m_SmoothTypeCtrl, &m_SmoothType));
  m_SmoothTypeCtrl->Enable(m_Smooth != 0);
  m_SmoothingStepsCtrl->SetValidator(mafGUIValidator(this, ID_SMOOTH_STEPS, 
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
	h_sizer3->Add(b_fill,0,wxRIGHT);
  h_sizer3->Add(b_undo,0,wxRIGHT);
	h_sizer3->Add(b_ok,0,wxRIGHT);
	h_sizer3->Add(b_cancel,0,wxRIGHT);

	wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
	v_sizer->Add(m_Rwi->m_RwiBase, 1,wxEXPAND);
	v_sizer->Add(h_sizer2,     0,wxEXPAND | wxALL,5);
	v_sizer->Add(h_sizer3,     0,wxEXPAND | wxALL,5);

	m_Dialog->Add(v_sizer, 1, wxEXPAND);

	int x_pos,y_pos,w,h;
	mafGetFrame()->GetPosition(&x_pos,&y_pos);
	m_Dialog->GetSize(&w,&h);
	m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

	CreatePolydataPipeline();
	this->m_Rwi->CameraUpdate();
}

#include "vtkUnstructuredGrid.h"
//----------------------------------------------------------------------------
void mmoFillHoles::Fill()
//----------------------------------------------------------------------------
{
  wxBusyCursor busy;

	//fill the boundary polydata
	vtkFillingHole *fillingHoleFilter;
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

    vtkUnstructuredGrid* pTmp = vtkUnstructuredGrid::New();
    pTmp->SetPoints(m_VTKResult[m_VTKResult.size()-1]->GetPoints());
    int pointID = pTmp->FindPoint(m_ExctractHole->GetOutput()->GetPoint(0));
    pTmp->Delete();
  
    /*int pointID = m_VTKResult[m_VTKResult.size()-1]
      ->FindPoint(m_ExctractHole->GetOutput()->GetPoint(0));*/
		fillingHoleFilter->SetFillAHole(pointID);  
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

  b_undo->Enable(m_VTKResult.size()>1);

  m_MapperSurface->SetInput(m_VTKResult[m_VTKResult.size()-1]);
  m_MapperSurface->Update();

  m_ExctractFreeEdges->SetInput(m_VTKResult[m_VTKResult.size()-1]);
  m_ExctractFreeEdges->Update();

	m_Rwi->m_RenFront->RemoveActor(m_ActorSelectedHole);
	m_SelectedPoint = false;

	this->m_Rwi->CameraUpdate();

	b_ok->Enable(true);
	b_fill->Enable(m_SelectedPoint);

	vtkDEL(fillingHoleFilter);
	
}
//----------------------------------------------------------------------------
void mmoFillHoles::SelectHole(int pointID)
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
	b_fill->Enable(m_SelectedPoint);  
}
//----------------------------------------------------------------------------
void mmoFillHoles::CreatePolydataPipeline()
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