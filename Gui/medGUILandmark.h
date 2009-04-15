/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILandmark.h,v $
  Language:  C++
  Date:      $Date: 2009-04-15 14:11:22 $
  Version:   $Revision: 1.4.2.1 $
  Authors:   Stefano Perticoni - porting Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medGUILandmark_H__
#define __medGUILandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIButton;
class mmiGenericMouse;
class mmiCompositorMouse;
class mmiPicker;
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

class medGUILandmark : public mafObserver
{
public:

  medGUILandmark(mafNode *InputVME, mafObserver *listener = NULL);
	~medGUILandmark(); 

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
  
  mafInteractor* m_OldInteractor;
  mmiPicker  *m_PickerInteractor;
  mmiCompositorMouse *m_IsaCompositor;
  mmiGenericMouse *m_IsaTranslate;
  mmiGenericMouse *m_IsaTranslateSnap;
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
