/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSceneNode.cpp,v $
  Language:  C++
  Date:      $Date: 2007-03-16 08:15:04 $
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

#include "mafIndent.h"
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
mafSceneNode::mafSceneNode(mafSceneGraph *sg,mafSceneNode *parent, const mafNode* vme, vtkRenderer *ren, vtkRenderer *ren2)
//----------------------------------------------------------------------------
{
	m_Sg            = sg;
	m_Parent				= parent;
	m_Vme						= (mafNode *)vme;
	m_RenFront			= ren;
	m_RenBack				= ren2;
  m_Next					= NULL;
  m_Pipe					= NULL;
  m_PipeCreatable = true;
  m_Mutex         = false;
  //m_visible      = false;
  m_AssemblyFront = NULL;
  m_AssemblyBack  = NULL;
  
  assert(m_Vme);
  vtkLinearTransform *transform = NULL;
  if(m_Vme->IsA("mafVME"))
  {
    mafVME* v = ((mafVME*)m_Vme);
    assert(v->GetOutput());
    assert(v->GetOutput()->GetTransform());
    assert(v->GetOutput()->GetTransform()->GetVTKTransform());
    transform = v->GetOutput()->GetTransform()->GetVTKTransform();
  }

  m_AssemblyFront = vtkMAFAssembly::New();
  m_AssemblyFront->SetVme(m_Vme);
  m_AssemblyFront->SetUserTransform(transform);

  if(m_RenFront != NULL) //modified by Vladik. 03-03-2004
  {
	  if(m_Vme->IsA("mafNodeRoot") || m_Vme->IsA("mafVMERoot")) 
		  m_RenFront->AddActor(m_AssemblyFront); 
	  else if (m_AssemblyFront)  //modified by Marco. 20-7-2004
		  m_Parent->m_AssemblyFront->AddPart(m_AssemblyFront);
  }

	if(m_RenBack != NULL)
	{
		m_AssemblyBack = vtkMAFAssembly::New();
    m_AssemblyBack->SetVme(m_Vme);
    m_AssemblyBack->SetUserTransform(transform);
    if(m_Vme->IsA("mafNodeRoot") || m_Vme->IsA("mafVMERoot")) 
			m_RenBack->AddActor(m_AssemblyBack); 
		else
			m_Parent->m_AssemblyBack->AddPart(m_AssemblyBack);
	}


  /* @@@ 
	
//@@@   m_cloud					 = NULL;
  m_CloudOpenClose_observer = 0;
	m_CloudRadiusModified_observer = 0;
  m_CloudSphereResolutionModified_observer = 0;
	if( m_Vme->IsA("mafNodeLandmarkCloud") )
	{
		m_cloud = (mafNodeLandmarkCloud *)m_Vme;
		m_CloudOpenClose_observer = mflAgent::PlugEventSource(m_Vme,OnOpenCloseEvent,this,mafNodeLandmarkCloud::OpenCloseEvent); 
		m_CloudRadiusModified_observer = mflAgent::PlugEventSource(m_Vme,OnRadiusModifiedEvent,this,mafNodeLandmarkCloud::RadiusModifiedEvent); 
		m_CloudSphereResolutionModified_observer = mflAgent::PlugEventSource(m_Vme,OnSphereResolutionModifiedEvent,this,mafNodeLandmarkCloud::SphereResolutionModifiedEvent); 
	}
	m_scalar							= NULL;
	m_Scalar_ScalingModified_observer	= 0;
	m_Scalar_DiameterModified_observer	= 0;
	m_Scalar_HeadModified_observer		= 0;
 
	if( m_Vme->IsA("mafNodeScalar") )
	{
		m_scalar = (mafNodeScalar *)m_Vme;
		m_Scalar_ScalingModified_observer = mflAgent::PlugEventSource(m_Vme,OnScalingModifiedEvent,this,mafNodeScalar::ScalingModifiedEvent); 
		m_Scalar_DiameterModified_observer = mflAgent::PlugEventSource(m_Vme,OnDiameterModifiedEvent,this,mafNodeScalar::DiameterModifiedEvent);
		m_Scalar_HeadModified_observer = mflAgent::PlugEventSource(m_Vme,OnHeadModifiedEvent,this,mafNodeScalar::HeadModifiedEvent); 
	}
@@@ */

}
//----------------------------------------------------------------------------
mafSceneNode::~mafSceneNode()
//----------------------------------------------------------------------------
{
	cppDEL(m_Pipe);
	
	if(m_RenFront != NULL)  //modified by Vladik. 03-03-2004
  {
    if(m_Vme->IsA("mafNodeRoot") || m_Vme->IsA("mafVMERoot")) 
		  m_RenFront->RemoveActor(m_AssemblyFront);
    else if (m_AssemblyFront)  //modified by Marco. 20-7-2004
	   	m_Parent->m_AssemblyFront->RemovePart(m_AssemblyFront); 
  }

  vtkDEL(m_AssemblyFront); 

	if(m_RenBack != NULL)
	{
    if(m_Vme->IsA("mafNodeRoot") || m_Vme->IsA("mafVMERoot")) 
			m_RenBack->RemoveActor(m_AssemblyBack);
		else
			m_Parent->m_AssemblyBack->RemovePart(m_AssemblyBack); 

    vtkDEL(m_AssemblyBack);  
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
  if(m_Pipe) m_Pipe->Select(select);
}
/*
//----------------------------------------------------------------------------
void mafSceneNode::Show(bool show)   
//----------------------------------------------------------------------------
{
	m_visible = show;
  if(m_Pipe) m_Pipe->Show(show);
}
*/
//----------------------------------------------------------------------------
void mafSceneNode::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  if(m_Pipe) m_Pipe->UpdateProperty(fromTag);
}
/* @@@
//----------------------------------------------------------------------------
void mafSceneNode::OnOpenCloseEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self)
    self->m_Sg->OnOpenCloseEvent(self);
}
//----------------------------------------------------------------------------
void mafSceneNode::OnRadiusModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_Sg && self->m_cloud)
	{
    if(self->m_cloud->IsOpen())
    {
      mafNodeIterator *iter=self->m_cloud->NewIterator();
      for (mafNode *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
	      if(vme->IsA("mafNodeLandmark"))
				  self->m_Sg->VmeUpdateProperty(vme);
      iter->Delete();
    }
    else
      self->m_Sg->VmeUpdateProperty(self->m_cloud);
	}
}
@@@ */
/* @@@
//----------------------------------------------------------------------------
void mafSceneNode::OnSphereResolutionModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_Sg && self->m_cloud)
	{
    if(self->m_cloud->IsOpen())
    {
      mafNodeIterator *iter=self->m_cloud->NewIterator();
      for (mafNode *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
	      if(vme->IsA("mafNodeLandmark"))
				  self->m_Sg->VmeUpdateProperty(vme);
      iter->Delete();
    }
    else
      self->m_Sg->VmeUpdateProperty(self->m_cloud);
  } 
}
//----------------------------------------------------------------------------
void mafSceneNode::OnScalingModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_Sg && self->m_scalar)
      self->m_Sg->VmeUpdateProperty(self->m_scalar);
	  
 }
//----------------------------------------------------------------------------
void mafSceneNode::OnDiameterModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_Sg && self->m_scalar)
	  self->m_Sg->VmeUpdateProperty(self->m_scalar);
}
//----------------------------------------------------------------------------
void mafSceneNode::OnHeadModifiedEvent(void *arg)
//----------------------------------------------------------------------------
{
  mafSceneNode *self=(mafSceneNode *)arg;
  if(self && self->m_Sg && self->m_scalar)
	  self->m_Sg->VmeUpdateProperty(self->m_scalar);
}
@@@ */

//-------------------------------------------------------------------------
void mafSceneNode::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  // print the scene node
  os << indent << "mafSceneNode" << '\t' << this << std::endl;
  os << indent << "Linked VME" << '\t' << m_Vme << '\t' << m_Vme->GetName() << std::endl;
  os << indent << "Visual Pipe" << '\t' << m_Pipe << '\t' <<  (m_Pipe ? m_Pipe->GetTypeName() : "")  << std::endl;  
  os << indent << "Front Renderer" << '\t' << m_RenFront << std::endl;
  os <<  indent << "Back Renderer" << '\t' << m_RenBack << std::endl;
  os << std::endl;
}
