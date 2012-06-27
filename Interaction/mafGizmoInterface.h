/*=========================================================================

 Program: MAF2
 Module: mafGizmoInterface
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafGizmoInterface : public mafObserver 
{
public:
	
  /** 
  Set the gizmo name */
  void SetName(mafString name) {m_Name = name;};

  /** 
  Get the gizmo name */
  mafString GetName() {return m_Name;}
  
  /** 
  Set input vme for the gizmo: gizmo are tools used to manipulate VMEs and this variable holds a reference to the controlled VME.
  Can be used also to initialize gizmo dimensions and position*/
  virtual void SetInput(mafVME *vme);
  mafVME *GetInput();
  
  /** 
  Set the orchestrator object: mafGizmo's are used togetheter in more complex objects 
  like mafGizmoRotate, mafGizmoTranslate ,... The master gizmo act as mediator (see GOF mediator pattern)
  between single mafVMEGizmos.
  The optional mediator ivar holds a reference to the orchestrator if needed by the client.
  For example see mafGizmoTranslate which act as mediator between 3 mafGizmoTranslateAxis and 3 mafGizmoTranslatePlane
  each one wrapping several mafVMEGizmo's
  */
  void SetMediator(mafObserver *mediator) {m_Mediator = mediator;};
  
  /** Return the meditor object */
  mafObserver *GetMediator() {return m_Mediator;};

  /** Enable/Disable gizmo autoscaling (default is false ie no autoscaling): if autoscale is enabled the gizmo will maintain a fixed dimension
  trying to follow camera zoom. Use SetRenderWindowHeightPercentage to set gizmo / renderWindowHeight fixed ratio*/
  void SetAutoscale(bool autoscale) {m_Autoscale = autoscale;};

  /** Get if gizmo autoscaling is enabled*/
  bool GetAutoscale() {return m_Autoscale;};

  /** Set the size of the gizmo when autoscale is enabled. The size is expressed as gizmoHeight / renderWindowHeight.
  Max size is 1 (gizmo height equal to the render window one). This setting has no effect when autoscale is off*/
  void SetRenderWindowHeightPercentage(double percentage) {m_RenderWindowHeightPercentage = percentage;};
  double GetRenderWindowHeightPercentage() {return m_RenderWindowHeightPercentage;};

  /** Put the gizmo on the superimposed layer ie make it always visible (default to false)*/
  void SetAlwaysVisible(bool alwaysVisible) {m_AlwaysVisible = alwaysVisible;};
  bool GetAlwaysVisible() {return m_AlwaysVisible;};
  
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

  /** (default) Modality to be used when gizmo reference frame is the input vme abs pose: using this modality
  gizmo abs pose will change during the interaction. See mafGizmoTranslate behavior with local (default) vs a different (global) refsys 
  in mafOpMAFTransform for a Local vs Global use case*/
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

  mafObserver *m_Mediator;

  mafString m_Name;

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
  
  /** gizmoHeight / RenderWindowHeight in autoscale mode */
  double m_RenderWindowHeightPercentage;

  /** Gimzo always visible ie on the superimposed layer */
  bool m_AlwaysVisible;

  /** Gizmo autoscale */
  bool m_Autoscale;

  void SendTransformMatrix(mafMatrix* matrix, int eventId, long arg);

    /** Gizmo components events handling */
  virtual void OnEventGizmoGui(mafEventBase *maf_event) {};

  /** Gizmo components events handling */
  virtual void OnEventGizmoComponents(mafEventBase *maf_event) {};

  /** Test friend */
  friend class mafGizmoInterfaceTest;

};
#endif
