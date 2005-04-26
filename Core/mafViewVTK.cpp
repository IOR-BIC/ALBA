/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-26 17:23:06 $
  Version:   $Revision: 1.6 $
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

#include "mafViewVTK.h"
#include "vtkCamera.h"


//----------------------------------------------------------------------------
mafViewVTK::mafViewVTK(wxString label)
:mafView(label)
//----------------------------------------------------------------------------
{
  m_Sg  = NULL;
  m_Rwi = NULL;
}
//----------------------------------------------------------------------------
mafViewVTK::~mafViewVTK( ) 
//----------------------------------------------------------------------------
{
  cppDEL(m_Sg);
  cppDEL(m_Rwi);
}
//----------------------------------------------------------------------------
mafView *mafViewVTK::Copy(mafEventListener *Listener)
//----------------------------------------------------------------------------
{
  mafViewVTK *v = new mafViewVTK(m_Label);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewVTK::Create( ) 
//----------------------------------------------------------------------------
{
  m_Rwi = new mafRWI(mafGetFrame() /*, ONE_LAYER, m_show_grid == 1, m_stereo_type*/);
  m_Rwi->SetListener(this); //SIL. 16-6-2004: 
  //m_Rwi->CameraSet(m_cam_position);
  //m_Rwi->SetAxesVisibility(m_show_axes != 0);
  m_Win = m_Rwi->m_Rwi;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;
  /*
  m_Sg->SetCreatableFlag(VME_SURFACE);
  m_Sg->SetCreatableFlag(VME_GRAY_VOLUME);
  m_Sg->SetCreatableFlag(VME_VOLUME);
  m_Sg->SetCreatableFlag(VME_IMAGE);
  m_Sg->SetCreatableFlag(VME_POINTSET);
  m_Sg->SetCreatableFlag(VME_TOOL);
  m_Sg->SetCreatableFlag(VME_SCALAR);
  */

  m_Rwi->m_Camera->SetClippingRange(0.1,1000); 
  this->CameraReset();
}

//----------------------------------------------------------------------------
void mafViewVTK::VmeAdd(mafNode *vme)                                   {assert(m_Sg); m_Sg->VmeAdd(vme);}
void mafViewVTK::VmeRemove(mafNode *vme)                                {assert(m_Sg); m_Sg->VmeRemove(vme);}
void mafViewVTK::VmeSelect(mafNode *vme, bool select)                   {assert(m_Sg); m_Sg->VmeSelect(vme,select);}
void mafViewVTK::VmeShow(mafNode *vme, bool show)												{assert(m_Sg); m_Sg->VmeShow(vme,show);}
void mafViewVTK::VmeUpdateProperty(mafNode *vme, bool fromTag)	        {assert(m_Sg); m_Sg->VmeUpdateProperty(vme,fromTag);}
int  mafViewVTK::GetNodeStatus(mafNode *vme)                            {return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;}
void mafViewVTK::CameraReset()	{assert(m_Rwi); m_Rwi->CameraReset();}
void mafViewVTK::CameraUpdate() {assert(m_Rwi); m_Rwi->CameraUpdate();}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipe* mafViewVTK::GetNodePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
   assert(m_Sg);
   mafSceneNode *n = m_Sg->Vme2Node(vme);
   if(!n) return NULL;
   return n->m_Pipe;
}
