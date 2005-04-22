/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-22 20:02:53 $
  Version:   $Revision: 1.2 $
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

mafCxxTypeMacro(mafPipeSurface);

//----------------------------------------------------------------------------
mafPipeSurface::mafPipeSurface() 
//----------------------------------------------------------------------------
{
  m_act_m    =NULL;
  m_act_a    =NULL;
  m_sel_ocf  =NULL;
  m_sel_m    =NULL;
  m_sel_p    =NULL;
  m_sel_a    =NULL;
}
//----------------------------------------------------------------------------
void mafPipeSurface::Create(mafSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_selected = false;
  m_act_m   = NULL;
  m_act_a   = NULL;;
  m_sel_ocf = NULL;;
  m_sel_m   = NULL;;
  m_sel_p   = NULL;;
  m_sel_a   = NULL;;

	//@@@ m_use_axes = use_axes;
	//@@@ mafVmeData *data = (mafVmeData*) m_vme->GetClientData();
	//@@@ assert(data);
  //@@@ m_vme->UpdateCurrentData();

  assert(m_vme->IsA("mafVMESurface"));
  mafVMESurface *vme = ((mafVMESurface*) m_vme);
  assert(vme->GetSurfaceOutput());
  vme->GetSurfaceOutput()->Update();
  vtkPolyData *data = vme->GetSurfaceOutput()->GetSurfaceData();
  assert(data);

  m_act_m = vtkPolyDataMapper::New();
	m_act_m->SetInput(data);
  
	if(vme->IsAnimated())				
		m_act_m->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
	else
		m_act_m->ImmediateModeRenderingOff();

  m_act_a = vtkActor::New();
	//@@@ m_act_a->SetProperty(data->m_mat_gui->GetMaterial()->m_prop);
	m_act_a->SetMapper(m_act_m);

  m_asm1->AddPart(m_act_a);

  // selection hilight
	m_sel_ocf = vtkOutlineCornerFilter::New();
	m_sel_ocf->SetInput(data);  

	m_sel_m = vtkPolyDataMapper::New();
	m_sel_m->SetInput(m_sel_ocf->GetOutput());

	m_sel_p = vtkProperty::New();
	m_sel_p->SetColor(1,1,1);
	m_sel_p->SetAmbient(1);
	m_sel_p->SetRepresentationToWireframe();
	m_sel_p->SetInterpolationToFlat();

	m_sel_a = vtkActor::New();
	m_sel_a->SetMapper(m_sel_m);
	m_sel_a->VisibilityOff();
	m_sel_a->PickableOff();
	m_sel_a->SetProperty(m_sel_p);

  m_asm1->AddPart(m_sel_a);

  /*
  m_axes = NULL;
	if(m_use_axes) m_axes = new mafAxes(m_ren1,m_vme);
	if(m_use_axes) m_axes->SetVisibility(0);
	*/
}
//----------------------------------------------------------------------------
mafPipeSurface::~mafPipeSurface()
//----------------------------------------------------------------------------
{
  m_asm1->RemovePart(m_act_a);
  m_asm1->RemovePart(m_sel_a);

	vtkDEL(m_act_m);
  vtkDEL(m_act_a);
  vtkDEL(m_sel_ocf);
  vtkDEL(m_sel_m);
  vtkDEL(m_sel_p);
  vtkDEL(m_sel_a);
	//@@@ if(m_use_axes) wxDEL(m_axes);  
}
/*
//----------------------------------------------------------------------------
void mafPipeSurface::Show(bool show)
//----------------------------------------------------------------------------
{
	m_act_a->SetVisibility(show);
	if(m_selected)
	{
	  m_sel_a->SetVisibility(show);
		//@@@ if(m_use_axes) m_axes->SetVisibility(show);
	}
}
*/
//----------------------------------------------------------------------------
void mafPipeSurface::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_selected = sel;
	if(m_act_a->GetVisibility()) 
	{
			m_sel_a->SetVisibility(sel);
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
		((mafVmeData *)m_vme->GetClientData())->UpdateFromTag();
    int idx = m_vme->GetTagArray()->FindTag("VME_CENTER_ROTATION_POSE");
    vtkTagItem *item = NULL;
    double vec[16];
    if (idx != -1)
    {
      item = m_vme->GetTagArray()->GetTag(idx);
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
	  m_act_m->SetScalarVisibility(((mafVmeData *)m_vme->GetClientData())->GetColorByScalar());
	*/
}
