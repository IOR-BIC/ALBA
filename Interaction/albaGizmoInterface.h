/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoInterface
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __albaGizmoInterface_H__
#define __albaGizmoInterface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaVME;
class albaMatrix;

//----------------------------------------------------------------------------
/** Base class for operations and views gizmos.
  This class defines the interface for gizmos that can be plugged into
  a view or into an operation

  @sa
 
  @todo
*/
class ALBA_EXPORT albaGizmoInterface : public albaObserver, public albaServiceClient 
{
public:
	
  /** 
  Set the gizmo name */
  void SetName(albaString name) {m_Name = name;};

  /** 
  Get the gizmo name */
  albaString GetName() {return m_Name;}
  
  /** 
  Set input vme for the gizmo: gizmo are tools used to manipulate VMEs and this variable holds a reference to the controlled VME.
  Can be used also to initialize gizmo dimensions and position*/
  virtual void SetInput(albaVME *vme);
  albaVME *GetInput();
  
  /** 
  Set the orchestrator object: albaGizmo's are used togetheter in more complex objects 
  like albaGizmoRotate, albaGizmoTranslate ,... The master gizmo act as mediator (see GOF mediator pattern)
  between single albaVMEGizmos.
  The optional mediator ivar holds a reference to the orchestrator if needed by the client.
  For example see albaGizmoTranslate which act as mediator between 3 albaGizmoTranslateAxis and 3 albaGizmoTranslatePlane
  each one wrapping several albaVMEGizmo's
  */
  void SetMediator(albaObserver *mediator) {m_Mediator = mediator;};
  
  /** Return the meditor object */
  albaObserver *GetMediator() {return m_Mediator;};

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
  void SetListener(albaObserver *listener);
  albaObserver *GetListener();

  /**
  Events handling (not implemented)*/        
  virtual void OnEvent(albaEventBase *alba_event);
 
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
  gizmo abs pose will change during the interaction. See albaGizmoTranslate behavior with local (default) vs a different (global) refsys 
  in albaOpTransformOld for a Local vs Global use case*/
  void SetModalityToLocal();

  /** Modality to be used when gizmo reference frame is different from input vme abs pose */
  void SetModalityToGlobal();

  /** Return the working modality */
  int  GetModality();

  //----------------------------------------------------------------------------
  /**
  Set the gizmo abs pose (not implemented)*/
  virtual void SetAbsPose(albaMatrix *absPose);
  
  /**
  Get the gizmo abs pose*/
  virtual albaMatrix *GetAbsPose();
  
  //----------------------------------------------------------------------------
  // Gizmo gui
  //----------------------------------------------------------------------------

  /** Return gui owned by the gizmo*/
  virtual albaGUI *GetGui();
  
protected:

           albaGizmoInterface();
  virtual ~albaGizmoInterface(); 

  albaObserver *m_Mediator;

  albaString m_Name;

  /** 
  The input vme*/
  albaVME *m_InputVME;

  /** 
  The gizmo interaction mode*/
  int m_Modality;   

  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

  /** 
  Vme to be used as reference system */
  albaVME *m_RefSysVME;

  /** */
  bool m_Visibility;
  
  /** gizmoHeight / RenderWindowHeight in autoscale mode */
  double m_RenderWindowHeightPercentage;

  /** Gimzo always visible ie on the superimposed layer */
  bool m_AlwaysVisible;

  /** Gizmo autoscale */
  bool m_Autoscale;

  void SendTransformMatrix(albaMatrix* matrix, int eventId, long arg);

    /** Gizmo components events handling */
  virtual void OnEventGizmoGui(albaEventBase *alba_event) {};

  /** Gizmo components events handling */
  virtual void OnEventGizmoComponents(albaEventBase *alba_event) {};

  /** Test friend */
  friend class albaGizmoInterfaceTest;

};
#endif
