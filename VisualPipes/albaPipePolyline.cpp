/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePolyline
 Authors: Matteo Giacomoni - Daniele Giunchi
 
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

#include "albaPipePolyline.h"
#include "albaDecl.h"
#include "albaSceneNode.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"
#include "mmaMaterial.h"

#include "albaVME.h"
#include "albaVMEGenericAbstract.h"
#include "albaVMEOutputPolyline.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaDataVector.h"
#include "albaDataPipe.h"
#include "albaVMEItem.h"
#include "albaVMEItemVTK.h"
#include "albaTransform.h"

#include "vtkALBAAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkALBATubeFilter.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkScalarBarActor.h"
#include "vtkColorTransferFunction.h"
#include "vtkKochanekSpline.h" 
#include "vtkALBASmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h" 
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"
#include "vtkSplineFilter.h"
#include "vtkPolyDataMapper.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipePolyline);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipePolyline::albaPipePolyline()
:albaPipeWithScalar()
{
	m_Representation  = -1; // line by default
	m_TubeRadius      = 1.0;
	m_SphereRadius      = 1.0;
	m_Capping         = 0;

	m_ScalarDim				= 1;
	m_ShowSpheres = false;

	m_Sphere          = NULL;
	m_Glyph           = NULL;
	m_Tube            = NULL;
	m_Mapper          = NULL;
	m_Actor           = NULL;
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_OutlineProperty = NULL;
	m_OutlineActor    = NULL;
	m_AppendPolyData  = NULL;
	m_SplineFilter		= NULL;
	
  m_SplineMode      = -1;
	m_SplineBias = m_SplineContinuty = m_SplineTension = 0;
}
//----------------------------------------------------------------------------
void albaPipePolyline::Create(albaSceneNode *n)
{
	Superclass::Create(n);

	m_Selected = false;

	albaVMEOutputPolyline *out_polyline = albaVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
	assert(out_polyline);
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
	assert(data);

	m_ObjectMaterial = out_polyline->GetMaterial();
	m_Vme->AddObserver(this);

	InitializeFromTag();

	double sr[2] = { 0,1 };
	vtkDataArray *scalarArray = data->GetPointData()->GetScalars();
	if (scalarArray)
		scalarArray->GetRange(sr);

	vtkNEW(m_Sphere);
	m_Sphere->SetRadius(m_SphereRadius);
	m_Sphere->SetPhiResolution(m_SphereResolution);
	m_Sphere->SetThetaResolution(m_SphereResolution);

	vtkNEW(m_Glyph);
	m_Glyph->SetInputData(data);
	m_Glyph->SetSourceConnection(m_Sphere->GetOutputPort());

	m_Glyph->SetScaleModeToDataScalingOff();
	m_Glyph->SetRange(sr);

	vtkALBASmartPointer<vtkKochanekSpline> spline;
	vtkNEW(m_SplineFilter);
	m_SplineFilter->SetSubdivideToLength();
	m_SplineFilter->SetLength(5.0);
	m_SplineFilter->SetInputData(data);
	m_SplineFilter->SetSpline(spline);
	m_SplineFilter->Update();

	vtkNEW(m_Tube);
	m_Tube->UseDefaultNormalOff();
	m_Tube->SetInputData(data);
	m_Tube->SetRadius(m_TubeRadius);
	m_Tube->SetCapping(m_Capping);
	m_Tube->SetNumberOfSides(m_TubeResolution);

	vtkNEW(m_Mapper);
	m_AppendPolyData = vtkAppendPolyData::New();

	if (m_Vme->IsAnimated())
		m_Mapper->ImmediateModeRenderingOn();
	else
		m_Mapper->ImmediateModeRenderingOff();

	m_Mapper->SetInputConnection(m_AppendPolyData->GetOutputPort());

	ManageScalarOnExecutePipe(data);

	m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);

	if (m_ObjectMaterial)
	{
		if (m_ObjectMaterial->m_MaterialType == mmaMaterial::USE_LOOKUPTABLE)
		{
			/*m_UseVTKProperty = 0;
			m_UseLookupTable = 1;*/
			m_Mapper->SetScalarModeToUsePointData();
			m_Mapper->ScalarVisibilityOn();
			SetLookupTableToMapper();
			m_Mapper->SetScalarRange(sr);
		}
		if (m_ObjectMaterial->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
		{
			/*m_UseVTKProperty = 1;
			m_UseLookupTable = 0;*/
			m_Actor->SetProperty(m_ObjectMaterial->m_Prop);
		}
	}
	m_Mapper->Modified();

	m_AssemblyFront->AddPart(m_Actor);

	// selection highlight
	m_OutlineBox = vtkOutlineCornerFilter::New();
	m_OutlineBox->SetInputData(data);  

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
albaPipePolyline::~albaPipePolyline()
{
	m_Vme->RemoveObserver(this);

	m_AssemblyFront->RemovePart(m_Actor);
	m_AssemblyFront->RemovePart(m_OutlineActor);

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
}
//----------------------------------------------------------------------------
void albaPipePolyline::Select(bool sel)
{
	m_Selected = sel;
	if (m_Actor && m_Actor->GetVisibility())
	{
		m_OutlineActor->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
albaGUI *albaPipePolyline::CreateGui()
{	
	m_Gui = new albaGUI(this);

	//m_Gui->Combo(ID_SCALAR,"",&m_Scalar,numberOfArrays,m_ScalarsName);
	const wxString representation_string[] = { _("Lines"), _("Tubes"), _("Spheres") };
	m_Gui->Combo(ID_POLYLINE_REPRESENTATION,"",&m_Representation,3,representation_string);
	m_Gui->Bool(ID_SHOW_SPHERES, _("Show Spheres"), &m_ShowSpheres,true);
	m_Gui->Bool(ID_SPLINE, _("Spline"), &m_SplineMode,true);

	m_Gui->Divider(2);
	m_Gui->Label(_("Tubes:"));
	m_Gui->Double(ID_TUBE_RADIUS,_("Radius"),&m_TubeRadius,0);
	m_Gui->Double(ID_TUBE_RESOLUTION,_("Resolution"),&m_TubeResolution,0);
	m_Gui->Bool(ID_TUBE_CAPPING,_("Capping"),&m_Capping,true);
	
	m_Gui->Divider(2);
	m_Gui->Label(_("Spheres:"));
	m_Gui->Bool(ID_SCALAR_DIMENSION,_("Scalar dim."),&m_ScalarDim,0,_("Check to scale the sphere radius proportional to the selected scalars"));
	m_Gui->Double(ID_SPHERE_RADIUS,_("Radius"),&m_SphereRadius,0);
	m_Gui->Double(ID_SPHERE_RESOLUTION,_("Resolution"),&m_SphereResolution,0);
	
	m_Gui->Divider(2);
	m_Gui->Label(_("Spline:"));
	m_Gui->FloatSlider(ID_SPLINE_PARAMETERS, "Bias", &m_SplineBias, -1, 1);
	m_Gui->FloatSlider(ID_SPLINE_PARAMETERS, "Continuity", &m_SplineContinuty, -1, 1);
	m_Gui->FloatSlider(ID_SPLINE_PARAMETERS, "Tension", &m_SplineTension, -1, 1);
	m_Gui->Divider(2);

	EnableDisableGui();

	m_Gui->Divider();
  m_MaterialButton = new albaGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_Gui->Divider();

	CreateScalarsGui(m_Gui);

	return m_Gui;
}

//----------------------------------------------------------------------------
void albaPipePolyline::EnableDisableGui()
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBES);
		m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBES);
		m_Gui->Enable(ID_SHOW_SPHERES, m_Representation != SPHERES);
		m_Gui->Enable(ID_SPLINE, m_Representation != SPHERES);
		m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBES);
		m_Gui->Enable(ID_SCALAR_DIMENSION, m_ShowSpheres || m_Representation == SPHERES);
		m_Gui->Enable(ID_SPHERE_RADIUS, m_ShowSpheres || m_Representation == SPHERES);
		m_Gui->Enable(ID_SPHERE_RESOLUTION, m_ShowSpheres || m_Representation == SPHERES);
		m_Gui->Enable(ID_SPLINE_PARAMETERS, m_SplineMode);
	}
}

//----------------------------------------------------------------------------
void albaPipePolyline::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_POLYLINE_REPRESENTATION:
		{
			SetRepresentation(m_Representation);
		}
		break;
		case ID_TUBE_RADIUS:
		{
			m_Tube->SetRadius(m_TubeRadius);
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
			item->SetValue(m_TubeRadius);
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_SHOW_SPHERES:
		{
			SetShowSphere(m_ShowSpheres);
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
		case ID_TUBE_RESOLUTION:
		{
			m_Tube->SetNumberOfSides(m_TubeResolution);
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
			item->SetValue(m_TubeResolution);
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_SPHERE_RADIUS:
		{
			m_Sphere->SetRadius(m_SphereRadius);
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
			item->SetValue(m_SphereRadius);
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_SPHERE_RESOLUTION:
		{
			m_Sphere->SetPhiResolution(m_SphereResolution);
			m_Sphere->SetThetaResolution(m_SphereResolution);
			albaTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
			item->SetValue(m_SphereResolution);
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_SCALAR_DIMENSION:
			UpdateProperty();
			GetLogicManager()->CameraUpdate();
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
			Superclass::OnEvent(e);
			break;
		}
	}
	else if (alba_event->GetId() == VME_TIME_SET)
	{
		UpdateActiveScalarsInVMEDataVectorItems();
		UpdateProperty();
	}
	else if (alba_event->GetSender() == m_Vme)
	{
		if (alba_event->GetId() == VME_OUTPUT_DATA_UPDATE)
		{
			UpdateActiveScalarsInVMEDataVectorItems();
			UpdateProperty();
		}
	}
}
//----------------------------------------------------------------------------
void albaPipePolyline::UpdateProperty(bool fromTag)
{
	if (!m_Vme)
		return;

	albaVMEOutputPolyline *out_polyline = albaVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
	out_polyline->Update();
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());

	if (data->GetNumberOfPoints() <= 0) return;
	data->Modified();

	if (m_Mapper)
	{
		m_AppendPolyData->RemoveAllInputs();

		if (m_SplineMode == true)
			data = m_SplineFilter->GetOutput();

		if (m_Representation == TUBES)
		{
			m_Tube->SetInputData(data);
			m_AppendPolyData->SetInputConnection(m_Tube->GetOutputPort());
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
}

//----------------------------------------------------------------------------
void albaPipePolyline::ExecutePipe()
{
	albaVMEOutput *vmeOutput = m_Vme->GetOutput();
	assert(vmeOutput);
	vmeOutput->Update();
	vtkDataSet *dataSet = vtkDataSet::SafeDownCast(vmeOutput->GetVTKData());
	assert(dataSet);

	ManageScalarOnExecutePipe(dataSet);
}

//----------------------------------------------------------------------------
void albaPipePolyline::InitializeFromTag()
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
  if(m_Representation==-1)
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
  if(m_SplineMode==-1)
  {
    item = m_Vme->GetTagArray()->GetTag("SPLINE_MODE");
    m_SplineMode = (int)item->GetValueAsDouble();
  }
}
//----------------------------------------------------------------------------
void albaPipePolyline::SetRepresentation(int representation)
{
	m_Representation = representation;
	
	EnableDisableGui();
	UpdateProperty();

	albaTagItem *item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
	item->SetValue(m_Representation);

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipePolyline::SetShowSphere(bool show)
{
	m_ShowSpheres = show;
	EnableDisableGui();
	UpdateProperty();

	albaTagItem *item = m_Vme->GetTagArray()->GetTag("SHOW_SPHERES");
	item->SetValue(m_ShowSpheres);

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipePolyline::SetRadius(double radius)
//----------------------------------------------------------------------------
{
	m_TubeRadius = radius; 
	m_SphereRadius = radius;

	m_Sphere->SetRadius(m_SphereRadius);
	albaTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
	item->SetValue(m_SphereRadius);
  item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
  item->SetValue(m_TubeRadius);

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipePolyline::SetColor(double color[3])
//----------------------------------------------------------------------------
{
	if(m_Actor)
	{
		m_Actor->GetProperty()->SetDiffuseColor(color);
		m_Actor->Modified();
	}
}
//----------------------------------------------------------------------------
void albaPipePolyline::SetOpacity(double opacity)
//----------------------------------------------------------------------------
{
  m_Opacity = opacity;

  if(m_Actor) m_Actor->GetProperty()->SetOpacity(m_Opacity);
  if(m_OutlineActor) m_OutlineActor->GetProperty()->SetOpacity(m_Opacity);
}
//----------------------------------------------------------------------------
void albaPipePolyline::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
  m_Actor->SetPickable(enable);
  m_Actor->Modified();
}
