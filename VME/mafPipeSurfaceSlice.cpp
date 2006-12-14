/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurfaceSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-14 09:55:55 $
  Version:   $Revision: 1.4 $
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

#include "mafPipeSurfaceSlice.h"
#include "mafSceneNode.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mafAxes.h"
#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMELandmark.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFToLinearTransform.h"
#include "mafVMEOutputPointSet.h"
#include "mafVMEOutputLandmarkCloud.h"
#include "mafVMELandmarkCloud.h"
#include "mafEventSource.h"

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
#include "vtkFixedCutter.h"
#include "vtkPlane.h"
#include "vtkExtendedGlyph3D.h"
#include "vtkSphereSource.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeSurfaceSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeSurfaceSlice::mafPipeSurfaceSlice()
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
  m_Cutter			= NULL;
  m_Plane			= NULL;

  m_SphereSource = NULL;

  m_Origin[0] = 0;
  m_Origin[1] = 0;
  m_Origin[2] = 0;

  m_Normal[0] = 0;
  m_Normal[1] = 0;
  m_Normal[2] = 1;

  m_ScalarVisibility = 0;
  m_RenderingDisplayListFlag = 0;
  m_Border=1;
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice::Create(mafSceneNode *n/*, bool use_axes*/)
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

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface) \
         || m_Vme->GetOutput()->IsMAFType(mafVMEOutputPointSet)\
         || m_Vme->GetOutput()->IsMAFType(mafVMEOutputLandmarkCloud));
  vtkPolyData *data = NULL;
  mmaMaterial *material = NULL;
  
  if(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface))
  {
    mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
    assert(surface_output);
    surface_output->Update();
    data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
    data->Update();
    material = surface_output->GetMaterial();
  }
  else if(m_Vme->GetOutput()->IsMAFType(mafVMEOutputPointSet))
  {
    if(m_Vme->IsMAFType(mafVMELandmarkCloud))
      m_Vme->GetEventSource()->AddObserver(this);
    if (m_Vme->IsMAFType(mafVMELandmarkCloud) && !((mafVMELandmarkCloud *)m_Vme)->IsOpen())
    {
      mafVMEOutputLandmarkCloud *landmark_cloud_output = mafVMEOutputLandmarkCloud::SafeDownCast(m_Vme->GetOutput());
      assert(landmark_cloud_output);
      landmark_cloud_output->Update();
      
      vtkNEW(m_SphereSource);
      m_SphereSource->SetRadius(((mafVMELandmarkCloud *)m_Vme)->GetRadius());
      m_SphereSource->SetThetaResolution(((mafVMELandmarkCloud *)m_Vme)->GetSphereResolution());
      m_SphereSource->SetPhiResolution(((mafVMELandmarkCloud *)m_Vme)->GetSphereResolution());
      m_SphereSource->Update();

      vtkExtendedGlyph3D *glyph = vtkExtendedGlyph3D::New();
      glyph->SetSource(m_SphereSource->GetOutput());
      glyph->SetInput(landmark_cloud_output->GetVTKData());
      glyph->OrientOff();
      glyph->ScalingOff();
      glyph->ScalarVisibilityOn();
      glyph->Update();

      data = vtkPolyData::SafeDownCast(glyph->GetOutput());
      data->Update();

      material = landmark_cloud_output->GetMaterial();
    }
    else if(m_Vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud *)m_Vme)->IsOpen()){return;}
    else if(m_Vme->IsMAFType(mafVMELandmark))
    {
      mafVMEOutputPointSet *pointset_output = mafVMEOutputPointSet::SafeDownCast(m_Vme->GetOutput());
      assert(pointset_output);
      pointset_output->Update();
      
      vtkNEW(m_SphereSource);
      m_SphereSource->SetRadius(((mafVMELandmark *)m_Vme)->GetRadius());
      m_SphereSource->SetThetaResolution(((mafVMELandmark *)m_Vme)->GetSphereResolution());
      m_SphereSource->SetPhiResolution(((mafVMELandmark *)m_Vme)->GetSphereResolution());
      m_SphereSource->Update();

      vtkExtendedGlyph3D *glyph = vtkExtendedGlyph3D::New();
      glyph->SetSource(m_SphereSource->GetOutput());
      glyph->SetInput(pointset_output->GetVTKData());
      glyph->OrientOff();
      glyph->ScalingOff();
      glyph->ScalarVisibilityOn();
      glyph->Update();

      data = vtkPolyData::SafeDownCast(glyph->GetOutput());
      data->Update();

      material = pointset_output->GetMaterial();
    }
  }
  
  
  assert(data);
  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};

	m_Plane	= vtkPlane::New();
	m_Cutter = vtkFixedCutter::New();

	m_Plane->SetOrigin(m_Origin);
	m_Plane->SetNormal(m_Normal);
	vtkMAFToLinearTransform* m_VTKTransform = vtkMAFToLinearTransform::New();
  m_VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
	m_Plane->SetTransform(m_VTKTransform);

	m_Cutter->SetInput(data);
	m_Cutter->SetCutFunction(m_Plane);
	m_Cutter->Update();
  if(scalars != NULL)
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
      m_Mapper->SetInput(m_Cutter->GetOutput());
    }
  }
  else
  {
    m_Mapper->SetInput(m_Cutter->GetOutput());
  }
  if(m_Vme->GetOutput()->IsMAFType(mafVMEOutputPointSet)) m_ScalarVisibility = 0;
  m_Mapper->SetScalarVisibility(m_ScalarVisibility);
  m_Mapper->SetScalarRange(sr);
  
	if(m_Vme->IsAnimated())
  {
    m_RenderingDisplayListFlag = 1;
    m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  }
	else
  {
    m_RenderingDisplayListFlag = 0;
    m_Mapper->ImmediateModeRenderingOff();
  }

  m_Texture = vtkTexture::New();
  m_Texture->SetQualityTo32Bit();
  m_Texture->InterpolateOn();
  if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    if (material->GetMaterialTexture() != NULL)
    {
      m_Texture->SetInput(material->GetMaterialTexture());
    }
    else if (material->GetMaterialTextureID() != -1)
    {
      mafVME *texture_vme = mafVME::SafeDownCast(m_Vme->GetRoot()->FindInTreeById(material->GetMaterialTextureID()));
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
  if (material->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  {
  m_Actor->SetProperty(material->m_Prop);
  }
//  else if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
  if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    m_Actor->SetTexture(m_Texture);
  }
  m_Actor->GetProperty()->SetLineWidth (1);
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
  //CreateGui();
}
//----------------------------------------------------------------------------
mafPipeSurfaceSlice::~mafPipeSurfaceSlice()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);
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
  vtkDEL(m_Plane);
  vtkDEL(m_Cutter);
  cppDEL(m_Axes);
  vtkDEL(m_SphereSource);
	//@@@ if(m_use_axes) wxDEL(m_axes);  
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor && m_Actor->GetVisibility()) 
	{
			//m_OutlineActor->SetVisibility(sel);
      //m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
mmgGui *mafPipeSurfaceSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,_("Border"),&m_Border,1.0,5.0);
  m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
	  case ID_BORDER_CHANGE:
		  {
			  m_Actor->GetProperty()->SetLineWidth(m_Border);
			  m_Actor->Modified();
			  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
		  }
	  break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
  else if (maf_event->GetId() == mafVMELandmarkCloud::CLOUDE_OPEN_CLOSE)
  {
    if(((mafVMELandmarkCloud *)m_Vme)->IsOpen())
    {
      RemoveClosedCloudPipe();
      int num_lm = ((mafVMELandmarkCloud *)m_Vme)->GetNumberOfLandmarks();
      for (int i = 0; i < num_lm; i++)
      {
        mafVME *child_lm = ((mafVMELandmarkCloud *)m_Vme)->GetLandmark(i);
        mafEvent e(this,VME_SHOW,child_lm,true);
        //((mafVMELandmarkCloud *)m_Vme)->ForwardUpEvent(&e);
        mafEventMacro(e);
      }
    }
    else
    {
      /*int num_lm = m_Cloud->GetNumberOfLandmarks();
      for (int i = 0; i < num_lm; i++)
      {
        mafVME *child_lm = m_Cloud->GetLandmark(i);
        mafEvent e(this,VME_SHOW,child_lm,false);
        m_Cloud->ForwardUpEvent(&e);
      }*/
      CreateClosedCloudPipe();
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
  else if (maf_event->GetId() == mafVMELandmarkCloud::CLOUDE_RADIUS_MODIFIED)
  {
    if (m_SphereSource)
    {
      m_SphereSource->SetRadius(((mafVMELandmarkCloud *)m_Vme)->GetRadius());
    }
  }
  else if (maf_event->GetId() == mafVMELandmarkCloud::CLOUDE_SPHERE_RES)
  {
    if (m_SphereSource)
    {
      m_SphereSource->SetThetaResolution(((mafVMELandmarkCloud *)m_Vme)->GetSphereResolution());
      m_SphereSource->SetPhiResolution(((mafVMELandmarkCloud *)m_Vme)->GetSphereResolution());
    }
  }
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice::GenerateTextureMapCoordinate()
//----------------------------------------------------------------------------
{
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  mmaMaterial *material = surface_output->GetMaterial();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  data->Update();

  if (material->m_TextureMappingMode == mmaMaterial::PLANE_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToPlane> plane_texture_mapper;
    plane_texture_mapper->SetInput(data);
    m_Mapper->SetInput((vtkPolyData *)plane_texture_mapper->GetOutput());
  }
  else if (material->m_TextureMappingMode == mmaMaterial::CYLINDER_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToCylinder> cylinder_texture_mapper;
    cylinder_texture_mapper->SetInput(data);
    cylinder_texture_mapper->PreventSeamOff();
    m_Mapper->SetInput((vtkPolyData *)cylinder_texture_mapper->GetOutput());
  }
  else if (material->m_TextureMappingMode == mmaMaterial::SPHERE_MAPPING)
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
void mafPipeSurfaceSlice::SetSlice(double *Origin)
//----------------------------------------------------------------------------
{
	m_Origin[0] = Origin[0];
	m_Origin[1] = Origin[1];
	m_Origin[2] = Origin[2];
	
	if(m_Plane && m_Cutter)
	{
		m_Plane->SetOrigin(m_Origin);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
	}
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice::SetNormal(double *Normal)
//----------------------------------------------------------------------------
{
	m_Normal[0] = Normal[0];
	m_Normal[1] = Normal[1];
	m_Normal[2] = Normal[2];
	

	if(m_Plane && m_Cutter)
	{
		m_Plane->SetNormal(m_Normal);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
	}
}
//----------------------------------------------------------------------------
double mafPipeSurfaceSlice::GetThickness()
//----------------------------------------------------------------------------
{
	return m_Border;
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice::SetThickness(double thickness)
//----------------------------------------------------------------------------
{
	m_Border=thickness;
	m_Actor->GetProperty()->SetLineWidth(m_Border);
  m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice::CreateClosedCloudPipe()
//----------------------------------------------------------------------------
{
  mafVMEOutputLandmarkCloud *landmark_cloud_output = mafVMEOutputLandmarkCloud::SafeDownCast(m_Vme->GetOutput());
  assert(landmark_cloud_output);
  landmark_cloud_output->Update();
      
  vtkNEW(m_SphereSource);
  m_SphereSource->SetRadius(((mafVMELandmarkCloud *)m_Vme)->GetRadius());
  m_SphereSource->SetThetaResolution(((mafVMELandmarkCloud *)m_Vme)->GetSphereResolution());
  m_SphereSource->SetPhiResolution(((mafVMELandmarkCloud *)m_Vme)->GetSphereResolution());
  m_SphereSource->Update();

  vtkExtendedGlyph3D *glyph = vtkExtendedGlyph3D::New();
  glyph->SetSource(m_SphereSource->GetOutput());
  glyph->SetInput(landmark_cloud_output->GetVTKData());
  glyph->OrientOff();
  glyph->ScalingOff();
  glyph->ScalarVisibilityOn();
  glyph->Update();

  vtkPolyData *data = vtkPolyData::SafeDownCast(glyph->GetOutput());
  data->Update();

  mmaMaterial *material = landmark_cloud_output->GetMaterial();

  assert(data);
  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};

	m_Plane	= vtkPlane::New();
	m_Cutter = vtkFixedCutter::New();

	m_Plane->SetOrigin(m_Origin);
	m_Plane->SetNormal(m_Normal);
	vtkMAFToLinearTransform* m_VTKTransform = vtkMAFToLinearTransform::New();
  m_VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
	m_Plane->SetTransform(m_VTKTransform);

	m_Cutter->SetInput(data);
	m_Cutter->SetCutFunction(m_Plane);
	m_Cutter->Update();
  if(scalars != NULL)
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
      m_Mapper->SetInput(m_Cutter->GetOutput());
    }
  }
  else
  {
    m_Mapper->SetInput(m_Cutter->GetOutput());
  }
  if(m_Vme->GetOutput()->IsMAFType(mafVMEOutputPointSet)) m_ScalarVisibility = 0;
  m_Mapper->SetScalarVisibility(m_ScalarVisibility);
  m_Mapper->SetScalarRange(sr);
  
	if(m_Vme->IsAnimated())
  {
    m_RenderingDisplayListFlag = 1;
    m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  }
	else
  {
    m_RenderingDisplayListFlag = 0;
    m_Mapper->ImmediateModeRenderingOff();
  }

  m_Texture = vtkTexture::New();
  m_Texture->SetQualityTo32Bit();
  m_Texture->InterpolateOn();
  if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    if (material->GetMaterialTexture() != NULL)
    {
      m_Texture->SetInput(material->GetMaterialTexture());
    }
    else if (material->GetMaterialTextureID() != -1)
    {
      mafVME *texture_vme = mafVME::SafeDownCast(m_Vme->GetRoot()->FindInTreeById(material->GetMaterialTextureID()));
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
  if (material->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  {
  m_Actor->SetProperty(material->m_Prop);
  }
//  else if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
  if (material->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    m_Actor->SetTexture(m_Texture);
  }

  m_Actor->GetProperty()->SetLineWidth (1);
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

}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice::RemoveClosedCloudPipe()
//----------------------------------------------------------------------------
{
  if(m_Actor)
  {
    m_AssemblyFront->RemovePart(m_Actor);
  }

  m_AssemblyFront->RemovePart(m_OutlineActor);

  cppDEL(m_TextureAccept);

  vtkDEL(m_Texture);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
  vtkDEL(m_Plane);
  vtkDEL(m_Cutter);
  cppDEL(m_Axes);
}
