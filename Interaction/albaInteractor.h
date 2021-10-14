/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractor_h
#define __albaInteractor_h

#include "albaAgent.h"
#include "albaString.h"
#include "vtkALBASmartPointer.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaDevice;
class albaEventInteraction;
class albaVME;
class albaMatrix;

class vtkRenderer;
class vtkProp3D;

/** Abstract class for ALBA interactors
  This class provides base interface and features of ALBA continuous event interactors.
  Among the available features we find support for a current renderer member variable,
  that may be used by interactors to perform interaction according to user point
  of view, current device member variable, where the device performing the interaction
  is stored. In general the interactor starts working when a StartInteraction event
  is received and stops interacting when a m_StopInteractionEvent arrives. These two events
  are by default mapped to BUTTON_DOWN and BUTTON_UP, but can be redefined
  as necessary. The interactor can also be programmed to ignore start and stop interaction
  events and run continuously. During an interaction session the current render window and
  the current device cannot be changed!!! Subclasses can redefine StartInteraction() and 
  StopInteraction() functions to perform special tasks on starting/stopping of the interaction.
  When interaction is started the device is locked
  The interactor class can be programmed in different ways:
  - SINGLE_BUTTON_MODE: In this case the interaction starts when a button is pressend, and if other buttons
    are pressed they are simply ignored. The button starting the interaction can be specified with SetStartButton(),
    default is button 1 (LEFT_BUTTON). Interaction stops when the same button is pressed. If -1 is specified, ANY 
    button will start/stop the interaction.
  - MULTI_BUTTON_MODE: in this case interaction starts when pressing the m_StartButton (or any button in case of -1).
    If other buttons are pressed a button down event for them are sent too. Interaction is stopped when the last button is
    released! (end of interaction means the device is released!)

  Interactors usually send output events through the DefaultChannel, and also through the m_listener by wrapping
  the event inside a MFL_EVENT_ID event (event can be retrieved with albaEvent::GetVtkObject().
*/
class ALBA_EXPORT albaInteractor : public albaAgent
{
public:
  albaInteractor();
  virtual ~albaInteractor();

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  ALBA_ID_DEC(INTERACTION_STARTED); ///< Issued when interaction is started
  ALBA_ID_DEC(INTERACTION_STOPPED); ///< Issued when interaction is stopped
  ALBA_ID_DEC(BUTTON_DOWN);         ///< Issued when a button is pressed
  ALBA_ID_DEC(BUTTON_UP);           ///< Issued when a button is released

  albaAbstractTypeMacro(albaInteractor,albaAgent);

  /**  
    Set/Get the renderer this interactor is attached to. This methods is redefined
    in subclasses to perform specialized actions when the renderer is changed */
  virtual void SetRendererAndView(vtkRenderer *ren, albaView *view);
  vtkRenderer *GetRenderer() {return m_Renderer;}
	albaView *GetView() {return m_View;}

  /** Return the current input device. */
  albaDevice *GetDevice() {return m_Device;}
  /** Set the current input device. */
  virtual void SetDevice(albaDevice *);

  /** Set the node to be transformed */
  virtual void SetVME(albaVME *vme);

  /** Set the prop to be transformed (optional and to be removed!!!)*/
  void SetProp(vtkProp3D *prop);

  /** 
    Enable/Disable trigger events processing. Trigger events are StartInteraction
    and StopInteraction events which are used to start/stop the interaction. If enabled
    this flag makes the interactor to ignore these events and to be continuously active.
    Default is false. */
  void SetIgnoreTriggerEvents(bool flag) {m_IgnoreTriggerEvents=flag;}
  int GetIgnoreTriggerEvents(bool flag) {return m_IgnoreTriggerEvents;}
  void IgnoreTriggerEventsOn() {SetIgnoreTriggerEvents(true);}
  void IgnoreTriggerEventsOff() {SetIgnoreTriggerEvents(false);}

  /**  Set the event used to trigger the start of an interaction session */
  void SetStartInteractionEvent(albaID event) {m_StartInteractionEvent=event;}
  /**  Get the event used to trigger the start of an interaction session */
  albaID GetStartInteractionEvent() {return m_StartInteractionEvent;}
  
  /**  Set the event used to trigger the end of an interaction session */
  void SetStopInteractionEvent(albaID event) {m_StopInteractionEvent=event;}
  /**  Set the event used to trigger the end of an interaction session */
  albaID GetStopInteractionEvent() {return m_StopInteractionEvent;}

  /** Set the modifier that should start the interaction */
  void SetStartButton(int btn) {m_StartButton=btn;}
  int GetStartButton() {return m_StartButton;}

  /** Set the modifier that should start the interaction */
  void SetModifiers(int mod) {m_Modifiers=mod;}
  int GetModifiers() {return m_Modifiers;}

  /** return the button which started the interaction */
  int GetCurrentButton() {return m_CurrentButton;}

  /** return the modifers at start of interaction */
  unsigned long GetCurrentModifier() {return m_CurrentModifier;}

  /**
    Start the interaction, the optional argument force sending
    an event on the DefaultChannel advising the interaction has started.
    This function is responsible to decide if the interaction can start.
    It also stores the m_Device starting the interaction and locks it and 
    locks the interactor itself. */
  virtual int StartInteraction(albaDevice *device);
  
  /**  Stop the interaction, the optional argument force sending
    an event on the DefaultChannel advising the interaction has started.
    This also unlock the device. */
  virtual int StopInteraction(albaDevice *device);

  /** return true if currently in a interaction session */
  virtual bool IsInteracting();

  /** return true if the specified device is currently in a interaction session */
  virtual bool IsInteracting(albaDevice *device);

  enum modalities {SINGLE_BUTTON_MODE=0,MULTI_BUTTON_MODE};

  void SetButtonMode(int mode) {m_ButtonMode=mode;}
  int GetButtonMode() {return m_ButtonMode;}
  void SetButtonModeToSingleButton() {m_ButtonMode=SINGLE_BUTTON_MODE;}
  void SetButtonModeToMultiButton() {m_ButtonMode=MULTI_BUTTON_MODE;}

    
  /**  Process events coming from input device */
  virtual void OnEvent(albaEventBase *event);

  /** Turn On m_TestMode flag.*/
  void TestModeOn() {m_TestMode = true;};

  /** Used to turn off m_TestMode flag.*/
  void TestModeOff() {m_TestMode = false;};

  /** Get TestMode*/
  bool GetTestMode(){return m_TestMode;};

protected:

  /** 
    Process StartInteractionEvent. If m_IgnoreTriggerEvents flag or
    interaction is alredy ongoing it does nothing and return false (0),
    else if the event is one of main three buttons return
    true, else false. Subclasses can redefine this function to add support 
    for extra buttons. */
  virtual int OnStartInteraction(albaEventInteraction *e);

  /** 
    Process m_StopInteractionEvent. If interaction is not ongoing
    or sender is not the m_Device who started the interaction does
    nothing and return false, else return true. Subclasses can redefine this function to add
    extra functionalities. */
  virtual int OnStopInteraction(albaEventInteraction *e);

  virtual void OnButtonDown       (albaEventInteraction *e);
  virtual void OnButtonUp         (albaEventInteraction *e);

  void ComputeDisplayToWorld(double x, double y, double z, double worldPt[4]);
  void ComputeWorldToDisplay(double x, double y, double z, double displayPt[3]);


  /** 
    Find the pocked VME at button down. As argument the function needs
    the device which performed the action, and provides as result pointers
    to piked prop, vme and its behavior if it exists. */
//  virtual bool FindPokedVme(albaDevice *device,albaMatrix &point_pose,vtkProp3D *&picked_prop,albaVME *&picked_vme,albaInteractor *&picked_behavior);


  albaDevice     *m_Device;

  bool          m_DeviceIsSet;          ///< true if the device has been set with SetDevice by other classes  
  albaID         m_StartInteractionEvent;///< the event ID starting the interaction
  albaID         m_StopInteractionEvent; ///< the event ID starting the interaction
  int           m_IgnoreTriggerEvents;  ///< flag to ignore start/stop interaction
  int           m_InteractionFlag;      ///< flag if interacting
  int           m_StartButton;          ///< the button that can start the interaction
  unsigned long m_Modifiers;            ///< modifiers of the button for starting the interaction
  int           m_CurrentButton;        ///< the button that started the interaction
  unsigned long m_CurrentModifier;      ///< m_Modifiers state when interaction was started  
  int           m_ButtonMode;           ///< Specify if working in single button or multi button mode
  int           m_ButtonsCounter;       ///< Take count of the number of button pressed.

  albaVME       *m_VME;                  ///< the object being interacted (optional)
	albaView			*m_View;								 ///< the view this interactor is working on

  vtkALBASmartPointer<vtkRenderer> m_Renderer; ///< the renderer this interactor is working on
	vtkALBASmartPointer<vtkProp3D>   m_Prop;     ///< the prop being interacted (optional)

  bool m_TestMode;///< Flag used with cppunitTest

private:
  albaInteractor(const albaInteractor&);  // Not implemented.
  void operator=(const albaInteractor&);  // Not implemented.
};

#endif 
