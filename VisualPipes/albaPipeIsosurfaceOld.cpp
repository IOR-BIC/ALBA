/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurface
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

#include "albaPipeIsosurface.h"
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
#include "vtkContourValues.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkAssembly.h"


#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeIsosurface);

//----------------------------------------------------------------------------
albaPipeIsosurface::albaPipeIsosurface() 
: albaPipe()
{
	m_Volume          = NULL;
	m_OutlineActor    = NULL;
	m_ContourMapper   = NULL; 
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_ContourSlider   = NULL;

	m_ContourValue    = 300.0;

	m_AlphaValue			= 1.0;

  m_BoundingBoxVisibility = true;
}
//----------------------------------------------------------------------------
void albaPipeIsosurface::Create(albaSceneNode *n)
{
	Superclass::Create(n);

	assert(m_Vme->GetOutput()->IsA("albaVMEOutputVolume"));

  m_Vme->AddObserver(this);

	vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();

	// contour pipeline
	vtkNEW(m_ContourMapper);
	m_ContourMapper->SetBlendModeToIsoSurface();
	m_ContourMapper->SetInputData(dataset);
	m_ContourMapper->SetRequestedRenderModeToGPU();
	m_ContourMapper->SetSampleDistance(0.2);
	
	dataset->GetScalarRange(m_VolumeRange);

  m_ContourValue = m_VolumeRange[0]*0.3 + m_VolumeRange[1]*0.7;

	vtkNEW(m_VolumeProp);
	m_VolumeProp->GetIsoSurfaceValues()->SetNumberOfContours(1);
	m_VolumeProp->GetIsoSurfaceValues()->SetValue(0, m_ContourValue);

	m_VolumeProp->SetAmbient(0.4);
	m_VolumeProp->SetDiffuse(0.8);
	m_VolumeProp->SetSpecular(0.1);
	m_VolumeProp->SetSpecularPower(6.0);
	m_VolumeProp->ShadeOn();

	// Transfer function for colors
	vtkNEW(m_ColorFunc);
	m_ColorFunc->AddRGBPoint(0.0, 1, 1, 1);
	m_ColorFunc->AddRGBPoint(255.0, 1, 1, 1);


	vtkNEW(m_OpacityFunc);
	SetAlphaValue(m_AlphaValue);

	m_VolumeProp->SetColor(m_ColorFunc);
	m_VolumeProp->SetScalarOpacity(m_OpacityFunc);

  vtkNEW(m_Volume);
  m_Volume->SetMapper(m_ContourMapper);
	m_Volume->SetProperty(m_VolumeProp);
  m_Volume->PickableOff();
  m_AssemblyFront->AddPart(m_Volume);

	// selection box
	vtkNEW(m_OutlineBox);
	m_OutlineBox->SetInputData(dataset);

	vtkNEW(m_OutlineMapper);
	m_OutlineMapper->SetInputConnection(m_OutlineBox->GetOutputPort());

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
albaPipeIsosurface::~albaPipeIsosurface()
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
void albaPipeIsosurface::Select(bool sel) 
{
	m_Selected = sel;
	if (m_Volume->GetVisibility())
	{
		m_OutlineActor->SetVisibility(sel);
	}
}

//----------------------------------------------------------------------------
bool albaPipeIsosurface::SetContourValue(float value) 
{
	if (m_ContourMapper == NULL)
		return false;
  m_ContourValue = value;

	m_VolumeProp->GetIsoSurfaceValues()->SetValue(0, m_ContourValue);
	m_ContourMapper->Modified();
	return true;
}

//----------------------------------------------------------------------------
float albaPipeIsosurface::GetContourValue() 
{
	return (m_ContourMapper == NULL) ? 0 : m_ContourValue;
}
//----------------------------------------------------------------------------
albaGUI *albaPipeIsosurface::CreateGui()
{
	double range[2] = {0, 0};
	m_Vme->GetOutput()->GetVTKData()->GetScalarRange(range);

	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);
	m_ContourSlider = m_Gui->FloatSlider(ID_CONTOUR_VALUE,_("contour"), &m_ContourValue,range[0],range[1]);
	m_AlphaSlider = m_Gui->FloatSlider(ID_ALPHA_VALUE,_("alpha"), &m_AlphaValue,0.0,1.0);
	//m_Gui->Button(ID_GENERATE_ISOSURFACE,"generate iso");
	return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeIsosurface::OnEvent(albaEventBase *alba_event)
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
				SetAlphaValue(m_AlphaValue);
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
void albaPipeIsosurface::UpdateFromData()
{
  vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();
  if(dataset)
  {
    if (m_ContourMapper != NULL)
    {
      m_ContourMapper->SetInputData(dataset);
      m_ContourMapper->Update();
    }
  }
}
//----------------------------------------------------------------------------
void albaPipeIsosurface::ExctractIsosurface(albaVMESurface *isoSurface /* = NULL */)
{
	vtkPolyData *surface = vtkPolyData::New();
	//m_ContourMapper->GetOutput(0, surface);
	m_ContourMapper->Update();

  if(m_ExtractIsosurfaceName.Equals(""))
  {
    wxString name = albaString::Format(_("Isosurface %g"),m_ContourValue);
    m_ExtractIsosurfaceName = name;
  }
	

	if (isoSurface)
	{
		isoSurface->SetName(m_ExtractIsosurfaceName.GetCStr());
		isoSurface->SetData(surface,0);
	
		isoSurface->ReparentTo(m_Vme);
	}

	surface->Delete();
  m_ExtractIsosurfaceName = "";
}
//----------------------------------------------------------------------------
void albaPipeIsosurface::EnableBoundingBoxVisibility(bool enable)
{
	m_BoundingBoxVisibility = enable;
}
//----------------------------------------------------------------------------
void albaPipeIsosurface::SetAlphaValue(double value)
{
	m_AlphaValue=value;
	m_OpacityFunc->RemoveAllPoints();
		m_OpacityFunc->AddPoint(m_VolumeRange[0], m_AlphaValue);
		m_OpacityFunc->AddPoint(m_VolumeRange[1], m_AlphaValue);

	m_ContourMapper->Modified();
	m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void albaPipeIsosurface::SetActorVisibility(int visibility)
{
  m_Volume->SetVisibility(visibility);
  m_Volume->Modified();
}
//----------------------------------------------------------------------------
void albaPipeIsosurface::SetEnableContourAnalysis( bool clean )
{
  if (m_ContourMapper)
  {
	  //m_ContourMapper->SetEnableContourAnalysis(clean);
	  m_ContourMapper->Update();
  }
}
