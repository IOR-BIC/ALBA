/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.h,v $
  Language:  C++
  Date:      $Date: 2007-06-11 11:48:22 $
  Version:   $Revision: 1.15 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVME;
class mafSceneNode;
class mafSceneGraph;
class vtkMAFAssembly;
class vtkRenderer;
class mmgGui;

//----------------------------------------------------------------------------
// mafPipe :
//----------------------------------------------------------------------------
class mafPipe : public mafObject, public mafObserver
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

  /** IDs for the GUI */
  enum VISUAL_PIPE_WIDGET_ID
  {
    ID_FIRST = MINID,
    ID_LAST
  };

  /** create and return the GUI for changing the node parameters */
  mmgGui *GetGui() \
  { \
    if (m_Gui==NULL) CreateGui(); \
    assert(m_Gui); \
    return m_Gui; \
  }

  /** destroy the Gui */
  void DeleteGui();

  /** Return true if the editing visual pipe is active otherwise false is returned.*/
  bool IsEditingActive() {return m_EditingVME;};

  /** Turn On the editing visual pipe code.*/
  void EditingVisualPipeOn() {m_EditingVME = true;};

  /** Turn Off the editing visual pipe code.*/
  void EditingVisualPipeOff() {m_EditingVME = false;};

  mmgGui         *m_Gui;      ///< User Interface
  mafVME         *m_Vme;      ///< VME used as input for the visual pipe
	vtkMAFAssembly *m_AssemblyFront; ///< Assembly used to contain the actor in the front layer
	vtkMAFAssembly *m_AssemblyBack;  ///< Assembly used to contain the actor in the background layer
	mafSceneGraph  *m_Sg;       ///< Pointer to the SceneGraph
  bool            m_Selected; ///< Flag used to say if the rendered VME is selected.
  vtkRenderer    *m_RenFront; ///< Renderer associated to the front layer
  vtkRenderer    *m_RenBack;  ///< Renderer associated to the background layer

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mmgGui  *CreateGui();

  mafObserver *m_Listener;

  bool         m_EditingVME; ///< Flag to indicate if is active the editing visual pipe for the VME
};
#endif
