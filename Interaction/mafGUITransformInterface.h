/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUITransformInterface.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 08:44:32 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
class mafGUITransformInterface : public mafObserver
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
};
#endif
