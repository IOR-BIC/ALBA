/*=========================================================================

 Program: MAF2
 Module: mafPipeIsosurfaceGPU
 Authors: Alexander Savenko  -  Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#if defined(WIN32)
#pragma warning (disable : 4018)
#endif

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeIsosurfaceGPU.h"
#include "mafEvent.h"
#include "mafSceneNode.h"
#include "mafGUIFloatSlider.h"
#include "mafGUI.h"

#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafEventSource.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeIsosurfaceGPU);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeIsosurfaceGPU::mafPipeIsosurfaceGPU() 
: mafPipe()
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
void mafPipeIsosurfaceGPU::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	assert(m_Vme->GetOutput()->IsA("mafVMEOutputVolume"));

  m_Vme->GetEventSource()->AddObserver(this);

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

	vtkMAFSmartPointer<vtkProperty> property;
	property->SetColor(1,1,1);
	property->SetAmbient(1);
	property->SetRepresentationToWireframe();
	property->SetInterpolationToFlat();
	m_OutlineActor->SetProperty(property);

  if(m_BoundingBoxVisibility)
	  m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
mafPipeIsosurfaceGPU::~mafPipeIsosurfaceGPU()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);

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
void mafPipeIsosurfaceGPU::Select(bool sel) 
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if (m_Volume->GetVisibility())
	{
		m_OutlineActor->SetVisibility(sel);
	}
}

//----------------------------------------------------------------------------
bool mafPipeIsosurfaceGPU::SetContourValue(float value) 
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
float mafPipeIsosurfaceGPU::GetContourValue() 
//----------------------------------------------------------------------------
{
	if (m_ContourMapper == NULL)
		return 0.;
	return m_ContourMapper->GetContourValue();
}
//----------------------------------------------------------------------------
mafGUI *mafPipeIsosurfaceGPU::CreateGui()
//----------------------------------------------------------------------------
{
	double range[2] = {0, 0};
	m_Vme->GetOutput()->GetVTKData()->GetScalarRange(range);

	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);
	m_ContourSlider = m_Gui->FloatSlider(ID_CONTOUR_VALUE,_("contour"), &m_ContourValue,range[0],range[1]);
	m_AlphaSlider = m_Gui->FloatSlider(ID_ALPHA_VALUE,_("alpha"), &m_AlphaValue,0.0,1.0);
  m_Gui->Divider();
  m_Gui->Bool(ID_ENABLE_GPU, "Enable GPU", &m_EnableGPU, 1, 
    _("Enables / disables GPU support for iso-surfacing. GPU iso-surfacing is faster but may produce unexpected results on some hardware."));
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPU::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
		  case ID_CONTOUR_VALUE:
			{
				SetContourValue((float)m_ContourValue);
				m_Vme->ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
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
				m_Vme->ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
			}
			break;

      case ID_ENABLE_GPU:
        m_ContourMapper->SetGPUEnabled(m_EnableGPU);        
        m_Vme->ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        break;

		  default:
			break;
		}
	}
  if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
  {
    UpdateFromData();
  }
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPU::UpdateFromData()
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
void mafPipeIsosurfaceGPU::ExctractIsosurface()
//----------------------------------------------------------------------------
{
	vtkPolyData *surface = vtkPolyData::New();
	m_ContourMapper->GetOutput(0, surface);
	m_ContourMapper->Update();

	wxString name = wxString::Format(_("Isosurface %g"),m_ContourValue);

	mafNEW(m_IsosurfaceVme);
	m_IsosurfaceVme->SetName(name.c_str());
	m_IsosurfaceVme->SetDataByDetaching(surface,0);

	m_IsosurfaceVme->ReparentTo(m_Vme);

	surface->Delete(); 
	mafDEL(m_IsosurfaceVme);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPU::EnableBoundingBoxVisibility(bool enable)
//----------------------------------------------------------------------------
{
	m_BoundingBoxVisibility = enable;
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPU::SetAlphaValue(double value)
//----------------------------------------------------------------------------
{
	m_AlphaValue=value;
	m_ContourMapper->SetAlpha(m_AlphaValue);
	m_ContourMapper->Modified();
	m_Vme->ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
}
