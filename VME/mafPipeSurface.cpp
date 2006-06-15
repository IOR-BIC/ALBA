/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-15 17:30:12 $
  Version:   $Revision: 1.24 $
  Authors:   Silvano Imboden - Paolo Quadrani
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

#include "mafPipeSurface.h"
#include "mafSceneNode.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mafAxes.h"
#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
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

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeSurface::mafPipeSurface()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Texture         = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_MaterialButton  = NULL;
  m_TextureAccept   = NULL;
  m_SurfaceMaterial = NULL;

  m_ScalarVisibility = 0;
  m_RenderingDisplayListFlag = 0;

  m_UseVTKProperty  = 1;
  m_UseTexture      = 0;
  m_UseLookupTable  = 0;
}
//----------------------------------------------------------------------------
void mafPipeSurface::Create(mafSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;
  m_Texture         = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface));
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  assert(data);
  data->Update();
  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};
  if(scalars != NULL)
  {
    m_ScalarVisibility = 1;
    scalars->GetRange(sr);
  }

  m_SurfaceMaterial = surface_output->GetMaterial();
  assert(m_SurfaceMaterial);  // all vme that use PipeSurface must have the material correctly set

  m_Mapper = vtkPolyDataMapper::New();

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
  
  m_Mapper->SetScalarVisibility(m_ScalarVisibility);
  m_Mapper->SetScalarRange(sr);
  
  m_RenderingDisplayListFlag = m_Vme->IsAnimated() ? 1 : 0;
  m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);

  m_Texture = vtkTexture::New();
  m_Texture->SetQualityTo32Bit();
  m_Texture->InterpolateOn();
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    if (m_SurfaceMaterial->GetMaterialTexture() != NULL)
    {
      m_Texture->SetInput(m_SurfaceMaterial->GetMaterialTexture());
    }
    else if (m_SurfaceMaterial->GetMaterialTextureID() != -1)
    {
      mafVME *texture_vme = mafVME::SafeDownCast(m_Vme->GetRoot()->FindInTreeById(m_SurfaceMaterial->GetMaterialTextureID()));
      texture_vme->GetOutput()->GetVTKData()->Update();
      m_Texture->SetInput((vtkImageData *)texture_vme->GetOutput()->GetVTKData());
    }
    else
    {
      mafErrorMacro("texture info not correctly set inside material!! ");
    }
  }

  m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);
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
    m_Actor->SetTexture(m_Texture);
  }

  if (m_Gui)
  {
    m_Gui->Update();
  }

  m_AssemblyFront->AddPart(m_Actor);

  // selection highlight
	m_OutlineBox = vtkOutlineCornerFilter::New();
	m_OutlineBox->SetInput(data);  

	m_OutlineMapper = vtkPolyDataMapper::New();
	m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

	m_OutlineProperty = vtkProperty::New();
	m_OutlineProperty->SetColor(1,1,1);
	m_OutlineProperty->SetAmbient(1);
	m_OutlineProperty->SetRepresentationToWireframe();
	m_OutlineProperty->SetInterpolationToFlat();

	m_OutlineActor = vtkActor::New();
	m_OutlineActor->SetMapper(m_OutlineMapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(m_OutlineProperty);

  m_AssemblyFront->AddPart(m_OutlineActor);

  m_Axes = new mafAxes(m_RenFront, m_Vme);
  m_Axes->SetVisibility(0);

  /*
  m_axes = NULL;
	if(m_use_axes) m_axes = new mafAxes(m_ren1,m_Vme);
	if(m_use_axes) m_axes->SetVisibility(0);
	*/
}
//----------------------------------------------------------------------------
mafPipeSurface::~mafPipeSurface()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

  cppDEL(m_TextureAccept);

  vtkDEL(m_Texture);
	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
  cppDEL(m_Axes);
}
//----------------------------------------------------------------------------
void mafPipeSurface::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
    m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeSurface::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
	/*
	if(fromTag)
  {
		((mafVmeData *)m_Vme->GetClientData())->UpdateFromTag();
    int idx = m_Vme->GetTagArray()->FindTag("VME_CENTER_ROTATION_POSE");
    vtkTagItem *item = NULL;
    double vec[16];
    if (idx != -1)
    {
      item = m_Vme->GetTagArray()->GetTag(idx);
      mflSmartPointer<vtkMatrix4x4> pose;
      for (int el=0;el<16;el++)
      {
        vec[el] = item->GetValueAsDouble(el);
      }
      pose->DeepCopy(vec);
      m_axes->SetPose(pose);
    }
    else
      m_axes->SetPose();
  }
  else
	  m_Mapper->SetScalarVisibility(((mafVmeData *)m_Vme->GetClientData())->GetColorByScalar());
	*/
}
//----------------------------------------------------------------------------
mmgGui *mafPipeSurface::CreateGui()
//----------------------------------------------------------------------------
{
  wxString mapping_mode[3] = {"Plane", "Cylinder","Sphere"};
  m_TextureAccept = new mafTextureAccept();

  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_SCALAR_VISIBILITY,"scalar vis.", &m_ScalarVisibility,0,"turn on/off the scalar visibility");
  m_Gui->Divider();
  m_MaterialButton = new mmgMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_Gui->Bool(ID_RENDERING_DISPLAY_LIST,"displaylist",&m_RenderingDisplayListFlag,0,"turn on/off \nrendering displaylist calculation");
  m_Gui->Button(ID_CHOOSE_TEXTURE,"texture");
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  m_Gui->Combo(ID_TEXTURE_MAPPING_MODE,"mapping",&surface_output->GetMaterial()->m_TextureMappingMode,3,mapping_mode);
  m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,surface_output->GetMaterial()->m_MaterialType == mmaMaterial::USE_TEXTURE);
  m_Gui->Label(""); //SIL. 18-may-2006 : 
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"property",&m_UseVTKProperty);
  m_Gui->Bool(ID_USE_TEXTURE,"texture",&m_UseTexture);
  m_Gui->Bool(ID_USE_LOOKUP_TABLE,"lut",&m_UseLookupTable);

  if (m_SurfaceMaterial == NULL)
  {
    mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
    m_SurfaceMaterial = surface_output->GetMaterial();
  }
  bool texture_falg = m_SurfaceMaterial->GetMaterialTexture() != NULL || m_SurfaceMaterial->GetMaterialTextureID() != -1;
  m_Gui->Enable(ID_USE_TEXTURE, texture_falg);
  m_Gui->Enable(ID_USE_LOOKUP_TABLE, m_SurfaceMaterial->m_ColorLut != NULL);
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
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
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
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
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_USE_LOOKUP_TABLE:
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
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_CHOOSE_TEXTURE:
      {
        mafString title = "Choose texture";
        e->SetId(VME_CHOOSE);
        e->SetArg((long)m_TextureAccept);
        e->SetString(&title);
        mafEventMacro(*e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
          vtkImageData *image = vtkImageData::SafeDownCast(((mafVME *)n)->GetOutput()->GetVTKData());
          m_Gui->Enable(ID_USE_TEXTURE,image != NULL);
          if (image)
          {
            image->Update();
            m_SurfaceMaterial->SetMaterialTexture(n->GetId());
            m_SurfaceMaterial->m_MaterialType = mmaMaterial::USE_TEXTURE;
            m_Texture->SetInput(image);
            m_Actor->SetTexture(m_Texture);
            mafEventMacro(mafEvent(this,CAMERA_UPDATE));
            m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,true);
          }
        }
      }
      break;
      case ID_TEXTURE_MAPPING_MODE:
        GenerateTextureMapCoordinate();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_RENDERING_DISPLAY_LIST:
        m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafPipeSurface::GenerateTextureMapCoordinate()
//----------------------------------------------------------------------------
{
  vtkPolyData *data = vtkPolyData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  data->Update();

  if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::PLANE_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToPlane> plane_texture_mapper;
    plane_texture_mapper->SetInput(data);
    m_Mapper->SetInput((vtkPolyData *)plane_texture_mapper->GetOutput());
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
