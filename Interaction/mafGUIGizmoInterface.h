/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIGizmoInterface.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 08:44:32 $
  Version:   $Revision: 1.1 $
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
*/
class mafGUIGizmoInterface : public mafObserver
{
public:

  virtual ~mafGUIGizmoInterface(); 

  virtual void OnEvent(mafEventBase *maf_event) {};

  /** Return the gui to be plugged*/
  mafGUI *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable) {};

protected:
  virtual void CreateGui() {};
  
  mafGUIGizmoInterface(mafObserver *listener = NULL);

  mafObserver *m_Listener;
  mafGUI      *m_Gui;    
};
#endif
