/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor6DOF
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractor6DOF_h
#define __albaInteractor6DOF_h

#include "albaInteractorGenericInterface.h"

class vtkCamera;
class vtkProp3D;
class vtkRenderer;
class albaMatrix;
class albaTransform;
class albaDeviceButtonsPadTracker;
class albaOBB;
class albaAvatar3D;

/** 
  base class for 3D interaction modalities with 6DOF 
  @sa albaInteractor6DOFMove
  @todo
  - rewrite the UpdateDeltaTransform()
*/
class ALBA_EXPORT albaInteractor6DOF : public albaInteractorGenericInterface
{
public:
  albaAbstractTypeMacro(albaInteractor6DOF,albaInteractorGenericInterface);
  
  /**  Start the interaction with the selected object */
  virtual int StartInteraction(albaDeviceButtonsPadTracker *tracker,albaMatrix *pose=NULL);
  
  /**  Stop the interaction */
  virtual int StopInteraction(albaDeviceButtonsPadTracker *tracker,albaMatrix *pose=NULL);
  
  /**  Set/Get the current pose matrix */
  virtual void SetTrackerPoseMatrix(albaMatrix *pose);
  albaMatrix *GetTrackerPoseMatrix() {return this->m_TrackerPoseMatrix;}
  
  /**  Stores the current m_TrackerPoseMatrix. */
  void TrackerSnapshot(albaMatrix *pose);
  
  /**
   Update the delta transform, i.e. transform from last snapshot */
  void UpdateDeltaTransform();

  /**  Return pointer to the current input tracker */
  albaDeviceButtonsPadTracker *GetTracker() {return (albaDeviceButtonsPadTracker *)m_Device;}
  void SetTracker(albaDeviceButtonsPadTracker *tracker);

  /** 
    Enable/Disable trigger events processing. Trigger events are StartInteraction
    and StopInteraction events which are used to start/stop the interaction. If eabled
    this flag makes the interactor to ingore these events and to be continuously active.
    Default is false. */
  void SetIgnoreTriggerEvents(int flag) {m_IgnoreTriggerEvents=flag;}
  int GetIgnoreTriggerEvents() {return m_IgnoreTriggerEvents;}
  void IgnoreTriggerEventsOn() {SetIgnoreTriggerEvents(true);}
  void IgnoreTriggerEventsOff() {SetIgnoreTriggerEvents(false);}

  /** Used to hide default tracker's avatar */
  void HideAvatar();
  
  /** Used to show back default tracker's avatar */
  void ShowAvatar();
  
protected:
  
  albaInteractor6DOF();
  virtual ~albaInteractor6DOF();

  /** reimplemented to manage interaction events from trackers */
  virtual int OnStartInteraction(albaEventInteraction *event);
  /** reimplemented to manage interaction events from trackers */
  virtual int OnStopInteraction(albaEventInteraction *event);
  
  albaMatrix           *m_TrackerPoseMatrix;  
  albaMatrix           *m_StartTrackerPoseMatrix;
  albaMatrix           *m_InverseTrackerPoseMatrix;
  albaMatrix			      *m_InversePoseMatrix;
  albaTransform        *m_DeltaTransform; 
  albaTransform        *m_TmpTransform;   
  
  albaAvatar3D         *m_Avatar;

private:
  albaInteractor6DOF(const albaInteractor6DOF&);  // Not implemented.
  void operator=(const albaInteractor6DOF&);  // Not implemented.
};

#endif 
