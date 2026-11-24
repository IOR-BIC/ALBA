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
#include "albaString.h"
#include "albaSmartPointer.h"
#include "albaServiceClient.h"

#include <map>
#include <set>
#include <list>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaEventBase;
class albaEvent;
class albaInteractionBehavior;
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
class albaToolFlyTo;
class albaPositionalEventRouter;
class albaInteractionTool;
class albaVME;
class albaGUIDialog;
class albaGUI;
class albaGUITree;
class albaGUIHolder;
class albaGUINamedPanel;



/** This class takes care of mastering the interaction inside views.
  This class is responsible to coordinate interaction, i.e. 
  resources necessary for interaction (currently devices)
  and to manage communication routing for interaction. Management of devices is
  delegated to the DeviceManager object, picking and routing of the mouse events to 
  picked VME is delegated to PER (positional event router) object, which observes the mouse.
  @sa albaDeviceManager albaPositionalEventRouter 
  
  @todo
  - check the renderer to be RenFront
*/
class ALBA_EXPORT albaInteractionManager : public albaObject, public albaObserver, public albaEventSender, public albaServiceClient
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

  /** Set the selected view: the mouse works on it. */
  void ViewSelected(albaView *view);
  
  /** Return the currently selected view */
  albaView *GetSelectedView(albaView *view) {return m_SelectedView;}


  /** return the positional event router */
  albaPositionalEventRouter *GetPER() {return m_PositionalEventRouter;}

  /** Push a tool on the tool stack of the PER: it receives the events before the default interaction */
  void PushTool(albaInteractionTool *tool);

  /** Remove a tool from the tool stack of the PER */
  void PopTool(albaInteractionTool *tool);

  /** Send a key pressed or released to the tools of the PER; return true if a tool used it */
  bool OnKey(int keyCode, bool pressed);

  /** Enable/disable the event tracing: each input event routed by the PER is written in the log
  with the device, the button, the modifiers and who handled it. Disabled by default. */
  void SetEventTracing(bool enable);

  /** Start the fly-to mode: push the fly-to tool, the next click flies to the clicked point */
  void CameraFlyToMode();

  /** Open a dialog to choose among available devices */
  int DeviceChooser(wxString &dev_name,wxString &dev_type);

  /** This is used to allow also external objects to add devices */
  void AddDeviceToTree(albaDevice *device,albaDeviceSet *parent=NULL);
  void RemoveDeviceFromTree(albaDevice *device);

  /** Update names in device list */
  //void UpdateDeviceTree();

  /** Update the name of a device */
  void UpdateDevice(albaDevice *device);

  /** Show in modal configuration the settings dialog. */
  //bool ShowModal(); //SIL. 07-jun-2006 : 
  albaGUI* GetGui();  //SIL. 07-jun-2006 : 

protected:
  /** The camera updates requested while the device events are dispatched are fused into one at the end of the dispatching */
  virtual void OnStartDispatching();
  virtual void OnEndDispatching();
  virtual void OnDeviceAdded(albaEventBase *event);
  virtual void OnDeviceRemoving(albaEventBase *event);
  virtual void OnDeviceNameChanged(albaEventBase *event);

  /** Create the GUI dialog. */
  void CreateGUI();

  albaGUI*                 m_Gui;  //SIL. 07-jun-2006 : 
  wxFrame*                m_Frame;
  //albaGUIDialog*              m_Dialog;
  albaGUI*                 m_Devices;
  albaGUITree*                m_DeviceTree;
  albaGUIHolder*           m_SettingsPanel;

  albaDevice*              m_CurrentDevice;
  
  albaDeviceManager*       m_DeviceManager; 
  albaPositionalEventRouter*                 m_PositionalEventRouter;
  albaToolFlyTo*                          m_FlyToTool; ///< tool of the fly-to mode
 
  albaView*                            m_SelectedView;     ///< the view currently selected

};
#endif 
