/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi6DOF.h,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:34 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmi6DOF_h
#define __mmi6DOF_h

#ifdef __GNUG__
    #pragma interface "mmi6DOF.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mmiGenericInterface.h"

class vtkCamera;
class vtkProp3D;
class vtkRenderer;
class mflMatrix;
class vtkMatrix4x4;
class mflTransform;
class mmdTracker;
class mflBounds;
class mafAvatar3D;

/** 
  base class for 3D interaction modalities with 6DOF 
  @sa mmi6DOFMove
  @todo
  - rewrite the UpdateDeltaTransform()
*/
class mmi6DOF : public mmiGenericInterface
{
public:
  vtkTypeMacro(mmi6DOF,mmiGenericInterface);

  static mmi6DOF *New();
  
  /**  Start the interaction with the selected object */
  virtual int StartInteraction(mmdTracker *tracker,mflMatrix *pose=NULL);
  
  /**  Stop the interaction */
  virtual int StopInteraction(mmdTracker *tracker,mflMatrix *pose=NULL);
  
  /**  Set/Get the current pose matrix */
  virtual void SetTrackerPoseMatrix(mflMatrix *pose);
  mflMatrix *GetTrackerPoseMatrix() {return this->TrackerPoseMatrix;}
  
  /**  Stores the current TrackerPoseMatrix. */
  void TrackerSnapshot(mflMatrix *pose);
  
  /**
   Update the delta transform, i.e. transform from last snapshot */
  void UpdateDeltaTransform();

  /**  Return pointer to the current input tracker */
  mmdTracker *GetTracker() {return (mmdTracker *)Device;}
  void SetTracker(mmdTracker *tracker);

  /** 
    Enable/Disable trigger events processing. Trigger events are StartInteraction
    and StopInteraction events which are used to start/stop the interaction. If eabled
    this flag makes the interactor to ingore these events and to be continuously active.
    Default is false. */
  void SetIgnoreTriggerEvents(int flag) {IgnoreTriggerEvents=flag;Modified();}
  int GetIgnoreTriggerEvents(int flag) {return IgnoreTriggerEvents;}
  vtkBooleanMacro(IgnoreTriggerEvents,int);

  /** redefined to accomplish specific tasks*/
  virtual void SetRenderer(vtkRenderer *ren);

    /** Used to hide default tracker's avatar */
  void HideAvatar();
  
  /** Used to show back default tracker's avatar */
  void ShowAvatar();
  
protected:
  
  mmi6DOF();
  virtual ~mmi6DOF();

  /** reimplemented to manage interaction events from trackers */
  virtual int OnStartInteraction(mflEventInteraction *event);
  /** reimplemented to manage interaction events from trackers */
  virtual int OnStopInteraction(mflEventInteraction *event);
  
  mflMatrix           *TrackerPoseMatrix;  
  mflMatrix           *StartTrackerPoseMatrix;
  mflMatrix           *InverseTrackerPoseMatrix;
  mflMatrix			      *InversePoseMatrix;
  mflTransform        *DeltaTransform; 
  mflTransform        *TmpTransform;   
  
  mafAvatar3D         *Avatar;

private:
  mmi6DOF(const mmi6DOF&);  // Not implemented.
  void operator=(const mmi6DOF&);  // Not implemented.
};

#endif 
