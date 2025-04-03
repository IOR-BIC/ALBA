/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeProjected
 Authors: Paolo Quadrani - Matteo Giacomoni
 
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

#include "albaPipeVolumeProjected.h"
#include "albaDecl.h"
#include "albaSceneNode.h"

#include "albaVME.h"
#include "albaVMEVolumeGray.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"

#include "vtkActor.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPlaneSource.h"
#include "vtkTexture.h"
#include "vtkProbeFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkALBAProjectVolume.h"
#include "vtkProperty.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkOutlineCornerFilter.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVolumeProjected);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeVolumeProjected::albaPipeVolumeProjected()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_CamPosition = CAMERA_RX_FRONT;

	m_ProjectFilter = NULL;
	m_ProjectionRange[0] = m_ProjectionRange[1] = 0;
	m_RangeProjectionEnabled = false;
	m_VolumeBoxActor = NULL;
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjected::InitializeProjectParameters(int cam_position)
//----------------------------------------------------------------------------
{
  if(cam_position == CAMERA_RX_LEFT || cam_position == CAMERA_RX_RIGHT ) 
    m_CamPosition = cam_position;
  else
    m_CamPosition = CAMERA_RX_FRONT;
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjected::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars
  m_UsedAssembly = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

	assert(m_Vme->GetOutput()->IsA("albaVMEOutputVolume"));

	m_Vme->GetOutput()->Update();

	m_Lut = vtkWindowLevelLookupTable::New();

  //--volume pipeline-------------------------------
	vtkPlaneSource     *RXPlane       = NULL;
	vtkPolyDataMapper  *RXPlaneMapper	= NULL;
	vtkTexture         *RXTexture			= NULL;
		
  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();

  double range[2]; // used with lut
	double bounds[6];
  vtk_data->GetBounds(bounds);
	
	double xmin, xmax, ymin, ymax, zmin, zmax;
	xmin = bounds[0];
	xmax = bounds[1];
	ymin = bounds[2];
	ymax = bounds[3];
	zmin = bounds[4];
	zmax = bounds[5];

	//---- pipeline for the Plane --------------------------
	RXPlane = vtkPlaneSource::New();
	switch (m_CamPosition)
	{	
	  case CAMERA_RX_FRONT:
		  RXPlane->SetOrigin(xmin, ymin, zmin);
		  RXPlane->SetPoint1(xmax, ymin, zmin);
		  RXPlane->SetPoint2(xmin, ymin, zmax);
	  break;
	  case CAMERA_RX_LEFT:
		  RXPlane->SetOrigin(xmax, ymin, zmin);
		  RXPlane->SetPoint2(xmax, ymin, zmax);
		  RXPlane->SetPoint1(xmax, ymax, zmin);
	  break;
	  case CAMERA_RX_RIGHT:
		  RXPlane->SetOrigin(xmin, ymin, zmin);
		  RXPlane->SetPoint2(xmin, ymin, zmax);
		  RXPlane->SetPoint1(xmin, ymax, zmin);
	  break;
	}

	RXPlaneMapper = vtkPolyDataMapper::New();
	RXPlaneMapper->SetInputConnection(RXPlane->GetOutputPort());

	RXTexture = vtkTexture::New();
	RXTexture->SetInterpolate(1);

	m_ProjectFilter = vtkALBAProjectVolume::New();

	if (vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(vtk_data))
	{
		int dims[3];
		rg->GetDimensions(dims);
		dims[0]=0;
	}

	m_ProjectFilter->SetInputData(vtk_data); 
	if (m_CamPosition == CAMERA_RX_FRONT )
			m_ProjectFilter->SetProjectionSideToY();
	else
			m_ProjectFilter->SetProjectionSideToX();
	m_ProjectFilter->SetProjectSubRange(m_RangeProjectionEnabled);
	m_ProjectFilter->SetProjectionRange(m_ProjectionRange);
	m_ProjectFilter->Update();
	
	m_ProjectFilter->GetOutput()->GetScalarRange(range);
	RXTexture->SetInputConnection(m_ProjectFilter->GetOutputPort());

	m_Lut->SetTableRange(range[0], range[1]);
	m_Lut->SetWindow(range[1] - range[0]);
	m_Lut->SetLevel((range[1] + range[0])/2.0);
	m_Lut->Build();

	RXTexture->SetLookupTable(m_Lut);

  m_RXActor = vtkActor::New();
	m_RXActor->SetMapper(RXPlaneMapper);
	m_RXActor->SetTexture(RXTexture);
  
	//---- TICKs creation --------------------------
	vtkPolyData  *CTLinesPD      = vtkPolyData::New();	
	vtkPoints    *CTLinesPoints  = vtkPoints::New();	
	vtkCellArray *CTCells        = vtkCellArray::New();
	vtkIdType points_id[2];
	int	counter = 0;
  vtkRectilinearGrid *rg_data = vtkRectilinearGrid::SafeDownCast(vtk_data);
	if (rg_data)
	{
    vtkDoubleArray* z_fa = vtkDoubleArray::SafeDownCast(rg_data->GetZCoordinates());
		if(z_fa)
		{
			for (int i = 0; i < z_fa->GetNumberOfTuples(); i++)
			{
				CTLinesPoints->InsertNextPoint(xmax, ymax, z_fa->GetValue(i));
				CTLinesPoints->InsertNextPoint(xmax+(xmax-xmin)/30, ymax+(ymax-ymin)/30 ,z_fa->GetValue(i));
				points_id[0] = counter;
				points_id[1] = counter+1;
				counter+=2;
				CTCells->InsertNextCell(2 , points_id);
			}
		}
		else
		{
			vtkFloatArray* z_fa_f = vtkFloatArray::SafeDownCast(rg_data->GetZCoordinates());
			for (int i = 0; i < z_fa_f->GetNumberOfTuples(); i++)
			{
				CTLinesPoints->InsertNextPoint(xmax, ymax, z_fa_f->GetValue(i));
				CTLinesPoints->InsertNextPoint(xmax+(xmax-xmin)/30, ymax+(ymax-ymin)/30 ,z_fa_f->GetValue(i));
				points_id[0] = counter;
				points_id[1] = counter+1;
				counter+=2;
				CTCells->InsertNextCell(2 , points_id);
			}
		}
	}
  vtkImageData *sp_data = vtkImageData::SafeDownCast(vtk_data);
	if (sp_data)
	{
		int dim[3];
		double origin[3];
		double spacing[3];
		sp_data->GetDimensions(dim);
		sp_data->GetOrigin(origin);
		sp_data->GetSpacing(spacing);

		for (int i=0; i < dim[2]; i++)
		{
			float z_i = origin[2] + i*spacing[2];	//?
			CTLinesPoints->InsertNextPoint(xmax, ymax, z_i);
			CTLinesPoints->InsertNextPoint(xmax+(xmax-xmin)/30,ymax+(ymax-ymin)/30,z_i);
			
			points_id[0] = counter;
			points_id[1] = counter+1;
			counter+=2;
			CTCells->InsertNextCell(2 , points_id);
		}	
	}
	CTLinesPD->SetPoints(CTLinesPoints);
	CTLinesPD->SetLines(CTCells); 
	CTLinesPD->Modified();	  

	//Add tick to scene
  vtkPolyDataMapper *TickMapper = vtkPolyDataMapper::New();
  TickMapper->SetInputData(CTLinesPD);

	vtkProperty	*TickProperty = vtkProperty::New();
	TickProperty->SetColor(1,0,0);
	TickProperty->SetAmbient(1);
	TickProperty->SetRepresentationToWireframe();
	TickProperty->SetInterpolationToFlat();

	m_TickActor = vtkActor::New();
	m_TickActor->SetMapper(TickMapper);
	m_TickActor->VisibilityOn();
	m_TickActor->PickableOff();
	m_TickActor->SetProperty(TickProperty);

  //-----------------------------------------------
	m_UsedAssembly->AddPart(m_TickActor);
	m_UsedAssembly->AddPart(m_RXActor);	

  // selection pipeline ////////////////////////////////
	vtkALBASmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInputData(m_Vme->GetOutput()->GetVTKData());

	vtkALBASmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInputConnection(corner->GetOutputPort());

	vtkNEW(m_VolumeBoxActor);
	m_VolumeBoxActor->SetMapper(corner_mapper);
  m_VolumeBoxActor->PickableOff();
	m_VolumeBoxActor->SetVisibility(m_Selected);

	m_UsedAssembly->AddPart(m_VolumeBoxActor);

	//create something invisible in the front renderer so that ResetCamera will work
  m_Ghost = NULL;
  if(m_UsedAssembly == m_AssemblyBack)
	{
		m_Ghost = vtkActor::New();
		m_Ghost->SetMapper(RXPlaneMapper);
		m_Ghost->PickableOff();
    m_Ghost->GetProperty()->SetOpacity(0.1);
    m_Ghost->GetProperty()->SetRepresentationToPoints();
    m_Ghost->GetProperty()->SetInterpolationToFlat();
		m_AssemblyFront->AddPart(m_Ghost);
  }
  vtkDEL(CTLinesPoints);
  vtkDEL(CTCells);
  vtkDEL(CTLinesPD);
	
	vtkDEL(RXPlane);
	vtkDEL(RXPlaneMapper);
	vtkDEL(RXTexture);
	vtkDEL(TickMapper);
	vtkDEL(TickProperty);
}


//----------------------------------------------------------------------------
albaPipeVolumeProjected::~albaPipeVolumeProjected()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ProjectFilter);

	if(m_VolumeBoxActor)
    m_UsedAssembly->RemovePart(m_VolumeBoxActor);
	m_UsedAssembly->RemovePart(m_TickActor);
	m_UsedAssembly->RemovePart(m_RXActor);
  if(m_Ghost) 
    m_AssemblyFront->RemovePart(m_Ghost);

  vtkDEL(m_Lut);
	vtkDEL(m_TickActor);
	vtkDEL(m_RXActor);
  vtkDEL(m_Ghost);

	vtkDEL(m_VolumeBoxActor);
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjected::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	m_VolumeBoxActor->SetVisibility(sel);
  //there's no selection highlight for volume in rx-view
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjected::SetLutRange(double low, double hi)
//----------------------------------------------------------------------------
{
	if(!m_Lut) 
    return;
	m_Lut->SetWindow(hi - low); 
	m_Lut->SetLevel((low + hi)/2);
	m_Lut->Build();
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjected::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
	m_Lut->GetTableRange(range);
}

//----------------------------------------------------------------------------
void albaPipeVolumeProjected::EnableRangeProjection(bool enabled)
{
	if (m_RangeProjectionEnabled == enabled)
		return;

	m_RangeProjectionEnabled = enabled;
	if(m_ProjectFilter)
		m_ProjectFilter->SetProjectSubRange(enabled);

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipeVolumeProjected::SetProjectionRange(int range[2])
{
	m_ProjectionRange[0] = range[0];
	m_ProjectionRange[1] = range[1];

	if(m_ProjectFilter)
		m_ProjectFilter->SetProjectionRange(range);

	if(m_RangeProjectionEnabled)
		GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjected::GetProjectionRange(int *range)
{
	if (m_ProjectFilter)
		m_ProjectFilter->SetProjectSubRange(true);

	if (m_ProjectFilter)
		m_ProjectFilter->GetProjectionRange(range);
}

//----------------------------------------------------------------------------
void albaPipeVolumeProjected::SetProjectionModality(int modality)
{
	if (m_ProjectFilter)
	{
		if (modality == 0)
			m_ProjectFilter->SetProjectionModalityToMean();
		else
			m_ProjectFilter->SetProjectionModalityToMax();
	}
}

//----------------------------------------------------------------------------
void albaPipeVolumeProjected::TickActorVisibilityOn()
//----------------------------------------------------------------------------
{
  m_TickActor->VisibilityOn();
  m_TickActor->Modified();
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjected::TickActorVisibilityOff()
//----------------------------------------------------------------------------
{
  m_TickActor->VisibilityOff();
  m_TickActor->Modified();
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjected::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
  m_RXActor->SetPickable(enable);
  m_RXActor->Modified();
	
	GetLogicManager()->CameraUpdate();
}
