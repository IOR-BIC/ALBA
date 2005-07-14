/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-14 07:42:58 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
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

#include "mafPipeMeter.h"
#include "mafSceneNode.h"
#include "mafDataPipe.h"
#include "mafVMEMeter.h"
#include "mafEventSource.h"
#include "mmaMeter.h"
#include "mmaMaterial.h"
#include "mmgGui.h"
#include "mafVMELandmarkCloud.h"
#include "vtkMAFAssembly.h"

//@@@ #include "mafDecl.h"
//@@@ #include "mafAxes.h"  //SIL. 20-5-2003 added line - 
//@@@ #include "mafMaterial.h"  //SIL. 30-5-2003 added line -
//@@@ #include "mmgMaterialButton.h"
//@@@ #include "mafVmeData.h"

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

mafCxxTypeMacro(mafPipeMeter);

//----------------------------------------------------------------------------
mafPipeMeter::mafPipeMeter() 
//----------------------------------------------------------------------------
{
  m_DataMapper        = NULL;
  m_DataActor         = NULL;
  m_SelectionBox      = NULL;
  m_SelectionMapper   = NULL;
  m_SelectionProperty = NULL;
  m_SelectionActor    = NULL;
  m_Tube              = NULL;
  m_MeterVME          = NULL;
  m_Lut               = NULL;
  m_Caption           = NULL;
}
//----------------------------------------------------------------------------
void mafPipeMeter::Create(mafSceneNode *n/*, bool use_axes*/)
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
  m_MeterVME          = NULL;
  m_Lut               = NULL;
  m_Caption           = NULL;

	//@@@ m_use_axes = use_axes;
  //@@@ m_Vme->UpdateCurrentData();

  assert(m_Vme->IsA("mafVMEMeter"));
  m_MeterVME = mafVMEMeter::SafeDownCast(m_Vme);
  m_MeterVME->GetEventSource()->AddObserver(this);
  assert(m_MeterVME->GetPolylineOutput());
  m_MeterVME->GetPolylineOutput()->Update();
  vtkPolyData *data = m_MeterVME->GetPolylineOutput()->GetPolylineData();
  assert(data);

  vtkNEW(m_Tube);
  m_Tube->UseDefaultNormalOff();
  m_Tube->SetInput(data);
  m_Tube->SetRadius(m_MeterVME->GetMeterRadius());
  m_Tube->SetCapping(m_MeterVME->GetMeterCapping());
  m_Tube->SetNumberOfSides(20);
  m_Tube->UseDefaultNormalOff();

  double *range;
  range = m_MeterVME->GetDistanceRange();

  vtkNEW(m_Lut);
  m_Lut->SetTableRange(range[0],range[1]);
  m_Lut->SetHueRange(0.7,0);
  m_Lut->SetNumberOfTableValues(16);
  m_Lut->Build();

  vtkNEW(m_DataMapper);
  if (m_MeterVME->GetMeterRepresentation() == mafVMEMeter::LINE_REPRESENTATION)
    m_DataMapper->SetInput(data);
  else
  {
    m_Tube->Update();
    m_DataMapper->SetInput(m_Tube->GetOutput());
  }
    
	if(m_MeterVME->IsAnimated())				
		m_DataMapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
	else
		m_DataMapper->ImmediateModeRenderingOff();
  if(m_MeterVME->GetMeterColorMode() == mafVMEMeter::RANGE_COLOR)
    m_DataMapper->SetLookupTable(m_Lut);

  vtkNEW(m_DataActor);
	m_DataActor->SetProperty(m_MeterVME->GetMaterial()->m_Prop);
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
  if(m_MeterVME->GetMeterColorMode() == mafVMEMeter::RANGE_COLOR)
  {
    double c[3];
    m_DataActor->GetProperty()->GetColor(c);
    m_Caption->GetProperty()->SetColor(c);
  }
//  else
//    Caption->GetProperty()->SetColor(data->m_mat_gui->GetMaterial()->m_prop->GetColor());
  m_Caption->SetHeight(0.05);
  m_Caption->SetWidth(0.35);
  m_Caption->BorderOff();
  mafString dis;
  dis << m_MeterVME->GetDistance();
  m_Caption->SetCaption(dis.GetCStr());

  if(m_MeterVME->GetMeterMode() == mafVMEMeter::LINE_ANGLE)
    m_Caption->SetVisibility((m_MeterVME->GetAngle() != 0) && m_MeterVME->GetMeterAttributes()->m_LabelVisibility);
  else
    m_Caption->SetVisibility((m_MeterVME->GetDistance() > 0 || m_MeterVME->GetMeterMeasureType() == mafVMEMeter::RELATIVE_MEASURE) && m_MeterVME->GetMeterAttributes()->m_LabelVisibility);
  if(m_MeterVME->GetStartVME())
  {
    double pos[3], rot[3];
    mafVME *linked_vme = m_MeterVME->GetStartVME();
    /*if(linked_vme && linked_vme->IsA("mafVMELandmarkCloud") && m_MeterVME->GetStartVMELandmarkId() != -1)
    {
      ((mafVMELandmarkCloud *)linked_vme)->GetLandmark(m_MeterVME->GetStartVMELandmarkId(),pos,-1);
      TmpTransform->SetMatrix(linked_vme->GetAbsPose());
      TmpTransform->TransformPoint(pos,pos);
    }
    else*/
      m_MeterVME->GetStartVME()->GetOutput()->GetAbsPose(pos,rot);
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
mafPipeMeter::~mafPipeMeter()
//----------------------------------------------------------------------------
{
  m_MeterVME->GetEventSource()->RemoveObserver(this);

  m_AssemblyFront->RemovePart(m_DataActor);
  m_AssemblyFront->RemovePart(m_SelectionActor);
  m_RenFront->AddActor2D(m_Caption);

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
mmgGui *mafPipeMeter::CreateGui()
//----------------------------------------------------------------------------
{
  const wxString type_measure_string[] = {"absolute", "relative"};
  const wxString representation_string[] = {"line", "tube"};
  const wxString color_string[] = {"one", "range"};
  int num_choices = 2;

  mmaMeter *meter_attrib = m_MeterVME->GetMeterAttributes();

  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_SHOW_LABEL,"label", &meter_attrib->m_LabelVisibility);
  m_Gui->Combo(ID_COLOR_MODE,"color", &meter_attrib->m_ColorMode,num_choices,color_string);
  m_Gui->VectorN(ID_DISTANCE_RANGE,"range",meter_attrib->m_DistanceRange,2,0);
  m_Gui->Combo(ID_METER_REPRESENTATION,"",&meter_attrib->m_Representation,num_choices,representation_string);
  m_Gui->Double(ID_TUBE_RADIUS,"radius",&meter_attrib->m_TubeRadius,0);
  m_Gui->Bool(ID_TUBE_CAPPING,"capping",&meter_attrib->m_Capping);
  m_Gui->Combo(ID_METER_MEASURE_TYPE,"",&meter_attrib->m_MeasureType,num_choices,type_measure_string);
  m_Gui->Double(ID_INIT_MEASURE,"init",&meter_attrib->m_InitMeasure,0);
  m_Gui->Bool(ID_GENERATE_EVENT,"gen. event",&meter_attrib->m_GenerateEvent);
  m_Gui->Double(ID_DELTA_PERCENT,"delta %",&meter_attrib->m_DeltaPercent,0);

  m_Gui->Enable(ID_DISTANCE_RANGE, meter_attrib->m_ColorMode == mafVMEMeter::ONE_COLOR);
  m_Gui->Enable(ID_TUBE_RADIUS, meter_attrib->m_Representation == mafVMEMeter::LINE_REPRESENTATION);
  m_Gui->Enable(ID_TUBE_CAPPING, meter_attrib->m_Representation == mafVMEMeter::LINE_REPRESENTATION);

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeMeter::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    mmaMeter *meter_attrib = m_MeterVME->GetMeterAttributes();
    switch(e->GetId()) 
    {
      case ID_SHOW_LABEL:
        UpdateProperty();
      break;
      case ID_METER_REPRESENTATION:
        m_Gui->Enable(ID_TUBE_RADIUS, meter_attrib->m_Representation == mafVMEMeter::LINE_REPRESENTATION);
        m_Gui->Enable(ID_TUBE_CAPPING, meter_attrib->m_Representation == mafVMEMeter::LINE_REPRESENTATION);
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
        m_MeterVME->GetDataPipe()->Update();
      break;
      default:
      break;
    }
    mafEvent cam_event(this,CAMERA_UPDATE);
    m_MeterVME->ForwardUpEvent(cam_event);
  }
  else if (maf_event->GetSender() == m_MeterVME)
  {
    if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
        UpdateProperty();
    else if(maf_event->GetId() == mafVMEMeter::LENGTH_THRESHOLD_EVENT) 
    {
    }
  }
}
/*
//----------------------------------------------------------------------------
void mafPipeMeter::Show(bool show)
//----------------------------------------------------------------------------
{
	m_DataActor->SetVisibility(show);
	if(m_Selected)
	{
	  m_SelectionActor->SetVisibility(show);
		//@@@ if(m_use_axes) m_axes->SetVisibility(show);
	}
}
*/
//----------------------------------------------------------------------------
void mafPipeMeter::Select(bool sel)
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
void mafPipeMeter::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  //mafVMEMeter *vme = mafVMEMeter::SafeDownCast(m_Vme);
  m_MeterVME->GetPolylineOutput()->Update();
  vtkPolyData *data = m_MeterVME->GetPolylineOutput()->GetPolylineData();
  if (m_MeterVME->GetMeterRepresentation() == mafVMEMeter::LINE_REPRESENTATION)
    m_DataMapper->SetInput(data);
  else
  {
    m_Tube->Update();
    m_DataMapper->SetInput(m_Tube->GetOutput());
  }

  mafString dis;
  dis << m_MeterVME->GetDistance();
  m_Caption->SetCaption(dis.GetCStr());
  m_Caption->SetVisibility(m_MeterVME->GetMeterAttributes()->m_LabelVisibility);

  double rgb[3];
  double v = m_MeterVME->GetDistance();
  int color_mode = m_MeterVME->GetMeterColorMode();
  if(color_mode == mafVMEMeter::RANGE_COLOR)
  {
    double *range;
    m_DataMapper->SetLookupTable(m_Lut);
    range = m_MeterVME->GetDistanceRange();
    m_Lut->SetTableRange(range[0],range[1]);
    m_Lut->Build();
    m_Lut->GetColor(v,rgb);
    m_DataActor->GetProperty()->SetColor(rgb);
    m_Caption->GetProperty()->SetColor(rgb);
  }
  else
  {
    m_DataMapper->SetColorModeToDefault();
    //data->m_mat_gui->GetMaterial()->m_prop->GetColor(rgb);
    m_DataActor->GetProperty()->SetColor(rgb);
    m_Caption->GetProperty()->SetColor(rgb);
  }

  double pos[3], rot[3];
  if (m_MeterVME->GetStartVME())
  {
    m_MeterVME->GetStartVME()->GetOutput()->GetAbsPose(pos,rot);
    m_Caption->SetAttachmentPoint(pos[0],pos[1],pos[2]);
  }
  
  GetGui()->Update();

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
