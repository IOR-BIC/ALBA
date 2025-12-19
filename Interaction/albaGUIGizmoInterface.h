/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoInterface
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoGuiInterface_H__
#define __albaGizmoGuiInterface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mflMatrix;
class albaGUI;
class albaObserver;

//---------------------------------------------------------------------------- 
/** abstract base class for gizmo gui

  @sa
 
  @todo
  - add an ivar to register widgets enabling status: should also refactor dependencies to use this flag.
  - provide a barebone implementation for functions doing nothing
  - enforce superclass implementation by using pure virtuals
*/

class ALBA_EXPORT albaGUIGizmoInterface : public albaObserver
{
public:

  virtual ~albaGUIGizmoInterface(); 

  virtual void OnEvent(albaEventBase *alba_event) {};

  /** Return the gui to be plugged*/
  albaGUI *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable) {};
  
  /** Turn On m_TestMode flag. 
  The m_TestMode flag is used to exclude the execution of splash screen or wxBusyInfo that conflicts with test machine.*/
  void TestModeOn() {m_TestMode = true;};

  /** Used to turn off m_TestMode flag.*/
  void TestModeOff() {m_TestMode = false;};

  /** Get TestMode*/
  bool GetTestMode(){return m_TestMode;};

protected:
  virtual void CreateGui() {};
  
  albaGUIGizmoInterface(albaObserver *listener = NULL);

  albaObserver *m_Listener;
  albaGUI      *m_Gui;    
  bool m_TestMode;

  /** Test friend */
  friend class albaGUIGizmoInterfaceTest;
};
#endif
