/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettingsTimeBar.h,v $
Language:  C++
Date:      $Date: 2007-10-23 10:35:03 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUISettingsTimeBar_H__
#define __mafGUISettingsTimeBar_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUISettingsTimeBar :
//----------------------------------------------------------------------------
/**
*/
class mafGUISettingsTimeBar : public mafGUISettings
{
public:
	mafGUISettingsTimeBar(mafObserver *Listener, const mafString &label = _("Time Bar"));
	~mafGUISettingsTimeBar(); 

  enum STORAGE_SETTINGS_WIDGET_ID
  {
    ID_REAL_TIME = MINID,
    ID_TIME_MULTIPLIER,
    ID_ANIMATE_IN_SUBRANGE,
    ID_SUBRANGE,
    ID_LOOP,
    ID_PLAY_ACTIVE_VIEWPORT,
    ID_SPEED,
    ID_FPS,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Return true if real time mode is active.*/
  int GetRealTimeMode() {return m_RealTimeMode;};

  /** Set the play modality: real time or all frames.*/
  void SetRealTimeMode(int realTime = 0);

  /** Return the Frame Per Second generated from the time bar.*/
  int GetFPS() {return m_Fps;};

  /** Set the number f Frame Per Second to be rendered.*/
  void SetFPS(int fps = 25);

  /** Return true if loop animation is active.*/
  int GetLoop() {return m_Loop;};

  /** Turn On/Off the loop playback.*/
  void LoopAnimation(int loop = 0);

  /** Turn On/Off the possibility to do the playback in a sub-range of the whole time range.*/
  void AminateInSubrange(int animate_subrange = 0);

  /** Return true if animation in subrange is active.*/
  int GetAnimateSubrange() {return m_AnimateInSubrange;};

  /** Set the subrange playback interval.*/
  void SetSubrange(double sub_range[2]);

  /** Set the subrange playback interval.*/
  void SetSubrange(double min, double max);

  /** Return the playback subrange.*/
  void GetSubrange(double sub_range[2]) {sub_range[0] = m_SubRange[0]; sub_range[1] = m_SubRange[1];};

  /** Return the playback subrange.*/
  double *GetSubrange() {return m_SubRange;};

  /** Set the speed ID.*/
  void SetSpeedId(int idx = 0);

  /** Get the speed ID.*/
  int GetSpeedId() {return m_SpeedId;};

  /** Set the time multiplier to transform milliseconds to the VME unit time.
  Default value will transform milliseconds in seconds.*/
  void SetTimeMultiplier(double tmult = 1000.0);

  /** Return the time multiplier.*/
  double GetTimeMultiplier() {return m_TimeMultiplier;};

  /** Enable/Disable the update of the animation only in the active viewport.*/
  void PlayInActiveViewport(int active_viewport = 0);

  /** Return true if is active the flag for the update only into the active viewport.*/
  int GetPlayingInActiveViewport() {return m_PlayInActiveViewport;};

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize the application settings.*/
  void InitializeSettings();

  /** Enable/Disable widgets.*/
  void EnableWidgets();

  /** Update the GUI information according to the new values associated to the variables 
  and flush these new values to the registry.*/
  void Update();

  int m_RealTimeMode;
  int m_Fps;
  int m_Loop;
  int m_SpeedId;
  int m_PlayInActiveViewport;
  int m_AnimateInSubrange;
  double m_SubRange[2];
  double m_TimeMultiplier;
};
#endif
