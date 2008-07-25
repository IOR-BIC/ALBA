/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMeshQuality.cpp,v $
Language:  C++
Date:      $Date: 2008-07-25 11:12:22 $
Version:   $Revision: 1.3 $
Authors:   Matteo Giacomoni - Daniele Giunchi
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

#include "medOpMeshQuality.h"
#include "wx/busyinfo.h"

#include "mafGUIDialog.h"
#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mmiCameraMove.h"
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
#include "vtkCellData.h"
#include "vtkTriangleQualityRatio.h"
#include "vtkLookupTable.h"
#include "vtkCell.h"
#include "vtkScalarBarActor.h"
#include "vtkFeatureEdges.h"
#include "vtkTubeFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpMeshQuality);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpMeshQuality::medOpMeshQuality(const wxString &label) :
mafOp(label)
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
medOpMeshQuality::~medOpMeshQuality()
//----------------------------------------------------------------------------
{
	vtkDEL(m_CheckMeshQuality);
}
//----------------------------------------------------------------------------
bool medOpMeshQuality::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsA("mafVMESurface"));
}
//----------------------------------------------------------------------------
mafOp *medOpMeshQuality::Copy()   
//----------------------------------------------------------------------------
{
	return (new medOpMeshQuality(m_Label));
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
void medOpMeshQuality::OpRun()   
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

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpMeshQuality::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
void medOpMeshQuality::OpStop(int result)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void medOpMeshQuality::DeleteOpDialog()
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

	mafDEL(m_Picker);

	cppDEL(m_Rwi); 
	cppDEL(m_Dialog);
}
//----------------------------------------------------------------------------
void medOpMeshQuality::CreateOpDialog()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	//===== setup interface ====
	m_Dialog = new mafGUIDialog("Check Mesh Quality", mafCLOSEWINDOW | mafRESIZABLE);

	m_Rwi = new mafRWI(m_Dialog,ONE_LAYER,false);
	m_Rwi->SetListener(this);//SIL. 16-6-2004: 
	m_Rwi->CameraSet(CAMERA_PERSPECTIVE);
	m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
	m_Rwi->SetSize(0,0,500,500);
	//m_Rwi->m_RenderWindow->AddRenderer(m_PIPRen);
	m_Rwi->Show(true);
	m_Rwi->m_RwiBase->SetMouse(m_Mouse);
	mafNEW(m_Picker);
	m_Picker->SetListener(this);
	m_Mouse->AddObserver(m_Picker, MCH_INPUT);

	wxPoint p = wxDefaultPosition;

	wxStaticText *labelAspectRatio = new wxStaticText(m_Dialog,   -1, _("Aspect ratio: "));
	m_LabelAverageAspectRatio  = new wxStaticText(m_Dialog,   -1, _(" average : "));
	m_LabelMaxAspectRatio  = new wxStaticText(m_Dialog,   -1, _(" max : "));
	m_LabelMinAspectRatio  = new wxStaticText(m_Dialog,   -1, _(" min : "));

  wxStaticText *labelAngle = new wxStaticText(m_Dialog, -1, _("Angle"),p, wxSize(80, 16 ));
  wxTextCtrl *angleText		= new wxTextCtrl(m_Dialog,ID_RADIUS, _("Angle"),p,wxSize(50, 16 ), wxNO_BORDER );

  angleText->SetValidator(mafGUIValidator(this,ID_RADIUS,angleText,&m_Angle,0.0,999.0));

	mafGUIButton  *b_ok =     new mafGUIButton(m_Dialog, ID_OK,     "ok", p, wxSize(80,20));

	b_ok->SetValidator(mafGUIValidator(this,ID_OK,b_ok));

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
	mafGetFrame()->GetPosition(&x_pos,&y_pos);
	m_Dialog->GetSize(&w,&h);
	m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

	CreatePolydataPipeline();
	this->m_Rwi->CameraUpdate();
}
//----------------------------------------------------------------------------
void medOpMeshQuality::CreatePolydataPipeline()
//----------------------------------------------------------------------------
{
	mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
	vtkPolyData *dataset = vtkPolyData::SafeDownCast(surface->GetSurfaceOutput()->GetVTKData());
	dataset->Update();

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
	m_LabelAverageAspectRatio->SetLabel(wxString::Format(" average : %.3lf",averageRatio));
	m_LabelMaxAspectRatio->SetLabel(wxString::Format(" max : %.3lf",maxRatio));
	m_LabelMinAspectRatio->SetLabel(wxString::Format(" min : %.3lf",minRatio));

	double BoundingBox[6];
	dataset->GetBounds(BoundingBox);
	vtkNEW(m_Mapper);
	m_Mapper->SetInput(((vtkPolyData*)m_CheckMeshQuality->GetOutput()));

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
  m_FeatureEdgeFilter->SetInput(dataset);
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
  m_TubeFilter->SetInput(m_FeatureEdgeFilter->GetOutput());
  m_TubeFilter->SetNumberOfSides(10);
  m_TubeFilter->SetRadius(radius);
  m_TubeFilter->Update();

  vtkNEW(m_MapperFeatureEdge);
  m_MapperFeatureEdge->SetInput(m_TubeFilter->GetOutput());
  m_MapperFeatureEdge->Update();

  vtkNEW(m_ActorFeatureEdge);
  m_ActorFeatureEdge->SetMapper(m_MapperFeatureEdge);

  m_Rwi->m_RenFront->AddActor(m_ActorFeatureEdge);

	m_Rwi->m_RenFront->ResetCamera(BoundingBox);
}
