/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSceneNode.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 16:37:43 $
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


#include "mafPipe.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafNode.h"
#include "mafVME.h"
#include "mafMatrixPipe.h"
#include "vtkMAFAssembly.h"
#include "mafNodeIterator.h" 
#include "vtkRenderer.h"
//#include "vtkTransform.h"
#include "vtkLinearTransform.h"
//@@@ #include "mafNodeLandmarkCloud.h"
//@@@ #include "mafNodeScalar.h"
//@@@ #include "mflMatrixPipe.h"
//@@@ #include "mflAgent.h"

//----------------------------------------------------------------------------
mafSceneNode::mafSceneNode(mafSceneGraph *sg,mafSceneNode *parent, mafNode* vme, vtkRenderer *ren, vtkRenderer *ren2)
//----------------------------------------------------------------------------
{
	m_sg             = sg;
	m_parent				 = parent;
	m_vme						 = vme;
	m_ren1					 = ren;
	m_ren2					 = ren2;
  m_next					 = NULL;
  m_pipe					 = NULL;
  m_pipe_creatable = true;
  m_mutex          = false;
  //m_visible        = false;
  m_asm1 = NULL;
  m_asm2 = NULL;
  
  assert(vme);
  vtkLinearTransform *transform = NULL;
  if(vme->IsA("mafVME"))
  {
    mafVME* v = ((mafVME*)vme);
    assert(v->GetOutput());
    assert(v->GetOutput()->GetTransform());
    assert(v->GetOutput()->GetTransform()->GetVTKTransform());
    transform = v->GetOutput()->GetTransform()->GetVTKTransform();
  }

  m_asm1 = vtkMAFAssembly::New();
  m_asm1->SetVme(vme);
  m_asm1->SetUserTransform(transform);

  if(m_ren1 != NULL) //modified by Vladik. 03-03-2004
  {
	  if(m_vme->IsA("mafNodeRoot") || m_vme->IsA("mafVMERoot")) 
		  m_ren1->AddActor(m_asm1); 
	  else if (m_asm1)  //modified by Marco. 20-7-2004
		  m_parent->m_asm1->AddPart(m_asm1);
  }

	if(m_ren2 != NULL)
	{
		m_asm2 = vtkMAFAssembly::New();
    m_asm2->SetVme(vme);
    m_asm2->SetUserTransform(transform);
    if(m_vme->IsA("mafNodeRoot") || m_vme->IsA("mafVMERoot")) 
			m_ren2->AddActor(m_asm2); 
		else
			m_parent->m_asm2->AddPart(m_asm2);
	}


  /* @@@ 
	
//@@@   m_cloud					 = NULL;
  m_CloudOpenClose_observer = 0;
	m_CloudRadiusModified_observer = 0;
  m_CloudSphereResolutionModified_observer = 0;
	if( m_vme->IsA("mafNodeLandmarkCloud") )
	{
		m_cloud = (mafNodeLandmarkCloud *)m_vme;
		m_CloudOpenClose_observer = mflAgent::PlugEventSource(m_vme,OnOpenCloseEvent,this,mafNodeLandmarkCloud::OpenCloseEvent); 
		m_CloudRadiusModified_observer = mflAgent::PlugEventSource(m_vme,OnRadiusModifiedEvent,this,mafNodeLandmarkCloud::RadiusModifiedEvent); 
		m_CloudSphereResolutionModified_observer = mflAgent::PlugEventSource(m_vme,OnSphereResolutionModifiedEvent,this,mafNodeLandmarkCloud::SphereResolutionModifiedEvent); 
	}
	m_scalar							= NULL;
	m_Scalar_ScalingModified_observer	= 0;
	m_Scalar_DiameterModified_observer	= 0;
	m_Scalar_HeadModified_observer		= 0;
 
	if( m_vme->IsA("mafNodeScalar") )
	{
		m_scalar = (mafNodeScalar *)m_vme;
		m_Scalar_ScalingModified_observer = mflAgent::PlugEventSource(m_vme,OnScalingModifiedEvent,this,mafNodeScalar::ScalingModifiedEvent); 
		m_Scalar_DiameterModified_observer = mflAgent::PlugEventSource(m_vme,OnDiameterModifiedEvent,this,mafNodeScalar::DiameterModifiedEvent);
		m_Scalar_HeadModified_observer = mflAgent::PlugEventSource(m_vme,OnHeadModifiedEvent,this,mafNodeScalar::HeadModifiedEvent); 
	}
@@@ */

}
//----------------------------------------------------------------------------
mafSceneNode::~mafSceneNode()
//----------------------------------------------------------------------------
{
	cppDEL(m_pipe);
	
	if(m_ren1 != NULL)  //modified by Vladik. 03-03-2004
  {
    if(m_vme->IsA("mafNodeRoot") || m_vme->IsA("mafVMERoot")) 
		  m_ren1->RemoveActor(m_asm1);
    else if (m_asm1)  //modified by Marco. 20-7-2004
	   	m_parent->m_asm1->RemovePart(m_asm1); 
  }

  vtkDEL(m_asm1); 

	if(m_ren2 != NULL)
	{
    if(m_vme->IsA("mafNodeRoot") || m_vme->IsA("mafVMERoot")) 
			m_ren2->RemoveActor(m_asm2);
		else
			m_parent->m_asm2->RemovePart(m_asm2); 

    vtkDEL(m_asm2);  
	}
 /* @@@
	if (m_cloud && m_CloudOpenClose_observer > 0 )
	{
		m_cloud->RemoveObserver(m_CloudOpenClose_observer);
		m_cloud->RemoveObserver(m_CloudRadiusModified_observer);
		m_cloud->RemoveObserver(m_CloudSphereResolutionModified_observer);
	}

	if (m_scalar )
	{
		m_scalar->RemoveObserver(m_Scalar_ScalingModified_observer);
		m_scalar->RemoveObserver(m_Scalar_DiameterModified_observer);
		m_scalar->RemoveObserver(m_Scalar_HeadModified_observer);
	}
  @@@ */
}
//----------------------------------------------------------------------------
void mafSceneNode::Select(bool select)   
//----------------------------------------------------------------------------
{
  if(m_pipe) m_pipe->Select(select);
}
/*
//----------------------------------------------------------------------------
void mafSceneNode::Show(bool show)   
//----------------------------------------------------------------------------
{
	m_visible = show;
  if(m_pipe) m_pipe->Show(show);
}
*/
//----------------------------------------------------------------------------
void mafSceneNode::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  if(m_pipe) m_pipe->UpdateProperty(fromTag);
}
/* @@@
//----------------------------------------------------------------------------
void mafSceneNode::OnOpenCloseEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self)
    self->m_sg->OnOpenCloseEvent(self);
}
//----------------------------------------------------------------------------
void mafSceneNode::OnRadiusModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_sg && self->m_cloud)
	{
    if(self->m_cloud->IsOpen())
    {
      mafNodeIterator *iter=self->m_cloud->NewIterator();
      for (mafNode *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
	      if(vme->IsA("mafNodeLandmark"))
				  self->m_sg->VmeUpdateProperty(vme);
      iter->Delete();
    }
    else
      self->m_sg->VmeUpdateProperty(self->m_cloud);
	}
}
@@@ */
/* @@@
//----------------------------------------------------------------------------
void mafSceneNode::OnSphereResolutionModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_sg && self->m_cloud)
	{
    if(self->m_cloud->IsOpen())
    {
      mafNodeIterator *iter=self->m_cloud->NewIterator();
      for (mafNode *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
	      if(vme->IsA("mafNodeLandmark"))
				  self->m_sg->VmeUpdateProperty(vme);
      iter->Delete();
    }
    else
      self->m_sg->VmeUpdateProperty(self->m_cloud);
  } 
}
//----------------------------------------------------------------------------
void mafSceneNode::OnScalingModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_sg && self->m_scalar)
      self->m_sg->VmeUpdateProperty(self->m_scalar);
	  
 }
//----------------------------------------------------------------------------
void mafSceneNode::OnDiameterModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_sg && self->m_scalar)
	  self->m_sg->VmeUpdateProperty(self->m_scalar);
}
//----------------------------------------------------------------------------
void mafSceneNode::OnHeadModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_sg && self->m_scalar)
	  self->m_sg->VmeUpdateProperty(self->m_scalar);
}
@@@ */
