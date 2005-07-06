/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiTransformInterface.h,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:29 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGuiTransformInterface_H__
#define __mafGuiTransformInterface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;
class mafVME;

//----------------------------------------------------------------------------
/** transform gui interface

  This abstract class defines interface for object to be plugged in mmoTransformInterface concrete descendants

  @sa
  mmoTransformInterface, mmoMAFTransform
 
  @todo

*/
class mafGuiTransformInterface : public mafObserver
{
public:

  /** Return the gui to be plugged*/
  mmgGui *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable) = 0;

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(mafVME *refSysVme); 
  mafVME* GetRefSys() {return RefSysVME;};

  /** Reset the gui component to initial state */
  virtual void Reset() {};

  /** Events handling */  
  void OnEvent(mafEventBase *maf_event) {};
  
protected:
  
  mafGuiTransformInterface();
  ~mafGuiTransformInterface(); 
   
  /** Create the GUI */
  virtual void CreateGui() {};

  mafVME *InputVME;

  mafObserver *m_Listener;
  mmgGui      *m_Gui;
   
  /** Vme to be used as reference system */
  mafVME *RefSysVME;

  /** Internal actions to be performed when RefSys vme has changed */
  virtual void RefSysVmeChanged() {};

  mafTimeStamp CurrentTime;
};
#endif
