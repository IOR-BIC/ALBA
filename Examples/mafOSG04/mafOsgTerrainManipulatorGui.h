/*=========================================================================

 Program: MAF2
 Module: mafOsgTerrainManipulatorGui
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOsgTerrainManipulatorGui_H__
#define __mafOsgTerrainManipulatorGui_H__

#include <wx/notebook.h>
#include <wx/icon.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafOsgTerrainManipulator.h"
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;
class mafOsgTerrainManipulatorGuiCallback;
class mmgFlyPoseList;
//----------------------------------------------------------------------------
// mafOsgTerrainManipulatorGui :
//----------------------------------------------------------------------------
class mafOsgTerrainManipulatorGui : public mafObserver
{
public:

                  mafOsgTerrainManipulatorGui(mafObserver *listener, mafOsgTerrainManipulator *manip);
  virtual         ~mafOsgTerrainManipulatorGui(); 
  virtual void    OnEvent(mafEventBase *maf_event);

  virtual mmgGui *GetGui();
  virtual void    UpdateGui();

protected:  
  void AddFlyPose();
  void RemoveFlyPose();
  void DeleteFlyPose();

  mafObserver              *m_Listener;
  mmgGui                   *m_gui;
  mafOsgTerrainManipulator *m_manip;

  double  m_center[3];
  double  m_yaw;
  double  m_pitch;
  double  m_dist;
  double  m_mindist;
  int     m_gizmo;
  int     m_hud;
  int     m_auto_update;

  osg::ref_ptr<mafOsgTerrainManipulatorGuiCallback> m_updateCallback;
  void mafOsgTerrainManipulatorGui::SetAutoUpdate(bool enable);

  mmgFlyPoseList *m_poseList;
};
#endif




