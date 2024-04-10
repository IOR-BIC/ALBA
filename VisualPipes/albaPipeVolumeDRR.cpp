/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeDRR
 Authors: Paolo Quadrani - porting Daniele Giunchi
 
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

#include "albaPipeVolumeDRR.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUILutPreset.h"

#include "mmaVolumeMaterial.h"
#include "albaVME.h"
#include "albaVMEVolume.h"
#include "albaSceneGraph.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkXRayVolumeMapper.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkVolume.h" 
#include "vtkProperty.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkLookupTable.h"
#include "vtkALBAVolumeResample.h"
#include "albaTransform.h"
#include "albaTransformFrame.h"
#include "albaVMEVolumeGray.h"
#include "albaTagArray.h"
#include "albaDataVector.h"
#include "albaVMEItemVTK.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkImageResample.h"
//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVolumeDRR);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeVolumeDRR::albaPipeVolumeDRR()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_OpacityTransferFunction = NULL;
  m_VolumeProperty    = NULL;
  m_Volume            = NULL;
  //m_MIPFunction       = NULL;
  m_VolumeMapper      = NULL;
  //m_VolumeMapperLow   = NULL;
//  m_VolumeLOD         = NULL;
  m_SelectionActor    = NULL;
  m_ColorLUT          = NULL;
	m_ResampleFilter		= NULL;
  m_VolumeBounds[0] = m_VolumeBounds[1] = m_VolumeBounds[2] \
    = m_VolumeBounds[3] = m_VolumeBounds[4] = m_VolumeBounds[5] = 0;

  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;

	m_ResampleFactor = 1.0;
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;

  //wxBusyCursor wait;

  // image pipeline
  m_Vme->GetOutput()->Update();
  vtkDataSet* data = m_Vme->GetOutput()->GetVTKData();

  double sr[2];
	data->GetScalarRange(sr);

  vtkNEW(m_ColorLUT);
  m_ColorLUT->SetTableRange(sr);

  //vtkNEW(m_OpacityTransferFunction);
  mmaVolumeMaterial *material = ((albaVMEVolume *)m_Vme)->GetMaterial();
  m_OpacityTransferFunction = material->m_OpacityTransferFunction;

  vtkNEW(m_VolumeProperty);
  m_VolumeProperty->SetScalarOpacity(m_OpacityTransferFunction);
  m_VolumeProperty->SetInterpolationTypeToLinear();

  /*vtkNEW(m_MIPFunction);
  m_MIPFunction->SetMaximizeMethodToOpacity();*/

	vtkNEW(m_ResampleFilter);
	vtkNEW(m_VolumeMapper);
	if(vtkImageData::SafeDownCast(data))
	{
		m_ResampleFilter->SetInputData((vtkImageData*)data);
		for(int i=0;i<3;i++)
			m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);
		m_ResampleFilter->Update();
		m_VolumeMapper->SetInput(m_ResampleFilter->GetOutput());
	}
	else
		m_VolumeMapper->SetInput(data);

  m_VolumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
  vtkNEW(m_Volume);
  m_Volume->SetMapper(m_VolumeMapper);
  m_Volume->PickableOff();
  
  m_AssemblyFront->AddPart(m_Volume);
  
  vtkALBASmartPointer<vtkOutlineCornerFilter> selection_filter;
  selection_filter->SetInputData(data);  

  vtkALBASmartPointer<vtkPolyDataMapper> selection_papper;
  selection_papper->SetInputConnection(selection_filter->GetOutputPort());

  vtkALBASmartPointer<vtkProperty> selection_property;
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

  //m_AssemblyFront->AddPart(m_SelectionActor); // commented to avoid problems on ray cast volume rendering
}
//----------------------------------------------------------------------------
albaPipeVolumeDRR::~albaPipeVolumeDRR()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_Volume);
  //m_AssemblyFront->RemovePart(m_SelectionActor);

  vtkDEL(m_ColorLUT);
//  vtkDEL(m_OpacityTransferFunction);
  vtkDEL(m_VolumeProperty);
  //vtkDEL(m_MIPFunction);
  vtkDEL(m_VolumeMapper);
  //vtkDEL(m_VolumeMapperLow);
  vtkDEL(m_Volume);
  vtkDEL(m_SelectionActor);
	vtkDEL(m_ResampleFilter);
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
  if(m_Volume == NULL) return;
	if(m_Volume->GetVisibility())
			m_SelectionActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
albaGUI *albaPipeVolumeDRR::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  //lutPreset(15,m_ColorLUT);
  //m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
  //UpdateMIPFromLUT();

	//Gui

	this->m_Gui->SetListener(this);

	m_Gui->Double(ID_RESAMPLE_FACTOR,"Resample",&m_ResampleFactor,0.00001,1);

	this->m_Gui->Label("DRR settings:", true);
	m_Gui->Color(ID_VOLUME_COLOR, "Color", &this->m_VolumeColor);
	vtkXRayVolumeMapper::GetExposureCorrection(this->m_ExposureCorrection);
	m_Gui->FloatSlider(ID_EXPOSURE_CORRECTION_L,	"Min", &this->m_ExposureCorrection[0], -1.f, 1.f);
	m_Gui->FloatSlider(ID_EXPOSURE_CORRECTION_H,	"Max", &this->m_ExposureCorrection[1], -1.f, 1.f);
	m_Gui->FloatSlider(ID_GAMMA,	"Gamma", &this->m_Gamma, 0.1f, 3.f);

	this->m_Gui->Divider(0);
	this->m_Gui->Label("Camera settings:", true);
	vtkCamera *camera = this->m_Sg->m_RenFront->GetActiveCamera();
	this->m_CameraAngle = camera->GetViewAngle();
	m_Gui->FloatSlider(ID_CAMERA_ANGLE, "View angle", &m_CameraAngle, 0.5, 45.0);
	camera->GetPosition(this->m_CameraPosition);
	m_Gui->Vector(ID_CAMERA_POSITION, "Position",	m_CameraPosition);
	camera->GetFocalPoint(this->m_CameraFocus);
	m_Gui->Vector(ID_CAMERA_FOCUS, "Focal point",	m_CameraFocus);
	this->m_CameraRoll = camera->GetRoll();
	m_Gui->FloatSlider(ID_CAMERA_ROLL, "Roll angle", &m_CameraRoll, -180., 180.0);

	this->m_Gui->Divider(0);
	//this->m_Gui->Button(ID_EXPORT, "Export Registration Settings");

	this->m_Gui->Update();

	//end Gui

  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
		albaSceneNode *node = NULL;
		vtkCamera *camera = this->m_Sg->m_RenFront->GetActiveCamera();

    switch(e->GetId()) 
    {
      case ID_LUT_CHOOSER:
      break;
			case ID_VOLUME_COLOR:
				vtkXRayVolumeMapper::SetColor(this->m_VolumeColor.Red() / 255.f, this->m_VolumeColor.Green() / 255.f, this->m_VolumeColor.Blue() / 255.f);
				GetLogicManager()->CameraUpdate();
				break;
			case ID_EXPOSURE_CORRECTION_L:
			case ID_EXPOSURE_CORRECTION_H:
				if (!vtkXRayVolumeMapper::SetExposureCorrection(this->m_ExposureCorrection))
					vtkXRayVolumeMapper::GetExposureCorrection(this->m_ExposureCorrection);
				GetLogicManager()->CameraUpdate();
				break;

			case ID_GAMMA:
				if (!vtkXRayVolumeMapper::SetGamma(this->m_Gamma))
					this->m_Gamma = vtkXRayVolumeMapper::GetGamma();
				GetLogicManager()->CameraUpdate();
				break;

			case ID_RESAMPLE_FACTOR:
				{
					for(int i=0;i<3;i++)
						m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);

					m_ResampleFilter->Update();
					GetLogicManager()->CameraUpdate();
				}
				break;
			case ID_CAMERA_ANGLE:
				camera->SetViewAngle(this->m_CameraAngle);
				GetLogicManager()->CameraUpdate();
				break;
			case ID_CAMERA_POSITION:
				camera->SetPosition(this->m_CameraPosition);
				GetLogicManager()->CameraUpdate();
				break;
			case ID_CAMERA_FOCUS:
				camera->SetFocalPoint(this->m_CameraFocus);
				GetLogicManager()->CameraUpdate();
				break;
			case ID_CAMERA_ROLL:
				camera->SetRoll(this->m_CameraRoll);
				GetLogicManager()->CameraUpdate();
				break;
			default:
				albaEventMacro(*e);
				return;
			/////
    }

		this->m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::SetColor(wxColor color)
//----------------------------------------------------------------------------
{
	m_VolumeColor=color;
	
	vtkXRayVolumeMapper::SetColor(this->m_VolumeColor.Red() / 255.f, this->m_VolumeColor.Green() / 255.f, this->m_VolumeColor.Blue() / 255.f);
	
	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::SetExposureCorrection(double value[2])
//----------------------------------------------------------------------------
{
	m_ExposureCorrection[0]=value[0];
	m_ExposureCorrection[1]=value[1];
	
	if (!vtkXRayVolumeMapper::SetExposureCorrection(this->m_ExposureCorrection))
		vtkXRayVolumeMapper::GetExposureCorrection(this->m_ExposureCorrection);
	
	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::SetGamma(double value)
//----------------------------------------------------------------------------
{
	m_Gamma=value;

	if (!vtkXRayVolumeMapper::SetGamma(this->m_Gamma))
		this->m_Gamma = vtkXRayVolumeMapper::GetGamma();

	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::SetCameraAngle(double value)
//----------------------------------------------------------------------------
{
	m_CameraAngle=value;

	vtkCamera *camera = this->m_RenFront->GetActiveCamera();
	camera->SetViewAngle(this->m_CameraAngle);

	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::SetCameraPosition(double value[3])
//----------------------------------------------------------------------------
{
	m_CameraPosition[0]=value[0];
	m_CameraPosition[1]=value[1];
	m_CameraPosition[2]=value[2];

	vtkCamera *camera = this->m_RenFront->GetActiveCamera();
	camera->SetPosition(this->m_CameraPosition);

	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::SetCameraFocus(double value[3])
//----------------------------------------------------------------------------
{
	m_CameraFocus[0]=value[0];
	m_CameraFocus[1]=value[1];
	m_CameraFocus[2]=value[2];

	vtkCamera *camera = this->m_RenFront->GetActiveCamera();
	camera->SetFocalPoint(this->m_CameraFocus);

	if (m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::SetCameraRoll(double value)
//----------------------------------------------------------------------------
{
	m_CameraRoll=value;
	
	vtkCamera *camera = this->m_RenFront->GetActiveCamera();
	camera->SetRoll(this->m_CameraRoll);

	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
double albaPipeVolumeDRR::GetResampleFactor()
//----------------------------------------------------------------------------
{
	return m_ResampleFactor;
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRR::SetResampleFactor(double value)
//----------------------------------------------------------------------------
{
	m_ResampleFactor = value;

	if(m_ResampleFilter)
	{
		for(int i=0;i<3;i++)
			m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);

		m_ResampleFilter->Update();
	}

	if(m_Gui)
		m_Gui->Update();
}