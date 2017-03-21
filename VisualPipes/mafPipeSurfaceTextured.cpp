/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceTextured
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include "mafPipeSurfaceTextured.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
#include "mafGUIMaterialButton.h"
#include "mafAxes.h"
#include "mmaMaterial.h"

#include "mafDataVector.h"
#include "mafVMESurface.h"
#include "mafVMEGenericAbstract.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
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
#include "mafLODActor.h"
#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkActor.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeSurfaceTextured);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeSurfaceTextured::mafPipeSurfaceTextured()
:mafPipe()
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
void mafPipeSurfaceTextured::Create(mafSceneNode *n/*, bool use_axes*/)
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
  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface));
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  assert(data);
  data->Update();

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
      m_Mapper->SetInput(data);
    }
  }
  else
  {
    m_Mapper->SetInput(data);
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
      m_Texture->SetInput(image);
      image->GetScalarRange(sr);
    }
    else if (m_SurfaceMaterial->GetMaterialTextureID() != -1)
    {
      mafVME *texture_vme = m_Vme->GetRoot()->FindInTreeById(m_SurfaceMaterial->GetMaterialTextureID());
      texture_vme->GetOutput()->GetVTKData()->Update();
      vtkImageData *image = (vtkImageData *)texture_vme->GetOutput()->GetVTKData();
      m_Texture->SetInput(image);
      image->GetScalarRange(sr);
    }
    else
    {
      mafLogMessage(_("texture info not correctly stored inside material!!"));
    }
  }

  m_Mapper->SetScalarRange(sr);

  vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);
  m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
  
  if (m_EnableActorLOD == false)
  {
	  // bug 2454 fix
	  // http://bugzilla.b3c.it/show_bug.cgi?id=2454
	  // small LOD rectangle is activated even if LOD is disabled
	  // Setting PixelThreshold to 1 will deactivate small square visualization 
	  // ie entering
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
	  // code section in mafLODActor.cpp
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
  vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(data);  

  vtkMAFSmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInput(corner->GetOutput());

  vtkMAFSmartPointer<vtkProperty> corner_props;
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
      m_Axes = new mafAxes(m_RenFront, m_Vme);
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
mafPipeSurfaceTextured::~mafPipeSurfaceTextured()
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
void mafPipeSurfaceTextured::Select(bool sel)
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
void mafPipeSurfaceTextured::UpdateProperty(bool fromTag)
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
mafGUI *mafPipeSurfaceTextured::CreateGui()
//----------------------------------------------------------------------------
{
  wxString mapping_mode[3] = {"Plane", "Cylinder","Sphere"};

  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->Bool(ID_RENDERING_DISPLAY_LIST,"displaylist",&m_RenderingDisplayListFlag,0,"turn on/off \nrendering displaylist calculation");
  m_Gui->Bool(ID_SCALAR_VISIBILITY,"scalar vis.", &m_ScalarVisibility,0,"turn on/off the scalar visibility");
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"property",&m_UseVTKProperty);
  m_MaterialButton = new mafGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_TEXTURE,"texture",&m_UseTexture);
  m_Gui->Button(ID_CHOOSE_TEXTURE,"texture");
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  //m_Gui->Combo(ID_TEXTURE_MAPPING_MODE,"mapping",&surface_output->GetMaterial()->m_TextureMappingMode,3,mapping_mode);
  m_Gui->Enable(ID_CHOOSE_TEXTURE,m_UseTexture != 0);
  m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,m_UseTexture != 0);
  m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE && m_UseTexture != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_LOOKUP_TABLE,"lut",&m_UseLookupTable);
  m_Gui->Lut(ID_LUT,"lut",m_SurfaceMaterial->m_ColorLut);
  m_Gui->Enable(ID_LUT,m_UseLookupTable != 0);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_ENABLE_LOD,"LOD",&m_EnableActorLOD);
  m_Gui->Label("");

  if (m_SurfaceMaterial == NULL)
  {
    mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
    m_SurfaceMaterial = surface_output->GetMaterial();
  }
  bool texture_falg = m_SurfaceMaterial->GetMaterialTexture() != NULL || m_SurfaceMaterial->GetMaterialTextureID() != -1;
  m_Gui->Enable(ID_USE_TEXTURE, texture_falg);
  m_Gui->Enable(ID_USE_LOOKUP_TABLE, m_SurfaceMaterial->m_ColorLut != NULL);
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
			mafString title = "Choose texture";
			e->SetId(VME_CHOOSE);
			e->SetArg((long)&mafPipeSurfaceTextured::ImageAccept);
			e->SetString(&title);
			mafEventMacro(*e);
			mafVME *n = e->GetVme();
			if (n != NULL)
			{
				vtkImageData *image = vtkImageData::SafeDownCast(n->GetOutput()->GetVTKData());
				m_Gui->Enable(ID_USE_TEXTURE, image != NULL);
				if (image)
				{
					image->Update();
					m_SurfaceMaterial->SetMaterialTexture(n->GetId());
					m_SurfaceMaterial->m_MaterialType = mmaMaterial::USE_TEXTURE;
					m_Texture->SetInput(image);
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
			mafEventMacro(*e);
			break;
		}
	}
	else if (maf_event->GetSender() == m_Vme)
	{
		if (maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
		{
			UpdateProperty();
		}
	}
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::GenerateTextureMapCoordinate()
//----------------------------------------------------------------------------
{
  vtkPolyData *data = vtkPolyData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  data->Update();

  if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::PLANE_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToPlane> plane_texture_mapper;
    plane_texture_mapper->SetInput(data);
    plane_texture_mapper->AutomaticPlaneGenerationOn();
    vtkPolyData *tdata = (vtkPolyData *)plane_texture_mapper->GetOutput();
    m_Mapper->SetInput(data);
  }
  else if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::CYLINDER_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToCylinder> cylinder_texture_mapper;
    cylinder_texture_mapper->SetInput(data);
    cylinder_texture_mapper->PreventSeamOff();
    m_Mapper->SetInput((vtkPolyData *)cylinder_texture_mapper->GetOutput());
  }
  else if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::SPHERE_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToSphere> sphere_texture_mapper;
    sphere_texture_mapper->SetInput(data);
    sphere_texture_mapper->PreventSeamOff();
    m_Mapper->SetInput((vtkPolyData *)sphere_texture_mapper->GetOutput());
  }
  else
  {
    m_Mapper->SetInput(data);
  }
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::SetEnableActorLOD(bool value)
//----------------------------------------------------------------------------
{
  m_EnableActorLOD = (int) value;
  if(m_Gui)
    m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
	m_Actor->SetPickable(enable);
  m_Actor->Modified();
	GetLogicManager()->CameraUpdate();
}
