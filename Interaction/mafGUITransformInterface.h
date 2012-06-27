/*=========================================================================

 Program: MAF2
 Module: mafGUITransformInterface
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUITransformInterface_H__
#define __mafGUITransformInterface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafVME;

//----------------------------------------------------------------------------
/** transform gui interface

  This abstract class defines interface for object to be plugged in mafOpTransformInterface concrete descendants

  @sa
  mafOpTransformInterface, mafOpMAFTransform
 
  @todo

*/
class MAF_EXPORT mafGUITransformInterface : public mafObserver
{
public:

  /** Return the gui to be plugged*/
  mafGUI *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable) = 0;

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(mafVME *refSysVme); 
  mafVME* GetRefSys() {return m_RefSysVME;};

  /** Reset the gui component to initial state */
  virtual void Reset() {};

  /** Events handling */  
  void OnEvent(mafEventBase *maf_event) {};
  
  /** Turn On m_TestMode flag. 
  The m_TestMode flag is used to exclude the execution of splash screen or wxBusyInfo that conflicts with test machine.*/
  void TestModeOn() {m_TestMode = true;};

  /** Used to turn off m_TestMode flag.*/
  void TestModeOff() {m_TestMode = false;};

  /** Get TestMode*/
  bool GetTestMode(){return m_TestMode;};

protected:
  
  mafGUITransformInterface();
  ~mafGUITransformInterface(); 
   
  /** Create the GUI */
  virtual void CreateGui() {};

  mafVME *m_InputVME;

  mafObserver *m_Listener;
  mafGUI      *m_Gui;
   
  /** Vme to be used as reference system */
  mafVME *m_RefSysVME;

  /** Internal actions to be performed when RefSys vme has changed */
  virtual void RefSysVmeChanged() {};

  mafTimeStamp m_CurrentTime;

  bool m_TestMode;
};
#endif
