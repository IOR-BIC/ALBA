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
#include "vtkTubeFilter.h"
#include "vtkPolyDataMapper.h"
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
#include "vtkCardinalSpline.h" 
#include "vtkMAFSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h" 
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipePolyline);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipePolyline::mafPipePolyline()
:mafPipe()
//----------------------------------------------------------------------------
{
	m_Representation  = -1; // line by default
	m_TubeRadius      = 1.0;
	m_SphereRadius      = 1.0;
	m_Capping         = 0;

	m_ScalarDim				= 1;
	m_Scalar					= 0;

	m_Sphere          = NULL;
	m_Glyph           = NULL;
	m_Tube            = NULL;
	m_PolyFilteredLine= NULL;
	m_Mapper          = NULL;
	m_Actor           = NULL;
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_OutlineProperty = NULL;
	m_OutlineActor    = NULL;

	m_Table						= NULL;

  m_BorderData = NULL;
  m_BorderMapper = NULL;
  m_BorderProperty = NULL;
  m_BorderActor = NULL;

  m_SplineMode      = -1;
  m_SplineCoefficient = 10.0;
  m_DistanceBorder = 0.0;
  m_ScalarsName = NULL;
  m_HalfNumberOfBorders = 0;

  m_TextIdentifierBorderVisibility = FALSE;
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

	m_PolylineMaterial = out_polyline->GetMaterial();
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
	//m_Glyph->NomalizeScalingOn();
	//m_Glyph->SetScaleModeToScaleByScalar();
  m_Glyph->SetScaleModeToDataScalingOff();
  m_Glyph->SetRange(sr);


  if(m_SplineMode && m_Representation != GLYPH && m_Representation != GLYPH_UNCONNECTED)
    data = SplineProcess(data);
  else 
    data = LineProcess(data);

	vtkNEW(m_Tube);
	m_Tube->UseDefaultNormalOff();
	m_Tube->SetInput(data);
	m_Tube->SetRadius(m_TubeRadius);
	m_Tube->SetCapping(m_Capping);
	m_Tube->SetNumberOfSides(m_TubeResolution);

	m_Mapper = vtkPolyDataMapper::New();


	if (m_Representation == TUBE)
	{
		m_Tube->Update();
		m_Mapper->SetInput(m_Tube->GetOutput());
	}
	else if (m_Representation == GLYPH)
	{
		m_Glyph->Update();
		vtkAppendPolyData *apd = vtkAppendPolyData::New();
    
    if(m_SplineMode)
    {
      vtkPolyData *splinedPolyData;
      splinedPolyData = SplineProcess(data);
      apd->AddInput(splinedPolyData);
    }
    else
      apd->AddInput(data);

		apd->AddInput(m_Glyph->GetOutput());
		apd->Update();
		m_Mapper->SetInput(apd->GetOutput());
		apd->Delete();
  
	}
	else if (m_Representation == GLYPH_UNCONNECTED)
	{
		m_Glyph->Update();
		vtkAppendPolyData *apd = vtkAppendPolyData::New();
		//apd->AddInput(data);
		apd->AddInput(m_Glyph->GetOutput());
		apd->Update();
		m_Mapper->SetInput(m_Glyph->GetOutput());
		apd->Delete();
	}
	else
	{
		vtkAppendPolyData *apd = vtkAppendPolyData::New();
		apd->AddInput(data);
		apd->Update();
		m_Mapper->SetInput(apd->GetOutput());
		apd->Delete();
	}

	if(m_Vme->IsAnimated())
		m_Mapper->ImmediateModeRenderingOn();
	else
		m_Mapper->ImmediateModeRenderingOff();

	vtkNEW(m_Table);

	m_Table->AddRGBPoint(sr[0],0.0,0.0,1.0);
	m_Table->AddRGBPoint((sr[0]+sr[1])/2,0.0,1.0,0.0);;
	m_Table->AddRGBPoint(sr[1],1.0,0.0,0.0);
	m_Table->Build();

// 	m_Glyph->Update();

	m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);
	m_PolylineMaterial = out_polyline->GetMaterial();
  if(m_PolylineMaterial)
  {
    if (m_PolylineMaterial->m_MaterialType == mmaMaterial::USE_LOOKUPTABLE)
    {
      /*m_UseVTKProperty = 0;
      m_UseLookupTable = 1;*/
      m_Mapper->SetScalarModeToUsePointData();
      m_Mapper->ScalarVisibilityOn();
      m_Mapper->SetLookupTable(m_Table);
      m_Mapper->SetScalarRange(sr);
    }
    if (m_PolylineMaterial->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
    {
      /*m_UseVTKProperty = 1;
      m_UseLookupTable = 0;*/
      m_Actor->SetProperty(m_PolylineMaterial->m_Prop);
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


  m_BorderMapper = vtkPolyDataMapper::New();
  m_BorderMapper->SetInput(BorderCreation());

  m_BorderProperty = vtkProperty::New();
  m_BorderProperty->SetColor(1,1,1);
  m_BorderProperty->SetAmbient(1);
  m_BorderProperty->SetRepresentationToWireframe();
  m_BorderProperty->SetInterpolationToFlat();

  m_BorderActor = vtkActor::New();
  m_BorderActor->SetMapper(m_BorderMapper);
  m_BorderActor->SetProperty(m_BorderProperty);
  m_BorderActor->PickableOff();
  
  if(m_HalfNumberOfBorders == 0)
  {
    m_BorderActor->VisibilityOff();
    for(int j=0;j<m_CaptionActorList.size();j++)
    {
      m_CaptionActorList[j]->SetVisibility(false);
    }
  }
  else
  {
    m_BorderActor->VisibilityOn();
  }
    
  m_AssemblyFront->AddPart(m_BorderActor);
  m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
mafPipePolyline::~mafPipePolyline()
//----------------------------------------------------------------------------
{
	m_Vme->GetEventSource()->RemoveObserver(this);

	m_AssemblyFront->RemovePart(m_Actor);
	m_AssemblyFront->RemovePart(m_OutlineActor);
  m_AssemblyFront->RemovePart(m_BorderActor);
  DeleteCaptionActorList();

	vtkDEL(m_Sphere);
	vtkDEL(m_Glyph);
	vtkDEL(m_Tube);
	vtkDEL(m_PolyFilteredLine);
	vtkDEL(m_Mapper);
	vtkDEL(m_Actor);
	vtkDEL(m_OutlineBox);
	vtkDEL(m_OutlineMapper);
	vtkDEL(m_OutlineProperty);
	vtkDEL(m_OutlineActor);

  vtkDEL(m_BorderData);
  vtkDEL(m_BorderMapper);
  vtkDEL(m_BorderProperty);
  vtkDEL(m_BorderActor);

	vtkDEL(m_Table);
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
  int numberOfArrays = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfArrays();

  if(numberOfArrays)
  {
	  m_ScalarsName = new wxString[numberOfArrays];

    for(int i=0;i<numberOfArrays;i++)
	    m_ScalarsName[i]=m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetArrayName(i);

	  m_Glyph->SelectInputScalars(m_ScalarsName[m_Scalar].c_str());
	  m_Glyph->Modified();
  }

	const wxString representation_string[] = {_("line"), _("tube"), _("sphere"), _("unconnected sphere")};
	int num_choices = 4;
	m_Gui = new mafGUI(this);
	m_Gui->Combo(ID_SCALAR,"",&m_Scalar,numberOfArrays,m_ScalarsName);
  m_Gui->Bool(ID_SPLINE,_("spline"),&m_SplineMode);
  m_Gui->Double(ID_DISTANCE_BORDER,_("XY borders"),&m_DistanceBorder,0);
	m_Gui->Combo(ID_POLYLINE_REPRESENTATION,"",&m_Representation,num_choices,representation_string);
	m_Gui->Label(_("tube"));
	m_Gui->Double(ID_TUBE_RADIUS,_("radius"),&m_TubeRadius,0);
	m_Gui->Double(ID_TUBE_RESOLUTION,_("resolution"),&m_TubeResolution,0);
	m_Gui->Bool(ID_TUBE_CAPPING,_("capping"),&m_Capping);
	m_Gui->Divider(2);
	m_Gui->Label(_("sphere"));
	m_Gui->Bool(ID_SCALAR_DIMENSION,_("scalar dim."),&m_ScalarDim,0,_("Check to scale the sphere radius proportional to the selected scalars"));
	m_Gui->Double(ID_SPHERE_RADIUS,_("radius"),&m_SphereRadius,0);
	m_Gui->Double(ID_SPHERE_RESOLUTION,_("resolution"),&m_SphereResolution,0);

  m_Gui->Enable(ID_SCALAR, m_ScalarsName != NULL);
	m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBE);
	m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBE);
	m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBE);
	m_Gui->Enable(ID_SPHERE_RADIUS, m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED);
	m_Gui->Enable(ID_SPHERE_RESOLUTION, m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED);
	m_Gui->Divider();
  m_MaterialButton = new mafGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_Gui->Divider();

	return m_Gui;
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
				mafTagItem *item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
				item->SetValue(m_Representation);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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
		  case ID_SCALAR:
			{
				UpdateScalars();
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
      case ID_SPLINE:
      {
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPLINE_MODE");
        item->SetValue(m_SplineMode);
        UpdateProperty();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        break;
      }
      case ID_DISTANCE_BORDER:
      {
        UpdateProperty();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
		  default:
			  mafEventMacro(*e);
			break;
		}
	}
	else if(maf_event->GetId() == VME_TIME_SET)
	{
		UpdateScalars();
    UpdatePipeFromScalars();
    UpdateProperty();
	}
  else if (maf_event->GetSender() == m_Vme)
  {
    if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
    {
      UpdateData();
      UpdateProperty();
      UpdatePipeFromScalars();
    }
  }
  
}
//----------------------------------------------------------------------------
void mafPipePolyline::UpdateScalars()
//----------------------------------------------------------------------------
{
  if(m_ScalarsName == NULL) 
    return;

	mafVMEOutputPolyline *polyline_output = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
	vtkDataSet *data = polyline_output->GetVTKData();
  data->Update();

	data->GetPointData()->SetActiveScalars(m_ScalarsName[m_Scalar].c_str());
	polyline_output->Update();
	m_Vme->Modified();

  mafVMEGenericAbstract *genAbst = mafVMEGenericAbstract::SafeDownCast(m_Vme);
  if(NULL == genAbst)
  {
    m_Vme->Modified();
    m_Vme->Update();
    UpdatePipeFromScalars();
    return;
  }

	for (mafDataVector::Iterator it = genAbst->GetDataVector()->Begin(); it != genAbst->GetDataVector()->End(); it++)
	{
		mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
		assert(item);

		vtkPolyData *outputVTK = vtkPolyData::SafeDownCast(item->GetData());
		if(outputVTK)
		{
			outputVTK->GetPointData()->SetActiveScalars(m_ScalarsName[m_Scalar].c_str());
			outputVTK->Update();
			outputVTK->Modified();
		}
	}
	m_Vme->Modified();
	m_Vme->Update();

  UpdatePipeFromScalars();	
}
//----------------------------------------------------------------------------
void mafPipePolyline::UpdatePipeFromScalars()
//----------------------------------------------------------------------------
{
  mafVMEOutputPolyline *polyline_output = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  polyline_output->GetVTKData()->Update();
  polyline_output->Update();

  vtkPolyData *data = vtkPolyData::SafeDownCast(polyline_output->GetVTKData());
  double sr[2];
  if(data->GetPointData()->GetScalars() == NULL) 
    return; 
  data->GetPointData()->GetScalars()->Modified();
  data->GetPointData()->GetScalars()->GetRange(sr);

  m_Table->RemoveAllPoints();
  m_Table->AddRGBPoint(sr[0],0.0,0.0,1.0);
  m_Table->AddRGBPoint((sr[0]+sr[1])/2,0.0,1.0,0.0);;
  m_Table->AddRGBPoint(sr[1],1.0,0.0,0.0);
  m_Table->Build();

  m_Glyph->SelectInputScalars(data->GetPointData()->GetScalars()->GetName());
  m_Glyph->SetRange(sr);
  m_Glyph->Update();

  m_Mapper->SetLookupTable(m_Table);
  m_Mapper->SetScalarRange(data->GetPointData()->GetScalars()->GetRange());
  m_Mapper->Update();

  m_Actor->Modified();

  UpdateProperty();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

}
//----------------------------------------------------------------------------
void mafPipePolyline::UpdateData()
//----------------------------------------------------------------------------
{
	m_Vme->GetOutput()->GetVTKData()->Update();
	m_Vme->Update();
	mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
	out_polyline->Update();
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
	data->Modified();
	data->Update();

	m_OutlineBox->SetInput(data);
	m_OutlineBox->Update();
	m_OutlineBox->Modified();

	m_OutlineMapper->Update();

	m_OutlineActor->Modified();

	if(m_Representation == TUBE || m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED)
	{
    if(m_Vme->GetTagArray()->GetTag("TUBE_RADIUS"))
    {
      m_TubeRadius = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS")->GetValueAsDouble();
      m_Tube->SetRadius(m_TubeRadius);
    }

		m_Tube->SetInput(data);
		m_Tube->Update();
		m_Glyph->SetInput(data);
		m_Glyph->Update();
	}
	else
		m_Mapper->SetInput(data);
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

  if(m_SplineMode && m_Representation != GLYPH && m_Representation != GLYPH_UNCONNECTED)
    data = SplineProcess(data);
  else 
    data = LineProcess(data);

	data->Modified();
	data->Update();

  if(m_Mapper)
  {
	  if (m_Representation == TUBE)
	  {
      m_Tube->SetInput(data);
      m_Tube->Update();
      m_Mapper->SetInput(m_Tube->GetOutput());
	  }
	  else if (m_Representation == GLYPH)
	  {
		  /*if(!m_ScalarDim)
			  m_Glyph->SetScaleModeToDataScalingOff();
		  else
			  m_Glyph->SetScaleModeToScaleByScalar();*/

		  //m_Glyph->SetScaleFactor(m_SphereRadius);
  		
      m_Glyph->Update();
		  m_Glyph->Modified();
		  vtkAppendPolyData *apd = vtkAppendPolyData::New();
    	
      if(m_SplineMode)
      {
        vtkPolyData *splinedPolyData;
        splinedPolyData = SplineProcess(data);
        apd->AddInput(splinedPolyData);
      }
      else
        apd->AddInput(data);

      apd->AddInput(m_Glyph->GetOutput());
		  apd->Update();
		  m_Mapper->SetInput(apd->GetOutput());
		  apd->Delete();
	  }
	  else if (m_Representation == GLYPH_UNCONNECTED)
	  {
		  /*if(!m_ScalarDim)
			  m_Glyph->SetScaleModeToDataScalingOff();
		  else
			  m_Glyph->SetScaleModeToScaleByScalar();*/
  		
		  //m_Glyph->SetScaleFactor(m_SphereRadius);
      
      m_Glyph->SetInput(data);
		  m_Glyph->Update();
		  m_Glyph->Modified();
		  vtkAppendPolyData *apd = vtkAppendPolyData::New();
		  //apd->AddInput(data);
		  apd->AddInput(m_Glyph->GetOutput());
		  apd->Update();
		  m_Mapper->SetInput(apd->GetOutput());
		  apd->Delete();
	  }
	  else
	  {
		  vtkAppendPolyData *apd = vtkAppendPolyData::New();
		  apd->AddInput(data);
		  apd->Update();
		  m_Mapper->SetInput(apd->GetOutput());
		  apd->Delete();
	  }
  }

  if(m_BorderMapper)
  {
    if(m_HalfNumberOfBorders == 0)
    {
      m_BorderActor->VisibilityOff();
      for(int j=0;j<m_CaptionActorList.size();j++)
      {
        m_CaptionActorList[j]->SetVisibility(false);
        m_CaptionActorList[j]->Modified();
      }
    }
    else
    {
      m_BorderMapper->SetInput(BorderCreation());
      m_BorderMapper->Modified();
      m_BorderActor->SetMapper(m_BorderMapper);
      m_BorderActor->VisibilityOn();
    }
  }
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
//----------------------------------------------------------------------------
{
	if (representation < POLYLINE)
	{
		m_Representation = POLYLINE;
	}
	else if (representation > GLYPH_UNCONNECTED)
	{
		m_Representation = GLYPH_UNCONNECTED;
	}
	else
		m_Representation = representation;

	if (m_Gui)
	{
		m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBE);
		m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBE);
		m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBE);
		m_Gui->Enable(ID_SPHERE_RADIUS, m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED);
		m_Gui->Enable(ID_SPHERE_RESOLUTION, m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED);
    m_Gui->Enable(ID_SPLINE, m_Representation != GLYPH_UNCONNECTED);
	}
	UpdateProperty();
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
vtkPolyData *mafPipePolyline::SplineProcess(vtkPolyData *polyData)
//----------------------------------------------------------------------------
{
  //cleaned point list
  vtkPoints *pts;
  vtkMAFSmartPointer<vtkPoints> ptsSplined;
  
  //Clear old data
  if (m_PolyFilteredLine!=NULL)
    vtkDEL(m_PolyFilteredLine);

  vtkNEW(m_PolyFilteredLine);

  m_PolyFilteredLine->DeepCopy(polyData);

  vtkCellArray *cellArray;
  vtkNEW(cellArray);

  vtkCellArray *lines=polyData->GetLines();
  vtkIdType *linePoints;
  vtkIdType linePointsNum;
  int evaluedPoints=0;
  int cellID=0;

  pts=polyData->GetPoints();

  //generating one spline for each branch (cell) of input polyline
  for(int lin=0;lin<polyData->GetNumberOfLines();lin++)
  {

    vtkMAFSmartPointer<vtkCardinalSpline> splineX;
    vtkMAFSmartPointer<vtkCardinalSpline> splineY;
    vtkMAFSmartPointer<vtkCardinalSpline> splineZ;
    int branchStart=evaluedPoints;

    lines->GetCell(cellID,linePointsNum,linePoints);
    cellID+=linePointsNum+1;

    for(int i=0 ; i<linePointsNum; i++)
    {
      double *point=pts->GetPoint(linePoints[i]);
      splineX->AddPoint(i, point[0]);
      splineY->AddPoint(i, point[1]);
      splineZ->AddPoint(i, point[2]);
    }

    for(int i=0 ; i<(linePointsNum * m_SplineCoefficient); i++)
    {		 
      double t;
      t = ( linePointsNum - 1.0 ) / ( linePointsNum*m_SplineCoefficient - 1.0 ) * i;
      ptsSplined->InsertPoint(evaluedPoints , splineX->Evaluate(t), splineY->Evaluate(t), splineZ->Evaluate(t));
      evaluedPoints++;
    }

    cellArray->InsertNextCell(evaluedPoints-branchStart);
    for(int i = branchStart; i< evaluedPoints;i++)
    {
      cellArray->InsertCellPoint(i);
    }
  }

  m_PolyFilteredLine->SetPoints(ptsSplined);
  m_PolyFilteredLine->Update();

  m_PolyFilteredLine->SetLines(cellArray);
  m_PolyFilteredLine->Modified();
  m_PolyFilteredLine->Update();

  vtkDEL(cellArray);

  return m_PolyFilteredLine;
}


vtkPolyData * mafPipePolyline::LineProcess( vtkPolyData *polyData )
{
  //cleaned point list
  vtkPoints *pts;

  if (m_PolyFilteredLine==NULL)
    vtkNEW(m_PolyFilteredLine);

  vtkCellArray *cellArray;
  vtkNEW(cellArray);

  m_PolyFilteredLine->DeepCopy(polyData);

  vtkCellArray *lines=polyData->GetLines();
  vtkIdType *linePoints;
  double oldPoint[3],currPoint[3];
  vtkIdType linePointsNum;
  int evaluedPoints=0;
  int cellID=0;
  polyData->Update();
  pts=polyData->GetPoints();
  vtkPointData *pointData=polyData->GetPointData();
  int nArray=pointData->GetNumberOfArrays();
  
  
  //generating one branch for each branch (cell) of input polyline
  for(int lin=0;lin<polyData->GetNumberOfLines();lin++)
  {

    int branchStart=evaluedPoints;
    int cellSize=1; //is 1 for the first point

    lines->GetCell(cellID,linePointsNum,linePoints);
    cellID+=linePointsNum+1;

    pts->GetPoint(linePoints[0],oldPoint);
    for(int i=1; i<linePointsNum; i++)
    {
      pts->GetPoint(linePoints[i],currPoint);
      //adding points only if is not the same of the previsous;
      if (currPoint[0]!=oldPoint[0] || currPoint[1]!=oldPoint[1] || currPoint[2]!=oldPoint[2]) 
        cellSize++;
      pts->GetPoint(linePoints[i],oldPoint);
    }

    if (cellSize>1)
    {
      cellArray->InsertNextCell(cellSize);
      cellArray->InsertCellPoint(linePoints[0]);

      pts->GetPoint(linePoints[0],oldPoint);
      for(int i=1; i<linePointsNum; i++)
      {
        pts->GetPoint(linePoints[i],currPoint);
        //adding points only if is not the same of the previsous;
        if (currPoint[0]!=oldPoint[0] || currPoint[1]!=oldPoint[1] || currPoint[2]!=oldPoint[2]) 
          cellArray->InsertCellPoint(linePoints[i]);

        pts->GetPoint(linePoints[i],oldPoint);
      }
    }
  }

  m_PolyFilteredLine->SetPoints(polyData->GetPoints());
  m_PolyFilteredLine->Update();

  m_PolyFilteredLine->SetLines(cellArray);
  m_PolyFilteredLine->Modified();
  m_PolyFilteredLine->Update();

  vtkDEL(cellArray);

  return m_PolyFilteredLine;
}

//----------------------------------------------------------------------------
void mafPipePolyline::SetMapperScalarRange(double range[2])
//----------------------------------------------------------------------------
{
  m_Mapper->SetScalarRange(range);
	m_Mapper->Update();
}
//----------------------------------------------------------------------------
void mafPipePolyline::SetLookupTableColorRange(double range[2], double colorMin[3], double colorMax[3])
//----------------------------------------------------------------------------
{
	m_Table->RemoveAllPoints();
	m_Table->AddRGBPoint(range[0],colorMin[0],colorMin[1],colorMin[2]);	
	m_Table->AddRGBPoint(range[1],colorMax[0],colorMax[1],colorMax[2]);
	m_Table->Build();

	m_Mapper->SetLookupTable(m_Table);
	m_Mapper->Update();
}

//----------------------------------------------------------------------------
vtkPolyData *mafPipePolyline::BorderCreation()
//----------------------------------------------------------------------------
{
  mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  assert(out_polyline);
  vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
  assert(data);
  data->Update();

  vtkNEW(m_BorderData);
  if(m_BorderData->GetNumberOfInputs() !=0)
    m_BorderData->RemoveAllInputs();

  //calculate and create the two parallel lines
  int s = 0;
 
  DeleteCaptionActorList();

  for(; s < m_HalfNumberOfBorders; s++)
  {
    vtkMAFSmartPointer<vtkPolyData> polyUp;
    polyUp->DeepCopy(data); //value original

    vtkPoints *points = polyUp->GetPoints();
    vtkMAFSmartPointer<vtkPoints> temporaryPointsUp;

    vtkMAFSmartPointer<vtkPolyData> polyDown;
    vtkMAFSmartPointer<vtkPoints> temporaryPointsDown;

    if(polyUp->GetNumberOfPoints() == 0)
    {
      ;
    }
    else
    {

      temporaryPointsUp->DeepCopy(points);
      temporaryPointsDown->DeepCopy(points);

      double preiousNormal[3]; //used only for last point

      for(long int j = 0; j < temporaryPointsUp->GetNumberOfPoints(); j++)
      {
        
        double tempPoint1[3];
        double tempPoint2[3];
        if(j != temporaryPointsUp->GetNumberOfPoints()-1)
        {
          temporaryPointsUp->GetPoint(j, tempPoint1);
          temporaryPointsUp->GetPoint(j+1, tempPoint2);
        }
        else
        {
          temporaryPointsUp->GetPoint(j-1, tempPoint1);
          temporaryPointsUp->GetPoint(j, tempPoint2);
        }


        //search the versor
        double versor[3];
        versor[0] = (tempPoint2[0] - tempPoint1[0]);
        versor[1] = (tempPoint2[1] - tempPoint1[1]);
        versor[2] = (tempPoint2[2] - tempPoint1[2]);

        double zAxis[3] = {0,0,1};

        //vectorial product beetween my versor and zAxis
        double perpendicular[3];
        double *u , *v;
        u = versor;
        v = zAxis;

        vtkMath::Cross(versor,zAxis,perpendicular);

        if(j == temporaryPointsUp->GetNumberOfPoints()-2)
        {
          preiousNormal[0] = perpendicular[0];
          preiousNormal[1] = perpendicular[1];
          preiousNormal[2] = perpendicular[2];
        }

        double coord[3];
        coord[0] = perpendicular[0];
        coord[1] = perpendicular[1];
        coord[2] = perpendicular[2];


        if(j == temporaryPointsUp->GetNumberOfPoints()-1)
        {
          coord[0] = preiousNormal[0];
          coord[1] = preiousNormal[1];
          coord[2] = preiousNormal[2];
        }

        vtkMath::Normalize(coord);

        //now I can calculate the the coordinate of the point, distanced by the step
        double newPointUp[3], newPointDown[3];

        if(j != temporaryPointsUp->GetNumberOfPoints()-1)
        {
          newPointDown[0] = ((m_DistanceBorder*(s+1))) * (coord[0]) + tempPoint1[0];
          newPointDown[1] = ((m_DistanceBorder*(s+1))) * (coord[1]) + tempPoint1[1];
          newPointDown[2] = ((m_DistanceBorder*(s+1))) * (coord[2]) + tempPoint1[2];

          newPointUp[0] = ((-m_DistanceBorder*(s+1))) * (coord[0]) + tempPoint1[0];
          newPointUp[1] = ((-m_DistanceBorder*(s+1))) * (coord[1]) + tempPoint1[1];
          newPointUp[2] = ((-m_DistanceBorder*(s+1))) * (coord[2]) + tempPoint1[2];


        }
        else //the last point
        {
          newPointDown[0] = ((m_DistanceBorder*(s+1))) * (coord[0]) + tempPoint2[0];
          newPointDown[1] = ((m_DistanceBorder*(s+1))) * (coord[1]) + tempPoint2[1];
          newPointDown[2] = ((m_DistanceBorder*(s+1))) * (coord[2]) + tempPoint2[2];

          newPointUp[0] = ((-m_DistanceBorder*(s+1))) * (coord[0]) + tempPoint2[0];
          newPointUp[1] = ((-m_DistanceBorder*(s+1))) * (coord[1]) + tempPoint2[1];
          newPointUp[2] = ((-m_DistanceBorder*(s+1))) * (coord[2]) + tempPoint2[2];
        }

        if(j==0)
        {
          mafTransform t;
          t.SetMatrix(*m_Vme->GetOutput()->GetAbsMatrix());
          
          if(s == 0)
          {
            double p0[3];
            t.TransformPoint(tempPoint1, p0);
            SetCaptionActorBorder(m_HalfNumberOfBorders, p0);
          }

          double pUp[3];
          t.TransformPoint(newPointUp, pUp);
          SetCaptionActorBorder(m_HalfNumberOfBorders-s-1,pUp);

          double pDown[3];
          t.TransformPoint(newPointDown, pDown);
          SetCaptionActorBorder(m_HalfNumberOfBorders+s+1,pDown);
        }

        temporaryPointsUp->SetPoint(j, newPointUp);
        temporaryPointsDown->SetPoint(j, newPointDown);

      }

    }

    vtkMAFSmartPointer<vtkCellArray> cellArrayUp;
    vtkMAFSmartPointer<vtkCellArray> cellArrayDown;

    int pointId[2];
    for(int i = 0; i< temporaryPointsUp->GetNumberOfPoints();i++)
    {
      if (i > 0)
      {             
        pointId[0] = i - 1;
        pointId[1] = i;
        cellArrayUp->InsertNextCell(2 , pointId);  
        cellArrayDown->InsertNextCell(2 , pointId);  
      }
    }

    polyUp->SetPoints(temporaryPointsUp);
    polyUp->SetLines(cellArrayUp);
    polyUp->Modified();
    polyUp->Update();

    polyDown->SetPoints(temporaryPointsDown);
    polyDown->SetLines(cellArrayDown);
    polyDown->Modified();
    polyDown->Update();

    m_BorderData->AddInput(polyUp);
    m_BorderData->AddInput(polyDown);
    m_BorderData->Update();
  }

  return m_BorderData->GetOutput();
}
//----------------------------------------------------------------------------
void mafPipePolyline::SetOpacity(double opacity)
//----------------------------------------------------------------------------
{
  m_Opacity = opacity;

  if(m_Actor) m_Actor->GetProperty()->SetOpacity(m_Opacity);
  if(m_OutlineActor) m_OutlineActor->GetProperty()->SetOpacity(m_Opacity);
  if(m_BorderActor) m_BorderActor->GetProperty()->SetOpacity(m_Opacity);
}
//----------------------------------------------------------------------------
void mafPipePolyline::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
  m_Actor->SetPickable(enable);
  m_Actor->Modified();
}

//----------------------------------------------------------------------------
void mafPipePolyline::SetCaptionActorBorder(int index, double position[3])
//----------------------------------------------------------------------------
{
  //caption
  vtkCaptionActor2D *caption = NULL;
  if(m_CaptionActorList.size() != m_HalfNumberOfBorders * 2 + 1)
  {
    m_CaptionActorList.push_back(vtkCaptionActor2D::New());
     caption = m_CaptionActorList[m_CaptionActorList.size()-1];
    caption->SetPosition(-5,-10);
    caption->GetCaptionTextProperty()->SetFontFamilyToArial();
    caption->GetCaptionTextProperty()->BoldOn();
    caption->GetCaptionTextProperty()->AntiAliasingOn();
    caption->GetCaptionTextProperty()->ItalicOff();
    caption->GetCaptionTextProperty()->ShadowOn();
    caption->SetPadding(0);

    caption->LeaderOff();
    caption->ThreeDimensionalLeaderOff();
    

    caption->SetHeight(0.01);
    //m_CaptionActor->SetWidth(0.05);
    caption->BorderOff();

    caption->GetCaptionTextProperty()->SetColor(1.0,1.0,1.0);
  }
  else
  {
    caption = m_CaptionActorList[index];
  }
  
  if(caption)
  {
    if(m_RenFront)
    {
      m_RenFront->AddActor2D(caption);
    }
    
    caption->SetVisibility(m_TextIdentifierBorderVisibility);
    caption->SetCaption(wxString::Format("%d", index));
    caption->SetAttachmentPoint(position[0],position[1],position[2]);
  }
  
}
//----------------------------------------------------------------------------
void mafPipePolyline::DeleteCaptionActorList()
//----------------------------------------------------------------------------
{
  int i=0,size=m_CaptionActorList.size();
  for(;i<size;i++)
  {
    if(m_RenFront)
    {
      m_RenFront->RemoveActor2D(m_CaptionActorList[i]);
    }
    vtkDEL(m_CaptionActorList[i]);
  }
  m_CaptionActorList.clear();
}
