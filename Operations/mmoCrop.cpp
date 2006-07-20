/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCrop.cpp,v $
  Language:  C++
  Date:      $Date: 2006-07-20 17:38:29 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
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

#include <wx/busyinfo.h>
#include "mafEvent.h"
#include "mafOp.h"
#include "mafGizmoROI.h"
#include "mafVME.h"
#include "mafString.h"
#include "mafVMEVolumeGray.h"

#include "mmgGui.h"
#include "mmoCrop.h"

#include "mafNode.h"

#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkProbeFilter.h"
#include "vtkExtractRectilinearGrid.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

enum {
        ID_SHOW_HANDLES,
        ID_SHOW_ROI,
        ID_FIRST = MINID,	
        ID_CROP_DIR_X,
        ID_CROP_DIR_Y,
        ID_CROP_DIR_Z,
        ID_RESET_CROPPING_AREA,
     };


//----------------------------------------------------------------------------
void mmoCrop::UpdateGui()
//----------------------------------------------------------------------------

{
	double bounds[6];
  m_GizmoROI->GetBounds(bounds);
	this->m_XminXmax[0] = bounds[0];
	this->m_XminXmax[1] = bounds[1];
	this->m_YminYmax[0] = bounds[2];
	this->m_YminYmax[1] = bounds[3];
	this->m_ZminZmax[0] = bounds[4];
	this->m_ZminZmax[1] = bounds[5];
	this->m_Gui->Update();
}

//----------------------------------------------------------------------------
mmoCrop::mmoCrop(wxString label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;
  m_ShowHandles = 1;
  m_ShowROI = 1;


  m_Vme = NULL;
	m_InputRG = NULL;
	m_InputSP = NULL;	
  m_GizmoROI = NULL;

}
//----------------------------------------------------------------------------
mmoCrop::~mmoCrop()
//----------------------------------------------------------------------------
{
	m_InputRG = NULL;
	m_InputSP = NULL;
}

//----------------------------------------------------------------------------
mafOp *mmoCrop::Copy()
//----------------------------------------------------------------------------
{
	return new mmoCrop(m_Label);
}
//----------------------------------------------------------------------------
void mmoCrop::OpRun()   
//----------------------------------------------------------------------------
{
	m_Input->Modified();
  //m_Input->UpdateCurrentData();

  // create gizmo roi
	mafVME* Node=mafVME::SafeDownCast(m_Input);;
  m_GizmoROI = new mafGizmoROI(Node, this);
  m_GizmoROI->Show(true);

	if (Node->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
  {
		this->m_InputSP = vtkStructuredPoints::SafeDownCast(Node->GetOutput()->GetVTKData());
    this->m_InputSP->GetBounds(this->m_InputBounds);							
    double spc[3];	
    this->m_InputSP->GetSpacing(spc);
    this->m_XSpacing = spc[0];
    this->m_YSpacing = spc[1];
    this->m_ZSpacing = spc[2];
  }	
	else if (Node->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
	{
    this->m_InputRG = vtkRectilinearGrid::SafeDownCast(Node->GetOutput()->GetVTKData());		
    this->m_InputRG->GetBounds(this->m_InputBounds);
	}

	this->CreateGui();
		
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));

}
//----------------------------------------------------------------------------
void mmoCrop::OpDo()
//----------------------------------------------------------------------------
{
	m_GizmoROI->GetBounds(this->m_CroppingBoxBounds);

	mafString cropped_vme_name="cropped_";
	cropped_vme_name.Append(this->m_Input->GetName());
	mafNEW(m_Vme);
	m_Vme->SetName(cropped_vme_name);

	m_Input->Modified();
	mafVME *Node=mafVME::SafeDownCast(m_Input);
	if (Node->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))	
	{
    vtkRectilinearGrid *rgData = vtkRectilinearGrid::SafeDownCast(Node->GetOutput()->GetVTKData());

    // get cropping bounds
    double gizmoBounds[6] = {0,0,0,0,0,0};
    m_GizmoROI->GetBounds(gizmoBounds);

    // convert from bounds to index
    int boundsIndexArray[6] = {0,0,0,0,0,0};

    vtkDataArray *coordArray[3] = {NULL, NULL, NULL};
    coordArray[0] = rgData->GetXCoordinates();
    coordArray[1] = rgData->GetYCoordinates();
    coordArray[2] = rgData->GetZCoordinates();

    // for each coordinate array
    for (int numArray = 0; numArray < 3; numArray++)
    {
      int coordId = 0;
      int minId = 0, maxId = 0;      

      while (coordArray[numArray]->GetComponent(coordId,0) < gizmoBounds[2*numArray])
      {
        minId = coordId + 1;
        coordId++;
      }
      
      while (coordArray[numArray]->GetComponent(coordId,0) < gizmoBounds[2*numArray + 1])
      {
        maxId = coordId - 1;
        coordId++;
      }

      boundsIndexArray[2*numArray] = minId;
      boundsIndexArray[2*numArray + 1] = maxId; 
    }

    vtkExtractRectilinearGrid *extractRG = vtkExtractRectilinearGrid::New();
    extractRG->SetInput(vtkRectilinearGrid::SafeDownCast(Node->GetOutput()->GetVTKData()));
    extractRG->SetVOI(boundsIndexArray);   
    extractRG->Update();  
		
		m_Vme->SetDataByDetaching(extractRG->GetOutput(),Node->GetTimeStamp());

    extractRG->Delete();
    extractRG = NULL;
	}
  else if (Node->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
	{
	 	
		int voi_dim[6];
		
		voi_dim[0] = ceil((this->m_CroppingBoxBounds[0] - this->m_InputBounds[0])/(this->m_XSpacing));
		voi_dim[1] = floor((m_CroppingBoxBounds[1] - this->m_InputBounds[0])/ m_XSpacing);
 
		voi_dim[2] = ceil((this->m_CroppingBoxBounds[2] - this->m_InputBounds[2])/(this->m_YSpacing));
		voi_dim[3] = floor((m_CroppingBoxBounds[3] - this->m_InputBounds[2])/ m_YSpacing);
		
		voi_dim[4] = ceil((this->m_CroppingBoxBounds[4] - this->m_InputBounds[4])/(this->m_ZSpacing));
		voi_dim[5] = floor((m_CroppingBoxBounds[5] - this->m_InputBounds[4])/ m_ZSpacing);

	
		// vtkExtractVOI is not working as expected ???
		// output origin is always in (0, 0, 0).
		// As a workaround I'm using the probe filter...
		/*
		vtkStructuredPoints *v_sp = vtkStructuredPoints::New();
				v_sp->DeepCopy((vtkStructuredPoints *)(this->m_input->GetCurrentData()));
				

		vtkExtractVOI *v_evoi = vtkExtractVOI::New();
		v_evoi->SetInput(v_sp);
		v_evoi->SetVOI(voi_dim);
		v_evoi->SetSampleRate(1, 1, 1);
		v_evoi->Update();
		
		
		v_evoi->Delete();
		v_evoi = NULL;
		*////////////////////////////////////////////////
		
		double in_org[3];
		this->m_InputSP->GetOrigin(in_org);

		vtkStructuredPoints *v_esp = vtkStructuredPoints::New();
		v_esp->SetOrigin(in_org[0] + voi_dim[0] * m_XSpacing,
										 in_org[1] + voi_dim[2] * m_YSpacing,
										 in_org[2] + voi_dim[4] * m_ZSpacing);
		v_esp->SetSpacing(m_XSpacing, m_YSpacing, m_ZSpacing);
		v_esp->SetDimensions(voi_dim[1] - voi_dim[0] + 1,
												 voi_dim[3] - voi_dim[2] + 1,
												 voi_dim[5] - voi_dim[4] + 1);

    v_esp->Modified();

		// I'm using probe filter instead of ExtractVOI (see why before...)

    mafString progress_string("please wait, cropping...");
    wxBusyInfo wait(progress_string.GetCStr());

    vtkProbeFilter *probeFilter = vtkProbeFilter::New();
    probeFilter->SetInput(v_esp);
		probeFilter->SetSource(this->m_InputSP);
		probeFilter->Update();

		m_Vme->SetDataByDetaching(probeFilter->GetOutput(),Node->GetTimeStamp());

		probeFilter->Delete();
    v_esp->Delete();
	}

  m_Vme->ReparentTo(m_Input);

	mafEventMacro(mafEvent(this,VME_ADD,m_Vme));
	
	cppDEL(m_GizmoROI);
	m_GizmoROI = NULL;
}
//----------------------------------------------------------------------------
void mmoCrop::OpUndo()
//----------------------------------------------------------------------------
{   
	assert(m_Vme != NULL);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
}

//----------------------------------------------------------------------------
void mmoCrop::CreateGui() 
//----------------------------------------------------------------------------
{
	double bounds[6];
	mafVME *Node=mafVME::SafeDownCast(m_Input);
	Node->GetOutput()->GetVTKData()->GetBounds(bounds);
	this->m_XminXmax[0] = bounds[0];
	this->m_XminXmax[1] = bounds[1];
	this->m_YminYmax[0] = bounds[2];
	this->m_YminYmax[1] = bounds[3];
	this->m_ZminZmax[0] = bounds[4];
	this->m_ZminZmax[1] = bounds[5];

	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);
  m_Gui->Label("");
  m_Gui->Bool(ID_SHOW_HANDLES, "handles", &m_ShowHandles, 0, "toggle gizmo handles visibility");
  m_Gui->Bool(ID_SHOW_ROI, "ROI", &m_ShowROI, 0, "toggle region of interest visibility");     
	m_Gui->Label("");
	m_Gui->VectorN(ID_CROP_DIR_X, "range x", this->m_XminXmax, 2, bounds[0], bounds[1]);
	m_Gui->VectorN(ID_CROP_DIR_Y, "range y", this->m_YminYmax, 2, bounds[2], bounds[3]);
	m_Gui->VectorN(ID_CROP_DIR_Z, "range z", this->m_ZminZmax, 2, bounds[4], bounds[5]);

  m_Gui->Button(ID_RESET_CROPPING_AREA, "reset", "", "reset the cropping area");
	m_Gui->Label("");
	m_Gui->OkCancel();
	ShowGui();
	
}

//----------------------------------------------------------------------------
void mmoCrop::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
			case ID_SHOW_HANDLES:
			{
	      
				bool show = m_ShowHandles ? true : false;      
				m_GizmoROI->ShowHandles(show);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
			case ID_SHOW_ROI:      
			{
	  
				bool show = m_ShowROI ? true : false;      
				m_GizmoROI->ShowROI(show);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
			case ID_CROP_DIR_X:
					double bb[6];
					m_GizmoROI->GetBounds(bb);
					bb[0] = this->m_XminXmax[0];
					bb[1] = this->m_XminXmax[1];
					this->m_Gui->Update();
					m_GizmoROI->SetBounds(bb);
					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			break;
			case ID_CROP_DIR_Y:
					m_GizmoROI->GetBounds(bb);
					bb[2] = this->m_YminYmax[0];
					bb[3] = this->m_YminYmax[1];
					this->m_Gui->Update();
					m_GizmoROI->SetBounds(bb);
					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			break;
			case ID_CROP_DIR_Z:
					m_GizmoROI->GetBounds(bb);
					bb[4] = this->m_ZminZmax[0];
					bb[5] = this->m_ZminZmax[1];
					this->m_Gui->Update();
					m_GizmoROI->SetBounds(bb);
					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			break;

			case ID_RESET_CROPPING_AREA:
					m_GizmoROI->Reset();
					this->UpdateGui();
					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			break;    
			case wxOK:
				this->OpStop(OP_RUN_OK);
				return;   
			break;

			case wxCANCEL:
				this->OpStop(OP_RUN_CANCEL);
				return;
			break;

			case ID_TRANSFORM:
					UpdateGui();
			break;

			default:
				mafEventMacro(*e);
			break;
		}	
	}
 
}

//----------------------------------------------------------------------------
void mmoCrop::OpStop(int result)
//----------------------------------------------------------------------------
{  
  HideGui();
  delete m_Gui;
  m_GizmoROI->Show(false);
  mafEventMacro(mafEvent(this,result));  
}
