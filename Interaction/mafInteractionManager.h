/*=========================================================================

 Program: MAF2
 Module: mafInteractionManager
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafInteractionManager_h
#define __mafInteractionManager_h

#include "mafObject.h"
#include "mafObserver.h"
#include "mafEventSender.h"
#include "mafStorable.h"
#include "mafString.h"
#include "mafSmartPointer.h"

#include <map>
#include <set>
#include <list>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafEventBase;
class mafEvent;
class mafAction;
class mafInteractor;
#ifdef MAF_EXPORTS
#include "mafAvatar.h"
#else
class mafAvatar;
#endif
class mafDevice;
class mafDeviceManager;
class mafDeviceSet;
class mafDeviceButtonsPadMouse;
class mafDeviceButtonsPadMouseRemote;
class mafGUIInteractionSettings;
class vtkRenderer;
class vtkRendererWindow;
class mafView;
class mflXMLWriter;
class vtkXMLDataElement;
class vtkXMLDataParser;
class mafInteractorSER;
class mafInteractorPER;
class mafVME;
class mafGUIDialog;
class mafGUI;
class mafGUITree;
class mafGUICheckListBox;
class mafGUIHolder;
class mafGUINamedPanel;
class mafDeviceClientMAF;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_MAP(MAF_EXPORT,mafString,mafAutoPointer<mafAvatar>);
EXPORT_STL_LIST(MAF_EXPORT,mafAutoPointer<mafInteractorPER>);
#endif


/** This class takes care of mastering the interaction inside views.
  This class is responsible to coordinate interaction, i.e. 
  resources necessary for interaction (currently devices, actions and avatars)
  and to manage communication routing for interaction. Management of devices is
  delegated to the DeviceManager object, picking and routing of events to 
  picked VME is delegated to PER (positional event router) object, and low level 
  routing from devices to actions is delegated to the SER (static event router) 
  object.
  @sa mafDeviceManager mafInteractorPER mafInteractorSER 
  
  @todo
  - check the renderer to be RenFront
*/
class MAF_EXPORT mafInteractionManager : public mafObject, public mafObserver, public mafEventSender, public mafStorable
{
public:
  mafInteractionManager();
  virtual ~mafInteractionManager();

  mafTypeMacro(mafInteractionManager,mafObject);

  /** Enable/Disable VME selection by picking */
  void EnableSelect(bool enable);

  /**  Process incoming events. */
  virtual void OnEvent(mafEventBase *event);

  /** Get the device manager object */
  mafDeviceManager *GetDeviceManager() {return m_DeviceManager;}

  /** return the mouse device */
  mafDeviceButtonsPadMouse *GetMouseDevice();

  /** return the remote mouse device */
  mafDeviceButtonsPadMouseRemote *GetRemoteMouseDevice();

  /** return the ClientMAF device */
  mafDeviceClientMAF *GetClientDevice();

  /** return the mouse action, an action to which mouse is bound by default */
  //mafAction *GetMouseAction();

  /** Define a new action router.*/
  mafAction *AddAction(const char *name, float priority = 0.0);

  /** 
    Bind an agent to the specified action. 
    If the action is not present return -1 */
  int BindAction(const char *action,mafInteractor *agent);

  /** 
    Unbind an agent from the specified action. 
    If the action is not present return -1 */
  int UnBindAction(const char *action,mafInteractor *agent);

  /** Define a new avatar */
  void AddAvatar(mafAvatar *avatar);

  /** Remove an avatar */
  int RemoveAvatar(mafAvatar *avatar);

  /** Get an avatar given its name.*/
  mafAvatar *GetAvatar(const char *name);

  typedef std::map<mafString,mafAutoPointer<mafAvatar> > mmuAvatarsMap;
  typedef std::vector<mafAvatar *> mmuAvatarsVector;

  /** Return the avatars container */
  const mmuAvatarsMap &GetAvatars();

  /** return an array with the list of avatars currently connected */
  void GetAvatars(mmuAvatarsVector &avatars);

  /** Get an action router.*/
  mafAction *GetAction(const char *name);

  /** Set the selected view. This makes all dynamic avatars
    and interactors to be informed of the current renderer. */
  void ViewSelected(mafView *view);
  
  /** Return the currently selected view */
  mafView *GetSelectedView(mafView *view) {return m_SelectedView;}

  /** Set the selected VTK renderer */
  void SetCurrentRenderer(vtkRenderer *ren);

  /** Return the renderer of the currently selected view,
  if its a VTK based view otherwise return NULL.*/
  vtkRenderer *GetCurrentRenderer() {return m_CurrentRenderer;}

  /** used to propagate PreReset camera event */
  void PreResetCamera(vtkRenderer *ren);
  /** used to propagate PostReset camera event */
  void PostResetCamera(vtkRenderer *ren);

  /** propagate VME_SELECTED event */
  void  VmeSelected(mafVME *vme);

  /** store all interaction settings to an XML file (Multimod Interaction Settings - MIS - format) */
  int Store(const char *filename);

  /** restore interaction settings from an XML file (Multimod Interaction Settings - MIS - format) */
  int Restore(const char *filename);

  /** propagate a request for rendering the specified window or 
      all the windows (rw==NULL). Requests arriving within a single
      event dispatching cycle are are fused into a single request*/
  //void CameraUpdate(vtkRendererWindow *rw) {/* NOT YET IMPLEMENTED */}

  /** 
    Queue a request for rendering for the specified view. Default is 
    render all views */
  void CameraUpdate(mafView *view=NULL);


  /** return the positional event router */
  mafInteractorPER *GetPER() {return m_PositionalEventRouter;}

  /** used to override the default PER. To be called before initializations */
  void SetPER(mafInteractorPER *per);

  /** set a new PER and keep the old one in a list */
  void PushPER(mafInteractorPER *per);
  
  /** restore previous PER instance */
  bool PopPER();

  /** return the static event router */
  mafInteractorSER *GetSER() {return m_StaticEventRouter;}

  /** Set the FlyTo mode for the selected view. */
  void CameraFlyToMode();

  /** return the minimum time to elapse between two subsequent renderings */
  mafTimeStamp GetIntraFrameTime() {return m_IntraFrameTime;}
  /** set the minimum time to elapse between two subsequent renderings */
  void SetIntraFrameTime(mafTimeStamp iftime) {m_IntraFrameTime=iftime;}

  /** Open a dialog to choose among available devices */
  int DeviceChooser(wxString &dev_name,wxString &dev_type);

  /** This is used to allow also external objects to add devices */
  void AddDeviceToTree(mafDevice *device,mafDeviceSet *parent=NULL);
  void RemoveDeviceFromTree(mafDevice *device);

  /** Update names in device list */
  //void UpdateDeviceTree();

  /** Update the name of a device */
  void UpdateDevice(mafDevice *device);

  /** update bindings check list */
  void UpdateBindings();

  /** Show in modal configuration the settings dialog. */
  //bool ShowModal(); //SIL. 07-jun-2006 : 
  mafGUI* GetGui();  //SIL. 07-jun-2006 : 

  mafInteractorSER *GetStaticEventRouter() {return m_StaticEventRouter;}

protected:
  /** This is called by Store() to store information of this object.  */
  virtual int InternalStore(mafStorageElement *node);

  /** This is called by Restore() to restore information of this object. */
  virtual int InternalRestore(mafStorageElement *node);
  
  virtual void OnStartDispatching();
  virtual void OnEndDispatching();
  virtual void OnCameraUpdate(mafEventBase *e);
  virtual void OnViewSelected(mafEvent *event);
  virtual void OnDeviceAdded(mafEventBase *event);
  virtual void OnDeviceRemoving(mafEventBase *event);
  virtual void OnDeviceNameChanged(mafEventBase *event);
  virtual void OnBindDeviceToAction(mafEvent *e);
  virtual void OnAddAvatar(mafEventBase *event);
  virtual void OnRemoveAvatar(mafEventBase *event);

  /** Create the GUI dialog. */
  void CreateGUI();

  mafGUI*                 m_Gui;  //SIL. 07-jun-2006 : 
  wxFrame*                m_Frame;
  //mafGUIDialog*              m_Dialog;
  mafGUI*                 m_Devices;
  mafGUITree*                m_DeviceTree;
  mafGUICheckListBox*        m_ActionsList;
  mafGUIHolder*           m_SettingsPanel;
  //mafGUINamedPanel*          m_BindingsPanel;
  mafGUI*                 m_Bindings;

  mafDevice*              m_CurrentDevice;
  mafString			          m_SettingFileName;
  
  mafDeviceManager*       m_DeviceManager; 
  mafInteractorPER*                 m_PositionalEventRouter;
  mafInteractorSER*                 m_StaticEventRouter;
  
  mafDeviceClientMAF           *m_ClientDevice;
  
  mmuAvatarsMap                       m_Avatars; ///< keeps a list of visible avatars
  std::list<mafAutoPointer<mafInteractorPER> >  m_PERList; ///< the interactor devoted to Positional Event Routing

  std::set<mafView *>                 m_CameraUpdateRequests; ///< requests for Camera update of single views
  
  mafView*                            m_SelectedView;     ///< the view currently selected
  vtkRenderer*                        m_CurrentRenderer;  ///< the renderer of selected view: to be removed!
  int                                 m_LockRenderingFlag;///< 
  mafTimeStamp                        m_LastRenderTime;   ///< used to avoid overloading of the GUI process due to rendering
  mafTimeStamp                        m_IntraFrameTime;   ///< the minimum time to elapse between two subsequent renderings

private:
  /** hidden to not be called directly */
  int Store(mafStorageElement *element) {return mafStorable::Store(element);}
  
  /** hidden to not be called directly */
  int Restore(mafStorageElement *element) {return mafStorable::Restore(element);}
};
#endif 
