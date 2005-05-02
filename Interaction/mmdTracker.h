/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdTracker.h,v $
  Language:  C++
  Date:      $Date: 2005-05-02 15:18:17 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdTracker_h
#define __mmdTracker_h

#include "mmdButtonsPad.h"
#include "mafMutexLock.h"
#include "mafSmartPointer.h"
#include "mafOBB.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafTransform;
class mafMatrix;
class mafAvatar;
class mafOBB;

/** Generic spatial position tracking device.
  mmdTracker is a class providing basic functionalities for space trackers, 
  i.e. devices which track an object in space. Specialized classes for
  specific devices should add their own data structures and reimplemented
  InternalInitialize and InternalShutdow to activate/stop the device.
  Tracked pose is expressed by means of a 4x4 matrix. This class issues 
  events each time a new coordinate is available, but if a new coordinate
  arrives and the last pose hase not been consumed (i.e. the last MoveEvent
  has not been despatched yet, no event is issued, but the LastPoseMatrix is
  overwritten. This allows consumer classes to always have last tracked pose.
  MoveEvent's are mafActionEvent objects, which stores a pose and state of buttons.
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
  Each tracker can be assigned an avatar (mafAvatar) showing a graphical object
  moved by tracker's input.
  The mmdTracker class has a specialized settings class which implements gui for
  setting the avatar and the tracker's tracked volume.
  The mmdTracker is also a mmdButtonsPad supporting buttons.
  @sa mmdButtonsPad mmgTrackerSettings mafAvatar
  @todo 
    - manage fusion of position matrixes (not working any more)
    - manage multiple button press (rewrite mflActionEvent)
    - use an action event also for move events (rewrite SetLastPoseMatrix() ). 
*/
class mmdTracker : public mmdButtonsPad
{
public:
  mafTypeMacro(mmdTracker,mmdButtonsPad);

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Issued when the tracker is moved */
  MAF_ID_DEC(TRACKER_3D_MOVE)
  /** @ingroup Events
      Issued when the tracked bounds is changed */
  MAF_ID_DEC(TRACKER_BOUNDS_UPDATED)

  //----------------------------------------------------------------------------
  //    GUI Constants
  //----------------------------------------------------------------------------
  enum  
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

  /**
    Reimplemented to manage fusion of move events. */
  virtual void OnEvent(mafEventBase *event);

  /**
   Internally used to set the last pose matrix. This function make a copy of the matrix, and
   in case of vtkMatrix4x4 attach a time stamp with the current time. Finally a MoveEvent is 
   rosed. Notice that old MoveEvent's are overwritten by new ones if not yet served. */
  void SetLastPoseMatrix(const mafMatrix &matrix);
  mafMatrix &GetLastPoseMatrix();

  /** set the timeout after which the event is discarded (in seconds) */
  void SetMoveEventTimeOut(mafTimeStamp t) {m_MoveEventTimeOut=t;}
  mafTimeStamp GetMoveEventTimeOut() {return m_MoveEventTimeOut;}

  /**
   Set the transform to be used to map coordinates between tracker and canonical frames */
  void SetTrackerToCanonicalTransform(mafTransform *trans);
  
  /**
   return the transform mapping coordinates from tracker to canonical frame.
  */
  mafTransform *GetTrackerToCanonicalTransform();
  
  /**
   This is used to map the RAW transform in tracker's frame into the canonical frame. */
  //void TrackerToCanonical(vtkMatrix4x4 *pose,vtkMatrix4x4 *dest=NULL);
  void TrackerToCanonical(const mafMatrix &pose,mafMatrix &dest);
  void TrackerToCanonical(mafMatrix &pose) {TrackerToCanonical(pose,pose);}
  void TrackerToCanonical(mafTransform &trans);

  /**
   This is used to map a pose in canonical frame into tracker's frame. */
  void CanonicalToTracker(const mafMatrix &pose, mafMatrix &dest);
  void CanonicalToTracker(mafMatrix &pose) {CanonicalToTracker(pose,pose);}
  void CanonicalToTracker(mafTransform &trans);

  /**
    Set/Get the avatar assigned to this device */
  void SetAvatar(mafAvatar *avatar);
  mafAvatar *GetAvatar() {return m_Avatar;}

  /** 
    Set the default avatar, i.e. the avatar to be used as default one. An application
    module can set the avatar to a transient one and then restore the default one. 
    The default avatar is made persistent and will be recreated by means of the object
    factory when restarting the application. Setting the default avatar automatically
    calls the SetAvatar() too.  */    
  void SetDefaultAvatar(mafAvatar *avatar);
  mafAvatar *GetDefaultAvatar() {return m_DefaultAvatar;}

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
  mafOBB &GetCanonicalBounds();

  /** 
    return bounds of tracked volume. Tracked volume is expessed
    as an oriented bounding box. For orientation see 
    GetTrackedBoxOrientation() */
  mafOBB &GetTrackedBounds() {return m_TrackedBounds;}

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
  mmdTracker();
  virtual ~mmdTracker();

  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);

  virtual int InternalInitialize();
  virtual void InternalShutdown();

  /** add pose matrix to the event */ 
  virtual void SendButtonEvent(mafEventInteraction *event);

  int AvatarChooser(wxString &avatar_name,wxString &avatar_type);

  mafMatrix*            m_LastPoseMatrix; ///< stores the last pose matrix
  int                   m_LastPose;       ///< Flag used when last pose has not been served yet
  mafTimeStamp          m_LastMoveTime;   ///< Timestamp of last MoveEvent
  mafMutexLock          m_LastPoseMutex;
  //mafEventInteraction*  m_LastMoveEvent;     
  mafTimeStamp          m_MoveEventTimeOut;

  mafTransform*         m_TrackerToCanonicalTransform;

  mafAutoPointer<mafAvatar> m_Avatar;        ///< the current avatar
  mafAutoPointer<mafAvatar> m_DefaultAvatar; ///< the avatar set thorugh the GUI

  
  
  mafOBB m_CanonicalBounds;         ///< Bounds of canonical space
  int    m_AvatarFlag;              ///< used for the GUI
  mafOBB m_TrackedBounds;           ///< Stores tracked volume bounds (xmin,xmax,ymin,ymax,zmin,zmax)
  double m_TrackedBoxOrientation[3];///< Used to store the tracked box orientation (VTK's convention: cardan angles)
  double m_TBPosition[3];           ///< Used to store the tracked box center

private:
  mmdTracker(const mmdTracker&);  // Not implemented.
  void operator=(const mmdTracker&);  // Not implemented.
};

//------------------------------------------------------------------------------
// Return the maximum value among two.
inline double mmdTracker::GetMax(double x,double y)
//------------------------------------------------------------------------------
{
  return (x>=y)?x:y;
}

//------------------------------------------------------------------------------
// Return the maximum value among the 3 values for X,Y and Z axes.
inline double mmdTracker::GetMax3(double x,double y, double z)
//------------------------------------------------------------------------------
{
  return (x>=y)?x:((y>=z)?y:z);
}

#endif 
