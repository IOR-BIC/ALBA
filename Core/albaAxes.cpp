/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAxes
 Authors: Silvano Imboden , Stefano perticoni
 
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

#include "albaAxes.h"

#include "albaVME.h"
#include "albaAbsMatrixPipe.h"

#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkViewport.h"
#include "vtkCamera.h"
#include "vtkAxes.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkCoordinate.h"
#include "vtkALBALocalAxisCoordinate.h"
#include "vtkALBAGlobalAxisCoordinate.h"
#include "vtkRenderWindow.h"
#include "vtkALBAOrientationMarkerWidget.h"
#include "vtkALBAAnnotatedCubeActor.h"
#include "vtkALBAGlobalAxesPolydataActor.h"

//----------------------------------------------------------------------------
albaAxes::albaAxes(vtkRenderer *ren, albaVME* vme, int axesType)
{
	m_AxesType = axesType;
	m_OrientationMarkerWidget = NULL;
	m_AnnotatedCubeActor = NULL;
	m_GlobalAxesPolydataActor = NULL;
	m_Coord = NULL;
	m_AxesLUT = NULL;

	m_Vme = vme;
	m_Renderer = ren;
	m_AxesType = axesType;

	m_AxesActor2D = NULL;
	m_AxesMapper2D = NULL;
	m_TriadAxes = NULL;
	
	assert(m_Renderer);

	if (m_AxesType == TRIAD)
	{
		m_TriadAxes = vtkAxes::New();
		m_TriadAxes->SetScaleFactor(1);

		if(m_Vme)
		{
			m_Vme->GetOutput()->Update();  
			m_Coord = vtkALBALocalAxisCoordinate::New();
			((vtkALBALocalAxisCoordinate*) m_Coord)->SetMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
			((vtkALBALocalAxisCoordinate*) m_Coord)->SetDataSet(m_Vme->GetOutput()->GetVTKData());
		}
		else
		{
			m_Coord = vtkALBAGlobalAxisCoordinate::New();
		}	

		m_AxesLUT = vtkLookupTable::New();
		m_AxesLUT->SetNumberOfTableValues(3);
		m_AxesLUT->SetTableValue(0,1,0,0,1);
		m_AxesLUT->SetTableValue(1,0,1,0,1);
		m_AxesLUT->SetTableValue(2,0,0,1,1);

		m_AxesMapper2D = vtkPolyDataMapper2D::New();
		m_AxesMapper2D->SetInputConnection(m_TriadAxes->GetOutputPort());
		m_AxesMapper2D->SetScalarModeToUsePointData();

		m_AxesMapper2D->SetTransformCoordinate(m_Coord);
		m_AxesMapper2D->SetLookupTable(m_AxesLUT);

		m_AxesMapper2D->SetScalarRange(0,0.5);
		m_AxesMapper2D->ScalarVisibilityOn();

		m_AxesActor2D = vtkActor2D::New();
		m_AxesActor2D->SetMapper(m_AxesMapper2D); 
		m_AxesActor2D->GetProperty()->SetLineWidth(2);
		m_AxesActor2D->VisibilityOff();
		m_AxesActor2D->PickableOff();
		m_Renderer->AddActor2D(m_AxesActor2D);
	}
	else if (m_AxesType == CUBE)
	{
		m_OrientationMarkerWidget = vtkALBAOrientationMarkerWidget::New();
		m_AnnotatedCubeActor = vtkALBAAnnotatedCubeActor::New();

		m_AnnotatedCubeActor->SetFaceTextScale(0.5);
		m_OrientationMarkerWidget->SetOrientationMarker(m_AnnotatedCubeActor);
		m_OrientationMarkerWidget->SetInteractor(m_Renderer->GetRenderWindow()->GetInteractor());
		m_OrientationMarkerWidget->SetEnabled(1);
		m_OrientationMarkerWidget->SetInteractive(0);
		m_OrientationMarkerWidget->SetViewport(0., 0., 0.4, 0.4);
	}
	else if (m_AxesType == HEAD)
	{
		m_GlobalAxesPolydataActor = vtkALBAGlobalAxesPolydataActor::New();
		m_GlobalAxesPolydataActor->SetType(vtkALBAGlobalAxesPolydataActor::HEAD);
    m_OrientationMarkerWidget = vtkALBAOrientationMarkerWidget::New();
    if(m_Vme)
      m_GlobalAxesPolydataActor->SetInitialPose(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());

		wxString headABSFileName = m_GlobalAxesPolydataActor->GetABSFileName().c_str();

		bool exists = wxFileExists(headABSFileName.ToAscii());
		assert(exists);

		if (exists)
		{
			// continue
		}
		else
		{
			wxString tmp = "Head VTK file not found: ";
			tmp.Append(headABSFileName.ToAscii());
			tmp.Append(" cannot create 3d head marker");
			wxMessageBox(tmp);
			return;
		}

		m_OrientationMarkerWidget->SetOrientationMarker(m_GlobalAxesPolydataActor);
		m_OrientationMarkerWidget->SetInteractor(m_Renderer->GetRenderWindow()->GetInteractor());
		m_OrientationMarkerWidget->SetEnabled(1);
		m_OrientationMarkerWidget->SetInteractive(0);
		m_OrientationMarkerWidget->SetViewport(0.75, 0., 1, 0.25);
	}
	else if (m_AxesType == BODY)
	{
		m_GlobalAxesPolydataActor = vtkALBAGlobalAxesPolydataActor::New();
		m_GlobalAxesPolydataActor->SetType(vtkALBAGlobalAxesPolydataActor::BODY);
		//m_GlobalAxesPolydataActor = new vtkALBAGlobalAxesPolydataActor(vtkALBAGlobalAxesPolydataActor::BODY);
		m_OrientationMarkerWidget = vtkALBAOrientationMarkerWidget::New();

		if (m_Vme)
			m_GlobalAxesPolydataActor->SetInitialPose(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());

		wxString bodyABSFileName = m_GlobalAxesPolydataActor->GetABSFileName().c_str();

		bool exists = wxFileExists(bodyABSFileName.ToAscii());
		assert(exists);

		if (exists)
		{
			// continue
		}
		else
		{
			wxString tmp = "Body VTK file not found: ";
			tmp.Append(bodyABSFileName.ToAscii());
			tmp.Append(" cannot create 3d body marker");
			wxMessageBox(tmp);
			return;
		}

		m_OrientationMarkerWidget->SetOrientationMarker(m_GlobalAxesPolydataActor);
		m_OrientationMarkerWidget->SetInteractor(m_Renderer->GetRenderWindow()->GetInteractor());
		m_OrientationMarkerWidget->SetEnabled(1);
		m_OrientationMarkerWidget->SetInteractive(0);
		m_OrientationMarkerWidget->SetViewport(0.90, 0., 1, 0.18);
	}
}
//----------------------------------------------------------------------------
albaAxes::~albaAxes()
{
	vtkDEL(m_AnnotatedCubeActor);
	vtkDEL(m_GlobalAxesPolydataActor);

	if (m_OrientationMarkerWidget != NULL) m_OrientationMarkerWidget->SetInteractor(NULL);
	vtkDEL(m_OrientationMarkerWidget);

	m_Renderer->RemoveActor2D(m_AxesActor2D);
	vtkDEL(m_TriadAxes);
	vtkDEL(m_Coord);
	
	vtkDEL(m_AxesMapper2D);
	vtkDEL(m_AxesActor2D);

	vtkDEL(m_AxesLUT);
}
//----------------------------------------------------------------------------
void albaAxes::SetVisibility(bool show)
{
	if(m_AxesType == TRIAD)
		m_AxesActor2D->SetVisibility(show);
	else if (m_AxesType == CUBE)
		m_AnnotatedCubeActor->SetVisibility(show);
}
//----------------------------------------------------------------------------
void albaAxes::SetPose( vtkMatrix4x4 *abs_pose_matrix )
{
	// WARNING - I am assuming that if m_Vme != NULL --> m_Coord ISA vtkALBALocalAxisCoordinate
	if(!m_Vme) return;
	assert(m_Coord);
	vtkALBALocalAxisCoordinate *coord = (vtkALBALocalAxisCoordinate*) m_Coord; 
	if( abs_pose_matrix )
		coord->SetMatrix(abs_pose_matrix);
	else
		coord->SetMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
	coord->Modified();
}