/*=========================================================================

 Program: MAF2
 Module: mafPipe
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafPipe_H__
#define __mafPipe_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDecl.h" // for MINID
#include "mafObject.h"
#include "mafObserver.h"
#include "mafEvent.h"
#include "mafServiceClient.h"
#include "mafAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVME;
class mafSceneNode;
class mafSceneGraph;
class vtkMAFAssembly;
class vtkRenderer;
class mafGUI;

//----------------------------------------------------------------------------
// mafPipe :
//----------------------------------------------------------------------------
/*
  mafPipe is the base class for all visual pipes; each pipe represents how a vme can be
  visualized inside the view, so logically a pipe has as input a vme and in output
  creates actors that will be rendered in a render view.
  It can handle a GUI, which events can be catched by OnEvent.
*/
class MAF_EXPORT mafPipe : public mafObject, public mafObserver, public mafServiceClient
{
public:
  mafTypeMacro(mafPipe,mafObject);
  
  mafPipe();
	virtual			~mafPipe();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event)           {};

  /** The real setup must be performed here - not in the ctor */
  virtual void Create(mafSceneNode *n);

  void SetListener(mafObserver *listener) {m_Listener = listener;};

	/** Change the visibility of the bounding box actor representing the selection for the vme. */
	virtual	void Select(bool select)										{};

	/** Update the properties according to the vme's tags. */
	virtual	void UpdateProperty(bool fromTag = false)		{};

  /** Get assembly front/back */
  virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};

  /** IDs for the GUI */
  enum VISUAL_PIPE_WIDGET_ID
  {
    ID_FIRST = MINID,
    ID_LAST
  };

  /** create and return the GUI for changing the node parameters */
  mafGUI *GetGui() \
  { \
    if (m_Gui==NULL) CreateGui(); \
    assert(m_Gui); \
    return m_Gui; \
  }

  /** destroy the Gui */
  void DeleteGui();

  /* Return Listener */
  mafObserver *GetListener(){return m_Listener;};

  mafGUI         *m_Gui;      ///< User Interface
  mafVME         *m_Vme;      ///< VME used as input for the visual pipe
	vtkMAFAssembly *m_AssemblyFront; ///< Assembly used to contain the actor in the front layer
	vtkMAFAssembly *m_AssemblyBack;  ///< Assembly used to contain the actor in the background layer
	vtkMAFAssembly *m_AlwaysVisibleAssembly;
	mafSceneGraph  *m_Sg;       ///< Pointer to the SceneGraph
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
  virtual mafGUI  *CreateGui();

  mafObserver *m_Listener;
};
#endif
