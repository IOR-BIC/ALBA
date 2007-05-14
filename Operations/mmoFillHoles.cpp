/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoFillHoles.cpp,v $
Language:  C++
Date:      $Date: 2007-05-14 09:50:34 $
Version:   $Revision: 1.1 $
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

#include "mmoFillHoles.h"
#include "wx/busyinfo.h"

#include "mmgDialog.h"
#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mmiSelectPoint.h"
#include "mmdMouse.h"
#include "mmgButton.h"
#include "mmgValidator.h"
#include "mafVMESurface.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"
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

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoFillHoles);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoFillHoles::mmoFillHoles(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= false;

	m_ActorSurface = NULL;
	m_HoleConnectivity = NULL;
	m_FreeEdge = NULL;

	m_Picker = NULL;

	m_MaxBound = -1.0;
}
//----------------------------------------------------------------------------
mmoFillHoles::~mmoFillHoles()
//----------------------------------------------------------------------------
{
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
};
//----------------------------------------------------------------------------
void mmoFillHoles::OpRun()   
//----------------------------------------------------------------------------
{  
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
}
//----------------------------------------------------------------------------
void mmoFillHoles::OpUndo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoFillHoles::OnEvent(mafEventBase *maf_event)
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
				double coord[3];
				m_AppendPolydata->GetOutput()->GetPoint(pointID,coord);

				vtkNEW(m_HoleConnectivity);
				m_HoleConnectivity->SetInput(m_FreeEdge->GetOutput());
				m_HoleConnectivity->Setpoint(coord);
				m_HoleConnectivity->Modified();
				m_HoleConnectivity->Update(); 

				m_Rwi->m_RenFront->RemoveActor(m_ActorHoles);

				vtkSphereSource *sphere;
				vtkNEW(sphere);
				sphere->SetRadius(m_MaxBound/80);
				sphere->Modified();

				vtkGlyph3D *glyph;
				vtkNEW(glyph);
				glyph->SetInput(m_HoleConnectivity->GetOutput());
				glyph->SetSource(sphere->GetOutput());
				glyph->Modified();
				glyph->Update();

				vtkNEW(m_MapperHoles);
				m_MapperHoles->SetInput(glyph->GetOutput());
				m_MapperHoles->Modified();
				m_MapperHoles->Update();

				//Show the selected hole - selected hole is red
				vtkNEW(m_ActorHoles);
				m_ActorHoles->SetMapper(m_MapperHoles);
				m_ActorHoles->GetProperty()->SetColor(1,0,0);
				m_ActorHoles->Modified();

				m_Rwi->m_RenFront->AddActor(m_ActorHoles);

				m_Rwi->m_RenderWindow->Render();

				vtkDEL(glyph);
				vtkDEL(sphere);
			}
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
	
	if(m_ActorHoles)
		m_Rwi->m_RenFront->RemoveActor(m_ActorHoles);

	vtkDEL(m_HoleConnectivity);
	vtkDEL(m_ActorHoles);
	vtkDEL(m_MapperHoles);
	vtkDEL(m_ActorSurface);
	vtkDEL(m_MapperSurface);
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
	m_Dialog = new mmgDialog("Fill Holes", mafCLOSEWINDOW | mafRESIZABLE);

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

	mmgButton  *b_ok =     new mmgButton(m_Dialog, ID_OK,     "ok", p, wxSize(80,20));

	b_ok->SetValidator(mmgValidator(this,ID_OK,b_ok));

	wxBoxSizer *h_sizer3 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer3->Add(b_ok,      0,wxRIGHT);

	wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
	v_sizer->Add(m_Rwi->m_RwiBase, 1,wxEXPAND);
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
void mmoFillHoles::CreatePolydataPipeline()
//----------------------------------------------------------------------------
{
	mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
	vtkPolyData *dataset = vtkPolyData::SafeDownCast(surface->GetSurfaceOutput()->GetVTKData());
	dataset->Update();

	double bounds[6];
	dataset->GetBounds(bounds);

	double dimX, dimY, dimZ;
	dimX = (bounds[1] - bounds[0]);
	dimY = (bounds[3] - bounds[2]);
	dimZ = (bounds[5] - bounds[4]);

	//Compute Max bound
	m_MaxBound = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 
	
	//Exctract Holes from the input surface
	vtkNEW(m_FreeEdge);
	m_FreeEdge->SetInput(dataset);
	m_FreeEdge->SetBoundaryEdges(1);
	m_FreeEdge->SetFeatureEdges(0);
	m_FreeEdge->SetNonManifoldEdges(0);
	m_FreeEdge->SetManifoldEdges(0);
	m_FreeEdge->Update();
	
	vtkSphereSource *sphere;
	vtkNEW(sphere);
	sphere->SetRadius(m_MaxBound/80);
	sphere->Modified();

	//Create a Glyph to highlight the holes
	vtkGlyph3D *glyph;
	vtkNEW(glyph);
	glyph->SetInput(m_FreeEdge->GetOutput());
	glyph->SetSource(sphere->GetOutput());
	glyph->Modified();
	glyph->Update();

	//Create a append polydata in way to have only an actor in the scene
	vtkNEW(m_AppendPolydata);
	m_AppendPolydata->AddInput(glyph->GetOutput());
	m_AppendPolydata->AddInput(dataset);
	m_AppendPolydata->Modified();
	m_AppendPolydata->Update();

	m_AppendPolydata->GetOutput()->GetBounds(bounds);
	m_Rwi->m_RenFront->ResetCamera(bounds);

	vtkNEW(m_MapperSurface);
	m_MapperSurface->SetInput(m_AppendPolydata->GetOutput());

	vtkNEW(m_ActorSurface);
	m_ActorSurface->SetMapper(m_MapperSurface);

	m_Rwi->m_RenFront->AddActor(m_ActorSurface);

	m_Rwi->CameraUpdate();

	vtkDEL(glyph);
	vtkDEL(sphere);

}