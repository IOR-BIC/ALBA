/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurfaceGPU
 Authors: Alexander Savenko  -  Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#if defined(WIN32)
#pragma warning (disable : 4018)
#endif

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaPipeIsosurfaceGPU.h"
#include "albaEvent.h"
#include "albaSceneNode.h"
#include "albaGUIFloatSlider.h"
#include "albaGUI.h"

#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeIsosurfaceGPU);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeIsosurfaceGPU::albaPipeIsosurfaceGPU() 
: albaPipe()
//----------------------------------------------------------------------------
{
	m_Volume          = NULL;
	m_OutlineActor    = NULL;
	m_ContourMapper   = NULL; 
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_ContourSlider   = NULL;

	m_IsosurfaceVme   = NULL;

	m_ContourValue    = 300.0;
	m_AlphaValue			= 1.0;
  m_EnableGPU = 1;

  m_BoundingBoxVisibility = true;
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPU::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	assert(m_Vme->GetOutput()->IsA("albaVMEOutputVolume"));

  m_Vme->AddObserver(this);

	vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();
	dataset->Update();

	// contour pipeline
	vtkNEW(m_ContourMapper);
	m_ContourMapper->SetInput(dataset);
	m_ContourMapper->AutoLODRenderOn();
	m_ContourMapper->SetAlpha(m_AlphaValue);

	double range[2] = {0, 0};
	dataset->GetScalarRange(range);

	float value = 0.5f * (range[0] + range[1]);
	while (value < range[1] && m_ContourMapper->EstimateRelevantVolume(value) > 0.3f)
		value += 0.05f * (range[1] + range[0]) + 1.f;
	m_ContourMapper->SetContourValue(value);

	m_ContourValue = m_ContourMapper->GetContourValue();

  vtkNEW(m_Volume);
  m_Volume->SetMapper(m_ContourMapper);
  m_Volume->PickableOff();
  m_AssemblyFront->AddPart(m_Volume);

	// selection box
	vtkNEW(m_OutlineBox);
	m_OutlineBox->SetInput(dataset);

	vtkNEW(m_OutlineMapper);
	m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetMapper(m_OutlineMapper);
	m_OutlineActor->VisibilityOn();
	m_OutlineActor->PickableOff();

	vtkALBASmartPointer<vtkProperty> property;
	property->SetColor(1,1,1);
	property->SetAmbient(1);
	property->SetRepresentationToWireframe();
	property->SetInterpolationToFlat();
	m_OutlineActor->SetProperty(property);

  if(m_BoundingBoxVisibility)
	  m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
albaPipeIsosurfaceGPU::~albaPipeIsosurfaceGPU()
//----------------------------------------------------------------------------
{
  m_Vme->RemoveObserver(this);

	m_AssemblyFront->RemovePart(m_Volume);
	
  if(m_BoundingBoxVisibility)
    m_AssemblyFront->RemovePart(m_OutlineActor);

	vtkDEL(m_Volume);
	vtkDEL(m_OutlineActor);
	vtkDEL(m_ContourMapper);
	vtkDEL(m_OutlineBox);
	vtkDEL(m_OutlineMapper);
}

//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPU::Select(bool sel) 
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if (m_Volume->GetVisibility())
	{
		m_OutlineActor->SetVisibility(sel);
	}
}

//----------------------------------------------------------------------------
bool albaPipeIsosurfaceGPU::SetContourValue(float value) 
//----------------------------------------------------------------------------
{
	if (m_ContourMapper == NULL)
		return false;
  m_ContourValue = value;
	m_ContourMapper->SetContourValue(m_ContourValue);
	m_ContourMapper->Modified();
	return true;
}

//----------------------------------------------------------------------------
float albaPipeIsosurfaceGPU::GetContourValue() 
//----------------------------------------------------------------------------
{
	if (m_ContourMapper == NULL)
		return 0.;
	return m_ContourMapper->GetContourValue();
}
//----------------------------------------------------------------------------
albaGUI *albaPipeIsosurfaceGPU::CreateGui()
//----------------------------------------------------------------------------
{
	double range[2] = {0, 0};
	m_Vme->GetOutput()->GetVTKData()->GetScalarRange(range);

	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);
	m_ContourSlider = m_Gui->FloatSlider(ID_CONTOUR_VALUE,_("contour"), &m_ContourValue,range[0],range[1]);
	m_AlphaSlider = m_Gui->FloatSlider(ID_ALPHA_VALUE,_("alpha"), &m_AlphaValue,0.0,1.0);
  m_Gui->Divider();
  m_Gui->Bool(ID_ENABLE_GPU, "Enable GPU", &m_EnableGPU, 1, 
    _("Enables / disables GPU support for iso-surfacing. GPU iso-surfacing is faster but may produce unexpected results on some hardware."));
	return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPU::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId()) 
		{
		  case ID_CONTOUR_VALUE:
			{
				SetContourValue((float)m_ContourValue);
				m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
				m_Gui->Update();
			}
			break;
		  case ID_GENERATE_ISOSURFACE:
			{
				ExctractIsosurface();	
			}
			break;
		  case ID_ALPHA_VALUE:
			{
				m_ContourMapper->SetAlpha(m_AlphaValue);
				m_ContourMapper->Modified();
				m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
			}
			break;

      case ID_ENABLE_GPU:
        m_ContourMapper->SetGPUEnabled(m_EnableGPU);        
        m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
        break;

		  default:
			break;
		}
	}
  if(alba_event->GetId() == VME_OUTPUT_DATA_UPDATE)
  {
    UpdateFromData();
  }
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPU::UpdateFromData()
//----------------------------------------------------------------------------
{
  vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();
  if(dataset)
  {
    dataset->Update();

    if (m_ContourMapper != NULL)
    {
      m_ContourMapper->SetInput(dataset);
      m_ContourMapper->Update();
    }
  }
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPU::ExctractIsosurface()
//----------------------------------------------------------------------------
{
	vtkPolyData *surface = vtkPolyData::New();
	m_ContourMapper->GetOutput(0, surface);
	m_ContourMapper->Update();

	wxString name = albaString::Format(_("Isosurface %g"),m_ContourValue);

	albaNEW(m_IsosurfaceVme);
	m_IsosurfaceVme->SetName(name.ToAscii());
	m_IsosurfaceVme->SetDataByDetaching(surface,0);

	m_IsosurfaceVme->ReparentTo(m_Vme);

	surface->Delete(); 
	albaDEL(m_IsosurfaceVme);
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPU::EnableBoundingBoxVisibility(bool enable)
//----------------------------------------------------------------------------
{
	m_BoundingBoxVisibility = enable;
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPU::SetAlphaValue(double value)
//----------------------------------------------------------------------------
{
	m_AlphaValue=value;
	m_ContourMapper->SetAlpha(m_AlphaValue);
	m_ContourMapper->Modified();
	m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
}
