/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOsgTerrainManipulatorGui.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-21 16:14:11 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmgGui.h"
#include "mmgButton.h"
#include "mafOsgTerrainManipulatorGui.h"
#include "mmgFlyPoseList.h"
//---------------------------------------
// Update Callback 
//---------------------------------------
class mafOsgTerrainManipulatorGuiCallback : public osg::NodeCallback
{
public:
  mafOsgTerrainManipulatorGuiCallback(mafOsgTerrainManipulatorGui *m ) {_manipulator = m;};

  void operator()(osg::Node* node, osg::NodeVisitor* nv)
  { 
    if(_manipulator) _manipulator->UpdateGui();
  }
  mafOsgTerrainManipulatorGui *_manipulator; 
};
//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------
enum mafOsgTerrainManipulatorGuimafEventIDs
{
  ID_FOO = MINID,
  ID_CENTER,
  ID_YAW,
  ID_PITCH,
  ID_DIST,
  ID_MINDIST,
  ID_GIZMO,
  ID_HUD,
  ID_UPDATE,
  ID_AUTO_UPDATE
};
//----------------------------------------------------------------------------
mafOsgTerrainManipulatorGui::mafOsgTerrainManipulatorGui(mafObserver *listener, mafOsgTerrainManipulator *manip)
//----------------------------------------------------------------------------
{
  assert(listener);
  m_Listener = listener;
  assert(manip);
  m_manip = manip;

  m_center[0] = m_center[1] = m_center[2]=0;
  m_yaw         =0;
  m_pitch       =0;
  m_dist        =0;
  m_mindist     =0;
  m_gizmo       =0;
  m_hud         =0;
  m_auto_update =0;
  m_poseList    =NULL;

  //------------
  // create the GUI:
  //------------
  m_gui = new mmgGui(this);
  m_gui->SetListener(this);

  UpdateGui();

  // Camera Position
  m_gui->Label(""); 
  m_gui->Label("Terrain Manipulator:",true); 
  m_gui->Vector     (ID_CENTER,     "center",     m_center);
  m_gui->FloatSlider(ID_YAW,        "yaw",       &m_yaw,    0, 360);
  m_gui->FloatSlider(ID_PITCH,      "pitch",     &m_pitch,  0, 90 );
  m_gui->Double     (ID_DIST,       "dist",      &m_dist,   0.00001, 1000000);
  m_gui->Double     (ID_MINDIST,    "mindist",   &m_mindist,0.00001, 1000000);
  m_gui->Bool       (ID_GIZMO,      "show gizmo",&m_gizmo);
  m_gui->Bool       (ID_HUD,        "show hud",  &m_hud);
  m_gui->Button     (ID_UPDATE,     "update",    " ");
  m_gui->Bool       (ID_AUTO_UPDATE,"auto update", &m_auto_update);
  
  m_gui->Label("");
  m_poseList = new mmgFlyPoseList(m_manip);
  m_gui->AddGui(m_poseList->GetGui());
}
//----------------------------------------------------------------------------
mafOsgTerrainManipulatorGui::~mafOsgTerrainManipulatorGui( ) 
//----------------------------------------------------------------------------
{
	cppDEL(m_gui);
}
//----------------------------------------------------------------------------
mmgGui* mafOsgTerrainManipulatorGui::GetGui()
//----------------------------------------------------------------------------
{
	return m_gui;
}
//----------------------------------------------------------------------------
void mafOsgTerrainManipulatorGui::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case   ID_CENTER:
        m_manip->SetCenter(m_center);
        main_interpreter->doCommands("SetCenter center");
      break;
      case   ID_YAW:
        m_manip->SetYaw(m_yaw);
      break;
      case   ID_PITCH:
        m_manip->SetPitch(m_pitch);
      break;
      case   ID_DIST:
        m_manip->SetDistance(m_dist);
      break;
      case   ID_MINDIST:
        m_manip->SetMinDistance(m_mindist);
      break;
      case   ID_GIZMO:
        m_manip->showGizmo(m_gizmo != 0);
        m_gizmo = m_manip->isGizmoShown();
      break;
      case   ID_HUD:
        m_manip->showHud(m_hud != 0);
        m_gizmo = m_manip->isHudShown();
        break;
      case   ID_UPDATE:
        UpdateGui();
      break;
      case   ID_AUTO_UPDATE:
        SetAutoUpdate(m_auto_update != 0 );
      break;
      default:
        mafEventMacro(*maf_event);
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mafOsgTerrainManipulatorGui::UpdateGui()
//----------------------------------------------------------------------------
{
    m_manip->GetCenter(m_center);
    m_yaw     = m_manip->GetYaw();
    m_pitch   = m_manip->GetPitch();
    m_dist    = m_manip->GetDistance();
    m_mindist = m_manip->GetMinDistance();
    m_gizmo   = m_manip->isGizmoShown();
    m_hud     = m_manip->isHudShown();
    if(m_gui) m_gui->Update();
}
//----------------------------------------------------------------------------
void mafOsgTerrainManipulatorGui::SetAutoUpdate(bool enable)
//----------------------------------------------------------------------------
{
  // error prone --- se viene cambiato il Nodo del manipulator 
  // non me ne accorgo e non so come togliere la callback

  //todo - verificare se la callback viene referenziata o no


  if(enable && m_manip->getNode())
  {
    m_updateCallback = new mafOsgTerrainManipulatorGuiCallback(this);
    m_manip->getNode()->setUpdateCallback(m_updateCallback.get());
  }
  else
  {
    if(m_updateCallback.get())
    {
      if(m_manip->getNode()) m_manip->getNode()->setUpdateCallback(NULL);
      m_updateCallback = NULL;
    }
  }
}


