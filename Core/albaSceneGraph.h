/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSceneGraph
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaSceneGraph_H__
#define __albaSceneGraph_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaGUI;
class albaView;
class albaSceneNode;
class vtkRenderer;

//----------------------------------------------------------------------------
// albaSceneGraph :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaSceneGraph : public albaServiceClient  /*, public albaObserver*/
{
public:
								albaSceneGraph(albaView	*view, vtkRenderer *ren1, vtkRenderer *ren2=NULL, vtkRenderer *ren3 = NULL);
	virtual			 ~albaSceneGraph();
  virtual void	SetListener(albaObserver *Listener) {m_Listener = Listener;};
	//virtual void	OnEvent(albaEvent& e);

	/** Add a vme to the scene graph. */
	virtual void VmeAdd(albaVME *vme);

	/** Remove a vme from the scene graph. */
	virtual void VmeRemove(albaVME *vme);

	/** Select a vme and store a reference to it into m_SelectedVme variable. */
	virtual void VmeSelect(albaVME *vme, bool select);

	/** Change the visibility of the vme and if necessary create the vme related pipe. */
	virtual void VmeShow(albaVME *vme, bool show);

	/** Show/Hide the Vme with the same type of the passed vme */
	virtual void VmeShowByType(albaVME *vme, bool show);
		
	/** Show all the vme subtree. */
	virtual void VmeShowSubTree(albaVME *vme, bool show);

	/** Update the vme's properties. */
	virtual void VmeUpdateProperty(albaVME *vme, bool fromTag = false);

	/** Find the corresponding vme's node. */
	virtual albaSceneNode *Vme2Node(albaVME *vme);

  /** Return the list of node that are added to the view.*/
  albaSceneNode *GetNodeList() {return m_List;};

  vtkRenderer   *m_RenFront;  ///< pointer to the front renderer
  vtkRenderer   *m_RenBack;   ///< pointer to the back renderer
  vtkRenderer		*m_AlwaysVisibleRenderer; /// < Renderer used to superimpose utility stuff to main render window

  albaView		    *m_View;      ///< pointer to the view

  /**
  Return the selected vme.*/
	virtual albaVME *GetSelectedVme() {return m_SelectedVme;};

  /**
  Used by the albaGUICheckTree - return the status of a SceneNode*/
	virtual int  GetNodeStatus (albaVME *node);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

protected:
  /** 
  recursively kill all node - starting from the tail of the list. */
  virtual void DeleteNodeList(albaSceneNode *n);

	albaSceneNode *NodeAdd(albaVME *vme);

  albaSceneNode *m_List;      ///< list of visualized node
  albaObserver	 *m_Listener;
	albaGUI			 *m_Gui;
	albaVME			 *m_SelectedVme;
 
};
#endif
