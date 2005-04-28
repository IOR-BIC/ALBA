/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractor.h,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:12 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafInteractor_h
#define __mafInteractor_h

#ifdef __GNUG__
    #pragma interface "mafInteractor.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mflAgent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDevice;

class mflEventInteraction;
class mafEventListener;
class mflVME;
class mflMatrix;

class vtkRenderer;
class vtkProp3D;

/** Abstract class for MAF interactors
  This class provideds base interface and features of MAF continuous event interactors.
  Among the available features we find support for a current renderer member variable,
  that may be used by interactors to perform interaction according to user point
  of view, current device member variable, where the device performing the interaction
  is stored. In general the interactor starts working when a StartInteraction event
  is received and stops interacting when a StopInteractionEvent arrives. These two events
  are by default mapped to ButtonDownEvent and ButtonUpEvent, but can be redefined
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
  - MULTI_BUTTON_MODE: in this case interaction starts when pressing the StartButton (or any button in case of -1).
    If other buttons are pressed a button down event for them are sent too. Interaction is stopped when the last button is
    released! (end of interaction means the device is released!)

  Interactors usually send output events through the DefaultChannel, and also through the m_listener by wrapping
  the event inside a MFL_EVENT_ID event (event can be retrieved with mafEvent::GetVtkObject().
*/
class mafInteractor : public mflAgent
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  MFL_EVT_DEC(InteractionStartedEvent); ///< Issued when interaction is started
  MFL_EVT_DEC(InteractionStoppedEvent); ///< Issued when interaction is stopped
  MFL_EVT_DEC(ButtonDownEvent);         ///< Issued when a button is pressed
  MFL_EVT_DEC(ButtonUpEvent);           ///< Issued when a button is released

  vtkTypeMacro(mafInteractor,mflAgent);

  /**  
    Set/Get the renderer this interactor is attached to. This methos is redefined
    in subclasses to perform specialized actions when the renderer is changed */
  virtual void SetRenderer(vtkRenderer *ren);
  vtkRenderer *GetRenderer() {return Renderer;}

  /** Return the current input device. */
  mafDevice *GetDevice() {return Device;}
  /** Set the current input device. */
  virtual void SetDevice(mafDevice *);

  /** Set the node to be transformed */
  virtual void SetVME(mflVME *vme);

  /** Set the prop to be transformed (optional and to be removed!!!)*/
  void SetProp(vtkProp3D *prop);

  /** 
    Enable/Disable trigger events processing. Trigger events are StartInteraction
    and StopInteraction events which are used to start/stop the interaction. If enabled
    this flag makes the interactor to ingore these events and to be continuously active.
    Default is false. */
  void SetIgnoreTriggerEvents(int flag) {IgnoreTriggerEvents=flag;Modified();}
  int GetIgnoreTriggerEvents(int flag) {return IgnoreTriggerEvents;}
  vtkBooleanMacro(IgnoreTriggerEvents,int);

  /**  Set the event used to trigger the start of an interaction session */
  void SetStartInteractionEvent(mafID event) {StartInteractionEvent=event;Modified();}
  /**  Get the event used to trigger the start of an interaction session */
  mafID GetStartInteractionEvent() {return StartInteractionEvent;}
  
  /**  Set the event used to trigger the end of an interaction session */
  void SetStopInteractionEvent(mafID event) {StopInteractionEvent=event;Modified();}
  /**  Set the event used to trigger the end of an interaction session */
  mafID GetStopInteractionEvent() {return StopInteractionEvent;}

  /** Set the modifier that should start the interaction */
  void SetStartButton(int btn) {StartButton=btn;Modified();}
  int GetStartButton() {return StartButton;}

  /** Set the modifier that should start the interaction */
  void SetModifiers(int mod) {Modifiers=mod;Modified();}
  int GetModifiers() {return Modifiers;}

  /** return the button which started the interaction */
  int GetCurrentButton() {return CurrentButton;}

  /** return the modifers at start of interaction */
  unsigned long GetCurrentModifier() {return CurrentModifier;}

  /**
    Start the interaction, the optional argument force sending
    an event on the DefaultChannel advising the interaction has started.
    This function is responsible to decide if the interaction can start.
    It also stores the Device starting the interaction and locks it and 
    locks the interactor itself.
  */
  virtual int StartInteraction(mafDevice *device);
  
  /**  Stop the interaction, the optional argument force sending
    an event on the DefaultChannel advising the interaction has started.
    This also unlock the device.
  */
  virtual int StopInteraction(mafDevice *device);

  /** return true if currently in a interaction session */
  virtual bool IsInteracting();

  virtual bool IsInteracting(mafDevice *device);

  enum modalities {SINGLE_BUTTON_MODE=0,MULTI_BUTTON_MODE};

  void SetButtonMode(int mode) {ButtonMode=mode;}
  int GetButtonMode() {return ButtonMode;}
  void SetButtonModeToSingleButton() {ButtonMode=SINGLE_BUTTON_MODE;}
  void SetButtonModeToMultiButton() {ButtonMode=MULTI_BUTTON_MODE;}

  /** enable/disable device locking when interaction starts. Default is true! */
  void SetLockDevice(bool flag) {LockDevice = flag;}
  bool GetLockDevice() {return LockDevice;}

  /** Set the litener for MAF events: to be merged into SetListener in the future */
  void SetMListener(mafEventListener *listener) {m_listener=listener;}

  /** return the MAF listener of MAF events. events to the default channel are sent thorugh this too */
  mafEventListener *GetMListener() {return m_listener;}

  void ForwardEvent(mflEvent *event, mafID channel=DefaultChannel);
  void ForwardEvent(int id, mafID channel=DefaultChannel,vtkObjectBase *data=NULL);
  
  /**  Process events coming from input device */
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

protected:
  mafInteractor();
  virtual ~mafInteractor();

  /** 
    Process StartInteractionEvent. If IgnoreTriggerEvents flag or
    interaction is alredy ongoing it does nothing and return false (0),
    else if the event is one of main three buttons return
    true, else false. Subclasses can redefine this function to add support 
    for extra buttons. */
  virtual int OnStartInteraction(mflEventInteraction *e);

  /** 
    Process StopInteractionEvent. If interaction is not ongoing
    or sender is not the Device who started the interaction does
    nothing and return false, else return true. Subclasses can redefine this function to add
    extra functionalities. */
  virtual int OnStopInteraction(mflEventInteraction *e);

  virtual void OnButtonDown       (mflEventInteraction *e);
  virtual void OnButtonUp         (mflEventInteraction *e);

  void ComputeDisplayToWorld(double x, double y, double z, float worldPt[4]);
  void ComputeDisplayToWorld(double x, double y, double z, double worldPt[4]);
  void ComputeWorldToDisplay(double x, double y, double z, double displayPt[3]);
  void ComputeWorldToDisplay(double x, double y, double z, float displayPt[3]);

  /** 
    Find the pocked VME at button down. As argument the function needs
    the device which performed the action, and provides as result pointers
    to piked prop, vme and its behavior if it exists. */
  virtual bool FindPokedVme(mafDevice *device,mflMatrix *point_pose,vtkProp3D *&picked_prop,mflVME *&picked_vme,mafInteractor *&picked_behavior);


  mafDevice     *Device;
  vtkRenderer   *Renderer;
  
  bool          LockDevice;
  bool          DeviceIsSet;            ///< true if the device has been set with SetDevice by other classes  
  mafID         StartInteractionEvent;///< the event ID starting the interaction
  mafID         StopInteractionEvent; ///< the event ID starting the interaction
  int           IgnoreTriggerEvents;  ///< flag to ignore start/stop interaction
  int           InteractionFlag;      ///< flag if interacting
  int           StartButton;          ///< the button that can start the interaction
  unsigned long Modifiers;            ///< modifiers of the button for starting the interaction
  int           CurrentButton;        ///< the button that started the interaction
  unsigned long CurrentModifier;      ///< Modifiers state when interaction was started  
  int           ButtonMode;           ///< Specify if working in single button or multi button mode
  int           ButtonsCounter;       ///< Take count of the number of button pressed.

  mflVME       *VME;                  ///< the object being interacted (optional)
  vtkProp3D    *Prop;                 ///< the prop being interacted (optional)

  mafEventListener *m_listener;       ///< Listener for MAF events

private:
  mafInteractor(const mafInteractor&);  // Not implemented.
  void operator=(const mafInteractor&);  // Not implemented.
};

#endif 
