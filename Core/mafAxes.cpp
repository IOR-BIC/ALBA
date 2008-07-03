/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAxes.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:30:29 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
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

//----------------------------------------------------------------------------
mafAxes::mafAxes(vtkRenderer *ren, mafVME* vme)
//----------------------------------------------------------------------------
{
  m_Vme = vme;
  m_Renderer = ren;
  assert(m_Renderer);

	m_Axes = vtkAxes::New();
  m_Axes->SetScaleFactor(1);

	if(m_Vme)
  {
		m_Vme->GetOutput()->Update();  
    m_Coord = vtkMAFLocalAxisCoordinate::New();
		((vtkMAFLocalAxisCoordinate*) m_Coord)->SetMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
		((vtkMAFLocalAxisCoordinate*) m_Coord)->SetDataSet(m_Vme->GetOutput()->GetVTKData());

		/*
		vtkMAFLocalAxisCoordinate *coord = vtkMAFLocalAxisCoordinate::New();
		coord->SetMatrix(m_Vme->GetAbsMatrix());
		coord->SetDataSet(m_Vme->GetOutput()->GetVTKData());
		m_Coord = coord;
		*/
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
	m_AxesLUT->Build();

	m_AxesMapper = vtkPolyDataMapper2D::New();
	m_AxesMapper->SetInput(m_Axes->GetOutput());
	m_AxesMapper->SetTransformCoordinate(m_Coord);
  m_AxesMapper->SetLookupTable(m_AxesLUT);
  m_AxesMapper->SetScalarRange(0,0.5);

	m_AxesActor = vtkActor2D::New();
	m_AxesActor->SetMapper(m_AxesMapper); 
	m_AxesActor->GetProperty()->SetLineWidth(2);
	m_AxesActor->VisibilityOff();
  m_AxesActor->PickableOff();
	m_Renderer->AddActor2D(m_AxesActor);
}
//----------------------------------------------------------------------------
mafAxes::~mafAxes()
//----------------------------------------------------------------------------
{
	m_Renderer->RemoveActor2D(m_AxesActor);
  vtkDEL(m_Axes);
  vtkDEL(m_Coord);
  vtkDEL(m_AxesMapper);
  vtkDEL(m_AxesActor);
  vtkDEL(m_AxesLUT);
}
//----------------------------------------------------------------------------
void mafAxes::SetVisibility(bool show)
//----------------------------------------------------------------------------
{
  m_AxesActor->SetVisibility(show);
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
