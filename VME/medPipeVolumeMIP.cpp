/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeMIP.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-24 14:54:40 $
  Version:   $Revision: 1.1 $
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

#include "medPipeVolumeMIP.h"

#include "mafDecl.h"

#include "mafVME.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkLODProp3D.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkOutlineCornerFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeVolumeMIP);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeVolumeMIP::medPipeVolumeMIP()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Caster            = NULL;
  m_OpacityTransferFunction = NULL;
  m_VolumeProperty    = NULL;
  m_MIPFunction       = NULL;
  m_VolumeMapper      = NULL;
  m_VolumeMapperLow   = NULL;
  m_VolumeLOD         = NULL;
  m_SelectionActor    = NULL;
}
//----------------------------------------------------------------------------
void medPipeVolumeMIP::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;

  // image pipeline
  m_Vme->GetOutput()->Update();
  vtkImageData *image_data = vtkImageData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  assert(image_data);
  image_data->Update();

  vtkNEW(m_Caster);
  m_Caster->SetInput(image_data);
  m_Caster->SetInputMemoryLimit(0);
  m_Caster->SetNumberOfThreads(1);
  m_Caster->SetOutputScalarTypeToUnsignedChar();
  m_Caster->BypassOff();
  m_Caster->ClampOverflowOff();

  vtkNEW(m_OpacityTransferFunction);
  m_OpacityTransferFunction->AddPoint(10.0, 0.0);
  m_OpacityTransferFunction->AddPoint(20.0, 1.0);

  vtkNEW(m_VolumeProperty);
  m_VolumeProperty->SetScalarOpacity(m_OpacityTransferFunction);
  m_VolumeProperty->ShadeOn();
  m_VolumeProperty->SetInterpolationTypeToLinear();

  vtkNEW(m_MIPFunction);
  m_MIPFunction->SetMaximizeMethodToScalarValue();

  vtkNEW(m_VolumeMapper);
  m_VolumeMapper->SetInput(m_Caster->GetOutput());
  m_VolumeMapper->SetVolumeRayCastFunction(m_MIPFunction);
  m_VolumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
  m_VolumeMapper->SetImageSampleDistance(1);
  m_VolumeMapper->SetMaximumImageSampleDistance(10);
  m_VolumeMapper->SetMinimumImageSampleDistance(1);
  m_VolumeMapper->SetNumberOfThreads(1);
  m_VolumeMapper->SetSampleDistance(1);
//  m_VolumeMapper->AutoAdjustSampleDistancesOn();
  //m_VolumeMapper->SetComponentBlendModeToAdd();
  //m_VolumeMapper->IndependentComponentsOn();

  vtkNEW(m_VolumeMapperLow);
  m_VolumeMapperLow->SetInput(m_Caster->GetOutput());
  m_VolumeMapperLow->SetVolumeRayCastFunction(m_MIPFunction);
  m_VolumeMapperLow->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
  m_VolumeMapperLow->SetImageSampleDistance(5);
  m_VolumeMapperLow->SetMaximumImageSampleDistance(10);
  m_VolumeMapperLow->SetMinimumImageSampleDistance(5);
  m_VolumeMapperLow->SetNumberOfThreads(1);
  m_VolumeMapperLow->SetSampleDistance(5);

  vtkNEW(m_VolumeLOD);
  m_VolumeLOD->AddLOD(m_VolumeMapperLow, m_VolumeProperty,0);
  m_VolumeLOD->AddLOD(m_VolumeMapper, m_VolumeProperty,0);
  m_VolumeLOD->PickableOff();

  m_AssemblyFront->AddPart(m_VolumeLOD);

  vtkMAFSmartPointer<vtkOutlineCornerFilter> selection_filter;
  selection_filter->SetInput(image_data);  

  vtkMAFSmartPointer<vtkPolyDataMapper> selection_papper;
  selection_papper->SetInput(selection_filter->GetOutput());

  vtkMAFSmartPointer<vtkProperty> selection_property;
  selection_property->SetColor(1,1,1);
  selection_property->SetAmbient(1);
  selection_property->SetRepresentationToWireframe();
  selection_property->SetInterpolationToFlat();

  vtkNEW(m_SelectionActor);
  m_SelectionActor->SetMapper(selection_papper);
  m_SelectionActor->VisibilityOff();
  m_SelectionActor->PickableOff();
  m_SelectionActor->SetProperty(selection_property);
  m_SelectionActor->SetScale(1.01,1.01,1.01);
}
//----------------------------------------------------------------------------
medPipeVolumeMIP::~medPipeVolumeMIP()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_VolumeLOD);
  m_AssemblyFront->RemovePart(m_SelectionActor);

  vtkDEL(m_Caster);
  vtkDEL(m_OpacityTransferFunction);
  vtkDEL(m_VolumeProperty);
  vtkDEL(m_MIPFunction);
  vtkDEL(m_VolumeMapper);
  vtkDEL(m_VolumeMapperLow);
  vtkDEL(m_VolumeLOD);
  vtkDEL(m_SelectionActor);
}
//----------------------------------------------------------------------------
void medPipeVolumeMIP::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_VolumeLOD->GetVisibility())
			m_SelectionActor->SetVisibility(sel);
}
