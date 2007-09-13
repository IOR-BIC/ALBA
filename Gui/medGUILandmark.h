/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILandmark.h,v $
  Language:  C++
  Date:      $Date: 2007-09-13 09:06:56 $
  Version:   $Revision: 1.1 $
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
class mmgGui;
class mmgButton;
class mmiGenericMouse;
class mmiCompositorMouse;
class mmiPicker;
class mafInteractor;
class mafVMELandmark;
class mafVMELandmarkCloud;
 
//----------------------------------------------------------------------------
/** Pluggable component for mmoIterativeRegistration operation

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
  int GetGuiStatus() {return GUIStatus;};

  mafVMELandmarkCloud *GetLandmarkCLoud() {return LMCloud;};
 
  void SetInputVME(mafNode *vme); 
    
  mmgGui *GetGui() {return m_gui;};

  void SetLMCloudName(const char *name) {LMCloudName = name;};
  const char *GetLMCloudName() {return LMCloudName.GetCStr();};

  void SpawnLMOn() {SpawnLM = 1;};
  void SpawnLMOff() {SpawnLM = 0;};
  int GetSpawnLM() {return SpawnLM;};

  static bool VmeAccept(mafNode* node) {return(node != NULL);};

protected: 

  /** Attach interactor to vme; return 0 on success, -1 otherwise  */
  int AttachInteractor(mafNode *vme, mafInteractor *newInteractor,
  mafInteractor *storeOldInteractor);

  int AttachInteractor(mafNode *vme, mafInteractor *newInteractor);

  double CurrentTime;

  double Position[3];

  mafInteractor* OldInteractor;
   
  /** Create interactors */
  void CreateTranslateISACompositor();

  /** Override superclass */
  void CreateGui();
 
  mmiCompositorMouse *IsaCompositor;

  mmiGenericMouse *IsaTranslate;
  mmiGenericMouse *IsaTranslateSnap;
   
   
  void OnRefSysVmeChanged(); 
  void OnTextEntriesChanged();
  void OnTranslate(mafEvent &e);

  mmiPicker  *PickerInteractor;

  mafVME *InputVME;
  double LMPosition[3];

  mafVMELandmarkCloud *LMCloud;

  mafVMELandmark *Landmark;
  const char *LandmarkName;

  mafInteractor *OldInputVMEBehavior;

  int GUIStatus;

  void UpdateGuiInternal();

  mafObserver *m_Listener;
  mmgGui      	 *m_gui;

  mafVME *RefSysVME;

  mafString LMCloudName;

  int SpawnLM;

  void GetSpawnPointCoordinates(double newPointCoord[3]);
  void SpawnLandmark();
   
  void UpdateInteractor();

  void SetGuiAbsPosition(vtkMatrix4x4* absPose, mafTimeStamp timeStamp = -1);
  void TextEntriesChanged();
  void RefSysVmeChanged();

  void OnVmePicked(mafEvent& e);
  mafString RefSysVMEName;

  void SetRefSysVME(mafVME* refSysVME);
};
#endif
