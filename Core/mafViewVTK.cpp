/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:18:02 $
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

#include "mafViewVTK.h"
#include "vtkCamera.h"


//----------------------------------------------------------------------------
mafViewVTK::mafViewVTK(wxString label)
:mafView(label)
//----------------------------------------------------------------------------
{
  m_sg  = NULL;
  m_rwi = NULL;
}
//----------------------------------------------------------------------------
mafViewVTK::~mafViewVTK( ) 
//----------------------------------------------------------------------------
{
  cppDEL(m_sg);
  cppDEL(m_rwi);
}
//----------------------------------------------------------------------------
mafView *mafViewVTK::Copy(mafEventListener *Listener)
//----------------------------------------------------------------------------
{
  mafViewVTK *v = new mafViewVTK(m_label);
  v->m_Listener = Listener;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewVTK::Create( ) 
//----------------------------------------------------------------------------
{
  m_rwi = new mafRWI(mafGetFrame() /*, ONE_LAYER, m_show_grid == 1, m_stereo_type*/);
  m_rwi->SetListener(this); //SIL. 16-6-2004: 
  //m_rwi->CameraSet(m_cam_position);
  //m_rwi->SetAxesVisibility(m_show_axes != 0);
  m_win = m_rwi->m_rwi;

  m_sg  = new mafSceneGraph(this,m_rwi->m_r1,m_rwi->m_r2);
  m_sg->SetListener(this);
  m_rwi->m_sg = m_sg;
  /*
  m_sg->SetCreatableFlag(VME_SURFACE);
  m_sg->SetCreatableFlag(VME_GRAY_VOLUME);
  m_sg->SetCreatableFlag(VME_VOLUME);
  m_sg->SetCreatableFlag(VME_IMAGE);
  m_sg->SetCreatableFlag(VME_POINTSET);
  m_sg->SetCreatableFlag(VME_TOOL);
  m_sg->SetCreatableFlag(VME_SCALAR);
  */

  m_rwi->m_c->SetClippingRange(0.1,1000); 
  this->CameraReset();
}

//----------------------------------------------------------------------------
void mafViewVTK::VmeAdd(mafNode *vme)                                   {assert(m_sg); m_sg->VmeAdd(vme);}
void mafViewVTK::VmeRemove(mafNode *vme)                                {assert(m_sg); m_sg->VmeRemove(vme);}
void mafViewVTK::VmeSelect(mafNode *vme, bool select)                   {assert(m_sg); m_sg->VmeSelect(vme,select);}
void mafViewVTK::VmeShow(mafNode *vme, bool show)												{assert(m_sg); m_sg->VmeShow(vme,show);}
void mafViewVTK::VmeUpdateProperty(mafNode *vme, bool fromTag)	        {assert(m_sg); m_sg->VmeUpdateProperty(vme,fromTag);}
int  mafViewVTK::GetNodeStatus(mafNode *vme)                            {return m_sg ? m_sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;}
void mafViewVTK::CameraReset()	{assert(m_rwi); m_rwi->CameraReset();}
void mafViewVTK::CameraUpdate() {assert(m_rwi); m_rwi->CameraUpdate();}
//----------------------------------------------------------------------------
