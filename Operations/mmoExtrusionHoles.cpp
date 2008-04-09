/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoExtrusionHoles.cpp,v $
Language:  C++
Date:      $Date: 2008-04-09 12:51:21 $
Version:   $Revision: 1.8 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/
#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoExtrusionHoles.h"
#include "mafNode.h"

#include "mmgDialog.h"
#include "mmgGui.h"
#include "mmgButton.h"
#include "mmdMouse.h"
#include "mmgValidator.h"
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
#include "vtkLinearExtrusionFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkMEDExtrudeToCircle.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoExtrusionHoles);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoExtrusionHoles::mmoExtrusionHoles(const wxString &label) :
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

	m_ExtrusionFactor		= 0.0;

	m_MaxBounds = VTK_DOUBLE_MAX;
}
//----------------------------------------------------------------------------
mmoExtrusionHoles::~mmoExtrusionHoles()
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
bool mmoExtrusionHoles::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("mafVMESurface"));
}
//----------------------------------------------------------------------------
mafOp *mmoExtrusionHoles::Copy()   
//----------------------------------------------------------------------------
{
	return (new mmoExtrusionHoles(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_OK = MINID,
	ID_CANCEL,
	ID_EXTRUSION_FACTOR,
	ID_EXTRUDE,
	ID_RADIUS,
};
//----------------------------------------------------------------------------
void mmoExtrusionHoles::OpRun()   
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
void mmoExtrusionHoles::OpDo()
//----------------------------------------------------------------------------
{
	if(m_ResultPolydata)
		((mafVMESurface*)m_Input)->SetData(m_ResultPolydata,((mafVME*)m_Input)->GetTimeStamp());

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoExtrusionHoles::OpUndo()
//----------------------------------------------------------------------------
{
	if(m_OriginalPolydata)
		((mafVMESurface*)m_Input)->SetData(m_OriginalPolydata,((mafVME*)m_Input)->GetTimeStamp());

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoExtrusionHoles::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case VME_PICKED:
			{
				//Get the picked point
				int pointID = e->GetArg();
				if(pointID>=0)
				{
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
void mmoExtrusionHoles::SaveExtrusion()
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void mmoExtrusionHoles::Extrude()
//----------------------------------------------------------------------------
{
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
	vtkPolyData *appo=vtkPolyData::New();
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
  m_ExtrusionFilter->SetLength(m_ExtrusionFactor);
  m_ExtrusionFilter->SetDirection(normal) ;
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
}
//----------------------------------------------------------------------------
void mmoExtrusionHoles::SelectHole(int pointID)
//----------------------------------------------------------------------------
{
	double	coordPointSelected[3];
	m_ResultPolydata->GetPoint(pointID,coordPointSelected);

	m_ExtractHole->SetInput(m_ExtractFreeEdges->GetOutput());
	m_ExtractHole->Setpoint(coordPointSelected);
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
void mmoExtrusionHoles::OpStop(int result)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mmoExtrusionHoles::DeleteOpDialog()
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
void mmoExtrusionHoles::CreateOpDialog()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	//===== setup interface ====
	m_Dialog = new mmgDialog("Extrusion", mafCLOSEWINDOW | mafRESIZABLE);

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

	m_ButtonOk = new mmgButton(m_Dialog, ID_OK,_("ok"), p, wxSize(80,20));
	mmgButton  *b_cancel	= new mmgButton(m_Dialog, ID_CANCEL,_("cancel"), p, wxSize(80,20));


	m_ButtonOk->SetValidator(mmgValidator(this,ID_OK,m_ButtonOk));
	//m_ButtonOk->Enable(false);
	b_cancel->SetValidator(mmgValidator(this,ID_CANCEL,b_cancel));

	wxBoxSizer *h_sizer3 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer3->Add(m_ButtonOk,0,wxRIGHT);
	h_sizer3->Add(b_cancel,0,wxRIGHT);

	wxStaticText *label1 = new wxStaticText(m_Dialog, -1, _("sphere radius"),p, wxSize(80, 16 ));
	wxTextCtrl *radius		= new wxTextCtrl(m_Dialog,ID_RADIUS, _("sphere radius"),p,wxSize(50, 16 ), wxNO_BORDER );
	
	wxStaticText *label2 = new wxStaticText(m_Dialog, -1, _("extrusion factor"),p, wxSize(80, 16 ));
	wxTextCtrl *extrusion = new wxTextCtrl(m_Dialog,ID_EXTRUSION_FACTOR, _("extrusion factor"),p,wxSize(50, 16 ), wxNO_BORDER );
	mmgButton  *b_extrude	= new mmgButton(m_Dialog, ID_EXTRUDE,_("apply extrusion"), p, wxSize(90,20));

	b_extrude->SetValidator(mmgValidator(this,ID_EXTRUDE,b_extrude));
	radius->SetValidator(mmgValidator(this,ID_RADIUS,radius,&m_SphereRadius,0.0,999.0));
	extrusion->SetValidator(mmgValidator(this,ID_EXTRUSION_FACTOR,extrusion,&m_ExtrusionFactor,-999.0,999.0));

	wxBoxSizer *h_sizer1 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer1->Add(label1,0,wxRIGHT);
	h_sizer1->Add(radius,0,wxRIGHT);

	wxBoxSizer *h_sizer2 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer2->Add(label2,0,wxRIGHT);
	h_sizer2->Add(extrusion,0,wxRIGHT);
	h_sizer2->Add(b_extrude,0,wxRIGHT);

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
void mmoExtrusionHoles::ExtractFreeEdge()
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
void mmoExtrusionHoles::CreatePolydataPipeline()
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
