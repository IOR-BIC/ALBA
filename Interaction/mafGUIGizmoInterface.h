/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIGizmoInterface.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:20:00 $
  Version:   $Revision: 1.1.2.3 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGizmoGuiInterface_H__
#define __mafGizmoGuiInterface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mflMatrix;
class mafGUI;

//---------------------------------------------------------------------------- 
/** abstract base class for gizmo gui

  @sa
 
  @todo
  - add an ivar to register widgets enabling status: should also refactor dependencies to use this flag.
  - provide a barebone implementation for functions doing nothing
  - enforce superclass implementation by using pure virtuals
*/

class MAF_EXPORT mafGUIGizmoInterface : public mafObserver
{
public:

  virtual ~mafGUIGizmoInterface(); 

  virtual void OnEvent(mafEventBase *maf_event) {};

  /** Return the gui to be plugged*/
  mafGUI *GetGui() {return m_Gui;};

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
  
  mafGUIGizmoInterface(mafObserver *listener = NULL);

  mafObserver *m_Listener;
  mafGUI      *m_Gui;    
  bool m_TestMode;

  /** Test friend */
  friend class mafGUIGizmoInterfaceTest;
};
#endif
