/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPER.h,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:58 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, originally by Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiPER_h
#define __mmiPER_h

#ifdef __GNUG__
    #pragma interface "mmiPER.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafInteractor.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mflVME;
class vtkProp3D;
class mflMatrix;
class myPIMPL;

/** Class implementing routing of events based on picking.
  This class routes events from input devices depending on picking infromation.
  After first picking events are routed to interactor assigned to picked node
  until interaction is finished. To be completed... 
  This is a refactoring of mafISV class by Silvano Imboden
  @todo
  - complete documentation
  - create interactor for CameraBehavior
  - manage sending events to logic (VME_SELECT, CAMERA_FIT  the 
*/
class mmiPER : public mafInteractor
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  //MFL_EVT_DEC(MoveActionEvent); ///< Issued a move action has been performed
 
  vtkTypeMacro(mmiPER,mafInteractor);
  static mmiPER *New();

  virtual void EnableSelect(bool enable) {CanSelect = enable;};

  /** Enable fly to mode: next click will select where to fly to */
  virtual void FlyToMode() {FlyToFlag = true;};

  /** Weither to auto adjust camera clipping range */
  virtual void SetAutoAdjustCameraClippingRange( int value );

  /** 
   return true if device is in the set of devices currently interacting
   the optional parameter can be used in SINGLE_BUTTON mode to know if 
   interaction has been started by a specific button. */
  bool IsInteracting(mafDevice *device, int button=-1);

  /** Redefined to support multiple input devices */
  virtual int StartInteraction(mafDevice *device, int button);
  
  /** Redefined to support multiple input devices */
  virtual int StopInteraction(mafDevice *device, int button);

  virtual void ProcessEvent(mflEvent *event,mafID ch);

  void OnVmeSelected(mflVME *vme){ SetPickedVME(GetDevice(),vme); }

  enum modalities {SINGLE_BUTTON=0,MULTI_BUTTON};

  void SetMode(int mode) {Mode=mode;}
  int GetMode() {return Mode;}
  void SetModeToSingleButton() {Mode=SINGLE_BUTTON;}
  void SetModeToMultiButton() {Mode=MULTI_BUTTON;}

protected:
  mmiPER();
  virtual ~mmiPER();
  
  /** redefined to support the three buttons */  
  virtual int OnStartInteraction(mflEventInteraction *e);
  /** redefined to support the three buttons */
  virtual int OnStopInteraction(mflEventInteraction *e);

  /** 
    Perform a pick on start of interaction to find if the user pointed
    an object in the scene, and in case forward all the following 
    events to its behavior. if the user pointed to the background
    forward events to the camera interactor.*/
  virtual void OnButtonDown       (mflEventInteraction *e);
  
  virtual void OnButtonUp         (mflEventInteraction *e);
  virtual void OnLeftButtonDown   (mflEventInteraction *e);
  virtual void OnLeftButtonUp     (mflEventInteraction *e);
  virtual void OnMiddleButtonDown (mflEventInteraction *e);
  virtual void OnMiddleButtonUp   (mflEventInteraction *e);
  virtual void OnRightButtonDown  (mflEventInteraction *e);
  virtual void OnRightButtonUp    (mflEventInteraction *e);
  virtual void OnMove             (mflEventInteraction *e);
  virtual void OnChar             (mflEventInteraction *e);

  /** internally used to fly to a clicked point */
  virtual void FlyTo(mflEventInteraction *e,int numstep=20, float zoom=1);

  /** insert a device in the set of devices currently interacting */
  void InsertDevice(mafDevice *device,int button);

  /** remove a device in the set of devices currently interacting */
  void RemoveDevice(mafDevice *device);

  /** internally used to set which behavior was picked by device */
  void SetCurrentBehavior(mafDevice *device,mafInteractor *inter);
  mafInteractor *GetCurrentBehavior(mafDevice *device);

  /** internally used to set VME picked by device at button down */
  void SetPickedVME(mafDevice *device,mflVME *vme);

  /** internally used to return the picked VME for given device */
  mflVME *GetPickedVME(mafDevice *device);

 
  bool            FlyToFlag;
  bool            DraggingLeft; 
  bool            CanSelect; 
  bool		        ShowContextMenu; ///< flag for showing the context menù

  int             Mode;
  int             FirstTime;
  mafInteractor		*CameraBehavior; ///< the camera interactor
  mafInteractor		*CameraMouseBehavior; ///< the mouse camera interactor

  myPIMPL         *Devices;        ///< Stores the list of devices currently interacting  
  
private:
  mmiPER(const mmiPER&);  // Not implemented.
  void operator=(const mmiPER&);  // Not implemented.
};

#endif 
