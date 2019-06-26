/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipe
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaPipe_H__
#define __albaPipe_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDecl.h" // for MINID
#include "albaObject.h"
#include "albaObserver.h"
#include "albaEvent.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVME;
class albaSceneNode;
class albaSceneGraph;
class vtkALBAAssembly;
class vtkRenderer;
class albaGUI;

//----------------------------------------------------------------------------
// albaPipe :
//----------------------------------------------------------------------------
/*
  albaPipe is the base class for all visual pipes; each pipe represents how a vme can be
  visualized inside the view, so logically a pipe has as input a vme and in output
  creates actors that will be rendered in a render view.
  It can handle a GUI, which events can be catched by OnEvent.
*/
class ALBA_EXPORT albaPipe : public albaObject, public albaObserver, public albaServiceClient
{
public:
  albaTypeMacro(albaPipe,albaObject);
  
  albaPipe();
	virtual			~albaPipe();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event)           {};

  /** The real setup must be performed here - not in the ctor */
  virtual void Create(albaSceneNode *n);

  void SetListener(albaObserver *listener) {m_Listener = listener;};

	/** Change the visibility of the bounding box actor representing the selection for the vme. */
	virtual	void Select(bool select)										{};

	/** Update the properties according to the vme's tags. */
	virtual	void UpdateProperty(bool fromTag = false)		{};

  /** Get assembly front/back */
  virtual vtkALBAAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkALBAAssembly *GetAssemblyBack(){return m_AssemblyBack;};

  /** IDs for the GUI */
  enum VISUAL_PIPE_WIDGET_ID
  {
    ID_FIRST = MINID,
    ID_LAST
  };

  /** create and return the GUI for changing the node parameters */
  albaGUI *GetGui() \
  { \
    if (m_Gui==NULL) CreateGui(); \
    assert(m_Gui); \
    return m_Gui; \
  }

  /** destroy the Gui */
  void DeleteGui();

  /* Return Listener */
  albaObserver *GetListener(){return m_Listener;};

  albaGUI         *m_Gui;      ///< User Interface
  albaVME         *m_Vme;      ///< VME used as input for the visual pipe
	vtkALBAAssembly *m_AssemblyFront; ///< Assembly used to contain the actor in the front layer
	vtkALBAAssembly *m_AssemblyBack;  ///< Assembly used to contain the actor in the background layer
	vtkALBAAssembly *m_AlwaysVisibleAssembly;
	albaSceneGraph  *m_Sg;       ///< Pointer to the SceneGraph
  bool            m_Selected; ///< Flag used to say if the rendered VME is selected.
  vtkRenderer    *m_RenFront; ///< Renderer associated to the front layer
  vtkRenderer    *m_RenBack;  ///< Renderer associated to the background layer
  vtkRenderer		 *m_AlwaysVisibleRenderer; /// < Renderer used to superimpose utility stuff to main render window

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual albaGUI  *CreateGui();

  albaObserver *m_Listener;
};
#endif
