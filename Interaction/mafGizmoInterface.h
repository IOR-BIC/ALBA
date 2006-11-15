/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoInterface.h,v $
  Language:  C++
  Date:      $Date: 2006-11-15 18:18:45 $
  Version:   $Revision: 1.2 $
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
/** Abstract class for operations and views gizmos.
  This abstract class defines the interface for gizmos that can be plugged into
  a view or into an operation

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
  virtual void OnEvent(mafEventBase *maf_event) = 0;
 
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
  Set the gizmo working modality */
  enum MODALITY {G_LOCAL = 0, G_GLOBAL};

  /** Modality to be used when gizmo reference frame is the input vme abs pose */
  void SetModalityToLocal(){this->Modality = G_LOCAL;};

  /** Modality to be used when gizmo reference frame is different from input vme abs pose */
  void SetModalityToGlobal() {this->Modality = G_GLOBAL;};

  /** Return the working modality */
  int  GetModality() {return this->Modality;};

  //----------------------------------------------------------------------------
  /**
  Set the gizmo abs pose*/
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
  virtual void OnEventGizmoComponents(mafEventBase *maf_event) {};
};
#endif
