/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaMaterial.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-23 12:10:42 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, Paolo Quadrani
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
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkImageExport.h"

//----------------------------------------------------------------------------
mmaMaterial::mmaMaterial()
//----------------------------------------------------------------------------
{  
	m_Name        = "new material";
	m_Prop        = NULL;
  m_VolumeProp  = NULL;
  m_Icon        = NULL;

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
}
//----------------------------------------------------------------------------
mmaMaterial::~mmaMaterial()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Prop); 
  vtkDEL(m_VolumeProp); 
	cppDEL(m_Icon);
}
//----------------------------------------------------------------------------
wxBitmap *mmaMaterial::MakeIcon()
//----------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkCamera> camera;
  camera->ParallelProjectionOff();
  camera->SetViewAngle(30);
  camera->SetFocalPoint(0,0,0);
  camera->SetPosition(0,0,2);
  camera->SetViewUp(0,1,0);

	vtkMAFSmartPointer<vtkLight> light;
  light->SetPosition(-1,1,1);

  wxColour col = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DLIGHT);
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

	vtkMAFSmartPointer<vtkSphereSource> ss;
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
	
	vtkMAFSmartPointer<vtkPolyDataMapper> pdm;
	pdm->SetInput(ss->GetOutput());
	pdm->SetImmediateModeRendering(0);

	vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(pdm);
  actor->SetProperty(this->m_Prop);
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
	wxBitmap *bmp = new wxBitmap(img->ConvertToBitmap());
  delete img;

  cppDEL(this->m_Icon);

	this->m_Icon = bmp;
	return bmp;
}
//-------------------------------------------------------------------------
void mmaMaterial::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  m_Name             = ((mmaMaterial *)a)->m_Name;
  m_Value            = ((mmaMaterial *)a)->m_Value;
  m_Ambient[0]       = ((mmaMaterial *)a)->m_Ambient[0];
  m_Ambient[1]       = ((mmaMaterial *)a)->m_Ambient[1];
  m_Ambient[2]       = ((mmaMaterial *)a)->m_Ambient[2];
  m_AmbientIntensity = ((mmaMaterial *)a)->m_AmbientIntensity;
  m_Diffuse[0]       = ((mmaMaterial *)a)->m_Diffuse[0];
  m_Diffuse[1]       = ((mmaMaterial *)a)->m_Diffuse[1];
  m_Diffuse[2]       = ((mmaMaterial *)a)->m_Diffuse[2];
  m_DiffuseIntensity = ((mmaMaterial *)a)->m_DiffuseIntensity;
  m_Specular[0]      = ((mmaMaterial *)a)->m_Specular[0];
  m_Specular[1]      = ((mmaMaterial *)a)->m_Specular[1];
  m_Specular[2]      = ((mmaMaterial *)a)->m_Specular[2];
  m_SpecularIntensity= ((mmaMaterial *)a)->m_SpecularIntensity;
  m_SpecularPower    = ((mmaMaterial *)a)->m_SpecularPower;
  m_Opacity          = ((mmaMaterial *)a)->m_Opacity;
  m_Representation   = ((mmaMaterial *)a)->m_Representation;
}
//----------------------------------------------------------------------------
bool mmaMaterial::Equals(const mafAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    return (m_Name       == ((mmaMaterial *)a)->m_Name             &&
      m_Value            == ((mmaMaterial *)a)->m_Value            &&
      m_Ambient[0]       == ((mmaMaterial *)a)->m_Ambient[0]       &&
      m_Ambient[1]       == ((mmaMaterial *)a)->m_Ambient[1]       &&
      m_Ambient[2]       == ((mmaMaterial *)a)->m_Ambient[2]       &&
      m_AmbientIntensity == ((mmaMaterial *)a)->m_AmbientIntensity &&
      m_Diffuse[0]       == ((mmaMaterial *)a)->m_Diffuse[0]       &&
      m_Diffuse[1]       == ((mmaMaterial *)a)->m_Diffuse[1]       &&
      m_Diffuse[2]       == ((mmaMaterial *)a)->m_Diffuse[2]       &&
      m_DiffuseIntensity == ((mmaMaterial *)a)->m_DiffuseIntensity &&
      m_Specular[0]      == ((mmaMaterial *)a)->m_Specular[0]      &&
      m_Specular[1]      == ((mmaMaterial *)a)->m_Specular[1]      &&
      m_Specular[2]      == ((mmaMaterial *)a)->m_Specular[2]      &&
      m_SpecularIntensity== ((mmaMaterial *)a)->m_SpecularIntensity&&
      m_SpecularPower    == ((mmaMaterial *)a)->m_SpecularPower    &&
      m_Opacity          == ((mmaMaterial *)a)->m_Opacity          &&
      m_Representation   == ((mmaMaterial *)a)->m_Representation);
  }
  return false;
}
//-----------------------------------------------------------------------
int mmaMaterial::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreText("Name",m_Name.GetCStr());
    parent->StoreDouble("Value", m_Value);
    parent->StoreDouble("Ambient[0]", m_Ambient[0]);
    parent->StoreDouble("Ambient[1]", m_Ambient[1]);
    parent->StoreDouble("Ambient[2]", m_Ambient[2]);
    parent->StoreDouble("AmbientIntensity", m_AmbientIntensity);
    parent->StoreDouble("Diffuse[0]", m_Diffuse[0]);
    parent->StoreDouble("Diffuse[1]", m_Diffuse[1]);
    parent->StoreDouble("Diffuse[2]", m_Diffuse[2]);
    parent->StoreDouble("DiffuseIntensity", m_DiffuseIntensity);
    parent->StoreDouble("Specular[0]", m_Specular[0]);
    parent->StoreDouble("Specular[1]", m_Specular[1]);
    parent->StoreDouble("Specular[2]", m_Specular[2]);
    parent->StoreDouble("SpecularIntensity", m_SpecularIntensity);
    parent->StoreDouble("SpecularPower", m_SpecularPower);
    parent->StoreDouble("Opacity", m_Opacity);
    parent->StoreDouble("Representation", m_Representation);
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
    node->RestoreText("Name",m_Name);
    node->RestoreDouble("Value",m_Value);
    node->RestoreDouble("Ambient[0]", m_Ambient[0]);
    node->RestoreDouble("Ambient[1]", m_Ambient[1]);
    node->RestoreDouble("Ambient[2]", m_Ambient[2]);
    node->RestoreDouble("AmbientIntensity", m_AmbientIntensity);
    node->RestoreDouble("Diffuse[0]", m_Diffuse[0]);
    node->RestoreDouble("Diffuse[1]", m_Diffuse[1]);
    node->RestoreDouble("Diffuse[2]", m_Diffuse[2]);
    node->RestoreDouble("DiffuseIntensity", m_DiffuseIntensity);
    node->RestoreDouble("Specular[0]", m_Specular[0]);
    node->RestoreDouble("Specular[1]", m_Specular[1]);
    node->RestoreDouble("Specular[2]", m_Specular[2]);
    node->RestoreDouble("SpecularIntensity", m_SpecularIntensity);
    node->RestoreDouble("SpecularPower", m_SpecularPower);
    node->RestoreDouble("Opacity", m_Opacity);
    node->RestoreDouble("Representation", m_Representation);
    vtkNEW(m_Prop);
    m_Prop->SetAmbientColor(m_Ambient);
    m_Prop->SetAmbient(m_AmbientIntensity);
    m_Prop->SetDiffuseColor(m_Diffuse);
    m_Prop->SetDiffuse(m_DiffuseIntensity);
    m_Prop->SetSpecularColor(m_Specular);
    m_Prop->SetSpecular(m_SpecularIntensity);
    m_Prop->SetSpecularPower(m_SpecularPower);
    m_Prop->SetOpacity(m_Opacity);
    m_Prop->SetRepresentation((int)m_Representation);
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mmaMaterial::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}