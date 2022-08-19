/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionManager
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractionManager_h
#define __albaInteractionManager_h

#include "albaObject.h"
#include "albaObserver.h"
#include "albaEventSender.h"
#include "albaStorable.h"
#include "albaString.h"
#include "albaSmartPointer.h"

#include <map>
#include <set>
#include <list>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaEventBase;
class albaEvent;
class albaAction;
class albaInteractor;
#ifdef ALBA_EXPORTS
#include "albaAvatar.h"
#else
class albaAvatar;
#endif
class albaDevice;
class albaDeviceManager;
class albaDeviceSet;
class albaDeviceButtonsPadMouse;
class albaGUIInteractionSettings;
class vtkRenderer;
class vtkRendererWindow;
class albaView;
class mflXMLWriter;
class vtkXMLDataElement;
class vtkXMLDataParser;
class albaInteractorSER;
class albaInteractorPER;
class albaVME;
class albaGUIDialog;
class albaGUI;
class albaGUITree;
class albaGUICheckListBox;
class albaGUIHolder;
class albaGUINamedPanel;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_MAP(ALBA_EXPORT,albaString,albaAutoPointer<albaAvatar>);
EXPORT_STL_VECTOR(ALBA_EXPORT, albaAvatar *);
EXPORT_STL_LIST(ALBA_EXPORT,albaAutoPointer<albaInteractorPER>);
#endif


/** This class takes care of mastering the interaction inside views.
  This class is responsible to coordinate interaction, i.e. 
  resources necessary for interaction (currently devices, actions and avatars)
  and to manage communication routing for interaction. Management of devices is
  delegated to the DeviceManager object, picking and routing of events to 
  picked VME is delegated to PER (positional event router) object, and low level 
  routing from devices to actions is delegated to the SER (static event router) 
  object.
  @sa albaDeviceManager albaInteractorPER albaInteractorSER 
  
  @todo
  - check the renderer to be RenFront
*/
class ALBA_EXPORT albaInteractionManager : public albaObject, public albaObserver, public albaEventSender, public albaStorable
{
public:
  albaInteractionManager();
  virtual ~albaInteractionManager();

  albaTypeMacro(albaInteractionManager,albaObject);

  /** Enable/Disable VME selection by picking */
  void EnableSelect(bool enable);

  /**  Process incoming events. */
  virtual void OnEvent(albaEventBase *event);

  /** Get the device manager object */
  albaDeviceManager *GetDeviceManager() {return m_DeviceManager;}

  /** return the mouse device */
  albaDeviceButtonsPadMouse *GetMouseDevice();

  /** return the mouse action, an action to which mouse is bound by default */
  //albaAction *GetMouseAction();

  /** Define a new action router.*/
  albaAction *AddAction(const char *name, float priority = 0.0);

  /** 
    Bind an agent to the specified action. 
    If the action is not present return -1 */
  int BindAction(const char *action,albaInteractor *agent);

  /** 
    Unbind an agent from the specified action. 
    If the action is not present return -1 */
  int UnBindAction(const char *action,albaInteractor *agent);

  /** Define a new avatar */
  void AddAvatar(albaAvatar *avatar);

  /** Remove an avatar */
  int RemoveAvatar(albaAvatar *avatar);

  /** Get an avatar given its name.*/
  albaAvatar *GetAvatar(const char *name);

  typedef std::map<albaString,albaAutoPointer<albaAvatar> > mmuAvatarsMap;
  typedef std::vector<albaAvatar *> mmuAvatarsVector;

  /** Return the avatars container */
  const mmuAvatarsMap &GetAvatars();

  /** return an array with the list of avatars currently connected */
  void GetAvatars(mmuAvatarsVector &avatars);

  /** Get an action router.*/
  albaAction *GetAction(const char *name);

  /** Set the selected view. This makes all dynamic avatars
    and interactors to be informed of the current renderer. */
  void ViewSelected(albaView *view);
  
  /** Return the currently selected view */
  albaView *GetSelectedView(albaView *view) {return m_SelectedView;}

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
  void  VmeSelected(albaVME *vme);

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
  void CameraUpdate(albaView *view=NULL);


  /** return the positional event router */
  albaInteractorPER *GetPER() {return m_PositionalEventRouter;}

  /** used to override the default PER. To be called before initializations */
  void SetPER(albaInteractorPER *per);

  /** set a new PER and keep the old one in a list */
  void PushPER(albaInteractorPER *per);
  
  /** restore previous PER instance */
  bool PopPER();

  /** return the static event router */
  albaInteractorSER *GetSER() {return m_StaticEventRouter;}

  /** Set the FlyTo mode for the selected view. */
  void CameraFlyToMode();

  /** return the minimum time to elapse between two subsequent renderings */
  albaTimeStamp GetIntraFrameTime() {return m_IntraFrameTime;}
  /** set the minimum time to elapse between two subsequent renderings */
  void SetIntraFrameTime(albaTimeStamp iftime) {m_IntraFrameTime=iftime;}

  /** Open a dialog to choose among available devices */
  int DeviceChooser(wxString &dev_name,wxString &dev_type);

  /** This is used to allow also external objects to add devices */
  void AddDeviceToTree(albaDevice *device,albaDeviceSet *parent=NULL);
  void RemoveDeviceFromTree(albaDevice *device);

  /** Update names in device list */
  //void UpdateDeviceTree();

  /** Update the name of a device */
  void UpdateDevice(albaDevice *device);

  /** update bindings check list */
  void UpdateBindings();

  /** Show in modal configuration the settings dialog. */
  //bool ShowModal(); //SIL. 07-jun-2006 : 
  albaGUI* GetGui();  //SIL. 07-jun-2006 : 

  albaInteractorSER *GetStaticEventRouter() {return m_StaticEventRouter;}

protected:
  /** This is called by Store() to store information of this object.  */
  virtual int InternalStore(albaStorageElement *node);

  /** This is called by Restore() to restore information of this object. */
  virtual int InternalRestore(albaStorageElement *node);
  
  virtual void OnStartDispatching();
  virtual void OnEndDispatching();
  virtual void OnCameraUpdate(albaEventBase *e);
  virtual void OnViewSelected(albaEvent *event);
  virtual void OnDeviceAdded(albaEventBase *event);
  virtual void OnDeviceRemoving(albaEventBase *event);
  virtual void OnDeviceNameChanged(albaEventBase *event);
  virtual void OnBindDeviceToAction(albaEvent *e);
  virtual void OnAddAvatar(albaEventBase *event);
  virtual void OnRemoveAvatar(albaEventBase *event);

  /** Create the GUI dialog. */
  void CreateGUI();

  albaGUI*                 m_Gui;  //SIL. 07-jun-2006 : 
  wxFrame*                m_Frame;
  //albaGUIDialog*              m_Dialog;
  albaGUI*                 m_Devices;
  albaGUITree*                m_DeviceTree;
  albaGUICheckListBox*        m_ActionsList;
  albaGUIHolder*           m_SettingsPanel;
  //albaGUINamedPanel*          m_BindingsPanel;
  albaGUI*                 m_Bindings;

  albaDevice*              m_CurrentDevice;
  albaString			          m_SettingFileName;
  
  albaDeviceManager*       m_DeviceManager; 
  albaInteractorPER*                 m_PositionalEventRouter;
  albaInteractorSER*                 m_StaticEventRouter;
 
  mmuAvatarsMap                       m_Avatars; ///< keeps a list of visible avatars
  std::list<albaInteractorPER *>  m_PERList; ///< the interactor devoted to Positional Event Routing

  std::set<albaView *>                 m_CameraUpdateRequests; ///< requests for Camera update of single views
  
  albaView*                            m_SelectedView;     ///< the view currently selected
  vtkRenderer*                        m_CurrentRenderer;  ///< the renderer of selected view: to be removed!
  int                                 m_LockRenderingFlag;///< 
  albaTimeStamp                        m_LastRenderTime;   ///< used to avoid overloading of the GUI process due to rendering
  albaTimeStamp                        m_IntraFrameTime;   ///< the minimum time to elapse between two subsequent renderings

private:
  /** hidden to not be called directly */
  int Store(albaStorageElement *element) {return albaStorable::Store(element);}
  
  /** hidden to not be called directly */
  int Restore(albaStorageElement *element) {return albaStorable::Restore(element);}
};
#endif 
