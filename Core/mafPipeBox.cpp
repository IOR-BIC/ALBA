/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeBox.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:56:04 $
  Version:   $Revision: 1.11 $
  Authors:   Silvano Imboden, Paolo Quadrani
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

#include "mafPipeBox.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafAxes.h"
#include "mafEventSource.h"

#include "mafLODActor.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineSource.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkStripper.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeBox);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeBox::mafPipeBox() 
//----------------------------------------------------------------------------
{
  m_Box             = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;

  m_BoundsMode = 0;
}
//----------------------------------------------------------------------------
void mafPipeBox::Create(mafSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;
  m_Box             = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;

  m_BoundsMode = 0;

	m_Vme->GetEventSource()->AddObserver(this);

  double b[6];
  m_Vme->GetOutput()->Update();
  m_Vme->GetOutput()->GetVMELocalBounds(b);

  vtkNEW(m_Box);
  m_Box->SetBounds(b);

  vtkNEW(m_Mapper);
	m_Mapper->SetInput(m_Box->GetOutput());
  
	if(m_Vme->IsAnimated())
		m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
	else
		m_Mapper->ImmediateModeRenderingOff();

  vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);

  m_AssemblyFront->AddPart(m_Actor);

  // selection highlight
	vtkNEW(m_OutlineBox);
	m_OutlineBox->SetInput(m_Box->GetOutput());

	vtkNEW(m_OutlineMapper);
	m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

	vtkNEW(m_OutlineProperty);
	m_OutlineProperty->SetColor(1,1,1);
	m_OutlineProperty->SetAmbient(1);
	m_OutlineProperty->SetRepresentationToWireframe();
	m_OutlineProperty->SetInterpolationToFlat();

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetScale(1.01);
  m_OutlineActor->SetMapper(m_OutlineMapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(m_OutlineProperty);

  m_AssemblyFront->AddPart(m_OutlineActor);

  if(m_RenFront)
  {
    m_Axes = new mafAxes(m_RenFront, m_Vme);
    m_Axes->SetVisibility(0);
  }
}
//----------------------------------------------------------------------------
mafPipeBox::~mafPipeBox()
//----------------------------------------------------------------------------
{
	m_Vme->GetEventSource()->RemoveObserver(this);

  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

  vtkDEL(m_Box);
	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
	cppDEL(m_Axes);
}
//----------------------------------------------------------------------------
void mafPipeBox::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
		m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeBox::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
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
	  m_Mapper->SetScalarVisibility(((mafVmeData *)m_Vme->GetClientData())->GetColorByScalar());
	*/
	
  if(m_Box)
  {
	  double b[6];
	  m_Vme->GetOutput()->Update();
	  m_Vme->GetOutput()->GetVMELocalBounds(b);

		m_Box->SetBounds(b);
		m_Box->Update();
	}
}
//-------------------------------------------------------------------------
mafGUI *mafPipeBox::CreateGui()
//-------------------------------------------------------------------------
{
  const wxString box_type[] = {_("3D"), _("4D"),_("3D Subtree"),_("4D Subtree")};
  int num_choices = 4;

  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->Combo(ID_BOUNDS_MODE, _("bounds"), &m_BoundsMode, num_choices, box_type);
  m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeBox::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_BOUNDS_MODE:
      {
        double b[6];
        switch(m_BoundsMode) 
        {
          case BOUNDS_3D:
            m_Vme->GetOutput()->GetVMELocalBounds(b);
        	break;
          case BOUNDS_4D:
            m_Vme->GetOutput()->GetVME4DBounds(b);
          break;
          case BOUNDS_3D_SUBTREE:
            m_Vme->GetOutput()->GetBounds(b);
          break;
          default:
            m_Vme->GetOutput()->Get4DBounds(b);
          break;
        }
        if (m_BoundsMode != BOUNDS_3D)
        {
          double min_vector[4], max_vector[4];
          min_vector[0] = b[0];
          min_vector[1] = b[2];
          min_vector[2] = b[4];
          max_vector[0] = b[1];
          max_vector[1] = b[3];
          max_vector[2] = b[5];
          min_vector[3] = max_vector[3] = 1;
          mafMatrix *absMat = m_Vme->GetOutput()->GetAbsMatrix();
          mafMatrix invAbsMat;
          mafMatrix::Invert(*absMat, invAbsMat);
          invAbsMat.MultiplyPoint(min_vector, min_vector);
          invAbsMat.MultiplyPoint(max_vector, max_vector);
          b[0] = min_vector[0];
          b[2] = min_vector[1];
          b[4] = min_vector[2];
          b[1] = max_vector[0];
          b[3] = max_vector[1];
          b[5] = max_vector[2];
        }
        m_Box->SetBounds(b);
      }
   	  break;
      default:
        e->Log();
    }
    //mafEvent cam_event(this,CAMERA_UPDATE);
    //m_Vme->ForwardUpEvent(cam_event);
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
	if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
	{
		UpdateProperty();
	}
}
