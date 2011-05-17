/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAxes.cpp,v $
Language:  C++
Date:      $Date: 2011-05-17 16:44:11 $
Version:   $Revision: 1.2.2.8 $
Authors:   Silvano Imboden , Stefano perticoni
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

#include "mafAxes.h"

#include "mafVME.h"
#include "mafAbsMatrixPipe.h"

#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkViewport.h"
#include "vtkCamera.h"
#include "vtkAxes.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkCoordinate.h"
#include "vtkMAFLocalAxisCoordinate.h"
#include "vtkMAFGlobalAxisCoordinate.h"
#include "vtkRenderWindow.h"
#include "vtkMAFOrientationMarkerWidget.h"
#include "vtkMAFAnnotatedCubeActor.h"
#include "vtkMAFGlobalAxesHeadActor.h"

//----------------------------------------------------------------------------
mafAxes::mafAxes(vtkRenderer *ren, mafVME* vme, int axesType)
//----------------------------------------------------------------------------
{
	m_AxesType = axesType;
	m_OrientationMarkerWidget = NULL;
	m_AnnotatedCubeActor = NULL;
	m_GlobalAxesHeadActor = NULL;
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
			m_Coord = vtkMAFLocalAxisCoordinate::New();
			((vtkMAFLocalAxisCoordinate*) m_Coord)->SetMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
			((vtkMAFLocalAxisCoordinate*) m_Coord)->SetDataSet(m_Vme->GetOutput()->GetVTKData());
		}
		else
		{
			m_Coord = vtkMAFGlobalAxisCoordinate::New();
		}	

		m_AxesLUT = vtkLookupTable::New();
		m_AxesLUT->SetNumberOfTableValues(3);
		m_AxesLUT->SetTableValue(0,1,0,0,1);
		m_AxesLUT->SetTableValue(1,0,1,0,1);
		m_AxesLUT->SetTableValue(2,0,0,1,1);

		m_AxesMapper2D = vtkPolyDataMapper2D::New();
		m_AxesMapper2D->SetInput(m_TriadAxes->GetOutput());
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
		m_OrientationMarkerWidget = vtkMAFOrientationMarkerWidget::New();
		m_AnnotatedCubeActor = vtkMAFAnnotatedCubeActor::New();

		m_AnnotatedCubeActor->SetFaceTextScale(0.5);
		m_OrientationMarkerWidget->SetOrientationMarker(m_AnnotatedCubeActor);
		m_OrientationMarkerWidget->SetInteractor(m_Renderer->GetRenderWindow()->GetInteractor());
		m_OrientationMarkerWidget->SetEnabled(1);
		m_OrientationMarkerWidget->SetInteractive(0);
		m_OrientationMarkerWidget->SetViewport(0., 0., 0.4, 0.4);
	}
	else if (m_AxesType == HEAD)
	{
		m_OrientationMarkerWidget = vtkMAFOrientationMarkerWidget::New();
		m_GlobalAxesHeadActor = vtkMAFGlobalAxesHeadActor::New();	
		
		wxString headABSFileName = m_GlobalAxesHeadActor->GetHeadABSFileName().c_str();

		wxString cwd = wxGetCwd().c_str();
		std::ostringstream stringStream;
		stringStream << "Head ABS file name: " << headABSFileName.c_str() << std::endl;          
		
		mafLogMessage(stringStream.str().c_str());
		bool exists = wxFileExists(headABSFileName.c_str());
		assert(exists);

		if (exists)
		{
			// continue
		}
		else
		{
			wxString tmp = "Head VTK file not found: ";
			tmp.Append(headABSFileName.c_str());
			tmp.Append(" cannot create 3d head marker");
			wxMessageBox(tmp);
			return;
		}


		m_OrientationMarkerWidget->SetOrientationMarker(m_GlobalAxesHeadActor);
		m_OrientationMarkerWidget->SetInteractor(m_Renderer->GetRenderWindow()->GetInteractor());
		m_OrientationMarkerWidget->SetEnabled(1);
		m_OrientationMarkerWidget->SetInteractive(0);
		m_OrientationMarkerWidget->SetViewport(0., 0., 0.25, 0.25);
	}

}
//----------------------------------------------------------------------------
mafAxes::~mafAxes()
//----------------------------------------------------------------------------
{
	vtkDEL(m_AnnotatedCubeActor);
	vtkDEL(m_GlobalAxesHeadActor);

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
void mafAxes::SetVisibility(bool show)
//----------------------------------------------------------------------------
{
	if(m_AxesType == TRIAD)
		m_AxesActor2D->SetVisibility(show);
	else if (m_AxesType == CUBE)
		m_AnnotatedCubeActor->SetVisibility(show);
}
//----------------------------------------------------------------------------
void mafAxes::SetPose( vtkMatrix4x4 *abs_pose_matrix )
//----------------------------------------------------------------------------
{
	// WARNING - I am assuming that if m_Vme != NULL --> m_Coord ISA vtkMAFLocalAxisCoordinate
	if(!m_Vme) return;
	assert(m_Coord);
	vtkMAFLocalAxisCoordinate *coord = (vtkMAFLocalAxisCoordinate*) m_Coord; 
	if( abs_pose_matrix )
		coord->SetMatrix(abs_pose_matrix);
	else
		coord->SetMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
	coord->Modified();
}


