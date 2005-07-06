/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoInterface.h,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:24 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#ifndef __mafGizmoInterface_H__
#define __mafGizmoInterface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;
class mafVME;
class mafMatrix;

//----------------------------------------------------------------------------
/** Abstract class for operations gizmos.
  This abstract class defines the interface for gizmos that can be plugged into
  an operation

  @sa
 
  @todo
*/
class mafGizmoInterface : public mafObserver 
{
public:

  /** 
  Set input vme for the gizmo*/
  virtual void SetInput(mafVME *vme) = 0; 
  mafVME *GetInput() {return this->InputVME;};

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /**
  Set the event receiver object*/
  void SetListener(mafObserver *listener) {m_Listener = listener;};

  /**
  Events handling*/        
  virtual void OnEvent(mafEventBase *maf_event) {};
 
  //----------------------------------------------------------------------------
  // show 
  //----------------------------------------------------------------------------
  
  /**
  Show the gizmo*/
  virtual void Show(bool show) = 0;

  //----------------------------------------------------------------------------
  // Gizmo modality  
  //----------------------------------------------------------------------------
  /**
  Set the gizmo modality */
  enum MODALITY {G_LOCAL = 0, G_GLOBAL};
  void SetModalityToLocal(){this->Modality = G_LOCAL;};
  void SetModalityToGlobal() {this->Modality = G_GLOBAL;};
  int  GetModality() {return this->Modality;};

  //----------------------------------------------------------------------------
  /**
  Set the gizmo pose*/
  virtual void SetAbsPose(mafMatrix *absPose) {};
  virtual mafMatrix *GetAbsPose() {return NULL;};
  
  //----------------------------------------------------------------------------
  // Gizmo gui
  //----------------------------------------------------------------------------

  /** Return gui owned by the gizmo*/
  virtual mmgGui *GetGui() {return NULL;};
  
protected:
           mafGizmoInterface();
  virtual ~mafGizmoInterface(); 
  
  /** 
  The input vme*/
  mafVME *InputVME;

  /** 
  The gizmo interaction mode*/
  int Modality;

  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  /** 
  Vme to be used as reference system */
  mafVME *RefSysVME;

  /** */
  bool Visibility;

  void SendTransformMatrix(mafMatrix* matrix, int eventId, long arg);

    /** Gizmo components events handling */
  virtual void OnEventGizmoGui(mafEventBase *maf_event) {};

  /** Gizmo components events handling */
  virtual void OnEventGizmoComponents(mafEventBase *maf_event) = 0;
};
#endif
