/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeSlice_BES
 Authors: Paolo Quadrani
 
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

#include "mafPipeVolumeSlice_BES.h"
#include "mafDecl.h"
#include "mafGUI.h"
#include "mafSceneNode.h"
#include "mafTransform.h"
#include "mmaVolumeMaterial.h"
#include "mafEventSource.h"

#include "mafVME.h"
#include "mafVMEOutputVolume.h"

#include "mafGUIFloatSlider.h"

#include "mafLODActor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTexture.h"
#include "vtkPolyDataMapper.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkMAFVolumeSlicer_BES.h"
#include "vtkProperty.h"
#include "vtkDataSet.h"
#include "vtkRenderer.h"
#include "vtkOutlineSource.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVolumeSlice_BES);
//----------------------------------------------------------------------------

#include "mafMemDbg.h"

//----------------------------------------------------------------------------
mafPipeVolumeSlice_BES::mafPipeVolumeSlice_BES()
:mafPipeSlice()
//----------------------------------------------------------------------------
{ 
  for(int i = 0; i<3; i++)
  {
    m_SlicerPolygonal[i]	= NULL;
    m_SlicerImage[i]			= NULL;
    m_Image[i]            = NULL;
    m_Texture[i]		      = NULL;
    m_SlicePolydata[i]		= NULL;
    m_SliceMapper[i]		  = NULL;
    m_SliceActor[i]	      = NULL;
    m_SliceSlider[i]      = NULL;
  }
  m_SliceParametersInitialized  = false;
  m_ShowVolumeBox               = false;
	m_ShowBounds									= false;
  
  m_AssemblyUsed = NULL;
  m_ColorLUT  = NULL;
  m_CustomColorLUT = NULL;

  m_VolumeOutput = NULL;

	m_Box = NULL;
	m_Mapper = NULL;
	m_Actor = NULL;

	m_TickActor = NULL;

  m_SliceDirection  = SLICE_Z;
  m_SliceOpacity  = 1.0;
  m_TextureRes    = 512;

  m_XVector[0][0] = 0.0001;	//should be 0 !!! but there is a bug into vtkVolumeSlicer filter
  m_XVector[0][1] = 1;
  m_XVector[0][2] = 0;
  m_YVector[0][0] = 0;
  m_YVector[0][1] = 0;
  m_YVector[0][2] = 1;

  m_XVector[1][0] = 0;
  m_XVector[1][1] = 0;
  m_XVector[1][2] = 1;
  m_YVector[1][0] = 1;
  m_YVector[1][1] = 0;
  m_YVector[1][2] = 0;

  m_XVector[2][0] = 1;
  m_XVector[2][1] = 0;
  m_XVector[2][2] = 0;
  m_YVector[2][0] = 0;
  m_YVector[2][1] = 1;
  m_YVector[2][2] = 0;

	m_ShowSlider = true;
	m_ShowTICKs	 = false;
  m_EnableGPU = FALSE;
  m_Interpolate = true;
  m_TrilinearInterpolationOn = TRUE;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::InitializeSliceParameters(int direction, bool show_vol_bbox, bool show_bounds/* =false */, bool interpolate/* =true */)
//----------------------------------------------------------------------------
{
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
	m_ShowBounds = show_bounds;
  m_Interpolate = interpolate;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::InitializeSliceParameters(int direction, double slice_origin[3], bool show_vol_bbox,bool show_bounds/* =false */, bool interpolate/* =true */)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
	m_ShowBounds = show_bounds;
  m_Interpolate = interpolate;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::InitializeSliceParameters(int direction, double slice_origin[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox,bool show_bounds/* =false */, bool interpolate/* =true */)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
	m_ShowBounds = show_bounds;
  m_Interpolate = interpolate;
  
  m_SliceDirection= direction;
	if(m_SliceDirection == SLICE_ARB)
		m_SliceDirection = SLICE_Z;
  m_ShowVolumeBox = show_vol_bbox;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];

  if(m_SliceDirection != SLICE_ORTHO)
  {
    m_XVector[m_SliceDirection][0] = slice_xVect[0];
    m_XVector[m_SliceDirection][1] = slice_xVect[1];
    m_XVector[m_SliceDirection][2] = slice_xVect[2];

    m_YVector[m_SliceDirection][0] = slice_yVect[0];
    m_YVector[m_SliceDirection][1] = slice_yVect[1];
    m_YVector[m_SliceDirection][2] = slice_yVect[2];

    vtkMath::Normalize(m_XVector[m_SliceDirection]);
    vtkMath::Normalize(m_YVector[m_SliceDirection]);
    vtkMath::Cross(m_YVector[m_SliceDirection], m_XVector[m_SliceDirection], m_NormalVector[m_SliceDirection]);
    vtkMath::Normalize(m_NormalVector[m_SliceDirection]);
    vtkMath::Cross(m_NormalVector[m_SliceDirection], m_XVector[m_SliceDirection], m_YVector[m_SliceDirection]);
    vtkMath::Normalize(m_YVector[m_SliceDirection]);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  m_Vme->GetEventSource()->AddObserver(this);

  m_AssemblyUsed = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

  m_VolumeOutput = mafVMEOutputVolume::SafeDownCast(m_Vme->GetOutput());
  assert(m_VolumeOutput != NULL);

  vtkDataSet *data = m_Vme->GetOutput()->GetVTKData();
  double b[6];
  m_Vme->GetOutput()->Update();
  data->Update();
  m_Vme->GetOutput()->GetVMELocalBounds(b);

  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  if (material->m_TableRange[1] < material->m_TableRange[0]) 
  {
    data->GetScalarRange(material->m_TableRange);
    material->UpdateProp();
  }
  
  m_ColorLUT = material->m_ColorLut;

  if(m_SliceDirection == SLICE_ARB)
		m_SliceDirection = SLICE_Z;

	if (!m_SliceParametersInitialized)
	{
    m_Origin[0] = (b[0] + b[1])*.5;
    m_Origin[1] = (b[2] + b[3])*.5;
    m_Origin[2] = (b[4] + b[5])*.5;
	}

	if(m_SliceDirection == SLICE_ORTHO)
	{
    // overwrite the plane vector, because the slices have to be orthogonal
    m_XVector[0][0] = 0.0001;	//modified by Paolo 29-10-2003 should be 0 !!! check into Sasha's filter
    m_XVector[0][1] = 1;
    m_XVector[0][2] = 0;
    m_YVector[0][0] = 0;
    m_YVector[0][1] = 0;
    m_YVector[0][2] = 1;

    m_XVector[1][0] = 0;
    m_XVector[1][1] = 0;
    m_XVector[1][2] = 1;
    m_YVector[1][0] = 1;
    m_YVector[1][1] = 0;
    m_YVector[1][2] = 0;

    m_XVector[2][0] = 1;
    m_XVector[2][1] = 0;
    m_XVector[2][2] = 0;
    m_YVector[2][0] = 0;
    m_YVector[2][1] = 1;
    m_YVector[2][2] = 0;

    for(int i = 0; i < 3; i++)
		{
			vtkMath::Normalize(m_XVector[i]);
			vtkMath::Normalize(m_YVector[i]);
			vtkMath::Cross(m_YVector[i], m_XVector[i], m_NormalVector[i]);
			vtkMath::Normalize(m_NormalVector[i]);
			vtkMath::Cross(m_NormalVector[i], m_XVector[i], m_YVector[i]);
			vtkMath::Normalize(m_YVector[i]);
			CreateSlice(i);
		}
	}
	else
	{
		vtkMath::Normalize(m_XVector[m_SliceDirection]);
		vtkMath::Normalize(m_YVector[m_SliceDirection]);
		vtkMath::Cross(m_YVector[m_SliceDirection], m_XVector[m_SliceDirection], m_NormalVector[m_SliceDirection]);
		vtkMath::Normalize(m_NormalVector[m_SliceDirection]);
		vtkMath::Cross(m_NormalVector[m_SliceDirection], m_XVector[m_SliceDirection], m_YVector[m_SliceDirection]);
		vtkMath::Normalize(m_YVector[m_SliceDirection]);
		CreateSlice(m_SliceDirection);
	}

	CreateTICKs();

  vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(data);

  vtkMAFSmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInput(corner->GetOutput());

	vtkNEW(m_VolumeBoxActor);
	m_VolumeBoxActor->SetMapper(corner_mapper);
  m_VolumeBoxActor->PickableOff();
	m_VolumeBoxActor->SetVisibility(m_ShowVolumeBox);
	if(m_AssemblyUsed==m_AssemblyFront)
		m_VolumeBoxActor->SetScale(1.01);
	else
		m_VolumeBoxActor->SetScale(1);
	m_AssemblyFront->AddPart(m_VolumeBoxActor);

	if(m_ShowBounds)
	{
		double bounds[6];
		m_Vme->GetOutput()->Update();
		m_Vme->GetOutput()->GetVMELocalBounds(bounds);
		vtkNEW(m_Box);
		m_Box->SetBounds(bounds);
		vtkNEW(m_Mapper);
		m_Mapper->SetInput(m_Box->GetOutput());
		vtkNEW(m_Actor);
		m_Actor->SetMapper(m_Mapper);
		m_AssemblyUsed->AddPart(m_Actor);
	}

	// if the actor is in the background renderer
	// create something invisible in the front renderer so that ResetCamera will work
  m_GhostActor = NULL;
  if(m_AssemblyBack != NULL)
	{
		vtkNEW(m_GhostActor);
		m_GhostActor->SetMapper(m_SliceMapper[m_SliceDirection]);
		m_GhostActor->PickableOff();
		m_GhostActor->GetProperty()->SetOpacity(0);
		m_GhostActor->GetProperty()->SetRepresentationToPoints();
		m_GhostActor->GetProperty()->SetInterpolationToFlat();
		m_AssemblyFront->AddPart(m_GhostActor);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::CreateTICKs()
//----------------------------------------------------------------------------
{
	//---- TICKs creation --------------------------
	vtkPolyData  *CTLinesPD      = vtkPolyData::New();	
	vtkPoints    *CTLinesPoints  = vtkPoints::New();	
	vtkCellArray *CTCells        = vtkCellArray::New();
	int points_id[2];    
	int	counter = 0;

	vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
	vtk_data->Update();

	double bounds[6];
	vtk_data->GetBounds(bounds);

	double xmin, xmax, ymin, ymax, zmin, zmax;
	xmin = bounds[0];
	xmax = bounds[1];
	ymin = bounds[2];
	ymax = bounds[3];
	zmin = bounds[4];
	zmax = bounds[5];

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
	m_TickActor->SetVisibility(m_ShowTICKs);
	m_TickActor->PickableOff();
	m_TickActor->SetProperty(TickProperty);

	m_AssemblyUsed->AddPart(m_TickActor);

	vtkDEL(CTLinesPoints);
	vtkDEL(CTCells);
	vtkDEL(CTLinesPD);

	vtkDEL(TickMapper);
	vtkDEL(TickProperty);

}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::CreateSlice(int direction)
//----------------------------------------------------------------------------
{
	double xspc = 0.33, yspc = 0.33, zspc = 1.0;

  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  vtk_data->Update();
  if(vtk_data->IsA("vtkImageData") || vtk_data->IsA("vtkStructuredPoints"))
  {
    ((vtkImageData *)vtk_data)->GetSpacing(xspc,yspc,zspc);
  }

	vtkNEW(m_SlicerPolygonal[direction]);
	vtkNEW(m_SlicerImage[direction]);
	m_SlicerImage[direction]->SetPlaneOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);
	m_SlicerPolygonal[direction]->SetPlaneOrigin(m_SlicerImage[direction]->GetPlaneOrigin());
	m_SlicerImage[direction]->SetPlaneAxisX(m_XVector[direction]);
	m_SlicerImage[direction]->SetPlaneAxisY(m_YVector[direction]);
	m_SlicerPolygonal[direction]->SetPlaneAxisX(m_XVector[direction]);
	m_SlicerPolygonal[direction]->SetPlaneAxisY(m_YVector[direction]);
	m_SlicerImage[direction]->SetInput(vtk_data);
	m_SlicerPolygonal[direction]->SetInput(vtk_data);
//  m_SlicerImage[direction]->SetSliceTransform(m_Vme->GetOutput()->GetAbsTransform()->GetVTKTransform()->GetLinearInverse());
//  m_SlicerPolygonal[direction]->SetSliceTransform(m_Vme->GetOutput()->GetAbsTransform()->GetVTKTransform()->GetLinearInverse());
  
	vtkNEW(m_Image[direction]);
  m_Image[direction]->SetScalarType(vtk_data->GetPointData()->GetScalars()->GetDataType());
  //m_Image[direction]->SetScalarTypeToUnsignedChar();
	m_Image[direction]->SetNumberOfScalarComponents(vtk_data->GetPointData()->GetScalars()->GetNumberOfComponents());
  //m_Image[direction]->SetNumberOfScalarComponents(3);
	m_Image[direction]->SetExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
	m_Image[direction]->SetSpacing(xspc, yspc, zspc);

	m_SlicerImage[direction]->SetOutput(m_Image[direction]);
  m_SlicerImage[direction]->SetGPUEnabled(m_EnableGPU);
  m_SlicerImage[direction]->Update();

	vtkNEW(m_Texture[direction]);
	m_Texture[direction]->RepeatOff();
	if (m_Interpolate)
	{
		m_Texture[direction]->InterpolateOn();
	}
  else
  {
    m_Texture[direction]->InterpolateOff();
  }
	m_Texture[direction]->SetQualityTo32Bit();
	m_Texture[direction]->SetInput(m_Image[direction]);
  m_Texture[direction]->SetLookupTable(m_ColorLUT);
  m_Texture[direction]->MapColorScalarsThroughLookupTableOn();

  vtkNEW(m_SlicePolydata[direction]);
	m_SlicerPolygonal[direction]->SetOutput(m_SlicePolydata[direction]);
	m_SlicerPolygonal[direction]->SetTexture(m_Image[direction]);
  m_SlicerPolygonal[direction]->SetGPUEnabled(m_EnableGPU);
	m_SlicerPolygonal[direction]->Update();

	vtkNEW(m_SliceMapper[direction]);
	m_SliceMapper[direction]->SetInput(m_SlicePolydata[direction]);
	m_SliceMapper[direction]->ScalarVisibilityOff();

	vtkNEW(m_SliceActor[direction]);
	m_SliceActor[direction]->SetMapper(m_SliceMapper[direction]);
	m_SliceActor[direction]->SetTexture(m_Texture[direction]);
	m_SliceActor[direction]->GetProperty()->SetAmbient(1.f);
	m_SliceActor[direction]->GetProperty()->SetDiffuse(0.f);
  m_SliceActor[direction]->GetProperty()->SetOpacity(m_SliceOpacity);

	m_AssemblyUsed->AddPart(m_SliceActor[direction]);
}
//----------------------------------------------------------------------------
mafPipeVolumeSlice_BES::~mafPipeVolumeSlice_BES()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);
	
  if(m_VolumeBoxActor)
    m_AssemblyFront->RemovePart(m_VolumeBoxActor);
	if(m_Actor)
		m_AssemblyUsed->RemovePart(m_Actor);
	if(m_TickActor)
		m_AssemblyUsed->RemovePart(m_TickActor);

	for(int i = 0; i<3; i++)
	{
		if(m_SliceActor[i])
      m_AssemblyUsed->RemovePart(m_SliceActor[i]);
		if (m_SlicerImage[i])
		{
      m_SlicerImage[i]->SetSliceTransform(NULL);
		}
    if (m_SlicerPolygonal[i])
    {
      m_SlicerPolygonal[i]->SetSliceTransform(NULL);
    }
    vtkDEL(m_SlicerImage[i]);
		vtkDEL(m_SlicerPolygonal[i]);
		vtkDEL(m_Image[i]);
		vtkDEL(m_Texture[i]);
		vtkDEL(m_SliceMapper[i]);
		vtkDEL(m_SlicePolydata[i]);
		vtkDEL(m_SliceActor[i]);
	}
	vtkDEL(m_VolumeBoxActor);
	vtkDEL(m_Actor);
	vtkDEL(m_TickActor);

  if(m_GhostActor) 
    m_AssemblyFront->RemovePart(m_GhostActor);
  vtkDEL(m_GhostActor);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::SetLutRange(double low, double high)
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  material->m_Window_LUT = high-low;
  material->m_Level_LUT  = (low+high)*.5;
  material->m_TableRange[0] = low;
  material->m_TableRange[1] = high;
  material->UpdateProp();
  /*
  for(int i=0;i<3;i++)
	{
		if(m_SlicerImage[i])
		{
      m_SlicerImage[i]->SetWindow(high-low);
      m_SlicerImage[i]->SetLevel((low+high)*.5);
      m_SlicerImage[i]->Update();
      //m_Texture[i]->GetLookupTable()->SetRange(low,high);
      //m_Texture[i]->GetLookupTable()->Build();
		}
	}*/
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  material->m_ColorLut->GetTableRange(range);
  /*
	if(m_SliceDirection != SLICE_ORTHO)
    m_Texture[m_SliceDirection]->GetLookupTable()->GetRange();
	else
    m_Texture[0]->GetLookupTable()->GetRange();*/
}

//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::SetSlice(double origin[3], float xVect[3], float yVect[3])
//----------------------------------------------------------------------------
{
  m_Origin[0] = origin[0];
  m_Origin[1] = origin[1];
  m_Origin[2] = origin[2];

	m_XVector[m_SliceDirection][0] = xVect[0];
	m_XVector[m_SliceDirection][1] = xVect[1];
	m_XVector[m_SliceDirection][2] = xVect[2];

	m_YVector[m_SliceDirection][0] = yVect[0];
	m_YVector[m_SliceDirection][1] = yVect[1];
	m_YVector[m_SliceDirection][2] = yVect[2];

	vtkMath::Normalize(m_XVector[m_SliceDirection]);
	vtkMath::Normalize(m_YVector[m_SliceDirection]);
	vtkMath::Cross(m_YVector[m_SliceDirection], m_XVector[m_SliceDirection], m_NormalVector[m_SliceDirection]);
	vtkMath::Normalize(m_NormalVector[m_SliceDirection]);
	vtkMath::Cross(m_NormalVector[m_SliceDirection], m_XVector[m_SliceDirection], m_YVector[m_SliceDirection]);
	vtkMath::Normalize(m_YVector[m_SliceDirection]);  
}

//----------------------------------------------------------------------------
//Set the origin and normal of the slice
void mafPipeVolumeSlice_BES::SetSlice(double* Origin, double* Normal)
//----------------------------------------------------------------------------
{
  if (Origin != NULL)
  {
    m_Origin[0] = Origin[0];
    m_Origin[1] = Origin[1];
    m_Origin[2] = Origin[2];
  }

  if (Normal != NULL)
  {
    //arbitrary slicing =>
    //we need to compute XVector and YVector from the given normal    
    double n[3], xv[3], yv[3];
    for (int i = 0; i < 3; i++) 
    {
      m_NormalVector[m_SliceDirection][i] = (float)Normal[i];
      n[i] = Normal[i];
    }
        
    vtkMath::Normalize(n);               
    mafTransform::FindPerpendicularVersors(n, xv, yv) ;

    for (int i = 0; i < 3; i++) 
    {
      m_XVector[m_SliceDirection][i] = (float)xv[i];
      m_YVector[m_SliceDirection][i] = (float)yv[i];
    }

    vtkMath::Normalize(m_XVector[m_SliceDirection]);
    vtkMath::Normalize(m_YVector[m_SliceDirection]);

    //mafLogMessage("#### NEWSYS: (%.2f,%.2f,%.2f)x(%.2f,%.2f,%.2f)x(%.2f,%.2f,%.2f)",
    //  m_XVector[m_SliceDirection][0],m_XVector[m_SliceDirection][1],m_XVector[m_SliceDirection][2],
    //  m_YVector[m_SliceDirection][0],m_YVector[m_SliceDirection][1],m_YVector[m_SliceDirection][2],
    //  m_NormalVector[m_SliceDirection][0],m_NormalVector[m_SliceDirection][1],m_NormalVector[m_SliceDirection][2]);
  }

	for(int i=0;i<3;i++)
	{
		if(m_SlicerImage[i])
		{
			m_SlicerImage[i]->SetPlaneOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);
			m_SlicerPolygonal[i]->SetPlaneOrigin(m_SlicerImage[i]->GetPlaneOrigin());
			m_SlicerImage[i]->SetPlaneAxisX(m_XVector[i]);
			m_SlicerImage[i]->SetPlaneAxisY(m_YVector[i]);
			m_SlicerPolygonal[i]->SetPlaneAxisX(m_XVector[i]);
			m_SlicerPolygonal[i]->SetPlaneAxisY(m_YVector[i]);      
    }
	}

  UpdateSlice();
}

//----------------------------------------------------------------------------
//Get the slice origin coordinates and normal.
//Both, Origin and Normal may be NULL, if the value is not to be retrieved.
/*virtual*/ void mafPipeVolumeSlice_BES::GetSlice(double *Origin, double *Normal)
//----------------------------------------------------------------------------
{
  if (Origin != NULL)
  {
    Origin[0] = m_Origin[0];
    Origin[1] = m_Origin[1];
    Origin[2] = m_Origin[2];
  }

  if (Normal != NULL)
  {
    Normal[0] = (double)m_NormalVector[m_SliceDirection][0];
    Normal[1] = (double)m_NormalVector[m_SliceDirection][1];
    Normal[2] = (double)m_NormalVector[m_SliceDirection][2];
  }  
}

//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::SetSliceOpacity(double opacity)
//----------------------------------------------------------------------------
{
  m_SliceOpacity = opacity;
  
  for (int i=0;i<3;i++)
  {
    if(m_SliceActor[i])
      m_SliceActor[i]->GetProperty()->SetOpacity(m_SliceOpacity);
  }
}
//----------------------------------------------------------------------------
float mafPipeVolumeSlice_BES::GetSliceOpacity()
//----------------------------------------------------------------------------
{
  return m_SliceOpacity;
}
//----------------------------------------------------------------------------
mafGUI *mafPipeVolumeSlice_BES::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  double b[6] = {-1,1,-1,1,-1,1};
  m_Gui = new mafGUI(this);
  m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_Gui->FloatSlider(ID_OPACITY_SLIDER,"opacity",&m_SliceOpacity,0.1,1.0);
  m_Vme->GetOutput()->GetVMELocalBounds(b);
	if(m_ShowSlider)
	{
		if (m_SliceDirection == SLICE_X || m_SliceDirection == SLICE_ORTHO)
		{
			m_SliceSlider[0] = m_Gui->FloatSlider(ID_SLICE_SLIDER_X,"x",&m_Origin[0],b[0],b[1]);
		}
		if (m_SliceDirection == SLICE_Y || m_SliceDirection == SLICE_ORTHO)
		{
			m_SliceSlider[1] = m_Gui->FloatSlider(ID_SLICE_SLIDER_Y,"y",&m_Origin[1],b[2],b[3]);
		}
		if (m_SliceDirection == SLICE_Z || m_SliceDirection == SLICE_ORTHO)
		{
			m_SliceSlider[2] = m_Gui->FloatSlider(ID_SLICE_SLIDER_Z,"z",&m_Origin[2],b[4],b[5]);
		}
	}
	m_Gui->Divider();
//   m_Gui->Bool(ID_ENABLE_GPU, "Enable GPU", &m_EnableGPU, 1, 
//     _("Enables / disables GPU support for slicing. GPU slicing is typically faster but may produce wrong results on some hardware."));
  m_Gui->Bool(ID_ENABLE_TRILINEAR_INTERPOLATION,"Interpolation",&m_TrilinearInterpolationOn,1,"Enable/Disable tri-linear interpolation on slices");
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
 {
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_LUT_CHOOSER:
      {
        mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
        material->UpdateFromTables();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
      case ID_SLICE_SLIDER_X:
      case ID_SLICE_SLIDER_Y:
      case ID_SLICE_SLIDER_Z:
        SetOrigin(m_Origin);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
			case ID_OPACITY_SLIDER:
				SetSliceOpacity(m_SliceOpacity);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        break;
			break;
      case ID_ENABLE_GPU:
        UpdateSlice();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        break;
      case ID_ENABLE_TRILINEAR_INTERPOLATION:
        UpdateSlice();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        break;
      default:
      break;
    }
  }
  else if (maf_event->GetSender() == m_Vme)
  {
    if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
    {
      UpdateSlice();
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::SetColorLookupTable(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  int i;
  if (lut == NULL)
  {
    for (i=0;i<3;i++)
    {
      if(m_Texture[i])
        m_Texture[i]->SetLookupTable(m_ColorLUT);
    }
    return;
  }
  m_CustomColorLUT = lut;
  for (i=0;i<3;i++)
  {
    if(m_Texture[i])
      m_Texture[i]->SetLookupTable(m_CustomColorLUT);
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	m_ShowVolumeBox = sel;
	m_VolumeBoxActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::HideSlider()
//----------------------------------------------------------------------------
{
	m_ShowSlider=false;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::ShowSlider()
//----------------------------------------------------------------------------
{
	m_ShowSlider=true;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::ShowTICKsOn()
//----------------------------------------------------------------------------
{
	m_ShowTICKs=true;
	if(m_TickActor)
		m_TickActor->SetVisibility(m_ShowTICKs);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::ShowTICKsOff()
//----------------------------------------------------------------------------
{
	m_ShowTICKs=false;
	if(m_TickActor)
		m_TickActor->SetVisibility(m_ShowTICKs);
}

//------------------------------------------------------------------------
//Updates VTK slicers. It also sets GPUEnabled flag. 
void mafPipeVolumeSlice_BES::UpdateSlice()
//------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
  {
    if (m_SlicerImage[i] != NULL)
    {
      m_SlicerImage[i]->SetGPUEnabled(m_EnableGPU);
      m_SlicerImage[i]->SetTrilinearInterpolation(m_TrilinearInterpolationOn == 1);
      m_SlicerImage[i]->Update();
    }

    if (m_SlicerPolygonal[i] != NULL)
    {
      m_SlicerPolygonal[i]->SetGPUEnabled(m_EnableGPU);
      m_SlicerPolygonal[i]->SetTrilinearInterpolation(m_TrilinearInterpolationOn == 1);
      m_SlicerPolygonal[i]->Update();
    }
  }    
}

//------------------------------------------------------------------------
void mafPipeVolumeSlice_BES::SetEnableGPU(int enable)
//------------------------------------------------------------------------
{
  m_EnableGPU = enable;
  UpdateSlice();
};

//------------------------------------------------------------------------
int mafPipeVolumeSlice_BES::GetEnableGPU()
//------------------------------------------------------------------------
{
  return m_EnableGPU;
};