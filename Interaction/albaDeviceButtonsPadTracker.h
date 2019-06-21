/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadTracker
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaDeviceButtonsPadTracker_h
#define __albaDeviceButtonsPadTracker_h

#include "albaDeviceButtonsPad.h"
#include "albaMutexLock.h"
#include "albaSmartPointer.h"
#include "albaOBB.h"
#include "albaAvatar.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaTransform;
class albaMatrix;
class albaOBB;

template class ALBA_EXPORT albaAutoPointer<albaAvatar>;

/** Generic spatial position tracking device.
  albaDeviceButtonsPadTracker is a class providing basic functionalities for space trackers, 
  i.e. devices which track an object in space. Specialized classes for
  specific devices should add their own data structures and reimplemented
  InternalInitialize and InternalShutdow to activate/stop the device.
  Tracked pose is expressed by means of a 4x4 matrix. This class issues 
  events each time a new coordinate is available, but if a new coordinate
  arrives and the last pose has not been consumed (i.e. the last MoveEvent
  has not been dispatched yet, no event is issued, but the LastPoseMatrix is
  overwritten. This allows consumer classes to always have last tracked pose.
  MoveEvent's are albaActionEvent objects, which stores a pose and state of buttons.
  This class also gives support to transforming from the tracker coordinate
  system into a canonical coordinate system. The canonical frame is computed
  by ComputeTrackerToCanonicalTansform() which take as input the tracked volume
  definition. TrackedVolume is defined as a Oriented Bounding Box, whose bounds
  is defined by the TrackedBounds member, while the Orientation is defined by
  the TrackedBoxOrientation member(expressed as Cardan angles according to
  VTK convention on orientation).
  The canonical space is related to the tracked volume by TrackerToCanonicalTransform
  which transforms the TrackedBox into a box centered in the origin, zero rotation
  and isotropically scaled to normalize max input dimension (e.g. a box of size
  [-.5,.5,-2,2,-.1,.1] would be scaled to [-.25,.25,-1,1,-.05,.05].
  Each tracker can be assigned an avatar (albaAvatar) showing a graphical object
  moved by tracker's input.
  The albaDeviceButtonsPadTracker class has a specialized settings class which implements gui for
  setting the avatar and the tracker's tracked volume.
  The albaDeviceButtonsPadTracker is also a albaDeviceButtonsPad supporting buttons.
  @sa albaDeviceButtonsPad albaGUITrackerSettings albaAvatar
  @todo 
    - manage fusion of position matrices (not working any more)
    - manage multiple button press (rewrite mflActionEvent)
    - use an action event also for move events (rewrite SetLastPoseMatrix() ). 
*/
class ALBA_EXPORT albaDeviceButtonsPadTracker : public albaDeviceButtonsPad
{
public:
  albaTypeMacro(albaDeviceButtonsPadTracker,albaDeviceButtonsPad);

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Issued when the tracker is moved */
  // ALBA_ID_DEC(TRACKER_3D_MOVE)
  /** @ingroup Events
      Issued when the tracked bounds is changed */
  // ALBA_ID_DEC(TRACKER_BOUNDS_UPDATED)

  //----------------------------------------------------------------------------
  //    GUI Constants
  //----------------------------------------------------------------------------
  enum  TRACKER_WIDGET_ID
  {
    ID_TB_X_EXTENT=Superclass::ID_LAST,
    ID_TB_Y_EXTENT,
    ID_TB_Z_EXTENT,
    ID_TB_POSITION,
    ID_TB_ORIENTATION,
    ID_AVATAR_CHECK,
    ID_AVATAR_SELECT,
    ID_LAST
  };

  /** return the tracker 3d move id */
  static albaID GetTracker3DMoveId();

  /** return th tracker bounds updated id */
  static albaID GetTrackerBoundsUpdatedId();
  /**
    Reimplemented to manage fusion of move events. */
  virtual void OnEvent(albaEventBase *event);

  /**
   Internally used to set the last pose matrix. This function make a copy of the matrix, and
   in case of vtkMatrix4x4 attach a time stamp with the current time. Finally a MoveEvent is 
   rosed. Notice that old MoveEvent's are overwritten by new ones if not yet served. */
  void SetLastPoseMatrix(const albaMatrix &matrix);
  albaMatrix &GetLastPoseMatrix();

  /** set the timeout after which the event is discarded (in seconds) */
  void SetMoveEventTimeOut(albaTimeStamp t) {m_MoveEventTimeOut=t;}
  albaTimeStamp GetMoveEventTimeOut() {return m_MoveEventTimeOut;}

  /**
   Set the transform to be used to map coordinates between tracker and canonical frames */
  void SetTrackerToCanonicalTransform(albaTransform *trans);
  
  /**
   return the transform mapping coordinates from tracker to canonical frame.
  */
  albaTransform *GetTrackerToCanonicalTransform();
  
  /**
   This is used to map the RAW transform in tracker's frame into the canonical frame. */
  //void TrackerToCanonical(vtkMatrix4x4 *pose,vtkMatrix4x4 *dest=NULL);
  void TrackerToCanonical(const albaMatrix &pose,albaMatrix &dest);
  void TrackerToCanonical(albaMatrix &pose) {TrackerToCanonical(pose,pose);}
  void TrackerToCanonical(albaTransform *trans);

  /**
   This is used to map a pose in canonical frame into tracker's frame. */
  void CanonicalToTracker(const albaMatrix &pose, albaMatrix &dest);
  void CanonicalToTracker(albaMatrix &pose) {CanonicalToTracker(pose,pose);}
  void CanonicalToTracker(albaTransform *trans);

  /**
    Set/Get the avatar assigned to this device */
  void SetAvatar(albaAvatar *avatar);
  albaAvatar *GetAvatar() {return m_Avatar;}

  /** 
    Set the default avatar, i.e. the avatar to be used as default one. An application
    module can set the avatar to a transient one and then restore the default one. 
    The default avatar is made persistent and will be recreated by means of the object
    factory when restarting the application. Setting the default avatar automatically
    calls the SetAvatar() too.  */    
  void SetDefaultAvatar(albaAvatar *avatar);
  albaAvatar *GetDefaultAvatar() {return m_DefaultAvatar;}

  /** 
    Restore the avatar to the default one. An application
    module can set the avatar to a transient one and then 
    restore the default one.  */
  void RestoreDefaultAvatar();

  /** 
    Update the Tracker to Canonical transform. This transform
    maps the tracked working box into the canonical working
    box, which is a working box with no orientation and
    centered around the origin (i.e. a identical pose matrix) and 
    isotropically scaled to fit the [-1,1,-1,1,-1,1] box. */
  void ComputeTrackerToCanonicalTansform();

  /** return bounds of canonical space */
  albaOBB &GetCanonicalBounds();

  /** 
    return bounds of tracked volume. Tracked volume is expessed
    as an oriented bounding box. For orientation see 
    GetTrackedBoxOrientation() */
  albaOBB &GetTrackedBounds() {return m_TrackedBounds;}

  /** 
    return orientation of tracked volume. Tracked volume is expessed
    as an oriented bounding box. For box extents see orientation see
    GetTrackedBounds() */
  double *GetTrackedBoxOrientation() {return m_TrackedBoxOrientation;}

  /** utility function for finding maximum among 2 values */
  inline double GetMax(double x,double y);

  /** utility function for finding maximum among 3 values */
  inline double GetMax3(double x,double y,double z);

  /** internal function to create device GUI for settings */
  virtual void CreateGui();

  /** force GUI update */
  virtual void UpdateGui();
  
protected:
  albaDeviceButtonsPadTracker();
  virtual ~albaDeviceButtonsPadTracker();

  virtual int InternalStore(albaStorageElement *node);
  virtual int InternalRestore(albaStorageElement *node);

  virtual int InternalInitialize();
  virtual void InternalShutdown();

  /** add pose matrix to the event */ 
  virtual void SendButtonEvent(albaEventInteraction *event);

  int AvatarChooser(wxString &avatar_name,wxString &avatar_type);

  albaMatrix*            m_LastPoseMatrix; ///< stores the last pose matrix
  int                   m_LastPose;       ///< Flag used when last pose has not been served yet
  albaTimeStamp          m_LastMoveTime;   ///< Time stamp of last MoveEvent
  albaMutexLock          m_LastPoseMutex;
  //albaEventInteraction*  m_LastMoveEvent;     
  albaTimeStamp          m_MoveEventTimeOut;

  albaTransform*         m_TrackerToCanonicalTransform;

  albaAutoPointer<albaAvatar> m_Avatar;        ///< the current avatar
  albaAutoPointer<albaAvatar> m_DefaultAvatar; ///< the avatar set thorugh the GUI

  
  
  albaOBB m_CanonicalBounds;         ///< Bounds of canonical space
  int    m_AvatarFlag;              ///< used for the GUI
  albaOBB m_TrackedBounds;           ///< Stores tracked volume bounds (xmin,xmax,ymin,ymax,zmin,zmax)
  double m_TrackedBoxOrientation[3];///< Used to store the tracked box orientation (VTK's convention: cardan angles)
  double m_TBPosition[3];           ///< Used to store the tracked box center

private:
  albaDeviceButtonsPadTracker(const albaDeviceButtonsPadTracker&);  // Not implemented.
  void operator=(const albaDeviceButtonsPadTracker&);  // Not implemented.
};

//------------------------------------------------------------------------------
// Return the maximum value among two.
inline double albaDeviceButtonsPadTracker::GetMax(double x,double y)
//------------------------------------------------------------------------------
{
  return (x>=y)?x:y;
}

//------------------------------------------------------------------------------
// Return the maximum value among the 3 values for X,Y and Z axes.
inline double albaDeviceButtonsPadTracker::GetMax3(double x,double y, double z)
//------------------------------------------------------------------------------
{
  return (x>=y)?x:((y>=z)?y:z);
}

#endif 
