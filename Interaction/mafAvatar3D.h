/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar3D.h,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:54 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAvatar3D_h
#define __mafAvatar3D_h

#ifdef __GNUG__
    #pragma interface "mafAvatar3D.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafAvatar.h"
#include "vtkRayCast3DPicker.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkProp3D;
class vtkActor;
class vtkAssemblyPath;;
class vtkRenderer;
class mmi6DOF;
class vtkMatrix4x4;
class mflMatrix;
class vtkRenderWindowInteractor;
class mafAttribute;
class mmdTracker;
class mflXMLWriter;
class vtkXMLDataElement;
class vtkXMLDataParser;
class vtkTextMapper;
class vtkActor2D;
class vtkOutlineSource;
class vtkTransform;
class vtkCellPicker;
class mflTransform;
class mflCameraTransform;
class mafView;
class mafEventBase;
class mmgAvatarSettings;

/**
  Avatars are entities moving in the virtual world according to user's 
  movements, and thus representing thier presence in the virtual world.
  Specialized classes must set the Avatar variable with a vtkProp3D,
  this class will take care of adding it to the current renderer, moving,
  hiding and showing it at the right time. The Avatar moves according to an
  interactor, which must also be set by subclasses.
  @todo
  - implement rendering by means of a CameraUpdate event sent to the interaction manager
  - make the avatar have its own GUI plugged into the device gui settings
  - refactoring to have the avatar picking nodes (VME)
 */
class mafAvatar3D : public mafAvatar
{
public:
  vtkTypeMacro(mafAvatar3D,mafAvatar);
  static mafAvatar3D *New();

  enum 
  {
    TRACKER_COORDS=0,
    CANONICAL_COORDS,
    WORLD_COORDS,
  };
 
  /**  
    Set/Get the renderer this avatar is attached to. When the rederer is
    changed, all props created by this avatar are moved to the new renderer
    and the interaction is changed accordingly */
  virtual void SetRenderer(vtkRenderer *ren);
  
  /**  
    Hide the 3D cursor, i.e. remove actor from renderer's list. This is
    usually used to compute bounding box without considering avatars */
  virtual void Hide();

  /**  
    Show the 3D cursor. This function is conditional, in the 
    sense the old visibility state is retained when showing back. */
  virtual void Show();
  
  /** Return picker 3D used by 3DAvatar for picking */
  vtkRayCast3DPicker *GetPicker3D();

  /** Return picker 2D used by 3DAvatar for 2D picking */
  vtkCellPicker *GetPicker2D();

  /** return the picker used by this avatar: from the picker is possible to obtain information 
      about the picked object. */
  virtual vtkAbstractPropPicker *GetPicker();
  
  /** Display a box showing where input coords are mapped.
      This can be used for debug to understand if the tracker's
      settings and the mapping function are correct */
  void SetDisplayWorkingBox(int vis);
  vtkGetMacro(DisplayWorkingBox,int);
  vtkBooleanMacro(DisplayWorkingBox,int);

  /** 
    Display the debug text showing trackers coordinate. This can 
    be used to debug the code. */
  void SetDisplayDebugText(int vis);
  vtkGetMacro(DisplayDebugText,int);
  vtkBooleanMacro(DisplayDebugText,int);

  /** where to display the text */
  vtkSetVector2Macro(DebugTextPosition,float);
  vtkGetVector2Macro(DebugTextPosition,float);
  
  /** 
    Display the original tracker's coords in the debug text area.
    This can be used to know the tracked volume size to
    be set in the trackers' settings. */
  vtkSetMacro(CoordsFrame,int);
  vtkGetMacro(CoordsFrame,int);
  

  /**  Process events coming from tracker */
  virtual void ProcessEvent(mflEvent *event,mafID ch=mflAgent::DefaultChannel);

  /** return the tracker this avatar is the tracker attached to this avatar */
  virtual void SetTracker(mmdTracker *tracker);

  /**  Transform a matrix from Canonical to World space*/
  void CanonicalToWorld(vtkMatrix4x4 *pose,vtkMatrix4x4 *dest=NULL);
  void CanonicalToWorld(vtkTransform *trans);
  void CanonicalToWorld(mflTransform *trans);

  void WorldToCanonical(vtkMatrix4x4 *pose,vtkMatrix4x4 *dest=NULL);
  void WorldToCanonical(vtkTransform *trans);
  void WorldToCanonical(mflTransform *trans);


  /** 
    Transform a matrix from Tracker frame to World frame, the function
    can optionally avoid to map on the destination matrix some of the 
    6DOF: e.g. not using the scale or the rotational matrix */
  void TrackerToWorld(mflMatrix *tracker_pose,mflMatrix *world_pose,int use_scale=1,int use_rot=1, int use_trans=1);
  void WorldToTracker(vtkMatrix4x4 *world_pose,vtkMatrix4x4 *tracker_pose,int use_scale=1,int use_rot=1, int use_trans=1);

  
  /** map tracker coords to display normalized coords */
  void TrackerToDisplay(mflMatrix *tracker_pose,float xy[2]);

  /** map world coords to tracker coords */
  void WorldToDisplay(mflMatrix *world_pose,float xy[2]);

  /** map world coords to normalized display coords*/
  void WorldToNormalizedDisplay(mflMatrix *world_pose,float xy[2]);
  

  /**  return transform between canonical and world space */
  mflCameraTransform *GetCanonicalToWorldTransform() {return CanonicalToWorldTransform;}

   /** 
    Set mapping rule, i.e. the policy to be used for scaling the
    tracked volume to the world volume. */
  void SetFittingMode(int type);

  /** return the mapping rule currently in use */
  vtkGetMacro(FittingMode,int);

  /** 
    return the last pose matrix of the avatar's 3d actor in world coordinates.  
    */
  mflMatrix *GetLastPoseMatrix() {return LastPoseMatrix;};

protected:
  mafAvatar3D();
  virtual ~mafAvatar3D();

  /**
   Internally used to set the last pose matrix.
   @todo: Why should we use this instead of using the tracker's one? I'd like to remove this Marco. 28-10-2004: */
  void SetLastPoseMatrix(mflMatrix *matrix);

  /**  
    Allocate the settings object. This virtual function can be reimplemented
    by subclasses to add new settings to the GUI */
  virtual void CreateSettings();

  virtual int InternalStore(mflXMLWriter *writer);
  virtual int InternalRestore(vtkXMLDataElement *node,vtkXMLDataParser *parser);

  /** redefined to add the Cursor actor into the selected renderer */
  virtual int InternalInitialize();

  /** redefined to remove Cursor from renderer */
  virtual void InternalShutdown();

  /** internally used to update debug text on the screen */
  virtual void UpdateDebugText(const char *title, vtkMatrix4x4 *pose);

  /** process a move event */
  virtual void OnMove3DEvent(mflEventInteraction *e);
  
  /** process an event signaling a change of the tracked bounds */
  void OnUpdateBoundsEvent(mmdTracker *tracker);

  /** process event sent after a camera reset */
  virtual void OnPostResetCamera(mflEvent *event);

  /** Internally used to redefine the 3D picker class */  
  vtkSetObjectMacro(Picker3D,vtkRayCast3DPicker);

  vtkProp3D         *WorkingBoxActor; ///< Prop3D of the working box
  vtkOutlineSource  *WorkingBox; ///< Working box the tracker coords are mapped to

  vtkTextMapper     *DebugTextMapper;
  vtkActor2D        *DebugTextActor;

  mflMatrix         *LastPoseMatrix;

  vtkRayCast3DPicker  *Picker3D; ///< Used to pick in a VTK Render window
  vtkCellPicker		    *Picker2D;


  mflCameraTransform  *CanonicalToWorldTransform;
  
  int   FittingMode; ///< specify the current mapping rule from canonical to world

  //mafAttribute *Settings;  ///< Used to save/restore avatar's settings, 
  
  int DisplayWorkingBox; ///< Whether display the working box
  int DisplayDebugText; ///< Whether display Debug text
  int CoordsFrame; ///< Whether to display the original tracker's coords 

  float *DebugTextPosition;
private:
  mafAvatar3D(const mafAvatar3D&);  // Not implemented.
  void operator=(const mafAvatar3D&);  // Not implemented.
};

#endif 
