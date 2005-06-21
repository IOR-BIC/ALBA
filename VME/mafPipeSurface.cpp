/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-21 09:49:35 $
  Version:   $Revision: 1.7 $
  Authors:   Silvano Imboden
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

#include "mafPipeSurface.h"
#include "mafSceneNode.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"
#include "mmgGui.h"

#include "vtkMAFAssembly.h"

//@@@ #include "mafDecl.h"
//@@@ #include "mafAxes.h"  //SIL. 20-5-2003 added line - 
//@@@ #include "mafMaterial.h"  //SIL. 30-5-2003 added line -
//@@@ #include "mmgMaterialButton.h"
//@@@ #include "mafVmeData.h"

#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkStripper.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
//@@@ #include "vtkMultiResolutionActor.h"
#include "vtkActor.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeSurface::mafPipeSurface() 
//----------------------------------------------------------------------------
{
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;

  m_ScalarVisibility = 0;
}
//----------------------------------------------------------------------------
void mafPipeSurface::Create(mafSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_ScalarVisibility= 0;

  assert(m_Vme->IsA("mafVMESurface"));
  mafVMESurface *vme = ((mafVMESurface*) m_Vme);
  assert(vme->GetSurfaceOutput());
  vme->GetSurfaceOutput()->Update();
  vtkPolyData *data = vme->GetSurfaceOutput()->GetSurfaceData();
  assert(data);

  m_Mapper = vtkPolyDataMapper::New();
	m_Mapper->SetInput(data);
  m_Mapper->SetScalarVisibility(m_ScalarVisibility);
  
	if(vme->IsAnimated())				
		m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
	else
		m_Mapper->ImmediateModeRenderingOff();

  m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);
  m_Actor->SetProperty(vme->GetMaterial()->m_Prop);

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

  /*
  m_axes = NULL;
	if(m_use_axes) m_axes = new mafAxes(m_ren1,m_Vme);
	if(m_use_axes) m_axes->SetVisibility(0);
	*/
}
//----------------------------------------------------------------------------
mafPipeSurface::~mafPipeSurface()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
	//@@@ if(m_use_axes) wxDEL(m_axes);  
}
/*
//----------------------------------------------------------------------------
void mafPipeSurface::Show(bool show)
//----------------------------------------------------------------------------
{
	m_Actor->SetVisibility(show);
	if(m_Selected)
	{
	  m_OutlineActor->SetVisibility(show);
		//@@@ if(m_use_axes) m_axes->SetVisibility(show);
	}
}
*/
//----------------------------------------------------------------------------
void mafPipeSurface::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
			m_OutlineActor->SetVisibility(sel);
			//@@@ if(m_use_axes) m_axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeSurface::UpdateProperty(bool fromTag)
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
}
//----------------------------------------------------------------------------
mmgGui *mafPipeSurface::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_SCALAR_VISIBILITY,"scalar vis.", &m_ScalarVisibility,0,"turn on/off the scalar visibility");

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_SCALAR_VISIBILITY:
      {
        m_Mapper->SetScalarVisibility(m_ScalarVisibility);
        if (m_ScalarVisibility)
        {
          vtkPolyData *data = (vtkPolyData *)m_Vme->GetOutput()->GetVTKData();
          assert(data);
          double range[2];
          data->GetScalarRange(range);
          m_Mapper->SetScalarRange(range);
        }
      }
    	break;
      default:
      break;
    }
  }
}
