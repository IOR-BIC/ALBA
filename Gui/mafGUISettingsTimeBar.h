/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettingsTimeBar.h,v $
Language:  C++
Date:      $Date: 2008-05-15 15:19:03 $
Version:   $Revision: 1.5 $
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
    ID_TIME_SPEED,
    ID_TIME_SCALE,
    ID_TIME_STEP,
    ID_NUMBER_OF_FRAMES,
    ID_ANIMATE_IN_SUBRANGE,
    ID_SUBRANGE,
    ID_LOOP,
    ID_PLAY_ACTIVE_VIEWPORT,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Return true if real time mode is active.*/
  int GetRealTimeMode() {return m_RealTimeMode;};

  /** Set the play modality: real time or all frames.*/
  void SetRealTimeMode(int realTime = 0);

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

  /** Set the time multiplier to speed up or down the animation when the
  playback is not in real time.*/
  void SetTimeMultiplier(double tmult = 1.0);

  /** Return the time multiplier.*/
  double GetTimeMultiplier() {return m_TimeSpeed;};

  /** Set the time scale to transform milliseconds to the VME unit time.
  Default value will transform milliseconds in seconds.*/
  void SetTimeScale(double tscale = 1000.0);

  /** Return the time scale factor.*/
  double GetTimeScale() {return m_TimeScale;};

  /** Enable/Disable the update of the animation only in the active viewport.*/
  void PlayInActiveViewport(int active_viewport = 0);

  /** Return true if is active the flag for the update only into the active viewport.*/
  int GetPlayingInActiveViewport() {return m_PlayInActiveViewport;};

  /** Called by the time bar to set the time-bounds of the loaded data tree.*/
  void SetTimeBounds(double min, double max);

  /** Return the number of subdivision for the time bar.*/
  int GetNumberOfFrames() {return m_NumberOfFrames;};

  /** Set the number of frames.*/
  void SetNumberOfFrames(int frames);

  /** Set the time step for the animation.*/
  void SetTimeStep(double step);

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

  double m_TimeMin;
  double m_TimeMax;

  int m_NumberOfFrames; ///< Number of frames
  double m_TimeStep; ///< Time step linked to the number of frames and to the time range.
  int m_RealTimeMode; ///< Flag to play the animation in real time mode (checked) or to show all key-frames of the animation (unchecked)
  int m_Loop;
  int m_PlayInActiveViewport;
  int m_AnimateInSubrange;
  double m_SubRange[2]; ///< Time subrange animation.
  double m_TimeSpeed; ///< Multiplier to speed up the animation when is not in real time mode.
  double m_TimeScale; ///< Time scale referring to the time base = seconds.
};
#endif
