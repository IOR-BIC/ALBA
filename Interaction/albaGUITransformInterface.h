/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformInterface
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUITransformInterface_H__
#define __albaGUITransformInterface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaVME;

//----------------------------------------------------------------------------
/** transform gui interface

  This abstract class defines interface for object to be plugged in albaOpTransformInterface concrete descendants

  @sa
  albaOpTransformInterface, albaOpTransformOld
 
  @todo

*/
class ALBA_EXPORT albaGUITransformInterface : public albaObserver
{
public:

  /** Return the gui to be plugged*/
  albaGUI *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable) = 0;

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(albaVME *refSysVme); 
  albaVME* GetRefSys() {return m_RefSysVME;};

  /** Reset the gui component to initial state */
  virtual void Reset() {};

  /** Events handling */  
  void OnEvent(albaEventBase *alba_event) {};
  
  /** Turn On m_TestMode flag. 
  The m_TestMode flag is used to exclude the execution of splash screen or wxBusyInfo that conflicts with test machine.*/
  void TestModeOn() {m_TestMode = true;};

  /** Used to turn off m_TestMode flag.*/
  void TestModeOff() {m_TestMode = false;};

  /** Get TestMode*/
  bool GetTestMode(){return m_TestMode;};

protected:
  
  albaGUITransformInterface();
  ~albaGUITransformInterface(); 
   
  /** Create the GUI */
  virtual void CreateGui() {};

  albaVME *m_InputVME;

  albaObserver *m_Listener;
  albaGUI      *m_Gui;
   
  /** Vme to be used as reference system */
  albaVME *m_RefSysVME;

  /** Internal actions to be performed when RefSys vme has changed */
  virtual void RefSysVmeChanged() {};

  albaTimeStamp m_CurrentTime;

  bool m_TestMode;
};
#endif
