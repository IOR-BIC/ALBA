/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePointSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:45:49 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden - Paolo Quadrani
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

#include "mafPipePointSet.h"
#include "mafSceneNode.h"
#include "mmaMaterial.h"

#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMEOutputPointSet.h"

// from mafPipePointSet
#include "vtkMAFAssembly.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataNormals.h"
#include "vtkExtendedGlyph3D.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipePointSet);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipePointSet::mafPipePointSet()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Radius = 1.0;
}
//----------------------------------------------------------------------------
void mafPipePointSet::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;

  int resolution = 15;
  
  m_Vme->GetOutput()->Update();

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputPointSet));
  mafVMEOutputPointSet *pointset_output = mafVMEOutputPointSet::SafeDownCast(m_Vme->GetOutput());
  assert(pointset_output);
  pointset_output->Update();
  vtkPolyData *data = pointset_output->GetPointSetData();
  assert(data);

  // pointset pipeline
	m_act_s = vtkSphereSource::New();
	m_act_s->SetRadius(m_Radius);
	m_act_s->SetThetaResolution(resolution);
	m_act_s->SetPhiResolution(resolution);
	m_act_s->Update();

	m_normals = vtkPolyDataNormals::New();
	m_normals->SetInput(m_act_s->GetOutput());
	m_normals->Update();

	m_act_g = vtkExtendedGlyph3D::New();
	m_act_g->SetInput(data);
	//m_act_g->SetSource(m_act_s->GetOutput());
	m_act_g->SetSource(m_normals->GetOutput());
	m_act_g->OrientOff();
  m_act_g->ScalingOff();
  m_act_g->ScalarVisibilityOn();
	m_act_g->Update();

  m_act_m = vtkPolyDataMapper::New();
	m_act_m->SetInput(m_act_g->GetOutput());
	m_act_m->ScalarVisibilityOff();
  if(m_Vme->IsAnimated())				
    m_act_m->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  else
    m_act_m->ImmediateModeRenderingOff();

  m_act_a = vtkActor::New();
	m_act_a->SetProperty(pointset_output->GetMaterial()->m_Prop);
	m_act_a->GetProperty()->SetInterpolationToGouraud();
	m_act_a->SetMapper(m_act_m);

  m_AssemblyFront->AddPart(m_act_a);

  // selection hilight
	m_sel_ocf = vtkOutlineCornerFilter::New();
	m_sel_ocf->SetInput(m_act_g->GetOutput());  

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
	m_sel_a->SetScale(1.01,1.01,1.01);

  m_AssemblyFront->AddPart(m_sel_a);
}
//----------------------------------------------------------------------------
mafPipePointSet::~mafPipePointSet()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_act_a);
  m_AssemblyFront->RemovePart(m_sel_a);

  vtkDEL(m_act_s);
	vtkDEL(m_normals);
  vtkDEL(m_act_g);
  vtkDEL(m_act_m);
  vtkDEL(m_act_a);

  vtkDEL(m_sel_ocf);
  vtkDEL(m_sel_m);
  vtkDEL(m_sel_p);
  vtkDEL(m_sel_a);
}
//----------------------------------------------------------------------------
void mafPipePointSet::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_act_a->GetVisibility()) 
    m_sel_a->SetVisibility(sel);
}
//----------------------------------------------------------------------------
void mafPipePointSet::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
/*	double r = 10;
  double resolution = 15;

  if(m_Cloud)
  {
    r = m_Cloud->GetRadius();
    resolution = m_Cloud->GetSphereResolution();
  }
	else if(m_Landmark)
  {
    r = m_Landmark->GetRadius();
    resolution = ((mafVMELandmarkCloud *)m_Landmark->GetParent())->GetSphereResolution();
  }

	m_act_s->SetRadius(r);
	m_act_s->SetThetaResolution(resolution);
	m_act_s->SetPhiResolution(resolution);
	m_act_s->Update();
	m_act_g->Update();*/
}
