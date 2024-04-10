/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceTextured
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include "albaPipeSurfaceTextured.h"
#include "albaSceneNode.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"
#include "albaAxes.h"
#include "mmaMaterial.h"

#include "albaDataVector.h"
#include "albaVMESurface.h"
#include "albaVMEGenericAbstract.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTextureMapToCylinder.h"
#include "vtkTextureMapToPlane.h"
#include "vtkTextureMapToSphere.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkTexture.h"
#include "albaLODActor.h"
#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkActor.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeSurfaceTextured);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeSurfaceTextured::albaPipeSurfaceTextured()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_Texture         = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;
  m_MaterialButton  = NULL;
  m_SurfaceMaterial = NULL;
  m_Gui             = NULL;

  m_ScalarVisibility = 0;
  m_RenderingDisplayListFlag = 0;

  m_UseVTKProperty  = 1;
  m_UseTexture      = 0;
  m_UseLookupTable  = 0;

  m_EnableActorLOD  = 0;

	m_ShowAxis = 1;
  m_SelectionVisibility = 1;
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTextured::Create(albaSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;
  m_Texture         = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;

  m_Vme->Update();
  assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputSurface));
  albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  assert(data);

  m_Vme->AddObserver(this);

  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};
  if(scalars != NULL)
  {
    m_ScalarVisibility = 1;
    scalars->GetRange(sr);
  }

  m_SurfaceMaterial = surface_output->GetMaterial();
  assert(m_SurfaceMaterial);  // all vme that use PipeSurface must have the material correctly set

  vtkNEW(m_Mapper);

  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    m_UseTexture = 1;
    m_UseVTKProperty = 0;
    m_UseLookupTable = 0;
    if (data->GetPointData()->GetTCoords() == NULL)
    {
      GenerateTextureMapCoordinate();
    }
    else
    {
      m_Mapper->SetInputData(data);
    }
  }
  else
  {
    m_Mapper->SetInputData(data);
  }
  
  m_RenderingDisplayListFlag = m_Vme->IsAnimated() ? 1 : 0;
  m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
  m_Mapper->SetScalarVisibility(m_ScalarVisibility);

  vtkNEW(m_Texture);
  m_Texture->SetQualityTo32Bit();
  m_Texture->InterpolateOn();
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    if (m_SurfaceMaterial->GetMaterialTexture() != NULL)
    {
      vtkImageData *image = m_SurfaceMaterial->GetMaterialTexture();
      m_Texture->SetInputData(image);
      image->GetScalarRange(sr);
    }
    else if (m_SurfaceMaterial->GetMaterialTextureID() != -1)
    {
      albaVME *texture_vme = m_Vme->GetRoot()->FindInTreeById(m_SurfaceMaterial->GetMaterialTextureID());
      vtkImageData *image = (vtkImageData *)texture_vme->GetOutput()->GetVTKData();
      m_Texture->SetInputData(image);
      image->GetScalarRange(sr);
    }
    else
    {
      albaLogMessage(_("texture info not correctly stored inside material!!"));
    }
  }

  m_Mapper->SetUseLookupTableScalarRange(true);

  vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);
  m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
  
  if (m_EnableActorLOD == false)
  {

	  /*
	  
	  if( sz < m_PixelThreshold)
	  {
		  // Assign the actor's transformation matrix to the m_FlagActor 
		  // to put this one at the same position of the actor
		  m_FlagActor->PokeMatrix(GetMatrix());

		  glPointSize( m_FlagDimension );
		  m_FlagActor->Render(ren,m_FlagMapper); 
		  glPointSize( 1 );
	  }

	  */
	  //
	  // code section in albaLODActor.cpp
	  //
	  m_Actor->SetPixelThreshold(1);
  }

  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_LOOKUPTABLE)
  {
    m_UseTexture = 0;
    m_UseVTKProperty = 0;
    m_UseLookupTable = 1;
  }
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  {
    m_UseTexture = 0;
    m_UseVTKProperty = 1;
    m_UseLookupTable = 0;
    m_Actor->SetProperty(m_SurfaceMaterial->m_Prop);
  }
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    m_UseLookupTable = 1;
    m_Texture->SetLookupTable(m_SurfaceMaterial->m_ColorLut);
    m_Actor->SetTexture(m_Texture);
  }

  if (m_Gui)
  {
    m_Gui->Update();
  }

  if(m_AssemblyBack)
    m_AssemblyBack->AddPart(m_Actor);
  else
    m_AssemblyFront->AddPart(m_Actor);

  // selection highlight
  vtkALBASmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInputData(data);  

  vtkALBASmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInputConnection(corner->GetOutputPort());

  vtkALBASmartPointer<vtkProperty> corner_props;
	corner_props->SetColor(1,1,1);
	corner_props->SetAmbient(1);
	corner_props->SetRepresentationToWireframe();
	corner_props->SetInterpolationToFlat();

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetMapper(corner_mapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(corner_props);

  if(m_AssemblyBack)
    m_AssemblyBack->AddPart(m_OutlineActor);
  else
    m_AssemblyFront->AddPart(m_OutlineActor);

	if(m_RenFront)
	{
      m_Axes = new albaAxes(m_RenFront, m_Vme);
      m_Axes->SetVisibility(0);
	}

  m_GhostActor = NULL;
  if(m_AssemblyBack != NULL)
  {
    vtkNEW(m_GhostActor);
    m_GhostActor->SetMapper(m_Mapper);
    m_GhostActor->PickableOff();
    m_GhostActor->GetProperty()->SetOpacity(0);
    m_GhostActor->GetProperty()->SetRepresentationToPoints();

	// Set to gouraud shading instead of flat to solve pixellation 
	// look and feel in vertical application (medViewCrossCT in DP app) 
    m_GhostActor->GetProperty()->SetInterpolationToGouraud();
    m_AssemblyFront->AddPart(m_GhostActor);
  }
}
//----------------------------------------------------------------------------
albaPipeSurfaceTextured::~albaPipeSurfaceTextured()
//----------------------------------------------------------------------------
{
  m_Vme->RemoveObserver(this);

  if(m_AssemblyBack)
  {
    m_AssemblyBack->RemovePart(m_Actor);
    m_AssemblyBack->RemovePart(m_OutlineActor);
  }
  else
  {
    m_AssemblyFront->RemovePart(m_Actor);
    m_AssemblyFront->RemovePart(m_OutlineActor);
  }
  

  vtkDEL(m_Texture);
	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineActor);
  cppDEL(m_Axes);
  cppDEL(m_MaterialButton);

  if(m_GhostActor) 
  {
    m_AssemblyFront->RemovePart(m_GhostActor);
  }
  vtkDEL(m_GhostActor);
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTextured::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel && m_SelectionVisibility);
    m_Axes->SetVisibility(sel&&m_ShowAxis);
	}
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTextured::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    double sr[2];
    sr[0] = m_SurfaceMaterial->m_TableRange[0];
    sr[1] = m_SurfaceMaterial->m_TableRange[1];
    m_Mapper->SetScalarRange(sr);
  }
}
//----------------------------------------------------------------------------
albaGUI *albaPipeSurfaceTextured::CreateGui()
//----------------------------------------------------------------------------
{
  wxString mapping_mode[3] = {"Plane", "Cylinder","Sphere"};

  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  m_Gui->Bool(ID_RENDERING_DISPLAY_LIST,"Display list",&m_RenderingDisplayListFlag,0,"Turn on/off \nrendering display list calculation");
  m_Gui->Bool(ID_SCALAR_VISIBILITY,"Scalar vis.", &m_ScalarVisibility,0,"Turn on/off the scalar visibility");
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"Property",&m_UseVTKProperty);
  m_MaterialButton = new albaGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_TEXTURE,"Texture",&m_UseTexture);
  m_Gui->Button(ID_CHOOSE_TEXTURE,"Texture");
  albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  //m_Gui->Combo(ID_TEXTURE_MAPPING_MODE,"mapping",&surface_output->GetMaterial()->m_TextureMappingMode,3,mapping_mode);
  m_Gui->Enable(ID_CHOOSE_TEXTURE,m_UseTexture != 0);
  m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,m_UseTexture != 0);
  m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE && m_UseTexture != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_LOOKUP_TABLE,"Lut",&m_UseLookupTable);
  m_Gui->Lut(ID_LUT,"Lut",m_SurfaceMaterial->m_ColorLut);
  m_Gui->Enable(ID_LUT,m_UseLookupTable != 0);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_ENABLE_LOD,"LOD",&m_EnableActorLOD);

  if (m_SurfaceMaterial == NULL)
  {
    albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
    m_SurfaceMaterial = surface_output->GetMaterial();
  }
  bool texture_falg = m_SurfaceMaterial->GetMaterialTexture() != NULL || m_SurfaceMaterial->GetMaterialTextureID() != -1;
  m_Gui->Enable(ID_USE_TEXTURE, texture_falg);
  m_Gui->Enable(ID_USE_LOOKUP_TABLE, m_SurfaceMaterial->m_ColorLut != NULL);
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTextured::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_SCALAR_VISIBILITY:
		{
			m_Mapper->SetScalarVisibility(m_ScalarVisibility);
			if (m_ScalarVisibility)
			{
				vtkPolyData *data = (vtkPolyData *)m_Vme->GetOutput()->GetVTKData();
				assert(data);
				double range[2];
				data->GetScalarRange(range);
				m_Mapper->SetScalarRange(range);
			}
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_LUT:
			m_SurfaceMaterial->UpdateFromLut();
			GetLogicManager()->CameraUpdate();
			break;
		case ID_ENABLE_LOD:
			m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
			m_OutlineActor->SetEnableHighThreshold(m_EnableActorLOD);
			GetLogicManager()->CameraUpdate();
			break;
		case ID_USE_VTK_PROPERTY:
			if (m_UseVTKProperty != 0)
			{
				m_Actor->SetProperty(m_SurfaceMaterial->m_Prop);
			}
			else
			{
				m_Actor->SetProperty(NULL);
			}
			if (m_MaterialButton != NULL)
			{
				m_MaterialButton->Enable(m_UseVTKProperty != 0);
			}
			GetLogicManager()->CameraUpdate();
			break;
		case ID_USE_LOOKUP_TABLE:
			m_Gui->Enable(ID_LUT, m_UseLookupTable != 0);
			break;
		case ID_USE_TEXTURE:
			if (m_UseTexture)
			{
				m_Actor->SetTexture(m_Texture);
			}
			else
			{
				m_Actor->SetTexture(NULL);
			}
			m_Gui->Enable(ID_CHOOSE_TEXTURE, m_UseTexture != 0);
			m_Gui->Enable(ID_TEXTURE_MAPPING_MODE, m_UseTexture != 0);
			m_Gui->Enable(ID_TEXTURE_MAPPING_MODE, m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE && m_UseTexture != 0);
			GetLogicManager()->CameraUpdate();
			break;
		case ID_CHOOSE_TEXTURE:
		{
			albaString title = "Choose texture";
			e->SetId(VME_CHOOSE);
			e->SetPointer(&albaPipeSurfaceTextured::ImageAccept);
			e->SetString(&title);
			albaEventMacro(*e);
			albaVME *n = e->GetVme();
			if (n != NULL)
			{
				vtkImageData *image = vtkImageData::SafeDownCast(n->GetOutput()->GetVTKData());
				m_Gui->Enable(ID_USE_TEXTURE, image != NULL);
				if (image)
				{
					m_SurfaceMaterial->SetMaterialTexture(n->GetId());
					m_SurfaceMaterial->m_MaterialType = mmaMaterial::USE_TEXTURE;
            m_Texture->SetInputData(image);
					m_Actor->SetTexture(m_Texture);
					GetLogicManager()->CameraUpdate();
					m_Gui->Enable(ID_TEXTURE_MAPPING_MODE, true);
				}
			}
		}
		break;
		case ID_TEXTURE_MAPPING_MODE:
			GenerateTextureMapCoordinate();
			GetLogicManager()->CameraUpdate();
			break;
		case ID_RENDERING_DISPLAY_LIST:
			m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
			GetLogicManager()->CameraUpdate();
			break;
		default:
			albaEventMacro(*e);
			break;
		}
	}
	else if (alba_event->GetSender() == m_Vme)
	{
		if (alba_event->GetId() == VME_OUTPUT_DATA_UPDATE)
		{
			UpdateProperty();
		}
	}
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTextured::GenerateTextureMapCoordinate()
//----------------------------------------------------------------------------
{
  vtkPolyData *data = vtkPolyData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());

  if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::PLANE_MAPPING)
  {
    vtkALBASmartPointer<vtkTextureMapToPlane> plane_texture_mapper;
    plane_texture_mapper->SetInputData(data);
    plane_texture_mapper->AutomaticPlaneGenerationOn();
    vtkPolyData *tdata = (vtkPolyData *)plane_texture_mapper->GetOutput();
    m_Mapper->SetInputData(data);
  }
  else if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::CYLINDER_MAPPING)
  {
    vtkALBASmartPointer<vtkTextureMapToCylinder> cylinder_texture_mapper;
    cylinder_texture_mapper->SetInputData(data);
    cylinder_texture_mapper->PreventSeamOff();
    m_Mapper->SetInputConnection(cylinder_texture_mapper->GetOutputPort());
  }
  else if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::SPHERE_MAPPING)
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
void albaPipeSurfaceTextured::SetEnableActorLOD(bool value)
//----------------------------------------------------------------------------
{
  m_EnableActorLOD = (int) value;
  if(m_Gui)
    m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaPipeSurfaceTextured::GetBounds(double bounds[6])
{
	m_Mapper->GetBounds(bounds);
}

//----------------------------------------------------------------------------
void albaPipeSurfaceTextured::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
	m_Actor->SetPickable(enable);
  m_Actor->Modified();
	GetLogicManager()->CameraUpdate();
}
