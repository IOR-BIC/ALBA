/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaMaterial.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
  Version:   $Revision: 1.8 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/settings.h>

#include "mmaMaterial.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "vtkMAFSmartPointer.h"
#include "mafStorageElement.h"
#include "mafIndent.h"

#include "mafNode.h"

#include "vtkProperty.h"
#include "vtkTransferFunction2D.h"
#include "vtkTexturedSphereSource.h"
#include "vtkTexture.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkImageExport.h"
#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmaMaterial)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmaMaterial::mmaMaterial()
//----------------------------------------------------------------------------
{  
	m_MaterialType= USE_VTK_PROPERTY;

  m_Name        = "MaterialAttributes";
  m_MaterialName= "new material";
  vtkNEW(m_ColorLut);
  vtkNEW(m_GrayLut);
  vtkNEW(m_Prop);
  m_VolumeProp  = NULL;
  m_Icon        = NULL;
  m_TextureImage= NULL;

  m_TextureMappingMode = PLANE_MAPPING;

  m_Value            = 1.0;
  m_Ambient[0]       = 1.0;
  m_Ambient[1]       = 1.0;
  m_Ambient[2]       = 1.0;
  m_AmbientIntensity = 0.0;
  wxColour color = mafRandomColor();
  m_Diffuse[0]       = color.Red()   /255.0;
  m_Diffuse[1]       = color.Green() /255.0;
  m_Diffuse[2]       = color.Blue()  /255.0;
  m_DiffuseIntensity = 1.0;
  m_Specular[0]      = 1.0;
  m_Specular[1]      = 1.0;
  m_Specular[2]      = 1.0;
  m_SpecularIntensity= 0.0;
  m_SpecularPower    = 0.0;
  m_Opacity          = 1.0;
  m_Representation   = 2.0;

  m_Level_LUT           = 0.5;
  m_Window_LUT          = 1.0;
  m_HueRange[0]         = 0.0;
  m_HueRange[1]         = 0.6667;
  m_SaturationRange[0]  = 0;
  m_SaturationRange[1]  = 1;
  m_TableRange[0]       = 0.0;
  m_TableRange[1]       = 1.0;
  m_NumValues           = 256;

  m_TextureID           = -1;

  UpdateProp();
}
//----------------------------------------------------------------------------
mmaMaterial::~mmaMaterial()
//----------------------------------------------------------------------------
{
  vtkDEL(m_ColorLut);
  vtkDEL(m_GrayLut);
	vtkDEL(m_Prop); 
  vtkDEL(m_VolumeProp); 
	cppDEL(m_Icon);
}
//----------------------------------------------------------------------------
wxBitmap *mmaMaterial::MakeIcon()
//----------------------------------------------------------------------------
{
  UpdateProp();

	vtkMAFSmartPointer<vtkCamera> camera;
  camera->ParallelProjectionOff();
  camera->SetViewAngle(30);
  camera->SetFocalPoint(0,0,0);
  camera->SetPosition(0,0,2);
  camera->SetViewUp(0,1,0);

	vtkMAFSmartPointer<vtkLight> light;
  light->SetPosition(-1,1,1);

  wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
  float r = col.Red()  / 255.0;
  float g = col.Green()/ 255.0;
  float b = col.Blue() / 255.0;
  
	vtkMAFSmartPointer<vtkRenderer> ren;
  ren->SetBackground(r,g,b);
	ren->AddLight(light);
	ren->LightFollowCameraOff();
	ren->SetActiveCamera(camera);
	
  vtkMAFSmartPointer<vtkRenderWindow> renwin;
  renwin->AddRenderer(ren);
  renwin->OffScreenRenderingOn(); 
	renwin->SetSize(25, 25);

	vtkMAFSmartPointer<vtkTexturedSphereSource> ss;
	ss->SetPhiResolution(20);
	ss->SetThetaResolution(20);
	if (this->m_Prop->GetRepresentation() == 1)
	{
		ss->SetThetaResolution(10);
		ss->SetPhiResolution(3);
	}
	else 
	{
		ss->SetThetaResolution(20);
		ss->SetPhiResolution(20);
  } 
	
	vtkMAFSmartPointer<vtkTexture> texture;
  if (m_MaterialType == USE_TEXTURE)
  {
    texture->SetInput(m_TextureImage);
  }
  
  vtkMAFSmartPointer<vtkPolyDataMapper> pdm;
	pdm->SetInput(ss->GetOutput());
	pdm->SetImmediateModeRendering(0);

	vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(pdm);
  if (m_MaterialType == USE_VTK_PROPERTY)
  {
    actor->SetProperty(this->m_Prop);
  }
  else if (m_MaterialType == USE_TEXTURE)
  {
    actor->SetTexture(texture);
  }
  actor->SetPickable(1);
  actor->SetVisibility(1);
  actor->SetPosition(0,0,0); 
  actor->SetScale(1,1,1); 
	ren->AddActor(actor);
	
  renwin->Render();

  //capture image from renderer
	vtkMAFSmartPointer<vtkWindowToImageFilter> w2i;
	w2i->SetInput(renwin);
	w2i->Update();

  int dim[3];
	w2i->GetOutput()->GetDimensions(dim);
  assert( dim[0]==25 && dim[1]==25 );
	unsigned char buffer[25*25*3];

  //flip it - windows Bitmap are upside-down
  vtkMAFSmartPointer<vtkImageExport> ie;
	ie->SetInput(w2i->GetOutput());
  ie->ImageLowerLeftOff();
  ie->SetExportVoidPointer(buffer);
	ie->Export();

	//translate to a wxBitmap
	wxImage  *img = new wxImage(dim[0],dim[1],buffer,TRUE);
	wxBitmap *bmp = new wxBitmap(img);
  delete img;

  cppDEL(m_Icon);

	m_Icon = bmp;
	return bmp;
}
//-------------------------------------------------------------------------
void mmaMaterial::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  // property
  m_MaterialName        = ((mmaMaterial *)a)->m_MaterialName;
  m_Value               = ((mmaMaterial *)a)->m_Value;
  m_Ambient[0]          = ((mmaMaterial *)a)->m_Ambient[0];
  m_Ambient[1]          = ((mmaMaterial *)a)->m_Ambient[1];
  m_Ambient[2]          = ((mmaMaterial *)a)->m_Ambient[2];
  m_AmbientIntensity    = ((mmaMaterial *)a)->m_AmbientIntensity;
  m_Diffuse[0]          = ((mmaMaterial *)a)->m_Diffuse[0];
  m_Diffuse[1]          = ((mmaMaterial *)a)->m_Diffuse[1];
  m_Diffuse[2]          = ((mmaMaterial *)a)->m_Diffuse[2];
  m_DiffuseIntensity    = ((mmaMaterial *)a)->m_DiffuseIntensity;
  m_Specular[0]         = ((mmaMaterial *)a)->m_Specular[0];
  m_Specular[1]         = ((mmaMaterial *)a)->m_Specular[1];
  m_Specular[2]         = ((mmaMaterial *)a)->m_Specular[2];
  m_SpecularIntensity   = ((mmaMaterial *)a)->m_SpecularIntensity;
  m_SpecularPower       = ((mmaMaterial *)a)->m_SpecularPower;
  m_Opacity             = ((mmaMaterial *)a)->m_Opacity;
  m_Representation      = ((mmaMaterial *)a)->m_Representation;
  // texture
  m_TextureID           = ((mmaMaterial *)a)->m_TextureID;
  m_TextureMappingMode  = ((mmaMaterial *)a)->m_TextureMappingMode;
  // lut
  m_Level_LUT           = ((mmaMaterial *)a)->m_Level_LUT;
  m_Window_LUT          = ((mmaMaterial *)a)->m_Window_LUT;
  m_HueRange[0]         = ((mmaMaterial *)a)->m_HueRange[0];
  m_HueRange[1]         = ((mmaMaterial *)a)->m_HueRange[1];
  m_SaturationRange[0]  = ((mmaMaterial *)a)->m_SaturationRange[0];
  m_SaturationRange[1]  = ((mmaMaterial *)a)->m_SaturationRange[1];
  m_TableRange[0]       = ((mmaMaterial *)a)->m_TableRange[0];
  m_TableRange[1]       = ((mmaMaterial *)a)->m_TableRange[1];
  m_NumValues           = ((mmaMaterial *)a)->m_NumValues;
  
  m_MaterialType        = ((mmaMaterial *)a)->m_MaterialType;
}
//----------------------------------------------------------------------------
bool mmaMaterial::Equals(const mafAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    return (m_MaterialName  == ((mmaMaterial *)a)->m_MaterialName       &&
      m_Value               == ((mmaMaterial *)a)->m_Value              &&
      m_Ambient[0]          == ((mmaMaterial *)a)->m_Ambient[0]         &&
      m_Ambient[1]          == ((mmaMaterial *)a)->m_Ambient[1]         &&
      m_Ambient[2]          == ((mmaMaterial *)a)->m_Ambient[2]         &&
      m_AmbientIntensity    == ((mmaMaterial *)a)->m_AmbientIntensity   &&
      m_Diffuse[0]          == ((mmaMaterial *)a)->m_Diffuse[0]         &&
      m_Diffuse[1]          == ((mmaMaterial *)a)->m_Diffuse[1]         &&
      m_Diffuse[2]          == ((mmaMaterial *)a)->m_Diffuse[2]         &&
      m_DiffuseIntensity    == ((mmaMaterial *)a)->m_DiffuseIntensity   &&
      m_Specular[0]         == ((mmaMaterial *)a)->m_Specular[0]        &&
      m_Specular[1]         == ((mmaMaterial *)a)->m_Specular[1]        &&
      m_Specular[2]         == ((mmaMaterial *)a)->m_Specular[2]        &&
      m_SpecularIntensity   == ((mmaMaterial *)a)->m_SpecularIntensity  &&
      m_SpecularPower       == ((mmaMaterial *)a)->m_SpecularPower      &&
      m_Opacity             == ((mmaMaterial *)a)->m_Opacity            &&
      m_TextureID           == ((mmaMaterial *)a)->m_TextureID          &&
      m_Level_LUT           == ((mmaMaterial *)a)->m_Level_LUT          &&
      m_Window_LUT          == ((mmaMaterial *)a)->m_Window_LUT         &&
      m_HueRange[0]         == ((mmaMaterial *)a)->m_HueRange[0]        &&
      m_HueRange[1]         == ((mmaMaterial *)a)->m_HueRange[1]        &&
      m_SaturationRange[0]  == ((mmaMaterial *)a)->m_SaturationRange[0] &&
      m_SaturationRange[1]  == ((mmaMaterial *)a)->m_SaturationRange[1] &&
      m_TableRange[0]       == ((mmaMaterial *)a)->m_TableRange[0]      &&
      m_TableRange[1]       == ((mmaMaterial *)a)->m_TableRange[1]      &&
      m_NumValues           == ((mmaMaterial *)a)->m_NumValues          &&
      m_Representation      == ((mmaMaterial *)a)->m_Representation     &&
      m_MaterialType        == ((mmaMaterial *)a)->m_MaterialType       &&
      m_TextureMappingMode  == ((mmaMaterial *)a)->m_TextureMappingMode);
  }
  return false;
}
//-----------------------------------------------------------------------
int mmaMaterial::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    // property
    parent->StoreText("MaterialName",m_MaterialName.GetCStr());
    parent->StoreDouble("Value", m_Value);
    parent->StoreDouble("Ambient0", m_Ambient[0]);
    parent->StoreDouble("Ambient1", m_Ambient[1]);
    parent->StoreDouble("Ambient2", m_Ambient[2]);
    parent->StoreDouble("AmbientIntensity", m_AmbientIntensity);
    parent->StoreDouble("Diffuse0", m_Diffuse[0]);
    parent->StoreDouble("Diffuse1", m_Diffuse[1]);
    parent->StoreDouble("Diffuse2", m_Diffuse[2]);
    parent->StoreDouble("DiffuseIntensity", m_DiffuseIntensity);
    parent->StoreDouble("Specular0", m_Specular[0]);
    parent->StoreDouble("Specular1", m_Specular[1]);
    parent->StoreDouble("Specular2", m_Specular[2]);
    parent->StoreDouble("SpecularIntensity", m_SpecularIntensity);
    parent->StoreDouble("SpecularPower", m_SpecularPower);
    parent->StoreDouble("Opacity", m_Opacity);
    parent->StoreDouble("Representation", m_Representation);
    if (m_MaterialType == USE_LOOKUPTABLE)
    {
      // lut
      parent->StoreDouble("Level_LUT", m_Level_LUT);
      parent->StoreDouble("Window_LUT", m_Window_LUT);
      parent->StoreDouble("HueRange0", m_HueRange[0]);
      parent->StoreDouble("HueRange1", m_HueRange[1]);
      parent->StoreDouble("SaturationRange0", m_SaturationRange[0]);
      parent->StoreDouble("SaturationRange1", m_SaturationRange[1]);
      parent->StoreDouble("TableRange0", m_TableRange[0]);
      parent->StoreDouble("TableRange1", m_TableRange[1]);
      parent->StoreInteger("NumValues", m_NumValues);
      mafString lutvalues;
      lutvalues = "LUT_VALUE_#";
      for (int v = 0; v < m_NumValues; v++)
      {
        lutvalues << v;
        double *rgba = m_ColorLut->GetTableValue(v);
        parent->StoreVectorN(lutvalues.GetCStr(),rgba,4);
      }
    }
    else if (m_MaterialType == USE_TEXTURE)
    {
      // texture
      parent->StoreInteger("TextureID", m_TextureID);
      parent->StoreInteger("TextureMappingMode", m_TextureMappingMode);
    }
    parent->StoreInteger("MaterialType", m_MaterialType);
    return MAF_OK;
  }
  return MAF_ERROR;
}
//----------------------------------------------------------------------------
int mmaMaterial::InternalRestore(mafStorageElement *node)
//----------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    // property
    node->RestoreText("MaterialName",m_MaterialName);
    node->RestoreDouble("Value",m_Value);
    node->RestoreDouble("Ambient0", m_Ambient[0]);
    node->RestoreDouble("Ambient1", m_Ambient[1]);
    node->RestoreDouble("Ambient2", m_Ambient[2]);
    node->RestoreDouble("AmbientIntensity", m_AmbientIntensity);
    node->RestoreDouble("Diffuse0", m_Diffuse[0]);
    node->RestoreDouble("Diffuse1", m_Diffuse[1]);
    node->RestoreDouble("Diffuse2", m_Diffuse[2]);
    node->RestoreDouble("DiffuseIntensity", m_DiffuseIntensity);
    node->RestoreDouble("Specular0", m_Specular[0]);
    node->RestoreDouble("Specular1", m_Specular[1]);
    node->RestoreDouble("Specular2", m_Specular[2]);
    node->RestoreDouble("SpecularIntensity", m_SpecularIntensity);
    node->RestoreDouble("SpecularPower", m_SpecularPower);
    node->RestoreDouble("Opacity", m_Opacity);
    node->RestoreDouble("Representation", m_Representation);
    node->RestoreInteger("MaterialType", m_MaterialType);
    if (m_MaterialType == USE_LOOKUPTABLE)
    {
      // lut
      node->RestoreDouble("Level_LUT", m_Level_LUT);
      node->RestoreDouble("Window_LUT", m_Window_LUT);
      node->RestoreDouble("HueRange0", m_HueRange[0]);
      node->RestoreDouble("HueRange1", m_HueRange[1]);
      node->RestoreDouble("SaturationRange0", m_SaturationRange[0]);
      node->RestoreDouble("SaturationRange1", m_SaturationRange[1]);
      node->RestoreDouble("TableRange0", m_TableRange[0]);
      node->RestoreDouble("TableRange1", m_TableRange[1]);
      node->RestoreInteger("NumValues", m_NumValues);
      m_ColorLut->SetNumberOfTableValues(m_NumValues);
      mafString lutvalues;
      lutvalues = "LUT_VALUE_#";
      for (int v = 0; v < m_NumValues; v++)
      {
        lutvalues << v;
        double rgba[4];
        node->RestoreVectorN(lutvalues,rgba,4);
        m_ColorLut->SetTableValue(v,rgba);
      }
    }
    else if (m_MaterialType == USE_TEXTURE)
    {
      // texture
      node->RestoreInteger("TextureID", m_TextureID);
      node->RestoreInteger("TextureMappingMode", m_TextureMappingMode);
    }
    UpdateProp();
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mmaMaterial::UpdateProp()
//-----------------------------------------------------------------------
{
  m_Prop->SetAmbientColor(m_Ambient);
  m_Prop->SetAmbient(m_AmbientIntensity);
  m_Prop->SetDiffuseColor(m_Diffuse);
  m_Prop->SetDiffuse(m_DiffuseIntensity);
  m_Prop->SetSpecularColor(m_Specular);
  m_Prop->SetSpecular(m_SpecularIntensity);
  m_Prop->SetSpecularPower(m_SpecularPower);
  m_Prop->SetOpacity(m_Opacity);
  m_Prop->SetRepresentation((int)m_Representation);
  m_Prop->Modified();

  m_GrayLut->SetLevel(m_Level_LUT);
  m_GrayLut->SetWindow(m_Window_LUT);
  m_GrayLut->Build();

  m_ColorLut->SetHueRange(m_HueRange);
  m_ColorLut->SetSaturationRange(m_SaturationRange);
  m_ColorLut->SetNumberOfTableValues(m_NumValues);
  m_ColorLut->SetTableRange(m_TableRange);
  m_ColorLut->Build();
}
//-----------------------------------------------------------------------
void mmaMaterial::SetMaterialTexture(vtkImageData *tex)
//-----------------------------------------------------------------------
{
  m_TextureImage  = tex;
  m_TextureID     = -1;
}
//-----------------------------------------------------------------------
void mmaMaterial::SetMaterialTexture(int tex_id)
//-----------------------------------------------------------------------
{
  m_TextureID     = tex_id;
  m_TextureImage  = NULL;
}
//-----------------------------------------------------------------------
vtkImageData *mmaMaterial::GetMaterialTexture()
//-----------------------------------------------------------------------
{
  return m_TextureImage;
}
//-----------------------------------------------------------------------
int mmaMaterial::GetMaterialTextureID()
//-----------------------------------------------------------------------
{
  return m_TextureID;
}
//-----------------------------------------------------------------------
void mmaMaterial::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}
