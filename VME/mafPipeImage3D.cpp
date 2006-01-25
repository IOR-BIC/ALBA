/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeImage3D.cpp,v $
  Language:  C++
  Date:      $Date: 2006-01-25 12:04:08 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
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

#include "mafPipeImage3D.h"

#include "mafDecl.h"

#include "mafVME.h"

#include "vtkMAFAssembly.h"
#include "vtkPlaneSource.h"
#include "vtkTexture.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "vtkOutlineCornerFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeImage3D);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeImage3D::mafPipeImage3D()
:mafPipe()
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
void mafPipeImage3D::Create(mafSceneNode *n)
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
  image_data->Update();

  double b[6];
  image_data->GetBounds(b);
  m_ImagePlane = vtkPlaneSource::New();
  m_ImagePlane->SetOrigin(b[0],b[2],b[4]);
  m_ImagePlane->SetPoint1(b[1],b[2],b[4]);
  m_ImagePlane->SetPoint2(b[0],b[3],b[4]);

  double w,l, range[2];
  image_data->GetScalarRange(range);
  w = range[1] - range[0];
  l = (range[1] + range[0]) * .5;

  m_ImageLUT = vtkWindowLevelLookupTable::New();
  m_ImageLUT->SetWindow(w);
  m_ImageLUT->SetLevel(l);
  //m_ImageLUT->InverseVideoOn();
  m_ImageLUT->Build();

  m_ImageTexture = vtkTexture::New();
  m_ImageTexture->RepeatOff();
  m_ImageTexture->InterpolateOn();
  m_ImageTexture->SetQualityTo32Bit();
  m_ImageTexture->SetInput(image_data);
  int num = image_data->GetNumberOfScalarComponents();
  if (num == 1)
  {
    m_ImageTexture->SetLookupTable(m_ImageLUT);
    m_ImageTexture->MapColorScalarsThroughLookupTableOn();
  }
  m_ImageTexture->Modified();

  m_ImageMapper = vtkPolyDataMapper::New();
	m_ImageMapper->SetInput(m_ImagePlane->GetOutput());
	m_ImageMapper->ScalarVisibilityOff();
	if(m_Vme->IsAnimated())
		m_ImageMapper->ImmediateModeRenderingOn();
	else
		m_ImageMapper->ImmediateModeRenderingOff();

  m_ImageActor = vtkActor::New();
	m_ImageActor->SetMapper(m_ImageMapper);
	m_ImageActor->SetTexture(m_ImageTexture);

  m_UsedAssembly->AddPart(m_ImageActor);

  // selection hilight
	m_SelectionFilter = vtkOutlineCornerFilter::New();
	m_SelectionFilter->SetInput(m_Vme->GetOutput()->GetVTKData());  

	m_SelectionMapper = vtkPolyDataMapper::New();
	m_SelectionMapper->SetInput(m_SelectionFilter->GetOutput());

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
mafPipeImage3D::~mafPipeImage3D()
//----------------------------------------------------------------------------
{
  m_UsedAssembly->RemovePart(m_ImageActor); 
  m_UsedAssembly->RemovePart(m_SelectionActor);

  if(m_GhostActor) 
    m_AssemblyFront->RemovePart(m_GhostActor);

  vtkDEL(m_ImagePlane);
  vtkDEL(m_ImageLUT);
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
void mafPipeImage3D::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_ImageActor->GetVisibility()) 
			m_SelectionActor->SetVisibility(sel);
}
