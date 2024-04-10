/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeMIP
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

#include "albaPipeVolumeMIP.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUILutPreset.h"

#include "mmaVolumeMaterial.h"
#include "albaVME.h"
#include "albaVMEVolume.h"
#include "mmaVolumeMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkLODProp3D.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkLookupTable.h"
#include "vtkVolume.h"
#include "vtkImageResample.h"
#include "vtkImageCast.h"
#include "albaLODActor.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineSource.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkAlgorithm.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVolumeMIP);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeVolumeMIP::albaPipeVolumeMIP()
:albaPipe()
//----------------------------------------------------------------------------
{
  //m_ColorTransferFunction = NULL;
  m_OpacityTransferFunction = NULL;
  m_VolumeProperty    = NULL;
  m_MIPFunction       = NULL;
  m_VolumeMapper      = NULL;
  m_Volume	         = NULL;
  m_ResampleFactor = 0.5;
  m_ResampleFilter = NULL;

  m_ColorLUT          = NULL;
  m_Caster            = NULL;

  m_Box = NULL;
  m_Mapper = NULL;
  m_Actor =  NULL;
}
//----------------------------------------------------------------------------
void albaPipeVolumeMIP::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;

  //wxBusyCursor wait;

  // image pipeline
  double sr[2];
  m_Vme->GetOutput()->Update();

  if(m_Vme->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
  {
    wxMessageBox(_("Resample the RectilinearGrid before Visualizing in MIP View"));

    double b[6];
    m_Vme->GetOutput()->Update();
    m_Vme->GetOutput()->GetVMELocalBounds(b);

    vtkNEW(m_Box);
    m_Box->SetBounds(b);

    vtkNEW(m_Mapper);
    m_Mapper->SetInputConnection(m_Box->GetOutputPort());

    if(m_Vme->IsAnimated())
      m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
    else
      m_Mapper->ImmediateModeRenderingOff();

    vtkNEW(m_Actor);
    m_Actor->SetMapper(m_Mapper);

    m_AssemblyFront->AddPart(m_Actor);

    return;
  }

  vtkImageData *image_data = vtkImageData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  assert(image_data);
  image_data->GetScalarRange(sr);

  vtkNEW(m_ResampleFilter);
  double image_data_spacing[3];
  image_data->GetSpacing(image_data_spacing);
  m_ResampleFilter->SetInformationInput(image_data);
  m_ResampleFilter->SetInputData(image_data);
  m_ResampleFilter->SetDimensionality(3);
  m_ResampleFilter->SetOutputSpacing(image_data_spacing[0]/m_ResampleFactor,image_data_spacing[1]/m_ResampleFactor,image_data_spacing[2]/m_ResampleFactor);
  m_ResampleFilter->SetAxisMagnificationFactor(0,0.5);
  m_ResampleFilter->SetAxisMagnificationFactor(1,0.5);
  m_ResampleFilter->SetAxisMagnificationFactor(2,0.5);
  m_ResampleFilter->Update();

  vtkNEW(m_Caster);
  m_Caster->SetInputConnection(m_ResampleFilter->GetOutputPort());
  m_Caster->SetNumberOfThreads(1);
  m_Caster->SetOutputScalarType(VTK_UNSIGNED_SHORT);
  m_Caster->ClampOverflowOn();
  m_Caster->Update();


  mmaVolumeMaterial *material = ((albaVMEVolume *)m_Vme)->GetMaterial();

  m_OpacityTransferFunction = material->m_OpacityTransferFunction;
//  m_ColorTransferFunction = material->m_ColorTransferFunction;			//BES 19.2.2008

  vtkNEW(m_ColorLUT);
  m_ColorLUT->DeepCopy(material->m_ColorLut);

  UpdateMIPFromLUT();

  /*m_OpacityTransferFunction->AddPoint(sr[0],0.01);
  m_OpacityTransferFunction->AddPoint((sr[1]-sr[0])/2,0.2);
  m_OpacityTransferFunction->AddPoint(sr[1],0.3);*/

  vtkNEW(m_VolumeProperty);  
  //m_VolumeProperty->SetColor(m_ColorTransferFunction);
  m_VolumeProperty->SetScalarOpacity(m_OpacityTransferFunction);
  m_VolumeProperty->SetInterpolationTypeToLinear();

  vtkNEW(m_MIPFunction);
  m_MIPFunction->SetMaximizeMethodToOpacity();

  vtkNEW(m_VolumeMapper);
  m_VolumeMapper->SetInputConnection(m_Caster->GetOutputPort());

  m_VolumeMapper->SetVolumeRayCastFunction(m_MIPFunction);
  m_VolumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
  m_VolumeMapper->SetImageSampleDistance(1/m_ResampleFactor);
  m_VolumeMapper->SetMaximumImageSampleDistance(10);
  m_VolumeMapper->SetMinimumImageSampleDistance(1/m_ResampleFactor);
  m_VolumeMapper->SetSampleDistance(1);

  m_VolumeMapper->Update();	

  vtkNEW(m_Volume);
  m_Volume->SetMapper(m_VolumeMapper);
  m_Volume->SetProperty(m_VolumeProperty);
  m_Volume->PickableOff();

  m_AssemblyFront->AddPart(m_Volume);
}
//----------------------------------------------------------------------------
albaPipeVolumeMIP::~albaPipeVolumeMIP()
//----------------------------------------------------------------------------
{
  if(m_Volume)
    m_AssemblyFront->RemovePart(m_Volume);

  if(m_Actor)
    m_AssemblyFront->RemovePart(m_Actor);

  vtkDEL(m_Box);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);

  //BES: 2.2.2008 - vtkImageResample does not release object set by
  //SetInputSetInformationInput - fixed here
  if (m_ResampleFilter != NULL)
    m_ResampleFilter->SetInformationInput(NULL);
  vtkDEL(m_ResampleFilter);
  vtkDEL(m_VolumeProperty);
  vtkDEL(m_ColorLUT);
  vtkDEL(m_MIPFunction);
  vtkDEL(m_VolumeMapper);
  vtkDEL(m_Volume);
  vtkDEL(m_Caster);
}
//----------------------------------------------------------------------------
void albaPipeVolumeMIP::Select(bool sel)
//----------------------------------------------------------------------------
{
  m_Selected = sel;
}
//----------------------------------------------------------------------------
albaGUI *albaPipeVolumeMIP::CreateGui()
//----------------------------------------------------------------------------
{

  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);

  if(albaVMEVolume::SafeDownCast(m_Vme)->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
    return m_Gui;
  lutPreset(15,m_ColorLUT);
  m_Gui->Lut(ID_LUT_CHOOSER,_("lut"),m_ColorLUT);
  UpdateMIPFromLUT();

  m_Gui->Double(ID_RESAMPLE_FACTOR,"Resample Factor",&m_ResampleFactor,0.0,1.0,2,"Insert a value for the resampling factor");
  m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeVolumeMIP::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{	
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
    case ID_LUT_CHOOSER:
      UpdateMIPFromLUT();
      break;
    case ID_RESAMPLE_FACTOR:
      if(m_ResampleFilter!=0)
      {
        m_VolumeMapper->SetImageSampleDistance(1/m_ResampleFactor);
        m_VolumeMapper->SetMaximumImageSampleDistance(10);
        m_VolumeMapper->SetMinimumImageSampleDistance(1/m_ResampleFactor);

        m_Volume->Update();
				GetLogicManager()->CameraUpdate();
      }
      break;
    default:
      Superclass::OnEvent(alba_event);
    }
  }

}
//----------------------------------------------------------------------------
void albaPipeVolumeMIP::SetResampleFactor(double value)
//----------------------------------------------------------------------------
{
  m_ResampleFactor = value;
  if(m_ResampleFilter!=0)
  {
    m_VolumeMapper->SetImageSampleDistance(1/m_ResampleFactor);
    m_VolumeMapper->SetMaximumImageSampleDistance(10);
    m_VolumeMapper->SetMinimumImageSampleDistance(1/m_ResampleFactor);

    m_Volume->Update();
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void albaPipeVolumeMIP::UpdateMIPFromLUT()
//----------------------------------------------------------------------------
{
  if(m_Caster)
  {
    m_OpacityTransferFunction->RemoveAllPoints();
  //  m_ColorTransferFunction->RemoveAllPoints();

    int tv = m_ColorLUT->GetNumberOfTableValues();
    double rgba[4], sr[2],w,p;
    m_Caster->GetOutput()->GetScalarRange(sr);
    w = sr[1] - sr[0];
    for (int v=0;v<tv;v++)
    {
      m_ColorLUT->GetTableValue(v,rgba);
      p = v*w/tv+sr[0];

//      m_ColorTransferFunction->AddRGBPoint(p, rgba[0], rgba[1], rgba[2]);
      m_OpacityTransferFunction->AddPoint(p, (double)v/(double)tv);
    }
		GetLogicManager()->CameraUpdate();
  }

}