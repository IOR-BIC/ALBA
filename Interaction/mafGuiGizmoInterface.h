/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoInterface.h,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:27 $
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
class mmgGui;

//---------------------------------------------------------------------------- 
/** abstract base class for gizmo gui

  @sa
 
  @todo
*/
class mafGuiGizmoInterface : public mafObserver
{
public:

  virtual ~mafGuiGizmoInterface(); 

  virtual void OnEvent(mafEventBase *maf_event) {};

  /** Return the gui to be plugged*/
  mmgGui *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable) {};

protected:
  virtual void CreateGui() {};
  
  mafGuiGizmoInterface(mafObserver *listener = NULL);

  mafObserver *m_Listener;
  mmgGui      *m_Gui;    
};
#endif
