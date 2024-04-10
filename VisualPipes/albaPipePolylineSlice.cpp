/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePolylineSlice
 Authors: Daniele Giunchi
 
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


#include "albaAbsMatrixPipe.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"
#include "albaPipePolylineSlice.h"
#include "albaSceneNode.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEGenericAbstract.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEPolyline.h"
#include "mmaMaterial.h"

#include "vtkActor.h"
#include "vtkAppendPolyData.h"
#include "vtkCardinalSpline.h"
#include "vtkCellArray.h"
#include "vtkClipPolyData.h"
#include "vtkCubeSource.h"
#include "vtkGlyph3D.h"
#include "vtkKochanekSpline.h"
#include "vtkALBAAssembly.h"
#include "vtkALBAFixedCutter.h"
#include "vtkALBAImplicitPolyData.h"
#include "vtkALBAPolyDataToSinglePolyLine.h"
#include "vtkALBASmartPointer.h"
#include "vtkALBASmartPointer.h"
#include "vtkALBAToLinearTransform.h"
#include "vtkALBATubeFilter.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPlane.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkSplineFilter.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipePolylineSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipePolylineSlice::albaPipePolylineSlice()
	:albaPipeSlice()
{
	m_Cutter										= NULL;
	m_ClipPolyData							= NULL;
	m_ClipPolyDataUp						= NULL;
	m_ClipPolyDataDown					= NULL;
	m_Plane											= NULL;
	m_PolydataToPolylineFilter	= NULL;
	m_Sphere										= NULL;
	m_Glyph											= NULL;
	m_Tube											= NULL;
	m_Mapper										= NULL;
	m_Actor											= NULL;
	m_OutlineBox								= NULL;
	m_OutlineMapper							= NULL;
	m_OutlineProperty						= NULL;
	m_OutlineActor							= NULL;
	m_AppendPolyData						= NULL;
	m_CappingPolyData						= NULL;
	m_SplineFilter							= NULL;

	m_Normal[2]				= 0;  //rest is initialized in albaPipeSlice

	m_Border					= 1;
	m_Fill						= 0;
	m_Representation	= -1; // line by default
	m_TubeRadius			= 1.0;
	m_SphereRadius		= 1.0;
	m_Capping					= 0;
	m_ScalarDim				= 1;

	m_ShowSpheres	= false;
	m_ScalarVisibility = 0;

	m_SplineMode = -1;
	m_SplineBias = m_SplineContinuty = m_SplineTension = 0;
}
//----------------------------------------------------------------------------
void albaPipePolylineSlice::Create(albaSceneNode *n)
{
	Superclass::Create(n);

	m_Selected = false;

	m_Mapper						= NULL;
	m_Actor							= NULL;
	m_OutlineBox				= NULL;
	m_OutlineMapper			= NULL;
	m_OutlineProperty		= NULL;
	m_OutlineActor			= NULL;
	m_PolyFilteredLine	= NULL;

	m_Vme->AddObserver(this);

	InitializeFromTag();

	assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputPolyline));
	albaVMEOutputPolyline *polyline_output = albaVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
	assert(polyline_output);
	polyline_output->Update();

	vtkPolyData *data = vtkPolyData::SafeDownCast(polyline_output->GetVTKData());
	assert(data);

	//////////////////////////////////
	vtkNEW(m_PolydataToPolylineFilter);
	m_PolydataToPolylineFilter->SetInputData(data);
	m_PolydataToPolylineFilter->Update();

	//////////////////////////////////
	vtkNEW(m_Sphere);
	m_Sphere->SetRadius(m_SphereRadius);
	m_Sphere->SetPhiResolution(m_SphereResolution);
	m_Sphere->SetThetaResolution(m_SphereResolution);

	//////////////////////////////////
	vtkNEW(m_Glyph);
	m_Glyph->SetInputData(data);
	m_Glyph->SetSourceConnection(m_Sphere->GetOutputPort());
	m_Glyph->SetScaleModeToDataScalingOff();

  vtkDataArray *scalars = data->GetPointData()->GetScalars();
	double sr[2] = { 0,1 };

	if (scalars != NULL)
	{
		m_ScalarVisibility = 1;
		scalars->GetRange(sr);
	}

	m_Glyph->SetRange(sr);

	//////////////////////////////////
	vtkALBASmartPointer<vtkKochanekSpline> spline;
	vtkNEW(m_SplineFilter);
	m_SplineFilter->SetSubdivideToLength();
	m_SplineFilter->SetLength(5.0);
	m_SplineFilter->SetInputConnection(m_PolydataToPolylineFilter->GetOutputPort());
	m_SplineFilter->SetSpline(spline);
/*	m_SplineFilter->Update();*/

	//////////////////////////////////
	vtkNEW(m_Tube);
 	m_Tube->UseDefaultNormalOff();
 	m_Tube->SetRadius(m_TubeRadius);
 	m_Tube->SetCapping(m_Capping);
 	m_Tube->SetNumberOfSides(m_TubeResolution);

	m_Tube->Update();

	//////////////////////////////////
	m_Plane = vtkPlane::New();
	m_Plane->SetOrigin(m_Origin);
	m_Plane->SetNormal(m_Normal);

	vtkNEW(m_VTKTransform);
	m_VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
	m_Plane->SetTransform(m_VTKTransform);

	m_Cutter = vtkALBAFixedCutter::New();
	m_Cutter->SetCutFunction(m_Plane);

	//////////////////////////////////
	vtkNEW(m_AppendPolyData);
	vtkNEW(m_CappingPolyData);
	vtkNEW(m_ClipPolyData);
	vtkNEW(m_ClipPolyDataUp);
	vtkNEW(m_ClipPolyDataDown);

	//////////////////////////////////
	m_Mapper = vtkPolyDataMapper::New();
	m_Mapper->SetScalarVisibility(m_ScalarVisibility);
	m_Mapper->SetScalarRange(sr);

	if (m_Vme->IsAnimated())
	{
		m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
	}
	else
	{
		m_Mapper->ImmediateModeRenderingOff();
	}

	//////////////////////////////////
	m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);

	if (((albaVMEOutputPolyline *)m_Vme->GetOutput())->GetMaterial() != NULL)
	{
		m_Actor->GetProperty()->SetColor(((albaVMEOutputPolyline *)m_Vme->GetOutput())->GetMaterial()->m_Diffuse);
	}

	m_Actor->GetProperty()->SetLineWidth(1);
	m_AssemblyFront->AddPart(m_Actor);

	// Selection highlight
	m_OutlineBox = vtkOutlineCornerFilter::New();
	m_OutlineBox->SetInputData(data);

	//////////////////////////////////
	m_OutlineMapper = vtkPolyDataMapper::New();
	m_OutlineMapper->SetInputConnection(m_OutlineBox->GetOutputPort());

	m_OutlineProperty = vtkProperty::New();
	m_OutlineProperty->SetColor(1, 1, 1);
	m_OutlineProperty->SetAmbient(1);
	m_OutlineProperty->SetRepresentationToWireframe();
	m_OutlineProperty->SetInterpolationToFlat();

	m_OutlineActor = vtkActor::New();
	m_OutlineActor->SetMapper(m_OutlineMapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(m_OutlineProperty);

	m_AssemblyFront->AddPart(m_OutlineActor);

	UpdateProperty();
}
//----------------------------------------------------------------------------
albaPipePolylineSlice::~albaPipePolylineSlice()
{
	m_Vme->RemoveObserver(this);

	m_AssemblyFront->RemovePart(m_Actor);
	m_AssemblyFront->RemovePart(m_OutlineActor);

	vtkDEL(m_Cutter);
	vtkDEL(m_ClipPolyData);
	vtkDEL(m_ClipPolyDataUp);
	vtkDEL(m_ClipPolyDataDown);
	vtkDEL(m_Plane);
	vtkDEL(m_VTKTransform);
	vtkDEL(m_PolyFilteredLine);
	vtkDEL(m_Sphere);
	vtkDEL(m_Glyph);
	vtkDEL(m_Tube);
	vtkDEL(m_Mapper);
	vtkDEL(m_Actor);
	vtkDEL(m_OutlineBox);
	vtkDEL(m_OutlineMapper);
	vtkDEL(m_OutlineProperty);
	vtkDEL(m_OutlineActor);
	vtkDEL(m_SplineFilter);
	vtkDEL(m_AppendPolyData);
	vtkDEL(m_CappingPolyData);
}
//----------------------------------------------------------------------------
void albaPipePolylineSlice::Select(bool sel)
{
	m_Selected = sel;
}
//----------------------------------------------------------------------------
albaGUI *albaPipePolylineSlice::CreateGui()
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);

	//////////////////////////////////
	const wxString representation_string[] = { _("Lines"), _("Tubes"), _("Spheres") };
	m_Gui->Combo(ID_POLYLINE_REPRESENTATION, "", &m_Representation, 3, representation_string);
	m_Gui->Bool(ID_SHOW_SPHERES, _("Show Spheres"), &m_ShowSpheres, true);
	m_Gui->Bool(ID_SPLINE, _("Spline"), &m_SplineMode, true);
	m_Gui->Bool(ID_FILL, _("Fill"), &m_Fill, true);

	m_Gui->Divider(2);
	m_Gui->Label(_("Border:"));
	m_Gui->FloatSlider(ID_BORDER_CHANGE, _("Thickness"), &m_Border, 1.0, 5.0);

	m_Gui->Divider(2);
	m_Gui->Label(_("Tubes:"));
	m_Gui->Double(ID_TUBE_RADIUS, _("Radius"), &m_TubeRadius, 0);
	m_Gui->Double(ID_TUBE_RESOLUTION, _("Resolution"), &m_TubeResolution, 0);
	m_Gui->Bool(ID_TUBE_CAPPING, _("Capping"), &m_Capping, true);

	m_Gui->Divider(2);
	m_Gui->Label(_("Spheres:"));
	m_Gui->Bool(ID_SCALAR_DIMENSION, _("Scalar dim."), &m_ScalarDim, 0, _("Check to scale the sphere radius proportional to the selected scalars"));
	m_Gui->Double(ID_SPHERE_RADIUS, _("Radius"), &m_SphereRadius, 0);
	m_Gui->Double(ID_SPHERE_RESOLUTION, _("Resolution"), &m_SphereResolution, 0);

	m_Gui->Divider(2);
	m_Gui->Label(_("Spline:"));
	m_Gui->FloatSlider(ID_SPLINE_PARAMETERS, "Bias", &m_SplineBias, -1, 1);
	m_Gui->FloatSlider(ID_SPLINE_PARAMETERS, "Continuity", &m_SplineContinuty, -1, 1);
	m_Gui->FloatSlider(ID_SPLINE_PARAMETERS, "Tension", &m_SplineTension, -1, 1);
	
	m_Gui->Divider(2);
	EnableDisableGui();
	
	m_Gui->Divider();
	m_MaterialButton = new albaGUIMaterialButton(m_Vme, this);
	m_Gui->AddGui(m_MaterialButton->GetGui());
	m_Gui->Divider();

	////CreateScalarsGui(m_Gui);

  return m_Gui;
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::EnableDisableGui()
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_BORDER_CHANGE, m_Representation != LINES);
		m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBES);
		m_Gui->Enable(ID_SHOW_SPHERES, m_Representation != SPHERES);
		m_Gui->Enable(ID_SPLINE, m_Representation != SPHERES);
		m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBES);
		m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBES);
		m_Gui->Enable(ID_SCALAR_DIMENSION, m_ShowSpheres || m_Representation == SPHERES);
		m_Gui->Enable(ID_SPHERE_RADIUS, m_ShowSpheres || m_Representation == SPHERES);
		m_Gui->Enable(ID_SPHERE_RESOLUTION, m_ShowSpheres || m_Representation == SPHERES);
		m_Gui->Enable(ID_SPLINE_PARAMETERS, m_SplineMode);
	}
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_BORDER_CHANGE:
		{
			m_Actor->GetProperty()->SetLineWidth(m_Border);
			m_Actor->Modified();
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_FILL:
		{
			UpdateProperty();
			GetLogicManager()->CameraUpdate();
		}
		break;
		//////////////////////////////////
		case ID_POLYLINE_REPRESENTATION:
		{
			SetRepresentation(m_Representation);
		}
		break;
		case ID_SPLINE:
		{
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("SPLINE_MODE");
			item->SetValue(m_SplineMode);

			UpdateProperty();
			EnableDisableGui();
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_SHOW_SPHERES:
		{
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("SHOW_SPHERES");
			item->SetValue(m_ShowSpheres);

			EnableDisableGui();
			UpdateProperty();
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_TUBE_CAPPING:
		{
			m_Tube->SetCapping(m_Capping);
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_CAPPING");
			item->SetValue(m_Capping);

			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_TUBE_RADIUS:
		{
			m_Tube->SetRadius(m_TubeRadius);
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
			item->SetValue(m_TubeRadius);

			UpdateProperty();
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_TUBE_RESOLUTION:
		{
			SetTubeResolution(m_TubeResolution);
		}
		break;
		case ID_SPHERE_RADIUS:
		{
			m_Sphere->SetRadius(m_SphereRadius);
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
			item->SetValue(m_SphereRadius);

			UpdateProperty();
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_SPHERE_RESOLUTION:
		{
			SetSphereResolution(m_SphereResolution);
		}
		break;
		case ID_SCALAR_DIMENSION:
			UpdateProperty();
			GetLogicManager()->CameraUpdate();
			break;
		case ID_SPLINE_PARAMETERS:
		{
			vtkKochanekSpline *spline = (vtkKochanekSpline *)m_SplineFilter->GetSpline();
			spline->SetDefaultBias(m_SplineBias);
			spline->SetDefaultTension(m_SplineTension);
			spline->SetDefaultContinuity(m_SplineContinuty);
			m_SplineFilter->SetSpline(spline);
			m_SplineFilter->Modified();
			m_SplineFilter->Update();

			UpdateProperty();
			GetLogicManager()->CameraUpdate();
		}
		break;
		default:
			albaEventMacro(*e);
			break;
		}
	}
	else if (alba_event->GetId() == VME_OUTPUT_DATA_UPDATE && alba_event->GetSender() == m_Vme)
	{
		if (m_Vme->GetOutput() && m_Vme->GetOutput()->GetVTKData() && m_Actor)
			UpdateProperty();
	}
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::SetSlice(double *Origin, double *Normal)
{
  if (Origin != NULL)
  {
    m_Origin[0] = Origin[0];
    m_Origin[1] = Origin[1];
    m_Origin[2] = Origin[2];
  }

  if (Normal != NULL)
  {
    m_Normal[0] = Normal[0];
    m_Normal[1] = Normal[1];
    m_Normal[2] = Normal[2];
  }
	
	if(m_Plane && m_Cutter)
  {
    m_Plane->SetNormal(m_Normal);
    m_Plane->SetOrigin(m_Origin);
    m_Cutter->SetCutFunction(m_Plane);
    m_Cutter->Update(); 
    UpdateProperty();
  }

  if(m_Vme != NULL)
    m_Actor->GetProperty()->SetColor(((albaVMEOutputPolyline *)m_Vme->GetOutput())->GetMaterial()->m_Diffuse);
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::UpdateProperty()
{
	albaVMEOutputPolyline *out_polyline = albaVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
	if (out_polyline == NULL) return;
	out_polyline->Update();
	
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
	if (data == NULL) return;
	data->Modified();

	//////////////////////////////////	
	if (m_Mapper)
	{
		m_AppendPolyData->RemoveAllInputs();

		if (m_SplineMode)
			data = m_SplineFilter->GetOutput();
// 		else
// 			data = m_PolydataToPolylineFilter->GetOutput();

		if (m_Representation == TUBES)
		{
			m_Tube->SetInputData(data);
			m_AppendPolyData->AddInputConnection(m_Tube->GetOutputPort());
		}

		if (m_Representation == LINES)
		{
			m_AppendPolyData->AddInputData(data);
		}

		if (m_ShowSpheres)
		{
			m_Glyph->Update();
			m_Glyph->Modified();

			m_AppendPolyData->AddInputConnection(m_Glyph->GetOutputPort());
		}
	}

	m_AppendPolyData->Modified();
	m_AppendPolyData->Update();

	//////////////////////////////////
	m_Cutter->SetInputConnection(m_AppendPolyData->GetOutputPort());
	m_Cutter->Update();

	if (m_Fill)
		m_PolyData = RegionsCapping(m_Cutter->GetOutput());
	else
		m_PolyData = m_Cutter->GetOutput();
		
	m_Mapper->SetInputData(m_PolyData);
	m_Mapper->Update();

	if (m_Actor)
	{
		if (((albaVMEOutputPolyline *)m_Vme->GetOutput())->GetMaterial() != NULL)
		{
			m_Actor->GetProperty()->SetColor(((albaVMEOutputPolyline *)m_Vme->GetOutput())->GetMaterial()->m_Diffuse);
		}

		m_Actor->Modified();
	}
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::SetThickness(double thickness)
{
  m_Border=thickness;
  m_Actor->GetProperty()->SetLineWidth(m_Border);
  m_Actor->GetProperty()->SetPointSize(m_Border);

  if (((albaVMEOutputPolyline *)m_Vme->GetOutput())->GetMaterial()!=NULL)
  	m_Actor->GetProperty()->SetColor(((albaVMEOutputPolyline *)m_Vme->GetOutput())->GetMaterial()->m_Diffuse);

  m_Actor->Modified();
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipePolylineSlice::SetColor(double color[3])
{
	if (m_Actor)
	{
		m_Actor->GetProperty()->SetDiffuseColor(color);
		m_Actor->Modified();
	}
}
//----------------------------------------------------------------------------
void albaPipePolylineSlice::SetOpacity(double opacity)
{
	m_Opacity = opacity;

	if (m_Actor) m_Actor->GetProperty()->SetOpacity(m_Opacity);
	if (m_OutlineActor) m_OutlineActor->GetProperty()->SetOpacity(m_Opacity);
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::SetTubeResolution(double resolution)
{
	m_TubeResolution = resolution;

	m_Tube->SetNumberOfSides(m_TubeResolution);
	albaTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
	item->SetValue(m_TubeResolution);

	UpdateProperty();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::SetSphereResolution(double resolution)
{
	m_SphereResolution = resolution;

	m_Sphere->SetPhiResolution(m_SphereResolution);
	m_Sphere->SetThetaResolution(m_SphereResolution);
	albaTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
	item->SetValue(m_SphereResolution);

	UpdateProperty();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::ShowActorOn()
{
  if(m_Actor != NULL)
  {
    m_Actor->SetVisibility(true);
  }
}
//----------------------------------------------------------------------------
void albaPipePolylineSlice::ShowActorOff()
{
  if(m_Actor != NULL)
  {
    m_Actor->SetVisibility(false);
  }
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::InitializeFromTag()
{
	albaTagItem *item = NULL;
	if (!m_Vme->GetTagArray()->IsTagPresent("REPRESENTATION"))
	{
		item = new albaTagItem();
		item->SetName("REPRESENTATION");
		item->SetValue(0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}

	if (m_Representation == -1)
	{
		item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
		m_Representation = (int)item->GetValueAsDouble();
	}

	if (!m_Vme->GetTagArray()->IsTagPresent("SHOW_SPHERES"))
	{
		item = new albaTagItem();
		item->SetName("SHOW_SPHERES");
		item->SetValue(0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}

	if (!m_Vme->GetTagArray()->IsTagPresent("SPHERE_RADIUS"))
	{
		item = new albaTagItem();
		item->SetName("SPHERE_RADIUS");
		item->SetValue(1.0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}

	item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
	m_SphereRadius = item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("SPHERE_RESOLUTION"))
	{
		item = new albaTagItem();
		item->SetName("SPHERE_RESOLUTION");
		item->SetValue(10.0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}

	item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
	m_SphereResolution = item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_RADIUS"))
	{
		item = new albaTagItem();
		item->SetName("TUBE_RADIUS");
		item->SetValue(1.0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}

	item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
	m_TubeRadius = item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_RESOLUTION"))
	{
		item = new albaTagItem();
		item->SetName("TUBE_RESOLUTION");
		item->SetValue(10.0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}

	item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
	m_TubeResolution = item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_CAPPING"))
	{
		item = new albaTagItem();
		item->SetName("TUBE_CAPPING");
		item->SetValue(0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}

	item = m_Vme->GetTagArray()->GetTag("TUBE_CAPPING");
	m_Capping = (int)item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("SPLINE_MODE"))
	{
		item = new albaTagItem();
		item->SetName("SPLINE_MODE");
		item->SetValue(0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}

	if (m_SplineMode == -1)
	{
		item = m_Vme->GetTagArray()->GetTag("SPLINE_MODE");
		m_SplineMode = (int)item->GetValueAsDouble();
	}
}
//----------------------------------------------------------------------------
void albaPipePolylineSlice::SetRepresentation(int representation)
{
	m_Representation = representation;

	EnableDisableGui();
	UpdateProperty();

	albaTagItem *item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
	item->SetValue(m_Representation);

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
vtkPolyData *albaPipePolylineSlice::RegionsCapping(vtkPolyData* inputBorder)
{
	m_CappingPolyData->RemoveAllInputs();

  vtkALBASmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInputData(inputBorder);
  connectivityFilter->SetExtractionModeToSpecifiedRegions();
  connectivityFilter->Update();
  int regionNumbers = connectivityFilter->GetNumberOfExtractedRegions();

  for(int region = 0; region < regionNumbers; region++)
  {
    connectivityFilter->InitializeSpecifiedRegionList();
    connectivityFilter->AddSpecifiedRegion(region);
    connectivityFilter->Update();

    vtkALBASmartPointer<vtkPolyData> p;

    //write polydata

    p->SetPoints(connectivityFilter->GetOutput()->GetPoints());
    p->SetLines(connectivityFilter->GetOutput()->GetLines());
    /*albaString filename1 = "C:\\conn_";
    filename1 << region;
    filename1 << ".vtk";
    vtkALBASmartPointer<vtkPolyDataWriter> pdWriter;
    pdWriter->SetInput(p);
    pdWriter->SetFileName(filename1);
    pdWriter->Update();*/
    //end write polydata

    p->DeepCopy(CappingFilter(p));

    /*albaString filename2 = "C:\\connCAPP_";
    filename2 << region;
    filename2 << ".vtk";

    pdWriter->SetInput(p);
    pdWriter->SetFileName(filename2);
    pdWriter->Update();*/
    //end write polydata

    m_CappingPolyData->AddInputData(p);
		m_CappingPolyData->Update();
  }

	return  m_CappingPolyData->GetOutput();
}
//----------------------------------------------------------------------------
vtkPolyData *albaPipePolylineSlice::CappingFilter(vtkPolyData* inputBorder)
//----------------------------------------------------------------------------
{
  int i, iCell;
  // prerequisites: connected polydata with line cells that represent the edge of the hole to be capped. 
  // search average point
  double averagePoint[3] = {0.0,0.0,0.0};
  vtkALBASmartPointer<vtkPoints>outputPoints;
  vtkALBASmartPointer<vtkCellArray> outputCellArray;
  vtkPolyData *output;
  vtkNEW(output);
  outputPoints->DeepCopy(inputBorder->GetPoints());

  for(i = 0;i<inputBorder->GetNumberOfPoints();i++)
  {
    double currentPoint[3];
    inputBorder->GetPoint(i, currentPoint);
    averagePoint[0] += currentPoint[0];
    averagePoint[1] += currentPoint[1];
    averagePoint[2] += currentPoint[2];
  }
  // the new polydata that represents capping has input->NPoints + 1 points: the averagePoint
  double center[3];
  inputBorder->GetCenter(center);

  averagePoint[0] /= inputBorder->GetNumberOfPoints(); 
  averagePoint[1] /= inputBorder->GetNumberOfPoints();
  averagePoint[2] /= inputBorder->GetNumberOfPoints();
  outputPoints->InsertNextPoint(center);
  output->SetPoints(outputPoints);
  // create triangular cells with the new point.
  for(int i=0; i<inputBorder->GetNumberOfCells();i++)
  {
    //each line of the inputPolydata should be transformed into a triangle.
    vtkALBASmartPointer<vtkIdList> currentCellIds;
    for (iCell = 0; iCell < inputBorder->GetCell(i)->GetNumberOfPoints(); iCell++)
    {
      currentCellIds->InsertNextId(inputBorder->GetCell(i)->GetPointIds()->GetId(iCell));
    }
    // write the last id (the averagePoint) in the current cell Id list
    currentCellIds->InsertNextId(inputBorder->GetNumberOfPoints());
    // insert the Id list in the cell array
    outputCellArray->InsertNextCell(currentCellIds);
  }
  // set the cell array to the polydata
  output->SetPolys(outputCellArray);

  return output;
}

//----------------------------------------------------------------------------
void albaPipePolylineSlice::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
  m_Actor->SetPickable(enable);
  m_Actor->Modified();
}
