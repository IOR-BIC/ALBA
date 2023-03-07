/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeArbSlice
 Authors: Paolo Quadrani, Gianluigi Crimi
 
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

#include "albaPipeVolumeArbSlice.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSceneNode.h"
#include "albaTransform.h"
#include "mmaVolumeMaterial.h"

#include "albaVME.h"
#include "albaVMEOutputVolume.h"

#include "albaGUIFloatSlider.h"
#include "albaMatrix.h"

#include "albaLODActor.h"
#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTexture.h"
#include "vtkPolyDataMapper.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkALBAVolumeSlicer.h"
#include "vtkProperty.h"
#include "vtkDataSet.h"
#include "vtkRenderer.h"
#include "vtkOutlineSource.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkTransformFilter.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVolumeArbSlice);
//----------------------------------------------------------------------------

#include "albaMemDbg.h"
#include "vtkTransform.h"
#include "vtkObject.h"

#define EPSILON 1e-3

//----------------------------------------------------------------------------
albaPipeVolumeArbSlice::albaPipeVolumeArbSlice()
:albaPipeSlice()
//----------------------------------------------------------------------------
{ 
  m_SlicerPolygonal	= NULL;
  m_SlicerImage			= NULL;
  m_Image           = NULL;
  m_Texture		      = NULL;
  m_SlicePolydata		= NULL;
  m_SliceMapper		  = NULL;
  m_SliceActor      = NULL;

	m_SliceParametersInitialized  = false;
  m_ShowVolumeBox               = false;
	m_ShowBounds									= false;
	m_EnableSliceViewCorrection   = false;
  
  m_AssemblyUsed = NULL;
  m_ColorLUT  = NULL;
  m_CustomColorLUT = NULL;

  m_VolumeOutput = NULL;

	m_Box = NULL;
	m_Mapper = NULL;
	m_Actor = NULL;

	m_TickActor = NULL;

	m_SliceDirection = SLICE_ARB;
  m_SliceOpacity  = 1.0;
  m_TextureRes    = 512;

  m_XVector[0] = 0.0001;	//should be 0 !!! but there is a bug into vtkVolumeSlicer filter
  m_XVector[1] = 1;
  m_XVector[2] = 0;
  m_YVector[0] = 0;
  m_YVector[1] = 0;
  m_YVector[2] = 1;

	m_EpisolonNormal[0] = m_EpisolonNormal[1] = m_EpisolonNormal[2] = 0;
	m_Origin[0] = m_Origin[1] = m_Origin[2] = 0;
	m_Normal[0] = m_Normal[1] = m_Normal[2] = 0;
	m_Normal[3] = m_Origin[3] = 1;

	m_VolInverseMtr=m_VolInvRotMtr=NULL;

	m_ShowTICKs	 = false;
  m_EnableGPU = true;
  m_Interpolate = true;
  m_TrilinearInterpolationOn = TRUE;
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::InitializeSliceParameters(bool show_vol_bbox, bool show_bounds/* =false */, bool interpolate/* =true */)
//----------------------------------------------------------------------------
{
  m_ShowVolumeBox = show_vol_bbox;
	m_ShowBounds = show_bounds;
  m_Interpolate = interpolate;
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::InitializeSliceParameters(double slice_origin[3], bool show_vol_bbox,bool show_bounds/* =false */, bool interpolate/* =true */)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
  m_ShowVolumeBox = show_vol_bbox;
	m_ShowBounds = show_bounds;
  m_Interpolate = interpolate;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::InitializeSliceParameters( double slice_origin[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox,bool show_bounds/* =false */, bool interpolate/* =true */)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
	m_ShowBounds = show_bounds;
  m_Interpolate = interpolate;

  m_ShowVolumeBox = show_vol_bbox;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];

  m_XVector[0] = slice_xVect[0];
  m_XVector[1] = slice_xVect[1];
  m_XVector[2] = slice_xVect[2];

  m_YVector[0] = slice_yVect[0];
  m_YVector[1] = slice_yVect[1];
  m_YVector[2] = slice_yVect[2];

  vtkMath::Normalize(m_XVector);
  vtkMath::Normalize(m_YVector);
  vtkMath::Cross(m_YVector, m_XVector, m_NormalVector);
  vtkMath::Normalize(m_NormalVector);
  vtkMath::Cross(m_NormalVector, m_XVector, m_YVector);
  vtkMath::Normalize(m_YVector);

}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  m_Vme->AddObserver(this);

  m_AssemblyUsed = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

  m_VolumeOutput = albaVMEOutputVolume::SafeDownCast(m_Vme->GetOutput());
  assert(m_VolumeOutput != NULL);

	albaNEW(m_VolInverseMtr);
	m_VolInverseMtr->DeepCopy(m_VolumeOutput->GetAbsMatrix());
	m_VolIdentityMtr = m_VolInverseMtr->IsIdentity();
	if (!m_VolIdentityMtr)
	{
		m_VolInverseMtr->Invert();

		albaNEW(m_VolInvRotMtr);
		m_VolInvRotMtr->CopyRotation(*m_VolInverseMtr);
	}


  vtkDataSet *data = m_Vme->GetOutput()->GetVTKData();
  double b[6];
  m_Vme->GetOutput()->Update();
  data->Update();
  m_Vme->GetOutput()->GetVMELocalBounds(b);

  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  if (material->GetTableRange()[1] < material->GetTableRange()[0]) 
  {
		material->SetTableRange(data->GetScalarRange());
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
	
	vtkMath::Normalize(m_XVector);
	vtkMath::Normalize(m_YVector);
	vtkMath::Cross(m_YVector, m_XVector, m_NormalVector);
	vtkMath::Normalize(m_NormalVector);
	vtkMath::Cross(m_NormalVector, m_XVector, m_YVector);
	vtkMath::Normalize(m_YVector);
	CreateSlice();

	CreateTICKs();

  vtkALBASmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(data);

  vtkALBASmartPointer<vtkPolyDataMapper> corner_mapper;
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
		m_GhostActor->SetMapper(m_SliceMapper);
		m_GhostActor->PickableOff();
		m_GhostActor->GetProperty()->SetOpacity(0);
		m_GhostActor->GetProperty()->SetRepresentationToPoints();
		m_GhostActor->GetProperty()->SetInterpolationToFlat();
		m_AssemblyFront->AddPart(m_GhostActor);
  }
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::CreateTICKs()
//----------------------------------------------------------------------------
{
	//---- TICKs creation --------------------------
	vtkPolyData  *CTLinesPD      = vtkPolyData::New();	
	vtkPoints    *CTLinesPoints  = vtkPoints::New();	
	vtkCellArray *CTCells        = vtkCellArray::New();
	vtkIdType points_id[2];
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
void albaPipeVolumeArbSlice::CreateSlice()
//----------------------------------------------------------------------------
{
	double xspc = 0.33, yspc = 0.33, zspc = 1.0;

  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  vtk_data->Update();
  if(vtk_data->IsA("vtkImageData") || vtk_data->IsA("vtkImageData"))
  {
    ((vtkImageData *)vtk_data)->GetSpacing(xspc,yspc,zspc);
  }

	vtkNEW(m_SlicerPolygonal);
	vtkNEW(m_SlicerImage);
	m_SlicerImage->SetPlaneOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);
	m_SlicerPolygonal->SetPlaneOrigin(m_SlicerImage->GetPlaneOrigin());
	m_SlicerImage->SetPlaneAxisX(m_XVector);
	m_SlicerImage->SetPlaneAxisY(m_YVector);
	m_SlicerPolygonal->SetPlaneAxisX(m_XVector);
	m_SlicerPolygonal->SetPlaneAxisY(m_YVector);
	m_SlicerImage->SetInput(vtk_data);
	m_SlicerPolygonal->SetInput(vtk_data);
  
	vtkNEW(m_Image);
  m_Image->SetScalarType(vtk_data->GetPointData()->GetScalars()->GetDataType());
	m_Image->SetNumberOfScalarComponents(vtk_data->GetPointData()->GetScalars()->GetNumberOfComponents());
	m_Image->SetExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
	m_Image->SetSpacing(xspc, yspc, zspc);

	m_SlicerImage->SetOutput(m_Image);
  m_SlicerImage->SetGPUEnabled(m_EnableGPU);
  m_SlicerImage->Update();

	vtkNEW(m_Texture);
	m_Texture->RepeatOff();
	if (m_Interpolate)
	{
		m_Texture->InterpolateOn();
	}
  else
  {
    m_Texture->InterpolateOff();
  }
	m_Texture->SetQualityTo32Bit();
	m_Texture->SetInput(m_Image);
  m_Texture->SetLookupTable(m_ColorLUT);
  m_Texture->MapColorScalarsThroughLookupTableOn();

  vtkNEW(m_SlicePolydata);
	m_SlicerPolygonal->SetOutput(m_SlicePolydata);
	m_SlicerPolygonal->SetTexture(m_Image);
  m_SlicerPolygonal->SetGPUEnabled(m_EnableGPU);
	m_SlicerPolygonal->Update();

	// apply abs matrix to geometry
	vtkNEW(m_NormalTranform);
	
	// to delete
	vtkNEW(m_NormalTranformFilter);

	m_NormalTranformFilter->SetInput(m_SlicePolydata);
	m_NormalTranformFilter->SetTransform(m_NormalTranform);
	m_NormalTranformFilter->Update();


	vtkNEW(m_SliceMapper);
	if (m_EnableSliceViewCorrection)
		m_SliceMapper->SetInput(m_NormalTranformFilter->GetPolyDataOutput());
	else
		m_SliceMapper->SetInput(m_SlicePolydata);

	m_SliceMapper->ScalarVisibilityOff();

	vtkNEW(m_SliceActor);
	m_SliceActor->SetMapper(m_SliceMapper);
	m_SliceActor->SetTexture(m_Texture);
	m_SliceActor->GetProperty()->SetAmbient(1.f);
	m_SliceActor->GetProperty()->SetDiffuse(0.f);
  m_SliceActor->GetProperty()->SetOpacity(m_SliceOpacity);

	m_AssemblyUsed->AddPart(m_SliceActor);
}
//----------------------------------------------------------------------------
albaPipeVolumeArbSlice::~albaPipeVolumeArbSlice()
//----------------------------------------------------------------------------
{
  m_Vme->RemoveObserver(this);
	
  if(m_VolumeBoxActor)
    m_AssemblyFront->RemovePart(m_VolumeBoxActor);
	if(m_Actor)
		m_AssemblyUsed->RemovePart(m_Actor);
	if(m_TickActor)
		m_AssemblyUsed->RemovePart(m_TickActor);

	
	if(m_SliceActor)
    m_AssemblyUsed->RemovePart(m_SliceActor);
	if (m_SlicerImage)
	{
    m_SlicerImage->SetSliceTransform(NULL);
	}
  if (m_SlicerPolygonal)
  {
    m_SlicerPolygonal->SetSliceTransform(NULL);
  }
  vtkDEL(m_SlicerImage);
	vtkDEL(m_SlicerPolygonal);
	vtkDEL(m_Image);
	vtkDEL(m_Texture);
	vtkDEL(m_SliceMapper);
	vtkDEL(m_SlicePolydata);
	vtkDEL(m_NormalTranform);
	vtkDEL(m_NormalTranformFilter);
	vtkDEL(m_SliceActor);
	vtkDEL(m_VolumeBoxActor);
	vtkDEL(m_Actor);
	vtkDEL(m_TickActor);

	albaDEL(m_VolInverseMtr);
	albaDEL(m_VolInvRotMtr);

  if(m_GhostActor) 
    m_AssemblyFront->RemovePart(m_GhostActor);
  vtkDEL(m_GhostActor);
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::SetLutRange(double low, double high)
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  material->m_Window_LUT = high-low;
  material->m_Level_LUT  = (low+high)*.5;
  material->SetTableRange(low,high);
  material->UpdateProp();
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  material->m_ColorLut->GetTableRange(range);
}

//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::SetSlice(double origin[3], float xVect[3], float yVect[3])
//----------------------------------------------------------------------------
{
  m_Origin[0] = origin[0];
  m_Origin[1] = origin[1];
  m_Origin[2] = origin[2];

	m_XVector[0] = xVect[0];
	m_XVector[1] = xVect[1];
	m_XVector[2] = xVect[2];

	m_YVector[0] = yVect[0];
	m_YVector[1] = yVect[1];
	m_YVector[2] = yVect[2];

	vtkMath::Normalize(m_XVector);
	vtkMath::Normalize(m_YVector);
	vtkMath::Cross(m_YVector, m_XVector, m_NormalVector);
	vtkMath::Normalize(m_NormalVector);
	vtkMath::Cross(m_NormalVector, m_XVector, m_YVector);
	vtkMath::Normalize(m_YVector);  

	SetSlice(NULL, NULL);
}

//----------------------------------------------------------------------------
//Set the origin and normal of the slice
void albaPipeVolumeArbSlice::SetSlice(double* Origin, double* Normal)
//----------------------------------------------------------------------------
{
	if (Origin != NULL)
	{
		if (m_VolIdentityMtr)
		{
			m_Origin[0] = Origin[0];
			m_Origin[1] = Origin[1];
			m_Origin[2] = Origin[2];
		}
		else
		{
			double originMult[4] = { Origin[0], Origin[1], Origin[2], 1 };
			m_VolInverseMtr->MultiplyPoint(originMult, m_Origin);
		}
	}

  if (Normal != NULL)
  {
		if (m_VolIdentityMtr)
		{
			m_Normal[0] = Normal[0];
			m_Normal[1] = Normal[1];
			m_Normal[2] = Normal[2];
		}
		else
		{
			double normalMult[4] = { Normal[0], Normal[1], Normal[2], 1 };
			m_VolInvRotMtr->MultiplyPoint(normalMult, m_Normal);
		}


    //arbitrary slicing =>
    //we need to compute XVector and YVector from the given normal    
    double n[3], xv[3], yv[3];
    for (int i = 0; i < 3; i++) 
    {
      m_NormalVector[i] = (float)m_Normal[i];
      n[i] = m_Normal[i];
			m_EpisolonNormal[i] = -m_Normal[i] * EPSILON;
    }
        
    vtkMath::Normalize(n);               
    albaTransform::FindPerpendicularVersors(n, xv, yv) ;

    for (int i = 0; i < 3; i++) 
    {
      m_XVector[i] = (float)xv[i];
      m_YVector[i] = (float)yv[i];
    }

    vtkMath::Normalize(m_XVector);
    vtkMath::Normalize(m_YVector);
  }

	
	if(m_SlicerImage)
	{
		m_SlicerImage->SetPlaneOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);
		m_SlicerPolygonal->SetPlaneOrigin(m_SlicerImage->GetPlaneOrigin());
		m_SlicerImage->SetPlaneAxisX(m_XVector);
		m_SlicerImage->SetPlaneAxisY(m_YVector);
		m_SlicerPolygonal->SetPlaneAxisX(m_XVector);
		m_SlicerPolygonal->SetPlaneAxisY(m_YVector);      
  }
	
	if(m_NormalTranformFilter && m_EnableSliceViewCorrection)
	{
		m_NormalTranform->Identity();
		m_NormalTranform->Translate(m_EpisolonNormal);
		m_NormalTranform->Update();

		m_NormalTranformFilter->Modified();
	}

  UpdateSlice();
}

//----------------------------------------------------------------------------
//Get the slice origin coordinates and normal.
//Both, Origin and Normal may be NULL, if the value is not to be retrieved.
/*virtual*/ void albaPipeVolumeArbSlice::GetSlice(double *Origin, double *Normal)
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
    Normal[0] = (double)m_NormalVector[0];
    Normal[1] = (double)m_NormalVector[1];
    Normal[2] = (double)m_NormalVector[2];
  }  
}

//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::SetSliceOpacity(double opacity)
//----------------------------------------------------------------------------
{
  m_SliceOpacity = opacity;
  
  for (int i=0;i<3;i++)
  {
    if(m_SliceActor)
      m_SliceActor->GetProperty()->SetOpacity(m_SliceOpacity);
  }
}
//----------------------------------------------------------------------------
float albaPipeVolumeArbSlice::GetSliceOpacity()
//----------------------------------------------------------------------------
{
  return m_SliceOpacity;
}
//----------------------------------------------------------------------------
albaGUI *albaPipeVolumeArbSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  double b[6] = {-1,1,-1,1,-1,1};
  m_Gui = new albaGUI(this);
  m_Gui->Lut(ID_LUT_CHOOSER,"Lut",m_ColorLUT);
	m_Gui->FloatSlider(ID_OPACITY_SLIDER,"Opacity",&m_SliceOpacity,0.1,1.0);
  m_Vme->GetOutput()->GetVMELocalBounds(b);
	m_Gui->Divider();
//   m_Gui->Bool(ID_ENABLE_GPU, "Enable GPU", &m_EnableGPU, 1, 
//     _("Enables / disables GPU support for slicing. GPU slicing is typically faster but may produce wrong results on some hardware."));
  m_Gui->Bool(ID_ENABLE_TRILINEAR_INTERPOLATION,"Interpolation",&m_TrilinearInterpolationOn,1,"Enable/Disable tri-linear interpolation on slices");
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
 {
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
      case ID_LUT_CHOOSER:
      {
        mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
        material->UpdateFromTables();
				GetLogicManager()->CameraUpdate();
      }
      break;
			case ID_OPACITY_SLIDER:
				SetSliceOpacity(m_SliceOpacity);
				GetLogicManager()->CameraUpdate();
        break;
			break;
      case ID_ENABLE_GPU:
        UpdateSlice();
				GetLogicManager()->CameraUpdate();
        break;
      case ID_ENABLE_TRILINEAR_INTERPOLATION:
        UpdateSlice();
				GetLogicManager()->CameraUpdate();
        break;
      default:
      break;
    }
  }
  else if (alba_event->GetSender() == m_Vme)
  {
    if(alba_event->GetId() == VME_OUTPUT_DATA_UPDATE)
    {
      UpdateSlice();
    }
  }
  else
  {
    albaEventMacro(*alba_event);
  }
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::SetColorLookupTable(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  int i;
  if (lut == NULL)
  {
    if(m_Texture)
      m_Texture->SetLookupTable(m_ColorLUT);
		return;
  }
  m_CustomColorLUT = lut;
  if(m_Texture)
    m_Texture->SetLookupTable(m_CustomColorLUT);
  
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	m_ShowVolumeBox = sel;
	m_VolumeBoxActor->SetVisibility(sel);
}

//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::ShowTICKsOn()
//----------------------------------------------------------------------------
{
	m_ShowTICKs=true;
	if(m_TickActor)
		m_TickActor->SetVisibility(m_ShowTICKs);
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::ShowTICKsOff()
//----------------------------------------------------------------------------
{
	m_ShowTICKs=false;
	if(m_TickActor)
		m_TickActor->SetVisibility(m_ShowTICKs);
}

//------------------------------------------------------------------------
//Updates VTK slicers. It also sets GPUEnabled flag. 
void albaPipeVolumeArbSlice::UpdateSlice()
//------------------------------------------------------------------------
{
	if (m_SlicerImage != NULL)
	{
		m_SlicerImage->SetGPUEnabled(m_EnableGPU);
		m_SlicerImage->SetTrilinearInterpolation(m_TrilinearInterpolationOn == 1);
		m_SlicerImage->Update();
	}

	if (m_SlicerPolygonal != NULL)
	{
		m_SlicerPolygonal->SetGPUEnabled(m_EnableGPU);
		m_SlicerPolygonal->SetTrilinearInterpolation(m_TrilinearInterpolationOn == 1);
		m_SlicerPolygonal->Update();
	}
}

//------------------------------------------------------------------------
void albaPipeVolumeArbSlice::SetEnableGPU(int enable)
//------------------------------------------------------------------------
{
  m_EnableGPU = enable;
  UpdateSlice();
};

//------------------------------------------------------------------------
int albaPipeVolumeArbSlice::GetEnableGPU()
//------------------------------------------------------------------------
{
  return m_EnableGPU;
};

//----------------------------------------------------------------------------
void albaPipeVolumeArbSlice::SetEnableSliceViewCorrection(bool val)
{
	m_EnableSliceViewCorrection = val;

	if (m_EnableSliceViewCorrection)
		m_SliceMapper->SetInput(m_NormalTranformFilter->GetPolyDataOutput());
	else
		m_SliceMapper->SetInput(m_SlicePolydata);
}
