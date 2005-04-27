/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-27 16:27:11 $
  Version:   $Revision: 1.1 $
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
#include "mafVMEMeter.h"
#include "vtkMAFAssembly.h"

//@@@ #include "mafDecl.h"
//@@@ #include "mafAxes.h"  //SIL. 20-5-2003 added line - 
//@@@ #include "mafMaterial.h"  //SIL. 30-5-2003 added line -
//@@@ #include "mmgMaterialButton.h"
//@@@ #include "mafVmeData.h"

#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkTubeFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkStripper.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"

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

	//@@@ m_use_axes = use_axes;
	//@@@ mafVmeData *data = (mafVmeData*) m_Vme->GetClientData();
	//@@@ assert(data);
  //@@@ m_Vme->UpdateCurrentData();

  assert(m_Vme->IsA("mafVMEMeter"));
  mafVMEMeter *vme = mafVMEMeter::SafeDownCast(m_Vme);
  assert(vme->GetPolylineOutput());
  vme->GetPolylineOutput()->Update();
  vtkPolyData *data = vme->GetPolylineOutput()->GetPolylineData();
  assert(data);

  vtkNEW(m_Tube);
  m_Tube->UseDefaultNormalOff();
  m_Tube->SetInput(data);
  m_Tube->SetRadius(vme->GetMeterRadius());
  m_Tube->SetCapping(vme->GetMeterCapping());
  m_Tube->SetNumberOfSides(20);
  m_Tube->UseDefaultNormalOff();

  m_DataMapper = vtkPolyDataMapper::New();
  if (vme->GetMeterRepresentation() == mafVMEMeter::LINE_REPRESENTATION)
    m_DataMapper->SetInput(data);
  else
  {
    m_Tube->Update();
    m_DataMapper->SetInput(m_Tube->GetOutput());
  }
    
	if(vme->IsAnimated())				
		m_DataMapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
	else
		m_DataMapper->ImmediateModeRenderingOff();

  m_DataActor = vtkActor::New();
	//@@@ m_DataActor->SetProperty(data->m_mat_gui->GetMaterial()->m_prop);
	m_DataActor->SetMapper(m_DataMapper);

  m_AssemblyFront->AddPart(m_DataActor);

  // selection hilight
	m_SelectionBox = vtkOutlineCornerFilter::New();
	m_SelectionBox->SetInput(data);  

	m_SelectionMapper = vtkPolyDataMapper::New();
	m_SelectionMapper->SetInput(m_SelectionBox->GetOutput());

	m_SelectionProperty = vtkProperty::New();
	m_SelectionProperty->SetColor(1,1,1);
	m_SelectionProperty->SetAmbient(1);
	m_SelectionProperty->SetRepresentationToWireframe();
	m_SelectionProperty->SetInterpolationToFlat();

	m_SelectionActor = vtkActor::New();
	m_SelectionActor->SetMapper(m_SelectionMapper);
	m_SelectionActor->VisibilityOff();
	m_SelectionActor->PickableOff();
	m_SelectionActor->SetProperty(m_SelectionProperty);

  m_AssemblyFront->AddPart(m_SelectionActor);

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
  m_AssemblyFront->RemovePart(m_DataActor);
  m_AssemblyFront->RemovePart(m_SelectionActor);

  vtkDEL(m_Tube);
  vtkDEL(m_DataMapper);
  vtkDEL(m_DataActor);
  vtkDEL(m_SelectionBox);
  vtkDEL(m_SelectionMapper);
  vtkDEL(m_SelectionProperty);
  vtkDEL(m_SelectionActor);
	//@@@ if(m_use_axes) wxDEL(m_axes);  
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
  mafVMEMeter *vme = mafVMEMeter::SafeDownCast(m_Vme);
  vme->GetPolylineOutput()->Update();
  vtkPolyData *data = vme->GetPolylineOutput()->GetPolylineData();
  if (vme->GetMeterRepresentation() == mafVMEMeter::LINE_REPRESENTATION)
    m_DataMapper->SetInput(data);
  else
  {
    m_Tube->Update();
    m_DataMapper->SetInput(m_Tube->GetOutput());
  }

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