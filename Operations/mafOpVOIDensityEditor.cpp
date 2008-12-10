/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpVOIDensityEditor.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-10 14:29:36 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Matteo Giacomoni & Paolo Quadrani
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

#include "mafOpVOIDensityEditor.h"
#include <wx/busyinfo.h>

#include "mafGUI.h"

#include "mafVMEVolumeGray.h"
#include "mafVMEOutputSurface.h"

#include "mafAbsMatrixPipe.h"

#include "vtkMAFSmartPointer.h"

#include "vtkDataSet.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"

#include "vtkFeatureEdges.h"
#include "vtkDataArray.h"
#include "vtkPlanes.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkTransformPolyDataFilter.h"

#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpVOIDensityEditor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpVOIDensityEditor::mafOpVOIDensityEditor(const wxString &label) 
: mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;
  m_InputPreserving = false;

  m_ScalarValue = 0;

  m_Surface = NULL;
  m_OldData = NULL;
}
//----------------------------------------------------------------------------
mafOpVOIDensityEditor::~mafOpVOIDensityEditor()
//----------------------------------------------------------------------------
{
	m_Surface = NULL;
  vtkDEL(m_OldData);
}
//----------------------------------------------------------------------------
mafOp* mafOpVOIDensityEditor::Copy()
//----------------------------------------------------------------------------
{
	return (new mafOpVOIDensityEditor(m_Label));
}
//----------------------------------------------------------------------------
bool mafOpVOIDensityEditor::Accept(mafNode* Node)
//----------------------------------------------------------------------------
{
	return (Node != NULL && Node->IsA("mafVMEVolumeGray"));
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum VOI_DENSITY_WIDGET_ID
{
  ID_CHOOSE_SURFACE = MINID,
  ID_SCALAR_VALUE
};
//----------------------------------------------------------------------------
void mafOpVOIDensityEditor::OpRun()   
//----------------------------------------------------------------------------
{
	if(!this->m_TestMode)
	{
		// setup Gui
		m_Gui = new mafGUI(this);
		m_Gui->SetListener(this);

		m_Gui->Divider();
    m_Gui->Double(ID_SCALAR_VALUE, "fill scalar", &m_ScalarValue);
		m_Gui->Button(ID_CHOOSE_SURFACE,_("VOI surface"));
		m_Gui->OkCancel();
		m_Gui->Divider();

    m_Gui->Enable(wxOK, m_Surface != NULL);

		ShowGui();
	}
}
//----------------------------------------------------------------------------
void mafOpVOIDensityEditor::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
			case ID_CHOOSE_SURFACE:
			{
				mafString title = _("VOI surface");
        mafEvent event(this,VME_CHOOSE,&title,(long)&mafOpVOIDensityEditor::OutputSurfaceAccept);
				mafEventMacro(event);
				m_Surface = event.GetVme();
				if(m_Surface == NULL)
					return;
				mafVME *VME = mafVME::SafeDownCast(m_Surface);
        if (VME == NULL)
        {
          mafMessage(_("Not valid surface choosed!!"), _("Warning"));
          m_Surface = NULL;
          return;
        }
				VME->Update();
				vtkMAFSmartPointer<vtkFeatureEdges> FE;
				FE->SetInput((vtkPolyData *)(VME->GetOutput()->GetVTKData()));
				FE->SetFeatureAngle(30);
				FE->SetBoundaryEdges(1);
				FE->SetColoring(0);
				FE->SetFeatureEdges(0);
				FE->SetManifoldEdges(0);
				FE->SetNonManifoldEdges(0);
				FE->Update();

				if(FE->GetOutput()->GetNumberOfCells() != 0)
				{
					//open polydata
					mafMessage(_("Open surface choosed!!"), _("Warning"));
					m_Surface = NULL;
					return;
				}
				
        m_Gui->Enable(wxOK, m_Surface != NULL);
				VME = NULL;
			}
			break;
			case wxOK:
				OpStop(OP_RUN_OK);
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
			break;
      case ID_SCALAR_VALUE:
      break;
			default:
				mafEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void mafOpVOIDensityEditor::EditVolumeScalars()
//----------------------------------------------------------------------------
{
  wxInfoFrame *wait;
  if(!m_TestMode)
  {
    wait = new wxInfoFrame(m_Gui, "Editing VME volume density...");
    wait->SetWindowStyleFlag(wxSTAY_ON_TOP); //to keep wait message on top
    wait->Show(true);
    wait->Refresh();
    wait->Update();
  }

  double b[6];
  double point[3];
  int numberVoxels, pointId;
  
	vtkAbstractTransform *transform;
	vtkPolyData *polydata;
	mafVME *VME = mafVME::SafeDownCast(m_Surface);
	VME->GetOutput()->GetBounds(b);
	VME->Update();
	transform=(vtkAbstractTransform*)VME->GetAbsMatrixPipe()->GetVTKTransform();
	polydata=(vtkPolyData *)VME->GetOutput()->GetVTKData();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformDataClipper;
  transformDataClipper->SetTransform(transform);
  transformDataClipper->SetInput(polydata);
  transformDataClipper->Update();

	vtkMAFSmartPointer<vtkMAFImplicitPolyData> implicitSurface;
	implicitSurface->SetInput(transformDataClipper->GetOutput());

	vtkMAFSmartPointer<vtkPlanes> implicitBox;
  implicitBox->SetBounds(b);
	implicitBox->Modified();

  vtkDataSet *volumeData = ((mafVME*)m_Input)->GetOutput()->GetVTKData();
  volumeData->Update();
  
  if (volumeData->IsA("vtkStructuredPoints"))
  {
    m_OldData = vtkStructuredPoints::New();
  }
  else if (volumeData->IsA("vtkRectilinearGrid"))
  {
    m_OldData = vtkRectilinearGrid::New();
  }
  m_OldData->DeepCopy(volumeData);
  m_CurrentTimestamp = ((mafVME *)m_Input)->GetTimeStamp();
  
	numberVoxels = volumeData->GetNumberOfPoints();
  
	for (int voxel = 0; voxel < numberVoxels; voxel++)
  {
    volumeData->GetPoint(voxel, point);
    if(implicitBox->EvaluateFunction(point) < 0)
    {
      //point is inside the bounding box of the surface: check
      //if the point is also inside the surface.
      if (implicitSurface->EvaluateFunction(point) < 0)
      {
        //edit the corresponding point's scalar value
        pointId = volumeData->FindPoint(point);
        volumeData->GetPointData()->SetTuple(pointId, &m_ScalarValue);
      }
    }
  }

  volumeData->GetPointData()->GetScalars()->Modified();
  volumeData->Update();

  if (!m_TestMode)
  {
    cppDEL(wait);
  }

  ((mafVME *)m_Input)->GetOutput()->Update();
  mafEventMacro(mafEvent(this, VME_MODIFIED, m_Input));
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mafOpVOIDensityEditor::OpDo()
//----------------------------------------------------------------------------
{
  EditVolumeScalars();
}

//----------------------------------------------------------------------------
void mafOpVOIDensityEditor::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_OldData != NULL)
  {
    int result = MAF_ERROR;
    if (m_OldData->IsA("vtkStructuredPoints"))
    {
      result = ((mafVMEVolumeGray *)m_Input)->SetData((vtkStructuredPoints *)m_OldData, m_CurrentTimestamp);
    }
    else if (m_OldData->IsA("vtkRectilinearGrid"))
    {
      result = ((mafVMEVolumeGray *)m_Input)->SetData((vtkRectilinearGrid *)m_OldData, m_CurrentTimestamp);
    }
    if (result != MAF_OK)
    {
      mafLogMessage("Error assigning the old dataset to %s", m_Input->GetName());
      return;
    }
    ((mafVMEVolumeGray *)m_Input)->Update();
    vtkDEL(m_OldData);
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  }
}
