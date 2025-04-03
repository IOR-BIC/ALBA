/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceSlice
 Authors: Silvano Imboden, Paolo Quadrani, Gianluigi Crimi 
 
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

#include "albaPipeSurfaceSlice.h"
#include "albaSceneNode.h"
#include "albaVMESurface.h"
#include "mmaMaterial.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"
#include "albaAxes.h"
#include "albaDataVector.h"
#include "albaVMEGenericAbstract.h"
#include "albaVMELandmark.h"
#include "albaAbsMatrixPipe.h"
#include "vtkALBAToLinearTransform.h"
#include "albaVMEOutputPointSet.h"
#include "albaVMEOutputLandmarkCloud.h"
#include "albaVMELandmarkCloud.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkTexture.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTextureMapToCylinder.h"
#include "vtkTextureMapToPlane.h"
#include "vtkTextureMapToSphere.h"
#include "vtkALBAFixedCutter.h"
#include "vtkPlane.h"
#include "vtkALBAExtendedGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkLookupTable.h"
#include "albaVMEMaps.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeSurfaceSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeSurfaceSlice::albaPipeSurfaceSlice()
:albaPipeSlice()
//----------------------------------------------------------------------------
{
  m_Texture         = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;
  m_MaterialButton  = NULL;
  m_Cutter			= NULL;
  m_Plane			= NULL;

  m_SphereSource = NULL;
  

  m_ScalarVisibility = 0;
  m_Border=1;
	m_Pickable = true;

	m_ShowSelection = false;

	m_VTKTransform = NULL;
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::Create(albaSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;
	m_Texture = NULL;
	m_Mapper = NULL;
	m_Actor = NULL;
	m_OutlineActor = NULL;
	m_Axes = NULL;

	assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputSurface) \
		|| m_Vme->GetOutput()->IsALBAType(albaVMEOutputPointSet)\
		|| m_Vme->GetOutput()->IsALBAType(albaVMEOutputLandmarkCloud));
	vtkPolyData *data = NULL;
	mmaMaterial *material = NULL;

	m_Vme->AddObserver(this);

	vtkALBASmartPointer<vtkALBAExtendedGlyph3D> glyph;

	if (m_Vme->GetOutput()->IsALBAType(albaVMEOutputSurface))
	{
		albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
		assert(surface_output);
		surface_output->Update();
		data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());


		material = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");
		if(!material)
			material = surface_output->GetMaterial();

		vtkNEW(m_Table);
		m_Table->DeepCopy(material->m_ColorLut);
	}
	else if (m_Vme->GetOutput()->IsALBAType(albaVMEOutputPointSet))
	{
		albaVMELandmarkCloud *lmc = albaVMELandmarkCloud::SafeDownCast(m_Vme);
		if (!lmc)
		{
			albaVMELandmark *lm = albaVMELandmark::SafeDownCast(m_Vme);
			if (lm)
				lmc = albaVMELandmarkCloud::SafeDownCast(lm->GetParent());
		}

		if (lmc)
		{
			albaVMEOutputPointSet *landmark_cloud_output = albaVMEOutputPointSet::SafeDownCast(m_Vme->GetOutput());
			assert(landmark_cloud_output);
			landmark_cloud_output->Update();


			vtkNEW(m_SphereSource);
			m_SphereSource->SetRadius(lmc->GetRadius());
			m_SphereSource->SetThetaResolution(lmc->GetSphereResolution());
			m_SphereSource->SetPhiResolution(lmc->GetSphereResolution());
			m_SphereSource->Update();

			glyph->SetSource(m_SphereSource->GetOutput());
      glyph->SetInputData(landmark_cloud_output->GetVTKData());
			glyph->OrientOff();
			glyph->ScalingOff();
			glyph->ScalarVisibilityOn();
			glyph->Update();

			data = vtkPolyData::SafeDownCast(glyph->GetOutput());

			material = landmark_cloud_output->GetMaterial();
		}
	}


	assert(data);
	vtkDataArray *scalars = data->GetPointData()->GetScalars();
	double sr[2] = { 0,1 };

	m_Plane = vtkPlane::New();
	m_Cutter = vtkALBAFixedCutter::New();

	m_Plane->SetOrigin(m_Origin);
	m_Plane->SetNormal(m_Normal);
	vtkNEW(m_VTKTransform);
	m_VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
	m_Plane->SetTransform(m_VTKTransform);

	m_Cutter->SetInputData(data);
	m_Cutter->SetCutFunction(m_Plane);
	m_Cutter->Update();
	if (scalars != NULL)
	{
		m_ScalarVisibility = 1;
		scalars->GetRange(sr);
	}

	assert(material);  // all vme that use PipeSurface must have the material correctly set

	m_Mapper = vtkPolyDataMapper::New();

	if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
	{
		if (data->GetPointData()->GetTCoords() == NULL)
		{
			GenerateTextureMapCoordinate();
		}
		else
		{
      m_Mapper->SetInputConnection(m_Cutter->GetOutputPort());
		}
	}
	else
	{
    m_Mapper->SetInputConnection(m_Cutter->GetOutputPort());
	}
	if (m_Vme->GetOutput()->IsALBAType(albaVMEOutputPointSet)) m_ScalarVisibility = 0;
	m_Mapper->SetScalarVisibility(m_ScalarVisibility);
	//m_Mapper->SetScalarRange(sr);

	
	m_Texture = vtkTexture::New();
	m_Texture->SetQualityTo32Bit();
	m_Texture->InterpolateOn();
	if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
	{
		if (material->GetMaterialTexture() != NULL)
		{
      m_Texture->SetInputData(material->GetMaterialTexture());
		}
		else if (material->GetMaterialTextureID() != -1)
		{
			albaVME *texture_vme = m_Vme->GetRoot()->FindInTreeById(material->GetMaterialTextureID());
      m_Texture->SetInputData((vtkImageData *)texture_vme->GetOutput()->GetVTKData());
		}
		else
		{
			albaErrorMacro("texture info not correctly set inside material!! ");
		}
	}

	m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);
	if (material->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
	{
		m_Actor->SetProperty(material->m_Prop);
	}
	//  else if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
	if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
	{
		m_Actor->SetTexture(m_Texture);
	}
	m_Actor->GetProperty()->SetLineWidth(1);

	m_Actor->SetPickable(m_Pickable);
	m_AssemblyFront->AddPart(m_Actor);

	// selection highlight
	vtkALBASmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInputData(data);  

	vtkALBASmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInputConnection(corner->GetOutputPort());

	vtkALBASmartPointer<vtkProperty> corner_props;
	corner_props->SetColor(1, 1, 1);
	corner_props->SetAmbient(1);
	corner_props->SetRepresentationToWireframe();
	corner_props->SetInterpolationToFlat();

	m_OutlineActor = vtkActor::New();
	m_OutlineActor->SetMapper(corner_mapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(corner_props);

	m_AssemblyFront->AddPart(m_OutlineActor);

	m_Axes = new albaAxes(m_RenFront, m_Vme);
	m_Axes->SetVisibility(0);
}
//----------------------------------------------------------------------------
albaPipeSurfaceSlice::~albaPipeSurfaceSlice()
//----------------------------------------------------------------------------
{
  m_Vme->RemoveObserver(this);
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

  vtkDEL(m_Texture);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineActor);
	vtkDEL(m_VTKTransform);
  vtkDEL(m_Plane);
  vtkDEL(m_Cutter);
  cppDEL(m_Axes);
  vtkDEL(m_SphereSource);
	//@@@ if(m_use_axes) wxDEL(m_axes);  
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor && m_Actor->GetVisibility() && m_ShowSelection) 
	{
			m_OutlineActor->SetVisibility(sel);
	}
}

//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::SetActorPicking(int enable)
{
	m_Pickable = enable;
	if (m_Actor != NULL)
	{
		m_Actor->SetPickable(enable);
		m_Actor->Modified();
	}
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::SetLookupTable(vtkLookupTable *table)
{
	m_Table->DeepCopy(table);
	m_Mapper->ScalarVisibilityOn();
	m_Mapper->SetLookupTable(m_Table);
	m_Mapper->SetUseLookupTableScalarRange(true);
	m_Mapper->Modified();
}

//----------------------------------------------------------------------------
albaGUI *albaPipeSurfaceSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,_("Border"),&m_Border,1.0,5.0);
  m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
	  case ID_BORDER_CHANGE:
		  {
			  m_Actor->GetProperty()->SetLineWidth(m_Border);
			  m_Actor->Modified();
				GetLogicManager()->CameraUpdate();
		  }
	  break;
		case ID_LUT:
		{
			double sr[2];
			m_Table->GetTableRange(sr);
			m_Mapper->SetScalarRange(sr);
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_SCALAR_MAP_ACTIVE:
		{

			m_Mapper->SetScalarVisibility(true);
						
			UpdateScalars();

			if (m_Vme->IsA("albaVMEMaps"))
			{
				mmaMaterial *material = (mmaMaterial *)((albaVMEMaps *)m_Vme)->GetMaterial();
				if (material)
					SetLookupTable(material->m_ColorLut);
			}
		
			GetLogicManager()->CameraUpdate();
		}
		break;

      default:
        albaEventMacro(*e);
      break;
    }
  }
  else if (alba_event->GetId() == albaVMELandmarkCloud::CLOUD_RADIUS_MODIFIED)
  {
    if (m_SphereSource)
    {
      m_SphereSource->SetRadius(((albaVMELandmarkCloud *)m_Vme)->GetRadius());
    }
  }
  else if (alba_event->GetId() == albaVMELandmarkCloud::CLOUD_SPHERE_RES)
  {
    if (m_SphereSource)
    {
      m_SphereSource->SetThetaResolution(((albaVMELandmarkCloud *)m_Vme)->GetSphereResolution());
      m_SphereSource->SetPhiResolution(((albaVMELandmarkCloud *)m_Vme)->GetSphereResolution());
    }
  }
	else if (alba_event->GetId() == VME_ABSMATRIX_UPDATE)
	{
		if(m_VTKTransform)  m_VTKTransform->SetInputMatrix(m_Vme->GetOutput()->GetAbsMatrix());
		if(m_Plane)  m_Plane->SetTransform(m_VTKTransform);
	}
}

//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::UpdateScalars()

{
	m_Vme->Update();

	UpdateLUTAndMapperFromNewActiveScalars();
}

//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::UpdateLUTAndMapperFromNewActiveScalars()
{
	vtkPolyData *data = vtkPolyData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
	double sr[2];


	m_Mapper->SetLookupTable(m_Table);
	m_Mapper->UseLookupTableScalarRangeOn();
	m_Mapper->Update();
	m_Actor->Modified();

	UpdateProperty();
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::GenerateTextureMapCoordinate()
//----------------------------------------------------------------------------
{
  albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  mmaMaterial *material = surface_output->GetMaterial();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());

  if (material->m_TextureMappingMode == mmaMaterial::PLANE_MAPPING)
  {
    vtkALBASmartPointer<vtkTextureMapToPlane> plane_texture_mapper;
    plane_texture_mapper->SetInputData(data);
    m_Mapper->SetInputConnection(plane_texture_mapper->GetOutputPort());
  }
  else if (material->m_TextureMappingMode == mmaMaterial::CYLINDER_MAPPING)
  {
    vtkALBASmartPointer<vtkTextureMapToCylinder> cylinder_texture_mapper;
    cylinder_texture_mapper->SetInputData(data);
    cylinder_texture_mapper->PreventSeamOff();
    m_Mapper->SetInputConnection(cylinder_texture_mapper->GetOutputPort());
  }
  else if (material->m_TextureMappingMode == mmaMaterial::SPHERE_MAPPING)
  {
    vtkALBASmartPointer<vtkTextureMapToSphere> sphere_texture_mapper;
    sphere_texture_mapper->SetInputData(data);
    sphere_texture_mapper->PreventSeamOff();
    m_Mapper->SetInputConnection(sphere_texture_mapper->GetOutputPort());
  }
  else
  {
    m_Mapper->SetInputData(data);
  }
}

//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::SetSlice(double *Origin, double *Normal)
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
    m_Normal[0] = Normal[0];
    m_Normal[1] = Normal[1];
    m_Normal[2] = Normal[2];
  }

  if (m_Plane != NULL && m_Cutter != NULL)
  {
    m_Plane->SetNormal(m_Normal);
    m_Plane->SetOrigin(m_Origin);
    m_Cutter->SetCutFunction(m_Plane);
    m_Cutter->Update();    
  }
}

//----------------------------------------------------------------------------
double albaPipeSurfaceSlice::GetThickness()
//----------------------------------------------------------------------------
{
	return m_Border;
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSlice::SetThickness(double thickness)
//----------------------------------------------------------------------------
{
	m_Border=thickness;
	m_Actor->GetProperty()->SetLineWidth(m_Border);
  m_Actor->Modified();
	GetLogicManager()->CameraUpdate();
}
