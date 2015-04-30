/*=========================================================================

 Program: MAF2
 Module: mafOpExtrusionHoles
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

#include "mafOpExtrusionHoles.h"
#include "mafNode.h"

#include "mafGUIDialog.h"
#include "mafGUIButton.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafGUIValidator.h"
#include "mafRWI.h"
#include "mmiSelectPoint.h"
#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"

#include "vtkRenderWindow.h"
#include "vtkHoleConnectivity.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkProperty.h"
#include "vtkTextureMapToPlane.h"
#include "vtkCleanPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkMAFExtrudeToCircle.h"
#include "vtkMath.h"
#include "vtkCell.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExtrusionHoles);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExtrusionHoles::mafOpExtrusionHoles(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_Canundo = true;
	m_InputPreserving = false;
	
	m_ExtractFreeEdges	= NULL;
	m_ExtractHole				= NULL;
	m_ExtrusionFilter		= NULL;

	m_SurfaceMapper			= NULL;
	m_SurfaceActor			= NULL;
	m_HolesMapper				= NULL;
	m_HolesActor				= NULL;
	m_SelectedHoleMapper= NULL;
	m_SelectedHoleActor	= NULL;

	m_Sphere						= NULL;
	m_Glyph							= NULL;
	m_ResultAfterExtrusion	= NULL;

	m_OriginalPolydata	= NULL;
	m_ResultPolydata		= NULL;

	m_ExtrusionFactor		= 2.5;

	m_MaxBounds = VTK_DOUBLE_MAX;

  m_ReverseExtrusion = FALSE;

  m_NumVertices = 50;
}
//----------------------------------------------------------------------------
mafOpExtrusionHoles::~mafOpExtrusionHoles()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ExtrusionFilter);
	vtkDEL(m_Sphere);
	vtkDEL(m_ExtractHole);
	vtkDEL(m_ExtractFreeEdges);
	vtkDEL(m_ResultAfterExtrusion);
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool mafOpExtrusionHoles::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("mafVMESurface"));
}
//----------------------------------------------------------------------------
mafOp *mafOpExtrusionHoles::Copy()   
//----------------------------------------------------------------------------
{
	return (new mafOpExtrusionHoles(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum EXTRUSION_HOLES_ID
{
	ID_OK = MINID,
	ID_CANCEL,
	ID_EXTRUSION_FACTOR,
	ID_EXTRUDE,
	ID_RADIUS,
  ID_REVERSE,
  ID_NUM_VERTICES,
};
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::OpRun()   
//----------------------------------------------------------------------------
{
	vtkNEW(m_OriginalPolydata);
	vtkNEW(m_ResultPolydata);
	vtkNEW(m_ResultAfterExtrusion);
	vtkNEW(m_ExtractFreeEdges);
	vtkNEW(m_ExtractHole);
	vtkNEW(m_ExtrusionFilter);

  ((mafVMESurface*)m_Input)->Update();

	m_OriginalPolydata->DeepCopy(((vtkPolyData*)((mafVMESurface*)m_Input)->GetOutput()->GetVTKData()));
	m_OriginalPolydata->Update();
	m_ResultPolydata->DeepCopy(((vtkPolyData*)((mafVMESurface*)m_Input)->GetOutput()->GetVTKData()));
	m_ResultPolydata->Update();

  double BoundingBox[6];
  m_ResultPolydata->GetBounds(BoundingBox);

  //Compute Max bound
  double dimX, dimY, dimZ;
  dimX = (BoundingBox[1] - BoundingBox[0]);
  dimY = (BoundingBox[3] - BoundingBox[2]);
  dimZ = (BoundingBox[5] - BoundingBox[4]);
  m_MaxBounds = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 
  m_SphereRadius = m_MaxBounds / 100;

	if(!m_TestMode)
	{
		CreateOpDialog();

		int ret_dlg = m_Dialog->ShowModal();
		int result = OP_RUN_CANCEL;
		if( ret_dlg == wxID_OK )
		{
			result = OP_RUN_OK;
		}

		DeleteOpDialog();

		OpStop(result);
	}
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::OpDo()
//----------------------------------------------------------------------------
{
	if(m_ResultPolydata)
		((mafVMESurface*)m_Input)->SetData(m_ResultPolydata,((mafVME*)m_Input)->GetTimeStamp());

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::OpUndo()
//----------------------------------------------------------------------------
{
	if(m_OriginalPolydata)
		((mafVMESurface*)m_Input)->SetData(m_OriginalPolydata,((mafVME*)m_Input)->GetTimeStamp());

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
    case ID_NUM_VERTICES:
      {
        m_ExtrusionFilter->SetMinNumberofEndPoints(m_NumVertices);
        m_ExtrusionFilter->Update();
        m_Rwi->CameraUpdate();
      }
      break;
    case ID_REVERSE:
      {
        double dir[3];
        m_ExtrusionFilter->GetDirection(dir);
        dir[0]=-dir[0];
        dir[1]=-dir[1];
        dir[2]=-dir[2];
        m_ExtrusionFilter->SetDirection(dir);
        m_ExtrusionFilter->Update();
        m_Rwi->CameraUpdate();
      }
      break;
		case VME_PICKED:
			{
				//Get the picked point
				int pointID = e->GetArg();
				if(pointID>=0)
				{
          m_ExtrusionFilter->SetDirectionToDefault(1);
					SelectHole(pointID);
					Extrude();
				}
			}
			break;
		case ID_EXTRUSION_FACTOR:
			{
				Extrude();
			}
			break;
		case ID_EXTRUDE:
			{
				SaveExtrusion();
			}
			break;
		case ID_RADIUS:
			{
				m_Sphere->SetRadius(m_SphereRadius);
				m_Rwi->CameraUpdate();
			}
			break;
		case ID_OK:
			m_Dialog->EndModal(wxID_OK);
			break;
		case ID_CANCEL:
			m_Dialog->EndModal(wxID_CANCEL);
			break;
		default:
			mafEventMacro(*e);
		}
	}
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::SaveExtrusion()
//----------------------------------------------------------------------------
{
  if(m_ResultAfterExtrusion->GetOutput()->GetNumberOfPoints()>0)
  {
	  vtkCleanPolyData  *clean;
	  vtkNEW(clean);
	  clean->SetInput(m_ResultAfterExtrusion->GetOutput());
	  clean->Update();
	  vtkMAFSmartPointer<vtkTriangleFilter> triangle;
	  triangle->SetInput(clean->GetOutput());
	  triangle->Update();
	  m_ResultPolydata->DeepCopy(triangle->GetOutput());
	  m_ResultPolydata->Update();
	  if(!m_TestMode)
	  {
		  m_SurfaceMapper->SetInput(m_ResultPolydata);
		  m_SurfaceMapper->Update();
	  }
	  if(!m_TestMode)
	  {
		  m_Rwi->m_RenFront->RemoveActor(m_SelectedHoleActor);
		  m_Rwi->CameraUpdate();
	  }
	  vtkDEL(clean);
  }
  else
  {
    wxMessageBox(_("An hole must be selected!"));
    m_Rwi->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::Extrude()
//----------------------------------------------------------------------------
{
  if (m_ExtractHole->GetOutput()->GetNumberOfPoints()==0)
  {
  	return;
  }

	vtkMAFSmartPointer<vtkPoints> points;
	if(m_MaxBounds<100)
	{
		for(int i=0;i<m_ExtractHole->GetOutput()->GetNumberOfPoints();i++)
		{
			double point[3];
			m_ExtractHole->GetOutput()->GetPoint(i,point);
			point[0]*=100/m_MaxBounds;
			point[1]*=100/m_MaxBounds;
			point[2]*=100/m_MaxBounds;
			points->InsertNextPoint(point);
		}
	}
  else
  {
    points->DeepCopy(m_ExtractHole->GetOutput()->GetPoints());
  }
	vtkPolyData *appo;
  vtkNEW(appo);
	appo->SetPoints(points);
	appo->Update();
	vtkTextureMapToPlane *computeMedianPlane;
	vtkNEW(computeMedianPlane);
	computeMedianPlane->SetInput(appo);
	computeMedianPlane->AutomaticPlaneGenerationOn();
	computeMedianPlane->Update();
	double normal[3];
	computeMedianPlane->GetNormal(normal);

	/*m_ExtrusionFilter->SetExtrusionTypeToVectorExtrusion();
	m_ExtrusionFilter->SetVector(normal);
	m_ExtrusionFilter->SetScaleFactor(m_ExtrusionFactor);*/

  double lenght=0.0;
  for(int i = 0;i<m_ExtractHole->GetOutput()->GetNumberOfCells();i++)
  {
    vtkPoints *pts=m_ExtractHole->GetOutput()->GetCell(i)->GetPoints();
    for (int iPts=0;iPts<pts->GetNumberOfPoints()-1;iPts++)
    {
      double pt1[3],pt2[3];
      pts->GetPoint(iPts,pt1);
      pts->GetPoint(iPts+1,pt2);
      lenght+=sqrt(vtkMath::Distance2BetweenPoints(pt1,pt2));
    }
  }

  double diameter = lenght/vtkMath::Pi();
  m_ExtrusionFilter->SetLength(m_ExtrusionFactor*diameter);
  //m_ExtrusionFilter->SetDirection(normal) ;
	m_ExtrusionFilter->SetInput(m_ExtractHole->GetOutput());
  m_ExtrusionFilter->GetOutput()->Update() ;
	m_ExtrusionFilter->Update();

	m_ResultAfterExtrusion->RemoveAllInputs();
	m_ResultAfterExtrusion->SetInput(m_ResultPolydata);
	m_ResultAfterExtrusion->AddInput(m_ExtrusionFilter->GetOutput());
	m_ResultAfterExtrusion->Update();

	if(!m_TestMode)
	{
		m_SurfaceMapper->SetInput(m_ResultAfterExtrusion->GetOutput());
		m_SurfaceMapper->Update();

		m_Rwi->CameraUpdate();
	}

	vtkDEL(computeMedianPlane);
  vtkDEL(appo);
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::SelectHole(int pointID)
//----------------------------------------------------------------------------
{
	double	coordPointSelected[3];
	m_ResultPolydata->GetPoint(pointID,coordPointSelected);

	m_ExtractHole->SetInput(m_ExtractFreeEdges->GetOutput());
	m_ExtractHole->SetPoint(coordPointSelected);
	m_ExtractHole->Update();

	if(!m_TestMode)
	{
		if(m_SelectedHoleActor)
			m_Rwi->m_RenFront->RemoveActor(m_SelectedHoleActor);

		vtkGlyph3D *glyph;
		vtkNEW(glyph);
		glyph->SetInput(m_ExtractHole->GetOutput());
		glyph->SetSource(m_Sphere->GetOutput());

		vtkNEW(m_SelectedHoleMapper);
		m_SelectedHoleMapper->SetInput(glyph->GetOutput());

		//Show the selected hole - selected hole is red
		vtkNEW(m_SelectedHoleActor);
		m_SelectedHoleActor->SetMapper(m_SelectedHoleMapper);
		m_SelectedHoleActor->GetProperty()->SetColor(1,0,0);
		m_SelectedHoleActor->PickableOff();

		m_Rwi->m_RenFront->AddActor(m_SelectedHoleActor);
		m_Rwi->m_RenderWindow->Render();
		vtkDEL(glyph);
	}
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::OpStop(int result)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::DeleteOpDialog()
//----------------------------------------------------------------------------
{
	m_Mouse->RemoveObserver(m_Picker);

	if(m_SurfaceActor)
		m_Rwi->m_RenFront->RemoveActor(m_SurfaceActor);

	if(m_HolesActor)
		m_Rwi->m_RenFront->RemoveActor(m_HolesActor);

	if(m_SelectedHoleActor)
		m_Rwi->m_RenFront->RemoveActor(m_SelectedHoleActor);

	vtkDEL(m_SelectedHoleActor);
	vtkDEL(m_SelectedHoleMapper);
	vtkDEL(m_HolesActor);
	vtkDEL(m_HolesMapper);
	vtkDEL(m_Sphere);
	vtkDEL(m_SurfaceActor);
	vtkDEL(m_SurfaceMapper);

	mafDEL(m_Picker);
	cppDEL(m_Rwi); 
	cppDEL(m_Dialog);
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::CreateOpDialog()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	//===== setup interface ====
	m_Dialog = new mafGUIDialog("Extrusion", mafCLOSEWINDOW | mafRESIZABLE);

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

	m_ButtonOk = new mafGUIButton(m_Dialog, ID_OK,_("ok"), p, wxSize(80,20));
	mafGUIButton  *b_cancel	= new mafGUIButton(m_Dialog, ID_CANCEL,_("cancel"), p, wxSize(80,20));


	m_ButtonOk->SetValidator(mafGUIValidator(this,ID_OK,m_ButtonOk));
	//m_ButtonOk->Enable(false);
	b_cancel->SetValidator(mafGUIValidator(this,ID_CANCEL,b_cancel));

	wxBoxSizer *h_sizer3 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer3->Add(m_ButtonOk,0,wxRIGHT);
	h_sizer3->Add(b_cancel,0,wxRIGHT);

	wxStaticText *label1 = new wxStaticText(m_Dialog, -1, _("sphere radius"),p, wxSize(80, 16 ));
	wxTextCtrl *radius		= new wxTextCtrl(m_Dialog,ID_RADIUS, _("sphere radius"),p,wxSize(50, 16 ), wxNO_BORDER );
	
	wxStaticText *label2 = new wxStaticText(m_Dialog, -1, _("extrusion factor (diameters)"),p, wxSize(150, 16 ));
	wxTextCtrl *extrusion = new wxTextCtrl(m_Dialog,ID_EXTRUSION_FACTOR, _("extrusion factor"),p,wxSize(50, 16 ), wxNO_BORDER );
	mafGUIButton  *b_extrude	= new mafGUIButton(m_Dialog, ID_EXTRUDE,_("apply extrusion"), p, wxSize(90,20));

  wxStaticText *label3 = new wxStaticText(m_Dialog, -1, _("num. vertices"),p, wxSize(150, 16 ));
  wxTextCtrl *num_vertices = new wxTextCtrl(m_Dialog,ID_NUM_VERTICES, _("num. vertices"),p,wxSize(50, 16 ), wxNO_BORDER );
  num_vertices->SetValidator(mafGUIValidator(this,ID_NUM_VERTICES,num_vertices,&m_NumVertices,0.0,999.0));

  //wxStaticText *label3 = new wxStaticText(m_Dialog, -1, _("extrusion factor (diameters)"),p, wxSize(150, 16 ));
  wxCheckBox *chk_reverse =   new wxCheckBox(m_Dialog, ID_REVERSE,_("reverse"), p, wxSize(80,20));
  chk_reverse->SetValidator( mafGUIValidator(this, ID_REVERSE, chk_reverse, &m_ReverseExtrusion));

	b_extrude->SetValidator(mafGUIValidator(this,ID_EXTRUDE,b_extrude));
	radius->SetValidator(mafGUIValidator(this,ID_RADIUS,radius,&m_SphereRadius,0.0,999.0));
	extrusion->SetValidator(mafGUIValidator(this,ID_EXTRUSION_FACTOR,extrusion,&m_ExtrusionFactor,0.0,999.0));

	wxBoxSizer *h_sizer1 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer1->Add(label1,0,wxRIGHT);
	h_sizer1->Add(radius,0,wxRIGHT);

	wxBoxSizer *h_sizer2 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer2->Add(label2,0,wxRIGHT);
	h_sizer2->Add(extrusion,0,wxRIGHT);
	h_sizer2->Add(b_extrude,0,wxRIGHT);
  h_sizer2->Add(chk_reverse,0,wxRIGHT);
  h_sizer2->Add(label3,0,wxRIGHT);
  h_sizer2->Add(num_vertices,0,wxRIGHT);

	wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
	v_sizer->Add(m_Rwi->m_RwiBase, 1,wxEXPAND);
	v_sizer->Add(h_sizer1,     0,wxEXPAND | wxALL,5);
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
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::ExtractFreeEdge()
//----------------------------------------------------------------------------
{
	//Extract Holes from the input surface
	m_ExtractFreeEdges->SetInput(m_ResultPolydata);
	m_ExtractFreeEdges->SetBoundaryEdges(1);
	m_ExtractFreeEdges->SetFeatureEdges(0);
	m_ExtractFreeEdges->SetNonManifoldEdges(0);
	m_ExtractFreeEdges->SetManifoldEdges(0);
	m_ExtractFreeEdges->Update();
}
//----------------------------------------------------------------------------
void mafOpExtrusionHoles::CreatePolydataPipeline()
//----------------------------------------------------------------------------
{

  m_ResultPolydata->Update();

	double BoundingBox[6];
	m_ResultPolydata->GetBounds(BoundingBox);
	vtkNEW(m_SurfaceMapper);
	m_SurfaceMapper->SetInput(m_ResultPolydata);
	m_SurfaceMapper->ScalarVisibilityOff();
  m_SurfaceMapper->Update();

	vtkNEW(m_SurfaceActor);
	m_SurfaceActor->SetMapper(m_SurfaceMapper);

	m_Rwi->m_RenFront->AddActor(m_SurfaceActor);

	//Compute Max bound
	double dimX, dimY, dimZ;
	dimX = (BoundingBox[1] - BoundingBox[0]);
	dimY = (BoundingBox[3] - BoundingBox[2]);
	dimZ = (BoundingBox[5] - BoundingBox[4]);
	m_MaxBounds = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 
	m_SphereRadius = m_MaxBounds / 100;

	ExtractFreeEdge();

	vtkNEW(m_Sphere);
	m_Sphere->SetRadius(m_SphereRadius);

	//Create a Glyph to highlight the holes
	vtkGlyph3D *glyph;
	vtkNEW(glyph);
	glyph->SetInput(m_ExtractFreeEdges->GetOutput());
	glyph->SetSource(m_Sphere->GetOutput());

	vtkNEW(m_HolesMapper);
	m_HolesMapper->SetInput(glyph->GetOutput());

	vtkNEW(m_HolesActor);
	m_HolesActor->SetMapper(m_HolesMapper);
	m_HolesActor->GetProperty()->SetColor(0.0,1.0,0.0);
	m_HolesActor->PickableOff();

	m_Rwi->m_RenFront->AddActor(m_HolesActor);

	m_Rwi->m_RenFront->ResetCamera(BoundingBox);
	vtkDEL(glyph);
}
