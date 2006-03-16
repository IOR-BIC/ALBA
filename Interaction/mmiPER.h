/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPER.h,v $
  Language:  C++
  Date:      $Date: 2006-03-16 09:19:42 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone, originally by Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiPER_h
#define __mmiPER_h

#include "mafInteractor.h"
#include <map>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVME;
class myPIMPL;
class vtkCamera;

/** Class implementing routing of events based on picking.
  This class routes events from input devices depending on picking information.
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
  mafTypeMacro(mmiPER,mafInteractor);

  virtual void EnableSelect(bool enable) {m_CanSelect = enable;};

  /** Enable fly to mode: next click will select where to fly to */
  virtual void FlyToMode() {m_FlyToFlag = true;};

  /** Waiter to auto adjust camera clipping range */
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

  virtual void OnEvent(mafEventBase *event);

  void OnVmeSelected(mafVME *vme){ SetPickedVME(GetDevice(),vme); }

  enum modalities {SINGLE_BUTTON=0,MULTI_BUTTON};

  void SetMode(int mode) {m_Mode=mode;}
  int GetMode() {return m_Mode;}
  void SetModeToSingleButton() {m_Mode=SINGLE_BUTTON;}
  void SetModeToMultiButton() {m_Mode=MULTI_BUTTON;}

  /** Add vtkCamera to linked camera vector of the mouse camera interactor.*/
  void LinkCameraAdd(vtkCamera *cam);

  /** Remove vtkCamera to linked camera vector of the mouse camera interactor.*/
  void LinkCameraRemove(vtkCamera *cam);

  /** Remove all vtkCamera to linked camera vector of the mouse camera interactor.*/
  void LinkCameraRemoveAll();

  mafInteractor *GetCameraMouseInteractor() {return m_CameraMouseBehavior;};

  mafInteractor *GetCameraInteractor() {return m_CameraBehavior;};

protected:
  mmiPER();
  virtual ~mmiPER();
  
  /** redefined to support the three buttons */  
  virtual int OnStartInteraction(mafEventInteraction *e);
  /** redefined to support the three buttons */
  virtual int OnStopInteraction(mafEventInteraction *e);

  /** 
    Perform a pick on start of interaction to find if the user pointed
    an object in the scene, and in case forward all the following 
    events to its behavior. if the user pointed to the background
    forward events to the camera interactor.*/
  virtual void OnButtonDown       (mafEventInteraction *e);
  
  virtual void OnButtonUp         (mafEventInteraction *e);
  virtual void OnLeftButtonDown   (mafEventInteraction *e);
  virtual void OnLeftButtonUp     (mafEventInteraction *e);
  virtual void OnMiddleButtonDown (mafEventInteraction *e);
  virtual void OnMiddleButtonUp   (mafEventInteraction *e);
  virtual void OnRightButtonDown  (mafEventInteraction *e);
  virtual void OnRightButtonUp    (mafEventInteraction *e);
  virtual void OnMove             (mafEventInteraction *e);
  virtual void OnChar             (mafEventInteraction *e);

  /** internally used to fly to a clicked point */
  virtual void FlyTo(mafEventInteraction *e,int numstep=20, double zoom=1);

  /** insert a device in the set of devices currently interacting */
  void InsertDevice(mafDevice *device,int button);

  /** remove a device in the set of devices currently interacting */
  void RemoveDevice(mafDevice *device);

  /** internally used to set which behavior was picked by device */
  void SetCurrentBehavior(mafDevice *device,mafInteractor *inter);
  mafInteractor *GetCurrentBehavior(mafDevice *device);

  /** internally used to set VME picked by device at button down */
  void SetPickedVME(mafDevice *device,mafVME *vme);

  /** internally used to return the picked VME for given device */
  mafVME *GetPickedVME(mafDevice *device);

  bool            m_FlyToFlag;
  bool            m_DraggingLeft; 
  bool            m_CanSelect; 
  bool	          m_ShowContextMenu; ///< flag for showing the context menu

  int             m_Mode;
  int             m_FirstTime;
  mafInteractor*  m_CameraBehavior; ///< the camera interactor
  mafInteractor*  m_CameraMouseBehavior; ///< the mouse camera interactor

  class DeviceItem
  {
  public:
    mafVME *VME;
    int     Button;
  };
  
  std::map<mafID,DeviceItem> m_Devices; ///< Stores the list of devices currently interacting
  
private:
  mmiPER(const mmiPER&);  // Not implemented.
  void operator=(const mmiPER&);  // Not implemented.
};

#endif 
