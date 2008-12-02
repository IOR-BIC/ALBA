/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoInterface.h,v $
  Language:  C++
  Date:      $Date: 2008-12-02 15:58:35 $
  Version:   $Revision: 1.4.2.2 $
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
class mafGUI;
class mafVME;
class mafMatrix;

//----------------------------------------------------------------------------
/** Base class for operations and views gizmos.
  This class defines the interface for gizmos that can be plugged into
  a view or into an operation

  @sa
 
  @todo
*/
class mafGizmoInterface : public mafObserver 
{
public:

  /** 
  Set input vme for the gizmo*/
  virtual void SetInput(mafVME *vme);
  mafVME *GetInput();

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /**
  Set the event receiver object*/
  void SetListener(mafObserver *listener);
  mafObserver *GetListener();

  /**
  Events handling (not implemented)*/        
  virtual void OnEvent(mafEventBase *maf_event);
 
  //----------------------------------------------------------------------------
  // show 
  //----------------------------------------------------------------------------
  
  /**
  Show the gizmo (not implemented)*/
  virtual void Show(bool show);

  //----------------------------------------------------------------------------
  // Gizmo modality: defaul is G_LOCAL
  //----------------------------------------------------------------------------
  /**
  Set the gizmo working modality */
  enum MODALITY {G_LOCAL = 0, G_GLOBAL};

  /** (default) Modality to be used when gizmo reference frame is the input vme abs pose */
  void SetModalityToLocal();

  /** Modality to be used when gizmo reference frame is different from input vme abs pose */
  void SetModalityToGlobal();

  /** Return the working modality */
  int  GetModality();

  //----------------------------------------------------------------------------
  /**
  Set the gizmo abs pose (not implemented)*/
  virtual void SetAbsPose(mafMatrix *absPose);
  
  /**
  Get the gizmo abs pose*/
  virtual mafMatrix *GetAbsPose();
  
  //----------------------------------------------------------------------------
  // Gizmo gui
  //----------------------------------------------------------------------------

  /** Return gui owned by the gizmo*/
  virtual mafGUI *GetGui();
  
protected:
           mafGizmoInterface();
  virtual ~mafGizmoInterface(); 
  
  /** 
  The input vme*/
  mafVME *m_InputVME;

  /** 
  The gizmo interaction mode*/
  int m_Modality;   

  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  /** 
  Vme to be used as reference system */
  mafVME *m_RefSysVME;

  /** */
  bool m_Visibility;

  void SendTransformMatrix(mafMatrix* matrix, int eventId, long arg);

    /** Gizmo components events handling */
  virtual void OnEventGizmoGui(mafEventBase *maf_event) {};

  /** Gizmo components events handling */
  virtual void OnEventGizmoComponents(mafEventBase *maf_event) {};

  /** Test friend */
  friend class mafGizmoInterfaceTest;

};
#endif
