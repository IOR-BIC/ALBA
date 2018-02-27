/*=========================================================================

 Program: MAF2
 Module: mafOpVOIDensityEditor
 Authors: Matteo Giacomoni & Paolo Quadrani
 
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
bool mafOpVOIDensityEditor::Accept(mafVME* Node)
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
  ID_SCALAR_VALUE,
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
        mafEvent event(this,VME_CHOOSE,&title);
				event.SetPointer(&mafOpVOIDensityEditor::OutputSurfaceAccept);
				mafEventMacro(event);
				m_Surface = event.GetVme();
				if(m_Surface == NULL)
					return;
				m_Surface->Update();
				vtkMAFSmartPointer<vtkFeatureEdges> FE;
				FE->SetInput((vtkPolyData *)(m_Surface->GetOutput()->GetVTKData()));
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
	m_Surface->GetOutput()->GetBounds(b);
	m_Surface->Update();
	transform=(vtkAbstractTransform*)m_Surface->GetAbsMatrixPipe()->GetVTKTransform();
	polydata=(vtkPolyData *)m_Surface->GetOutput()->GetVTKData();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformDataClipper;
  transformDataClipper->SetTransform(transform);
  transformDataClipper->SetInput(polydata);
  transformDataClipper->Update();

	vtkMAFSmartPointer<vtkMAFImplicitPolyData> implicitSurface;
	implicitSurface->SetInput(transformDataClipper->GetOutput());

	vtkMAFSmartPointer<vtkPlanes> implicitBox;
  implicitBox->SetBounds(b);
	implicitBox->Modified();

  vtkDataSet *volumeData = m_Input->GetOutput()->GetVTKData();
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
  m_CurrentTimestamp = m_Input->GetTimeStamp();
  
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

  m_Input->GetOutput()->Update();
  
	GetLogicManager()->VmeModified(m_Input);
	GetLogicManager()->CameraUpdate();
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
		GetLogicManager()->CameraUpdate();
  }
}
