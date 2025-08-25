/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVOIDensityEditor
 Authors: Matteo Giacomoni & Paolo Quadrani
 
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

#include "albaOpVOIDensityEditor.h"
#include <albaGUIBusyInfo.h>

#include "albaGUI.h"

#include "albaVMEVolumeGray.h"
#include "albaVMEOutputSurface.h"

#include "albaAbsMatrixPipe.h"

#include "vtkALBASmartPointer.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

#include "vtkFeatureEdges.h"
#include "vtkDataArray.h"
#include "vtkPlanes.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkALBAImplicitPolyData.h"
#include "vtkTransformPolyDataFilter.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpVOIDensityEditor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpVOIDensityEditor::albaOpVOIDensityEditor(const wxString &label) 
: albaOp(label)
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
albaOpVOIDensityEditor::~albaOpVOIDensityEditor()
//----------------------------------------------------------------------------
{
	m_Surface = NULL;
  vtkDEL(m_OldData);
}
//----------------------------------------------------------------------------
albaOp* albaOpVOIDensityEditor::Copy()
//----------------------------------------------------------------------------
{
	return (new albaOpVOIDensityEditor(m_Label));
}
//----------------------------------------------------------------------------
bool albaOpVOIDensityEditor::InternalAccept(albaVME* Node)
//----------------------------------------------------------------------------
{
	return (Node != NULL && Node->IsA("albaVMEVolumeGray"));
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
void albaOpVOIDensityEditor::OpRun()   
//----------------------------------------------------------------------------
{
	if(!this->m_TestMode)
	{
		// setup Gui
		m_Gui = new albaGUI(this);
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
void albaOpVOIDensityEditor::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
			case ID_CHOOSE_SURFACE:
			{
				albaString title = _("VOI surface");
        albaEvent event(this,VME_CHOOSE,&title);
				event.SetPointer(&albaOpVOIDensityEditor::OutputSurfaceAccept);
				albaEventMacro(event);
				m_Surface = event.GetVme();
				if(m_Surface == NULL)
					return;
				m_Surface->Update();
				vtkALBASmartPointer<vtkFeatureEdges> FE;
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
					albaMessage(_("Open surface choosed!!"), _("Warning"));
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
				albaEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void albaOpVOIDensityEditor::EditVolumeScalars()
//----------------------------------------------------------------------------
{
  albaGUIBusyInfo wait("Editing VME volume density...",m_Gui);

  double b[6];
  double point[3];
  int numberVoxels, pointId;
  
	vtkAbstractTransform *transform;
	vtkPolyData *polydata;
	m_Surface->GetOutput()->GetBounds(b);
	m_Surface->Update();
	transform=(vtkAbstractTransform*)m_Surface->GetAbsMatrixPipe()->GetVTKTransform();
	polydata=(vtkPolyData *)m_Surface->GetOutput()->GetVTKData();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformDataClipper;
  transformDataClipper->SetTransform(transform);
  transformDataClipper->SetInput(polydata);
  transformDataClipper->Update();

	vtkALBASmartPointer<vtkALBAImplicitPolyData> implicitSurface;
	implicitSurface->SetInput(transformDataClipper->GetOutput());

	vtkALBASmartPointer<vtkPlanes> implicitBox;
  implicitBox->SetBounds(b);
	implicitBox->Modified();

  vtkDataSet *volumeData = m_Input->GetOutput()->GetVTKData();
  volumeData->Update();
  
  if (volumeData->IsA("vtkImageData"))
  {
    m_OldData = vtkImageData::New();
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

  m_Input->GetOutput()->Update();
  
	GetLogicManager()->VmeModified(m_Input);
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpVOIDensityEditor::OpDo()
//----------------------------------------------------------------------------
{
  EditVolumeScalars();
}

//----------------------------------------------------------------------------
void albaOpVOIDensityEditor::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_OldData != NULL)
  {
    int result = ALBA_ERROR;
    if (m_OldData->IsA("vtkImageData"))
    {
      result = ((albaVMEVolumeGray *)m_Input)->SetData((vtkImageData *)m_OldData, m_CurrentTimestamp);
    }
    else if (m_OldData->IsA("vtkRectilinearGrid"))
    {
      result = ((albaVMEVolumeGray *)m_Input)->SetData((vtkRectilinearGrid *)m_OldData, m_CurrentTimestamp);
    }
    if (result != ALBA_OK)
    {
      albaLogMessage("Error assigning the old dataset to %s", m_Input->GetName());
      return;
    }
    ((albaVMEVolumeGray *)m_Input)->Update();
    vtkDEL(m_OldData);
		GetLogicManager()->CameraUpdate();
  }
}
