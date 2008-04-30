/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeWrappedMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-30 13:21:00 $
  Version:   $Revision: 1.4 $
  Authors:   Daniele Giunchi
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

#include "medPipeWrappedMeter.h"
#include "mmgGui.h"
#include "mafSceneNode.h"
#include "mafDataPipe.h"
#include "medVMEWrappedMeter.h"
#include "mafEventSource.h"
#include "mmaMeter.h"
#include "mmaMaterial.h"
#include "mmgMaterialButton.h"
#include "mafVMELandmarkCloud.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"

#include "vtkMAFAssembly.h"

//@@@ #include "mafDecl.h"
//@@@ #include "mafAxes.h"  //SIL. 20-5-2003 added line - 
//@@@ #include "mafMaterial.h"  //SIL. 30-5-2003 added line -
//@@@ #include "mmgMaterialButton.h"

#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkTubeFilter.h"
#include "vtkLookupTable.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkCaptionActor2D.h"

mafCxxTypeMacro(medPipeWrappedMeter);

//----------------------------------------------------------------------------
medPipeWrappedMeter::medPipeWrappedMeter() 
//----------------------------------------------------------------------------
{
  m_DataMapper        = NULL;
  m_DataActor         = NULL;
  m_SelectionBox      = NULL;
  m_SelectionMapper   = NULL;
  m_SelectionProperty = NULL;
  m_SelectionActor    = NULL;
  m_Tube              = NULL;
  m_WrappedMeterVME          = NULL;
  m_Lut               = NULL;
  m_Caption           = NULL;
  m_MaterialButton    = NULL;
}
//----------------------------------------------------------------------------
void medPipeWrappedMeter::Create(mafSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected          = false;
  m_DataMapper        = NULL;
  m_DataActor         = NULL;
  m_SelectionBox      = NULL;
  m_SelectionMapper   = NULL;
  m_SelectionProperty = NULL;
  m_SelectionActor    = NULL;
  m_Tube              = NULL;
  m_WrappedMeterVME          = NULL;
  m_Lut               = NULL;
  m_Caption           = NULL;

  assert(m_Vme->IsA("medVMEWrappedMeter"));
  m_WrappedMeterVME = medVMEWrappedMeter::SafeDownCast(m_Vme);
  m_WrappedMeterVME->GetEventSource()->AddObserver(this);
  assert(m_WrappedMeterVME->GetWrappedMeterOutput());
  m_WrappedMeterVME->GetWrappedMeterOutput()->Update();
  vtkPolyData *data = m_WrappedMeterVME->GetWrappedMeterOutput()->GetPolylineData();
  assert(data);

  vtkNEW(m_Tube);
  m_Tube->UseDefaultNormalOff();
  m_Tube->SetInput(data);
  m_Tube->SetRadius(m_WrappedMeterVME->GetMeterRadius());
  m_Tube->SetCapping(m_WrappedMeterVME->GetMeterCapping());
  m_Tube->SetNumberOfSides(20);
  m_Tube->UseDefaultNormalOff();

  double *range;
  range = m_WrappedMeterVME->GetDistanceRange();

  vtkNEW(m_Lut);
  m_Lut->SetTableRange(range[0],range[1]);
  m_Lut->SetHueRange(0.7,0);
  m_Lut->SetNumberOfTableValues(16);
  m_Lut->Build();

  vtkNEW(m_DataMapper);
  if (m_WrappedMeterVME->GetMeterRepresentation() == medVMEWrappedMeter::LINE_REPRESENTATION)
    m_DataMapper->SetInput(data);
  else
  {
    m_Tube->Update();
    m_DataMapper->SetInput(m_Tube->GetOutput());
  }
    
	m_DataMapper->ImmediateModeRenderingOff();
  if(m_WrappedMeterVME->GetMeterColorMode() == medVMEWrappedMeter::RANGE_COLOR)
    m_DataMapper->SetLookupTable(m_Lut);

  vtkNEW(m_DataActor);
	m_DataActor->SetProperty(m_WrappedMeterVME->GetMaterial()->m_Prop);
	m_DataActor->SetMapper(m_DataMapper);

  m_AssemblyFront->AddPart(m_DataActor);

  // selection hilight
	vtkNEW(m_SelectionBox);
	m_SelectionBox->SetInput(data);  

	vtkNEW(m_SelectionMapper);
	m_SelectionMapper->SetInput(m_SelectionBox->GetOutput());

	vtkNEW(m_SelectionProperty);
	m_SelectionProperty->SetColor(1,1,1);
	m_SelectionProperty->SetAmbient(1);
	m_SelectionProperty->SetRepresentationToWireframe();
	m_SelectionProperty->SetInterpolationToFlat();

	vtkNEW(m_SelectionActor);
	m_SelectionActor->SetMapper(m_SelectionMapper);
	m_SelectionActor->VisibilityOff();
	m_SelectionActor->PickableOff();
	m_SelectionActor->SetProperty(m_SelectionProperty);

  m_AssemblyFront->AddPart(m_SelectionActor);

  vtkNEW(m_Caption);
  m_Caption->SetPosition(25,10);
  m_Caption->ThreeDimensionalLeaderOff();
  if(m_WrappedMeterVME->GetMeterColorMode() != medVMEWrappedMeter::RANGE_COLOR)
  {
    double c[3];
    m_DataActor->GetProperty()->GetColor(c);
    m_Caption->GetProperty()->SetColor(c);
  }
  else
  {
    m_DataActor->GetProperty()->SetColor(m_WrappedMeterVME->GetMaterial()->m_Diffuse);
    m_Caption->GetProperty()->SetColor(m_WrappedMeterVME->GetMaterial()->m_Diffuse);
  }
//  else
//    Caption->GetProperty()->SetColor(data->m_mat_gui->GetMaterial()->m_prop->GetColor());
  m_Caption->SetHeight(0.05);
  m_Caption->SetWidth(0.35);
  m_Caption->BorderOff();
  wxString dis;
  dis = wxString::Format("%.2f",m_WrappedMeterVME->GetDistance());
  m_Caption->SetCaption(dis.c_str());

 /* if(m_WrappedMeterVME->GetMeterMode() == medVMEWrappedMeter::LINE_ANGLE)
    m_Caption->SetVisibility((m_WrappedMeterVME->GetAngle() != 0) && m_WrappedMeterVME->GetMeterAttributes()->m_LabelVisibility);
  else*/
    m_Caption->SetVisibility((m_WrappedMeterVME->GetDistance() >= 0 || m_WrappedMeterVME->GetMeterMeasureType() == medVMEWrappedMeter::RELATIVE_MEASURE) && m_WrappedMeterVME->GetMeterAttributes()->m_LabelVisibility);
  
  if(m_WrappedMeterVME->GetStartVME())
  {
    double pos[3], rot[3];
    mafVME *linked_vme = m_WrappedMeterVME->GetStartVME();
    mafSmartPointer<mafTransform> TmpTransform;
    if(linked_vme && linked_vme->IsMAFType(mafVMELandmarkCloud) && m_WrappedMeterVME->GetLinkSubId("StartVME") != -1)
    {
      ((mafVMELandmarkCloud *)linked_vme)->GetLandmark(m_WrappedMeterVME->GetLinkSubId("StartVME"),pos,-1);
      TmpTransform->SetMatrix(*linked_vme->GetOutput()->GetAbsMatrix());
      TmpTransform->TransformPoint(pos,pos);
    }
    else
      m_WrappedMeterVME->GetStartVME()->GetOutput()->GetAbsPose(pos,rot);
    m_Caption->SetAttachmentPoint(pos[0],pos[1],pos[2]);
  } 

  m_RenFront->AddActor2D(m_Caption);
  /*
  m_axes = NULL;
	if(m_use_axes) m_axes = new mafAxes(m_ren1,m_Vme);
	if(m_use_axes) m_axes->SetVisibility(0);
	*/
}
//----------------------------------------------------------------------------
medPipeWrappedMeter::~medPipeWrappedMeter()
//----------------------------------------------------------------------------
{
  m_WrappedMeterVME->GetEventSource()->RemoveObserver(this);

  m_AssemblyFront->RemovePart(m_DataActor);
  m_AssemblyFront->RemovePart(m_SelectionActor);
  m_RenFront->RemoveActor2D(m_Caption);

  vtkDEL(m_Tube);
  vtkDEL(m_Lut);
  vtkDEL(m_Caption);
  vtkDEL(m_DataMapper);
  vtkDEL(m_DataActor);
  vtkDEL(m_SelectionBox);
  vtkDEL(m_SelectionMapper);
  vtkDEL(m_SelectionProperty);
  vtkDEL(m_SelectionActor);
	//@@@ if(m_use_axes) wxDEL(m_axes);  
}
//----------------------------------------------------------------------------
mmgGui *medPipeWrappedMeter::CreateGui()
//----------------------------------------------------------------------------
{
  const wxString type_measure_string[] = {"absolute", "relative"};
  const wxString representation_string[] = {"line", "tube"};
  const wxString color_string[] = {"one", "range"};
  int num_choices = 2;

  mmaMeter *meter_attrib = m_WrappedMeterVME->GetMeterAttributes();

  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_SHOW_LABEL,"label", &meter_attrib->m_LabelVisibility);
  m_Gui->Combo(ID_COLOR_MODE,"color", &meter_attrib->m_ColorMode,num_choices,color_string);
  m_MaterialButton = new mmgMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_Gui->VectorN(ID_DISTANCE_RANGE,"range",meter_attrib->m_DistanceRange,2,0);
  m_Gui->Combo(ID_METER_REPRESENTATION,"",&meter_attrib->m_Representation,num_choices,representation_string);
  m_Gui->Double(ID_TUBE_RADIUS,"radius",&meter_attrib->m_TubeRadius,0);
  m_Gui->Bool(ID_TUBE_CAPPING,"capping",&meter_attrib->m_Capping);
  m_Gui->Combo(ID_METER_MEASURE_TYPE,"",&meter_attrib->m_MeasureType,num_choices,type_measure_string);
  m_Gui->Double(ID_INIT_MEASURE,"init",&meter_attrib->m_InitMeasure,0);
  m_Gui->Bool(ID_GENERATE_EVENT,"gen. event",&meter_attrib->m_GenerateEvent);
  m_Gui->Double(ID_DELTA_PERCENT,"delta %",&meter_attrib->m_DeltaPercent,0);

  m_MaterialButton->Enable(meter_attrib->m_ColorMode == medVMEWrappedMeter::ONE_COLOR);
  m_Gui->Enable(ID_DISTANCE_RANGE,meter_attrib->m_ColorMode == medVMEWrappedMeter::RANGE_COLOR);
  m_Gui->Enable(ID_TUBE_RADIUS, meter_attrib->m_Representation == medVMEWrappedMeter::TUBE_REPRESENTATION);
  m_Gui->Enable(ID_TUBE_CAPPING, meter_attrib->m_Representation == medVMEWrappedMeter::TUBE_REPRESENTATION);
	m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void medPipeWrappedMeter::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    mmaMeter *meter_attrib = m_WrappedMeterVME->GetMeterAttributes();
    switch(e->GetId()) 
    {
      case ID_SHOW_LABEL:
        UpdateProperty();
      break;
      case ID_COLOR_MODE:
        m_Gui->Enable(ID_DISTANCE_RANGE,meter_attrib->m_ColorMode == medVMEWrappedMeter::RANGE_COLOR);
        m_MaterialButton->Enable(meter_attrib->m_ColorMode == medVMEWrappedMeter::ONE_COLOR);
        UpdateProperty();
      break;
      case ID_METER_REPRESENTATION:
        m_Gui->Enable(ID_TUBE_RADIUS, meter_attrib->m_Representation == medVMEWrappedMeter::TUBE_REPRESENTATION);
        m_Gui->Enable(ID_TUBE_CAPPING, meter_attrib->m_Representation == medVMEWrappedMeter::TUBE_REPRESENTATION);
        UpdateProperty();
      break;
      case ID_TUBE_RADIUS:
        m_Tube->SetRadius(meter_attrib->m_TubeRadius);
      break;
      case ID_TUBE_CAPPING:
        m_Tube->SetCapping(meter_attrib->m_Capping);
      break;
      case ID_DISTANCE_RANGE:
      break;
      case ID_METER_MEASURE_TYPE:
      case ID_INIT_MEASURE:
      case ID_DELTA_PERCENT:
      case ID_GENERATE_EVENT:
        m_WrappedMeterVME->GetDataPipe()->Update();
      break;
      default:
        m_WrappedMeterVME->ForwardUpEvent(*e);
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
  else if (maf_event->GetSender() == m_WrappedMeterVME)
  {
    if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
    {
      UpdateProperty();
    }
    else if(maf_event->GetId() == medVMEWrappedMeter::LENGTH_THRESHOLD_EVENT) 
    {
    }
  }
}
//----------------------------------------------------------------------------
void medPipeWrappedMeter::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_DataActor->GetVisibility()) 
	{
			m_SelectionActor->SetVisibility(sel);
			//@@@ if(m_use_axes) m_axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void medPipeWrappedMeter::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  if (!m_DataMapper)
    return;
  

  if (!m_DataActor)
    return;

  if (!m_Caption)
    return;

  
	vtkPolyData *data =vtkPolyData::SafeDownCast(m_WrappedMeterVME->GetWrappedMeterOutput()->GetVTKData());
  data->Update();
  if (m_WrappedMeterVME->GetMeterRepresentation() == medVMEWrappedMeter::LINE_REPRESENTATION)
    m_DataMapper->SetInput(data);
  else
  {
    m_Tube->Update();
    m_DataMapper->SetInput(m_Tube->GetOutput());
  }

  double distance_value = m_WrappedMeterVME->GetDistance();
 // if(m_WrappedMeterVME->GetMeterMode() == medVMEWrappedMeter::LINE_ANGLE) distance_value = m_WrappedMeterVME->GetAngle();
  distance_value = RoundValue(distance_value);
  wxString dis;
  dis = wxString::Format("%.2f",distance_value);
  m_Caption->SetCaption(dis.c_str());
  m_Caption->SetVisibility(m_WrappedMeterVME->GetMeterAttributes()->m_LabelVisibility);

  double rgb[3];
  double v = m_WrappedMeterVME->GetDistance();
  int color_mode = m_WrappedMeterVME->GetMeterColorMode();
  if(color_mode == medVMEWrappedMeter::RANGE_COLOR)
  {
    double *range;
    m_DataMapper->SetLookupTable(m_Lut);
    range = m_WrappedMeterVME->GetDistanceRange();
    m_Lut->SetTableRange(range[0],range[1]);
    m_Lut->Build();
    m_Lut->GetColor(v,rgb);
    m_DataActor->GetProperty()->SetColor(rgb);
    m_Caption->GetProperty()->SetColor(rgb);
  }
  else
  {
    m_DataMapper->SetColorModeToDefault();
    m_DataActor->GetProperty()->SetColor(m_WrappedMeterVME->GetMaterial()->m_Diffuse);
    m_Caption->GetProperty()->SetColor(m_WrappedMeterVME->GetMaterial()->m_Diffuse);
  }

  double pos[3] = {0,0,0};
  double rot[3] = {0,0,0};
  mafVME *linked_vme = m_WrappedMeterVME->GetStartVME();
  mafSmartPointer<mafTransform> TmpTransform;
  if(linked_vme && linked_vme->IsMAFType(mafVMELandmarkCloud) && m_WrappedMeterVME->GetLinkSubId("StartVME") != -1)
  {
    ((mafVMELandmarkCloud *)linked_vme)->GetLandmark(m_WrappedMeterVME->GetLinkSubId("StartVME"),pos,-1);
    TmpTransform->SetMatrix(*linked_vme->GetOutput()->GetAbsMatrix());
    TmpTransform->TransformPoint(pos,pos);
  }
  else if(linked_vme)
  {
    linked_vme->GetOutput()->GetAbsPose(pos,rot);
  }
  m_Caption->SetAttachmentPoint(pos[0],pos[1],pos[2]);

  GetGui()->Update();

  m_SelectionBox->SetInput(data); 
  m_SelectionBox->Update();
  
  /*
	if(fromTag)
  {
		((mafVmeData *)m_Vme->GetClientData())->UpdateFromTag();
    int idx = m_Vme->GetTagArray()->FindTag("VME_CENTER_ROTATION_POSE");
    vtkTagItem *item = NULL;
    double vec[16];
    if (idx != -1)
    {
      item = m_Vme->GetTagArray()->GetTag(idx);
      mflSmartPointer<vtkMatrix4x4> pose;
      for (int el=0;el<16;el++)
      {
        vec[el] = item->GetValueAsDouble(el);
      }
      pose->DeepCopy(vec);
      m_axes->SetPose(pose);
    }
    else
      m_axes->SetPose();
  }
  else
	  m_DataMapper->SetScalarVisibility(((mafVmeData *)m_Vme->GetClientData())->GetColorByScalar());
	*/
}
