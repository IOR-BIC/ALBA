/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeProjected.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-01 13:59:03 $
  Version:   $Revision: 1.15 $
  Authors:   Paolo Quadrani - Matteo Giacomoni
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

#include "mafPipeVolumeProjected.h"
#include "mafDecl.h"
#include "mafSceneNode.h"

#include "mafVME.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"

#include "vtkActor.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkPlaneSource.h"
#include "vtkTexture.h"
#include "vtkProbeFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkProjectSP.h"
#include "vtkProjectRG.h"
#include "vtkProperty.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkOutlineCornerFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVolumeProjected);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeVolumeProjected::mafPipeVolumeProjected()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_CamPosition = CAMERA_RX_FRONT;

	m_VolumeBoxActor = NULL;
}
//----------------------------------------------------------------------------
void mafPipeVolumeProjected::InitializeProjectParameters(int cam_position)
//----------------------------------------------------------------------------
{
  if(cam_position == CAMERA_RX_LEFT || cam_position == CAMERA_RX_RIGHT ) 
    m_CamPosition = cam_position;
  else
    m_CamPosition = CAMERA_RX_FRONT;
}
//----------------------------------------------------------------------------
void mafPipeVolumeProjected::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars
  m_UsedAssembly = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

	assert(m_Vme->GetOutput()->IsA("mafVMEOutputVolume"));

	m_Vme->GetOutput()->Update();

	m_Lut = vtkWindowLevelLookupTable::New();

  //--volume pipeline-------------------------------
	vtkPlaneSource     *RXPlane       = NULL;
	vtkPolyDataMapper  *RXPlaneMapper	= NULL;
	vtkTexture         *RXTexture			= NULL;
	vtkProjectSP       *SPProjection	= NULL;
	vtkProjectRG       *RGProjection	= NULL;
	
  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  vtk_data->Update();

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
	RXPlaneMapper->SetInput(RXPlane->GetOutput());

	RXTexture = vtkTexture::New();
	RXTexture->SetInterpolate(1);
	RXTexture->SetMapColorScalarsThroughLookupTable(1);

	if (vtk_data->IsA("vtkStructuredPoints"))
	{
		SPProjection = vtkProjectSP::New();
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,SPProjection));
		SPProjection->SetInput(((vtkStructuredPoints *)vtk_data));
		if (m_CamPosition == CAMERA_RX_FRONT )
			SPProjection->SetProjectionModeToY();
		else
			SPProjection->SetProjectionModeToX();
		SPProjection->Update();
		SPProjection->GetOutput()->GetScalarRange(range);
		RXTexture->SetInput(SPProjection->GetOutput());
	}

  if (vtk_data->IsA("vtkRectilinearGrid"))
	{
		RGProjection = vtkProjectRG::New();
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,RGProjection));
		RGProjection->SetInput(vtkRectilinearGrid::SafeDownCast(vtk_data));
		vtkStructuredPoints *SP = vtkStructuredPoints::New();
		if (m_CamPosition == CAMERA_RX_FRONT)
			RGProjection->SetProjectionModeToY();
		else
			RGProjection->SetProjectionModeToX();

		RGProjection->Update();
		double b[6];
		RGProjection->GetOutput()->GetBounds(b);
		double x0 = b[0];
		double x1 = b[1];
		double y0 = b[2];
		double y1 = b[3];
		double z0 = b[4];
		double z1 = b[5];
		SP->SetOrigin(x0,y0,z0);
		SP->SetDimensions(128, 128, 1);
		SP->SetSpacing((x1-x0)/128, (y1-y0)/128, 1);
  
		vtkProbeFilter *pf = vtkProbeFilter::New();
		pf->SetInput(SP);
		pf->SetSource(RGProjection->GetOutput());
		pf->Update();
		((vtkImageData* )pf->GetOutput())->GetScalarRange(range);
		RXTexture->SetInput( (vtkImageData* )pf->GetOutput() );
		SP->Delete();
		pf->Delete();
	}

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
	int points_id[2];    
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
  vtkStructuredPoints *sp_data = vtkStructuredPoints::SafeDownCast(vtk_data);
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
  TickMapper->SetInput(CTLinesPD);

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
	vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(m_Vme->GetOutput()->GetVTKData());

	vtkMAFSmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInput(corner->GetOutput());

	vtkNEW(m_VolumeBoxActor);
	m_VolumeBoxActor->SetMapper(corner_mapper);
  m_VolumeBoxActor->PickableOff();
	m_VolumeBoxActor->SetVisibility(m_Selected);

	m_UsedAssembly->AddPart(m_VolumeBoxActor);

	//create something invisible in the front renderer so that ResetCamera will work
  m_ghost = NULL;
  if(m_UsedAssembly == m_AssemblyBack)
	{
		m_ghost = vtkActor::New();
		m_ghost->SetMapper(RXPlaneMapper);
		m_ghost->PickableOff();
    m_ghost->GetProperty()->SetOpacity(0.1);
    m_ghost->GetProperty()->SetRepresentationToPoints();
    m_ghost->GetProperty()->SetInterpolationToFlat();
		m_AssemblyFront->AddPart(m_ghost);
  }
  vtkDEL(CTLinesPoints);
  vtkDEL(CTCells);
  vtkDEL(CTLinesPD);
	
	vtkDEL(RXPlane);
	vtkDEL(RXPlaneMapper);
	vtkDEL(RXTexture);
	vtkDEL(SPProjection);
	vtkDEL(RGProjection);
	vtkDEL(TickMapper);
	vtkDEL(TickProperty);
}
//----------------------------------------------------------------------------
mafPipeVolumeProjected::~mafPipeVolumeProjected()
//----------------------------------------------------------------------------
{
	if(m_VolumeBoxActor)
    m_UsedAssembly->RemovePart(m_VolumeBoxActor);
	m_UsedAssembly->RemovePart(m_TickActor);
	m_UsedAssembly->RemovePart(m_RXActor);
  if(m_ghost) 
    m_AssemblyFront->RemovePart(m_ghost);

  vtkDEL(m_Lut);
	vtkDEL(m_TickActor);
	vtkDEL(m_RXActor);
  vtkDEL(m_ghost);

	vtkDEL(m_VolumeBoxActor);
}
//----------------------------------------------------------------------------
void mafPipeVolumeProjected::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	m_VolumeBoxActor->SetVisibility(sel);
  //there's no selection highlight for volume in rx-view
}
//----------------------------------------------------------------------------
void mafPipeVolumeProjected::SetLutRange(double low, double hi)
//----------------------------------------------------------------------------
{
	if(!m_Lut) 
    return;
	m_Lut->SetWindow(hi - low); 
	m_Lut->SetLevel((low + hi)/2);
	m_Lut->Build();
}
//----------------------------------------------------------------------------
void mafPipeVolumeProjected::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
	m_Lut->GetTableRange(range);
}
//----------------------------------------------------------------------------
void mafPipeVolumeProjected::TickActorVisibilityOn()
//----------------------------------------------------------------------------
{
  m_TickActor->VisibilityOn();
  m_TickActor->Modified();
}
//----------------------------------------------------------------------------
void mafPipeVolumeProjected::TickActorVisibilityOff()
//----------------------------------------------------------------------------
{
  m_TickActor->VisibilityOff();
  m_TickActor->Modified();
}
//----------------------------------------------------------------------------
void mafPipeVolumeProjected::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
  m_RXActor->SetPickable(enable);
  m_RXActor->Modified();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
