/*=========================================================================

 Program: MAF2
 Module: mafGUILandmark
 Authors: Stefano Perticoni - porting Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUILandmark_H__
#define __mafGUILandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVME.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIButton;
class mafInteractorGenericMouse;
class mafInteractorCompositorMouse;
class mafInteractorPicker;
class mafInteractor;
class mafVMELandmark;
class mafVMELandmarkCloud;
 
//----------------------------------------------------------------------------
/** Pluggable component for medOpIterativeRegistration operation

  @sa
  - medOpIterativeRegistration 
 
  @todo
  - Code cleaning and improve documentation
*/

class MAF_EXPORT mafGUILandmark : public mafObserver
{
public:
  /** constructor */
  mafGUILandmark(mafNode *InputVME, mafObserver *listener = NULL, bool testMode  = false);
	/** destructor */
  ~mafGUILandmark(); 

  /** Set the event receiver object*/
  void  SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Events handling*/        
  virtual void OnEvent(mafEventBase *maf_event);
 
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

  mafVMELandmarkCloud *GetLandmarkCLoud() {return m_LMCloud;};
 
  void SetInputVME(mafNode *vme); 
    
  mafGUI *GetGui() {return m_Gui;};

  void SetLMCloudName(const char *name) {m_LMCloudName = name;};
  const char *GetLMCloudName() {return m_LMCloudName.GetCStr();};

  void SpawnLMOn() {m_SpawnLM = 1;};
  void SpawnLMOff() {m_SpawnLM = 0;};
  int GetSpawnLM() {return m_SpawnLM;};

  static bool VmeAccept(mafNode* node) {return(node != NULL);};

  void SetRadiusFromBoundsFraction(int fraction){m_BoundsFraction = fraction;}

protected: 
  /** Create interactors */
  void CreateTranslateISACompositor();

  /** Override superclass */
  void CreateGui();

  /** Attach interactor to vme; return 0 on success, -1 otherwise  */
  int AttachInteractor(mafNode *vme, mafInteractor *newInteractor,
  mafInteractor *storeOldInteractor);

  int AttachInteractor(mafNode *vme, mafInteractor *newInteractor);

  void UpdateGuiInternal();

  void GetSpawnPointCoordinates(double newPointCoord[3]);
  void SpawnLandmark();

  void UpdateInteractor();

  void SetGuiAbsPosition(vtkMatrix4x4* absPose, mafTimeStamp timeStamp = -1);
  void TextEntriesChanged();
  void RefSysVmeChanged();

  void OnVmePicked(mafEvent& e);
  void SetRefSysVME(mafVME* refSysVME);

  void OnRefSysVmeChanged(); 
  void OnTextEntriesChanged();
  void OnTranslate(mafEvent &e);

  double m_CurrentTime;
  double m_Position[3];
//  double m_LMPosition[3];

  bool m_TestMode;
  
  mafInteractor* m_OldInteractor;
  mafInteractorPicker *m_PickerInteractor;
  mafInteractorCompositorMouse *m_IsaCompositor;
  mafInteractorGenericMouse *m_IsaTranslate;
  mafInteractorGenericMouse *m_IsaTranslateSnap;
  mafInteractor *m_OldInputVMEBehavior;

  mafObserver *m_Listener;
  mafGUI      *m_Gui;

  mafString m_LMCloudName;
  mafString m_RefSysVMEName;

  mafVME *m_InputVME;
  mafVME *m_RefSysVME;
  mafVMELandmarkCloud *m_LMCloud;
  mafVMELandmark *m_Landmark;

  const char *m_LandmarkName;

  int m_GUIStatus;
  int m_SpawnLM;
  int m_BoundsFraction;

};
#endif
