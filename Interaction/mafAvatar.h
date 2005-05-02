/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar.h,v $
  Language:  C++
  Date:      $Date: 2005-05-02 15:18:16 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone & Michele Diegoli
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAvatar_h
#define __mafAvatar_h

#include "mafInteractor.h"
#include "mafStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafStorageElement;
class mmgGui;
class mafEvent;
class mafEventBase;
class mafMatrix;
class mmdTracker;
class mafView;
class vtkProp3D;
class vtkActor;
class vtkRenderer;
class vtkActor2D;
class vtkAbstractPropPicker;

/**
  Avatars are entities moving in the virtual world according to user's 
  movements, and thus representing their presence in the virtual world.
  Specialized classes must set the Avatar variable with a vtkProp3D,
  this class will take care of adding it to the current renderer, moving,
  hiding and showing it at the right time. The Avatar moves according to an
  interactor, which must also be set by subclasses.
 */
class mafAvatar : public mafInteractor, public mafStorable
{
public:
  /** @ingroup Events
  /** @{ */
  MAF_ID_DEC(AVATAR_SCREEN_PLANE_SETTINGS); ///< Issued to update all the avatar with the same screen plane
  MAF_ID_DEC(AVATAR_WORKING_BOX_SETTINGS);  ///< Issued to update all the avatar with the same working box 
  /** @} */
  
  //----------------------------------------------------------------------------
  //    GUI Constants
  //----------------------------------------------------------------------------
  enum 
  {
    ID_NAME=MINID,
    ID_LAST
  };

  enum AvatarModality {MODE_2D=0, MODE_3D};

  mafTypeMacro(mafAvatar,mafInteractor);

  /**  
    Set the renderer this avatar is attached to. When the rederer is
    changed, all props created by this avatar are moved to the new renderer
    and the interaction is changed accordingly */
  virtual void SetRenderer(vtkRenderer *ren);

  /** set the current view */
  void SetView(mafView *view);

  /** return current view */
  mafView *GetView() {return m_View;}

  /**  
    Set the actor used to display the 3D cursor.
    @attention
    In case of ActivePositioning The initial vtkProp3D pose matrix is intended
    as relative to the camera, thus its stored in the AvatarTransform member and
    a new pose continuously changed according to the camera position is set to 
    the vtkProp3D. Thus to change the pose of the graphics object visually representing
    the avatar the AvatarsTransform. */
  void SetActor3D(vtkProp3D *avatar);
  /** Return the prop3D used for this avatar */
  vtkProp3D *GetActor3D() {return m_Actor3D;}

  /**  
    Set the actor used to display the 2D cursor.
     */
  void SetActor2D(vtkActor2D *avatar);
  /** Return the prop3D used for this avatar */
  vtkActor2D *GetActor2D() {return m_Actor2D;}

  /**  
    Hide all the Avatar's graphics actors, i.e. remove actor from renderer's list. This is
    usually used to compute bounding box without considering avatars */
  virtual void Hide();

  /**  
    Show all the Avatar's graphics actors. This function is conditional, in the 
    sense the old visibility state is retained when showing back. */
  virtual void Show();

  /**  
    Hide the Avatar's 3D cursor actor, i.e. remove its actor from renderer's list. */
  virtual void HideCursor3D();

  /**  
    Show the Avatar's 3D cursor actor. This function is conditional, in the 
    sense the old visibility state is retained when showing back. */
  virtual void ShowCursor3D();

  /**  
    Hide the Avatar's 2D cursor actor, i.e. remove its actor from renderer's list. */
  virtual void HideCursor2D();

  /**  
    Show the Avatar's 2D cursor actor. This function is conditional, in the 
    sense the old visibility state is retained when showing back. */
  virtual void ShowCursor2D();
  
  /** pick in the scene with this avatar, give the avatar pose */
  virtual int Pick(mafMatrix *pose=NULL) {return false;}
  virtual int Pick(double X, double Y) {return false;}  

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

  /** 
    Return settings object for this Avatar, and if not yet allocated 
    create it. Settings objects usually include a GUI for interactive settings.*/
  //mmgAvatarSettings *GetSettings() {if (!Settings) CreateSettings(); return Settings;}

   /** 
     return the tracker attached to this avatar: the avatar usually follows the 
     movements of the tracker*/
  mmdTracker *GetTracker();
   /** 
     set the tracker attached to this avatar: the avatar usually follows the 
     movements of the tracker*/
  virtual void SetTracker(mmdTracker *tracker);

  /** Set Avatar modality: MODE_2D or MODE_3D*/
  void SetMode(int mode) {m_Mode=mode;}

  /** return the current avatar modality. This is used by avatars with a double bhavior. */
  int GetMode() {return m_Mode;}

  /** Set Avatar modality to  MODE_2D */
  void SetModeTo2D() {SetMode(MODE_2D);}

  /** Set Avatar modality to MODE_3D */
  void SetModeTo3D() {SetMode(MODE_3D);}

  /** return the picker used by this avatar: from the picker is possible to obtain information 
      about the picked object. 
     */
  virtual vtkAbstractPropPicker *GetPicker(){ return NULL; }

  /** Return pointer to the GUI. */
  mmgGui *GetGui();

  /** internal function to create device GUI for settings */
  virtual void CreateGui();

  /** force GUI update */
  virtual void UpdateGui();

protected:

  mafAvatar();
  virtual ~mafAvatar();
 
  /** This is used to allow nested serialization of subclasses.
    This function is called by Store and is reimplemented in subclasses.
    Each subclass can Open/Close its own subelements which are
    closed inside the "Device" XML element. */
  virtual int InternalStore(mafStorageElement *node);

  virtual int InternalRestore(mafStorageElement *node);

  /** redefined to add the Cursor actor into the selected renderer */
  virtual int InternalInitialize();

  /** redefined to remove Cursor from renderer */
  virtual void InternalShutdown();

  /** process event sent before a camera reset */
  virtual void OnPreResetCamera(mafEventBase *event);

  /** process event sent after a camera reset */
  virtual void OnPostResetCamera(mafEventBase *event);

  /** process event sent when a view has been selected */
  virtual void OnViewSelected(mafEvent *e);

  vtkMAFSmartPointer<vtkProp3D>   m_Actor3D;  ///< 3D representation of the avatar
  vtkMAFSmartPointer<vtkActor2D>  m_Actor2D;  ///< 2D representation of the avatar

  mmgGui*           m_Gui;  ///< the settings Gui
  mafView*          m_View; ///< the selected view
  int               m_Mode; ///< Avatar modality, either 2D or 3D         

  int               m_Cursor3DHideCounter; ///< counter of the HideCursor3D() calls, used to manage concurrent calls
  int               m_Cursor2DHideCounter; ///< counter of the HideCursor2D() calls, used to manage concurrent calls

private:
  mafAvatar(const mafAvatar&);  // Not implemented.
  void operator=(const mafAvatar&);  // Not implemented.
};

#endif 
