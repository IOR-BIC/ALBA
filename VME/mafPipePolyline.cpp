/*=========================================================================

 Program: MAF2
 Module: mafPipePolyline
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipePolyline.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
#include "mafGUIMaterialButton.h"
#include "mmaMaterial.h"

#include "mafVME.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEOutputPolyline.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafEventSource.h"
#include "mafDataVector.h"
#include "mafDataPipe.h"
#include "mafVMEItem.h"
#include "mafVMEItemVTK.h"
#include "mafTransform.h"

#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkMAFTubeFilter.h"
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
#include "vtkMAFSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h" 
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"
#include "vtkSplineFilter.h"
#include "vtkPolyDataMapper.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipePolyline);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipePolyline::mafPipePolyline()
:mafPipeWithScalar()
//----------------------------------------------------------------------------
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
	m_SplineFilter = NULL;
	
  m_SplineMode      = -1;
	m_SplineBias = m_SplineContinuty = m_SplineTension = 0;
}
//----------------------------------------------------------------------------
void mafPipePolyline::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;

	mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
	assert(out_polyline);
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
	assert(data);
	data->Update();

	m_ObjectMaterial = out_polyline->GetMaterial();
	m_Vme->GetEventSource()->AddObserver(this);

	InitializeFromTag();

  double sr[2] = {0,1};
  vtkDataArray *scalarArray = data->GetPointData()->GetScalars();
  if(scalarArray)
    scalarArray->GetRange(sr);

	vtkNEW(m_Sphere);
	m_Sphere->SetRadius(m_SphereRadius);
	m_Sphere->SetPhiResolution(m_SphereResolution);
	m_Sphere->SetThetaResolution(m_SphereResolution);

	vtkNEW(m_Glyph);
	m_Glyph->SetInput(data);
	m_Glyph->SetSource(m_Sphere->GetOutput());
  m_Glyph->SetScaleModeToDataScalingOff();
  m_Glyph->SetRange(sr);

	vtkMAFSmartPointer<vtkKochanekSpline> spline;
	vtkNEW(m_SplineFilter);
	m_SplineFilter->SetSubdivideToLength();
	m_SplineFilter->SetLength(5.0);
	m_SplineFilter->SetInput(data);
	m_SplineFilter->SetSpline(spline);
	m_SplineFilter->Update();
	
	vtkNEW(m_Tube);
	m_Tube->UseDefaultNormalOff();
	m_Tube->SetInput(data);
	m_Tube->SetRadius(m_TubeRadius);
	m_Tube->SetCapping(m_Capping);
	m_Tube->SetNumberOfSides(m_TubeResolution);

	vtkNEW(m_Mapper);
	m_AppendPolyData = vtkAppendPolyData::New();
	
	if(m_Vme->IsAnimated())
		m_Mapper->ImmediateModeRenderingOn();
	else
		m_Mapper->ImmediateModeRenderingOff();

	m_Mapper->SetInput(m_AppendPolyData->GetOutput());

	ManageScalarOnExecutePipe(data);


	m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);

  if(m_ObjectMaterial)
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
	m_OutlineBox->SetInput(data);  

	m_OutlineMapper = vtkPolyDataMapper::New();
	m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

	m_OutlineProperty = vtkProperty::New();
	m_OutlineProperty->SetColor(1,1,1);
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
mafPipePolyline::~mafPipePolyline()
//----------------------------------------------------------------------------
{
	m_Vme->GetEventSource()->RemoveObserver(this);

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
void mafPipePolyline::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor && m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
mafGUI *mafPipePolyline::CreateGui()
//----------------------------------------------------------------------------
{
	const wxString representation_string[] = {_("Lines"), _("Tubes"), _("Spheres")};
	m_Gui = new mafGUI(this);
	//m_Gui->Combo(ID_SCALAR,"",&m_Scalar,numberOfArrays,m_ScalarsName);
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
  m_MaterialButton = new mafGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_Gui->Divider();

	CreateScalarsGui(m_Gui);

	return m_Gui;
}

//----------------------------------------------------------------------------
void mafPipePolyline::EnableDisableGui()
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBES);
		m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBES);
		m_Gui->Enable(ID_SHOW_SPHERES, m_Representation != SPHERES);
		m_Gui->Enable(ID_SPLINE, m_Representation != SPHERES);
		m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBES);
		m_Gui->Enable(ID_SCALAR_DIMENSION, m_ShowSpheres);
		m_Gui->Enable(ID_SPHERE_RADIUS, m_ShowSpheres);
		m_Gui->Enable(ID_SPHERE_RESOLUTION, m_ShowSpheres);
		m_Gui->Enable(ID_SPLINE_PARAMETERS, m_SplineMode);
	}
}

//----------------------------------------------------------------------------
void mafPipePolyline::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		  case ID_POLYLINE_REPRESENTATION:
			{
				SetRepresentation(m_Representation);
			}
			break;
		  case ID_TUBE_RADIUS:
			{
				m_Tube->SetRadius(m_TubeRadius);
				mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
				item->SetValue(m_TubeRadius);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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
				mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_CAPPING");
				item->SetValue(m_Capping);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		  case ID_TUBE_RESOLUTION:
			{
				m_Tube->SetNumberOfSides(m_TubeResolution);
				mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
				item->SetValue(m_TubeResolution);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		  case ID_SPHERE_RADIUS:
			{
				m_Sphere->SetRadius(m_SphereRadius);
				mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
				item->SetValue(m_SphereRadius);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		  case ID_SPHERE_RESOLUTION:
			{
				m_Sphere->SetPhiResolution(m_SphereResolution);
				m_Sphere->SetThetaResolution(m_SphereResolution);
				mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
				item->SetValue(m_SphereResolution);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		  case ID_SCALAR_DIMENSION:
			  UpdateProperty();
			  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			break;
      case ID_SPLINE:
      {
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPLINE_MODE");
        item->SetValue(m_SplineMode);
        UpdateProperty();
				EnableDisableGui();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));        
      }
			break;
			case ID_SPLINE_PARAMETERS:
			{
				vtkKochanekSpline *spline=(vtkKochanekSpline *)m_SplineFilter->GetSpline();
				spline->SetDefaultBias(m_SplineBias);
				spline->SetDefaultTension(m_SplineTension);
				spline->SetDefaultContinuity(m_SplineContinuty);
				m_SplineFilter->SetSpline(spline);
				m_SplineFilter->Modified();
				m_SplineFilter->Update();
				UpdateProperty();
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
		  default:
			  Superclass::OnEvent(e);
			break;
		}
	}
	else if (maf_event->GetId() == VME_TIME_SET)
	{
		UpdateActiveScalarsInVMEDataVectorItems();
		UpdateProperty();
	}
  else if (maf_event->GetSender() == m_Vme)
  {
    if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
    {
			UpdateActiveScalarsInVMEDataVectorItems();
			UpdateProperty();
    }
  }
  
}
//----------------------------------------------------------------------------
void mafPipePolyline::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  if(!m_Vme)
    return;

	mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
	out_polyline->Update();
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
  data->Update();

  if(data->GetNumberOfPoints() <= 0) return;
	data->Modified();
	data->Update();

  if(m_Mapper)
  {
		m_AppendPolyData->RemoveAllInputs();

		if (m_SplineMode == true)
			data = m_SplineFilter->GetOutput();

	  if (m_Representation == TUBES)
	  {
      m_Tube->SetInput(data);
			m_AppendPolyData->AddInput(m_Tube->GetOutput());
	  }
		
		if (m_Representation == LINES)
		{
			m_AppendPolyData->AddInput(data);
		}
		
	  if (m_ShowSpheres)
	  {
      m_Glyph->Update();
		  m_Glyph->Modified();
		 
      m_AppendPolyData->AddInput(m_Glyph->GetOutput());
	  }
  }
}

//----------------------------------------------------------------------------
void mafPipePolyline::ExecutePipe()
{
	mafVMEOutput *vmeOutput = m_Vme->GetOutput();
	assert(vmeOutput);
	vmeOutput->Update();
	vtkDataSet *dataSet = vtkDataSet::SafeDownCast(vmeOutput->GetVTKData());
	assert(dataSet);
	dataSet->Update();

	ManageScalarOnExecutePipe(dataSet);
}

//----------------------------------------------------------------------------
void mafPipePolyline::InitializeFromTag()
//----------------------------------------------------------------------------
{
	mafTagItem *item = NULL;
	if (!m_Vme->GetTagArray()->IsTagPresent("REPRESENTATION"))
	{
		item = new mafTagItem();
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
		item = new mafTagItem();
		item->SetName("SHOW_SPHERES");
		item->SetValue(0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}
	

	if (!m_Vme->GetTagArray()->IsTagPresent("SPHERE_RADIUS"))
	{
		item = new mafTagItem();
		item->SetName("SPHERE_RADIUS");
		item->SetValue(1.0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}
	item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
	m_SphereRadius = item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("SPHERE_RESOLUTION"))
	{
		item = new mafTagItem();
		item->SetName("SPHERE_RESOLUTION");
		item->SetValue(10.0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}
	item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
	m_SphereResolution = item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_RADIUS"))
	{
		item = new mafTagItem();
		item->SetName("TUBE_RADIUS");
		item->SetValue(1.0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}
	item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
	m_TubeRadius = item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_RESOLUTION"))
	{
		item = new mafTagItem();
		item->SetName("TUBE_RESOLUTION");
		item->SetValue(10.0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}
	item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
	m_TubeResolution = item->GetValueAsDouble();

	if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_CAPPING"))
	{
		item = new mafTagItem();
		item->SetName("TUBE_CAPPING");
		item->SetValue(0);
		m_Vme->GetTagArray()->SetTag(*item);
		cppDEL(item);
	}
	item = m_Vme->GetTagArray()->GetTag("TUBE_CAPPING");
	m_Capping = (int)item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("SPLINE_MODE"))
  {
    item = new mafTagItem();
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
void mafPipePolyline::SetRepresentation(int representation)
{
	m_Representation = representation;
	if (m_Representation == SPHERES)
		SetShowSphere(true);
	EnableDisableGui();
	UpdateProperty();
	
	mafTagItem *item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
	item->SetValue(m_Representation);

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipePolyline::SetShowSphere(bool show)
{
	m_ShowSpheres = show;
	EnableDisableGui();
	UpdateProperty();

	mafTagItem *item = m_Vme->GetTagArray()->GetTag("SHOW_SPHERES");
	item->SetValue(m_ShowSpheres);

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mafPipePolyline::SetRadius(double radius)
//----------------------------------------------------------------------------
{
	m_TubeRadius = radius; 
	m_SphereRadius = radius;

	m_Sphere->SetRadius(m_SphereRadius);
	mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
	item->SetValue(m_SphereRadius);
  item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
  item->SetValue(m_TubeRadius);

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipePolyline::SetColor(double color[3])
//----------------------------------------------------------------------------
{
	if(m_Actor)
	{
		m_Actor->GetProperty()->SetDiffuseColor(color);
		m_Actor->Modified();
	}
}

//----------------------------------------------------------------------------
void mafPipePolyline::SetOpacity(double opacity)
//----------------------------------------------------------------------------
{
  m_Opacity = opacity;

  if(m_Actor) m_Actor->GetProperty()->SetOpacity(m_Opacity);
  if(m_OutlineActor) m_OutlineActor->GetProperty()->SetOpacity(m_Opacity);
}
//----------------------------------------------------------------------------
void mafPipePolyline::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
  m_Actor->SetPickable(enable);
  m_Actor->Modified();
}
