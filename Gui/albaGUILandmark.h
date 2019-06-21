/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILandmark
 Authors: Stefano Perticoni - porting Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUILandmark_H__
#define __albaGUILandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVME.h"
#include "albaEvent.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIButton;
class albaInteractorGenericMouse;
class albaInteractorCompositorMouse;
class albaInteractorPicker;
class albaInteractor;
class albaVMELandmark;
class albaVMELandmarkCloud;
 
//----------------------------------------------------------------------------
/** Pluggable component for albaOpIterativeRegistration operation

  @sa
  - albaOpIterativeRegistration 
 
  @todo
  - Code cleaning and improve documentation
*/

class ALBA_EXPORT albaGUILandmark : public albaObserver, public albaServiceClient
{
public:
  /** constructor */
  albaGUILandmark(albaVME *InputVME, albaObserver *listener = NULL, bool testMode  = false);
	/** destructor */
  ~albaGUILandmark(); 

  /** Set the event receiver object*/
  void  SetListener(albaObserver *Listener) {m_Listener = Listener;};

  /** Events handling*/        
  virtual void OnEvent(albaEventBase *alba_event);
 
  //----------------------------------------------------------------------------
  //gui constants: 
  //----------------------------------------------------------------------------

  // this constants must be visible from the owner object  
  enum
  {
      ID_COLOR = MINID,
      ID_TRANSLATE_X,
      ID_TRANSLATE_Y,
      ID_TRANSLATE_Z,
      ID_REF_SYS,  
  };
      
  enum GUI_STATUS
  {
    DISABLED,
    PICK,
    ENABLED,
  };
 
  void SetGUIStatusToDisabled();
  void SetGUIStatusToPick();
  void SetGUIStatusToEnabled();
  int GetGuiStatus() {return m_GUIStatus;};

  albaVMELandmarkCloud *GetLandmarkCLoud() {return m_LMCloud;};
 
  void SetInputVME(albaVME *vme); 
    
  albaGUI *GetGui() {return m_Gui;};

  void SetLMCloudName(const char *name) {m_LMCloudName = name;};
  const char *GetLMCloudName() {return m_LMCloudName.GetCStr();};

  void SpawnLMOn() {m_SpawnLM = 1;};
  void SpawnLMOff() {m_SpawnLM = 0;};
  int GetSpawnLM() {return m_SpawnLM;};

  static bool VmeAccept(albaVME* node) {return(node != NULL);};

  void SetRadiusFromBoundsFraction(int fraction){m_BoundsFraction = fraction;}

protected: 
  /** Create interactors */
  void CreateTranslateISACompositor();

  /** Override superclass */
  void CreateGui();

  /** Attach interactor to vme; return 0 on success, -1 otherwise  */
  int AttachInteractor(albaVME *vme, albaInteractor *newInteractor,
  albaInteractor *storeOldInteractor);

  int AttachInteractor(albaVME *vme, albaInteractor *newInteractor);

  void UpdateGuiInternal();

  void GetSpawnPointCoordinates(double newPointCoord[3]);
  void SpawnLandmark();

  void UpdateInteractor();

  void SetGuiAbsPosition(vtkMatrix4x4* absPose, albaTimeStamp timeStamp = -1);
  void TextEntriesChanged();
  void RefSysVmeChanged();

  void OnVmePicked(albaEvent& e);
  void SetRefSysVME(albaVME* refSysVME);

  void OnRefSysVmeChanged(); 
  void OnTextEntriesChanged();
  void OnTranslate(albaEvent &e);

  double m_CurrentTime;
  double m_Position[3];
//  double m_LMPosition[3];

  bool m_TestMode;
  
  albaInteractor* m_OldInteractor;
  albaInteractorPicker *m_PickerInteractor;
  albaInteractorCompositorMouse *m_IsaCompositor;
  albaInteractorGenericMouse *m_IsaTranslate;
  albaInteractorGenericMouse *m_IsaTranslateSnap;
  albaInteractor *m_OldInputVMEBehavior;

  albaObserver *m_Listener;
  albaGUI      *m_Gui;

  albaString m_LMCloudName;
  albaString m_RefSysVMEName;

  albaVME *m_InputVME;
  albaVME *m_RefSysVME;
  albaVMELandmarkCloud *m_LMCloud;
  albaVMELandmark *m_Landmark;

  const char *m_LandmarkName;

  int m_GUIStatus;
  int m_SpawnLM;
  int m_BoundsFraction;

};
#endif
