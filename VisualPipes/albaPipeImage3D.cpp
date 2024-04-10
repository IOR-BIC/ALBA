/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeImage3D
 Authors: Paolo Quadrani
 
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

#include "albaPipeImage3D.h"

#include "albaDecl.h"

#include "albaVME.h"
#include "albaVMEOutputImage.h"

#include "mmaMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkPlaneSource.h"
#include "vtkTexture.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeImage3D);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeImage3D::albaPipeImage3D()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_ImagePlane        = NULL;
  m_ImageTexture      = NULL;
  m_ImageLUT          = NULL;
  m_ImageMapper       = NULL;
  m_ImageActor        = NULL;

  m_SelectionFilter   = NULL;
  m_SelectionMapper   = NULL;
  m_SelectionProperty = NULL;
  m_SelectionActor    = NULL;

  m_GhostActor        = NULL;
}
//----------------------------------------------------------------------------
void albaPipeImage3D::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  m_UsedAssembly = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

  m_ImagePlane        = NULL;
  m_ImageTexture      = NULL;
  m_ImageLUT          = NULL;
  m_ImageMapper       = NULL;
  m_ImageActor        = NULL;

  m_SelectionFilter   = NULL;
  m_SelectionMapper   = NULL;
  m_SelectionProperty = NULL;
  m_SelectionActor    = NULL;

  m_GhostActor        = NULL;

  m_Selected = false;

  // image pipeline
  m_Vme->GetOutput()->Update();
  vtkImageData *image_data = (vtkImageData *)m_Vme->GetOutput()->GetVTKData();

  double b[6];
  image_data->GetBounds(b);
  m_ImagePlane = vtkPlaneSource::New();
  m_ImagePlane->SetOrigin(b[0],b[2],b[4]);
  m_ImagePlane->SetPoint1(b[1],b[2],b[4]);
  m_ImagePlane->SetPoint2(b[0],b[3],b[4]);

  m_ImageTexture = vtkTexture::New();
  m_ImageTexture->RepeatOff();
  m_ImageTexture->InterpolateOn();
  m_ImageTexture->SetQualityTo32Bit();
  m_ImageTexture->SetInputData(image_data);
  
  if(IsGrayImage())
  {
    albaVMEOutputImage *output = (albaVMEOutputImage *)m_Vme->GetOutput();
    m_ImageLUT = output->GetMaterial()->m_ColorLut;
		double sr[2];
		output->GetVTKData()->GetScalarRange(sr);
		m_ImageLUT->SetRange(sr);
    m_ImageTexture->SetLookupTable(m_ImageLUT);
    m_ImageTexture->MapColorScalarsThroughLookupTableOn();
  }

  m_ImageTexture->Modified();

  m_ImageMapper = vtkPolyDataMapper::New();
	m_ImageMapper->SetInputConnection(m_ImagePlane->GetOutputPort());
	m_ImageMapper->ScalarVisibilityOff();
	if(m_Vme->IsAnimated())
		m_ImageMapper->ImmediateModeRenderingOn();
	else
		m_ImageMapper->ImmediateModeRenderingOff();

  m_ImageActor = vtkActor::New();
	m_ImageActor->SetMapper(m_ImageMapper);
	m_ImageActor->SetTexture(m_ImageTexture);

  m_UsedAssembly->AddPart(m_ImageActor);

  // selection highlight
	m_SelectionFilter = vtkOutlineCornerFilter::New();
	m_SelectionFilter->SetInputData(m_Vme->GetOutput()->GetVTKData());  

	m_SelectionMapper = vtkPolyDataMapper::New();
	m_SelectionMapper->SetInputConnection(m_SelectionFilter->GetOutputPort());

	m_SelectionProperty = vtkProperty::New();
	m_SelectionProperty->SetColor(1,1,1);
	m_SelectionProperty->SetAmbient(1);
	m_SelectionProperty->SetRepresentationToWireframe();
	m_SelectionProperty->SetInterpolationToFlat();

	m_SelectionActor = vtkActor::New();
	m_SelectionActor->SetMapper(m_SelectionMapper);
	m_SelectionActor->VisibilityOff();
	m_SelectionActor->PickableOff();
	m_SelectionActor->SetProperty(m_SelectionProperty);
	m_SelectionActor->SetScale(1.01,1.01,1.01);

  m_UsedAssembly->AddPart(m_SelectionActor);

	if(m_AssemblyBack)
	{
		m_GhostActor = vtkActor::New();
		m_GhostActor->SetMapper(m_ImageMapper);
		m_GhostActor->VisibilityOff();
		m_GhostActor->PickableOff();
		m_GhostActor->GetProperty()->SetOpacity(0);
		m_GhostActor->GetProperty()->SetRepresentationToPoints();
		m_GhostActor->GetProperty()->SetInterpolationToFlat();
		m_AssemblyFront->AddPart(m_GhostActor);
  }
	else
	{
		m_GhostActor = NULL;
	}
}
//----------------------------------------------------------------------------
albaPipeImage3D::~albaPipeImage3D()
//----------------------------------------------------------------------------
{
  m_UsedAssembly->RemovePart(m_ImageActor); 
  m_UsedAssembly->RemovePart(m_SelectionActor);

  if(m_GhostActor) 
    m_AssemblyFront->RemovePart(m_GhostActor);

  vtkDEL(m_ImagePlane);
  //vtkDEL(m_ImageLUT);  
  vtkDEL(m_ImageTexture);
  vtkDEL(m_ImageMapper);
  vtkDEL(m_ImageActor);
  

  vtkDEL(m_SelectionFilter);
  vtkDEL(m_SelectionMapper);
  vtkDEL(m_SelectionProperty);
  vtkDEL(m_SelectionActor);

  vtkDEL(m_GhostActor);

}
//----------------------------------------------------------------------------
void albaPipeImage3D::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_ImageActor->GetVisibility()) 
			m_SelectionActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
vtkProperty *albaPipeImage3D::GetProperty()
//----------------------------------------------------------------------------
{
  return m_ImageActor->GetProperty();
}
//----------------------------------------------------------------------------
bool albaPipeImage3D::IsGrayImage()
//----------------------------------------------------------------------------
{
  vtkImageData *image_data = (vtkImageData *)m_Vme->GetOutput()->GetVTKData();
 
  int num = image_data->GetNumberOfScalarComponents();
  if (num == 1) return true;
  
  bool grayImage = true;
  for(int i=0; i< image_data->GetNumberOfPoints(); i++)
  {
    if(!grayImage) break;

    double *value;
    value = image_data->GetPointData()->GetScalars()->GetTuple3(i);
    if(value[0] == value[1] && value[0] == value[2]);
    else
    {
      grayImage = false;
    }
  }
  return grayImage;
}