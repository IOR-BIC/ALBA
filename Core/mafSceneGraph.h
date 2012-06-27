/*=========================================================================

 Program: MAF2
 Module: mafSceneGraph
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafSceneGraph_H__
#define __mafSceneGraph_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafGUI;
class mafView;
class mafSceneNode;
class vtkRenderer;

//----------------------------------------------------------------------------
// mafSceneGraph :
//----------------------------------------------------------------------------
class MAF_EXPORT mafSceneGraph  /*: public mafObserver*/
{
public:
								mafSceneGraph(mafView	*view, vtkRenderer *ren1, vtkRenderer *ren2=NULL, vtkRenderer *ren3 = NULL);
	virtual			 ~mafSceneGraph();
  virtual void	SetListener(mafObserver *Listener) {m_Listener = Listener;};
	//virtual void	OnEvent(mafEvent& e);

	/** Add a vme to the scene graph. */
	virtual void VmeAdd(mafNode *vme);

	/** Remove a vme from the scene graph. */
	virtual void VmeRemove(mafNode *vme);

	/** Select a vme and store a reference to it into m_SelectedVme variable. */
	virtual void VmeSelect(mafNode *vme, bool select);

	/** Change the visibility of the vme and if necessary create the vme related pipe. */
	virtual void VmeShow(mafNode *vme, bool show);

	/** Show/Hide the Vme with the same type of the passed vme */
	virtual void VmeShowByType(mafNode *vme, bool show);

	/** 
  Show all the same vme type. */
	//@@@ virtual void					VmeShowByType			(mafNodeBaseTypes type,bool show);

	/** Show all the vme subtree. */
	virtual void VmeShowSubTree(mafNode *vme, bool show);

	/** Update the vme's properties. */
	virtual void VmeUpdateProperty(mafNode *vme, bool fromTag = false);

	/** Find the corresponding vme's node. */
	virtual mafSceneNode *Vme2Node(mafNode *vme);

  /** Return the list of node that are added to the view.*/
  mafSceneNode *GetNodeList() {return m_List;};

  vtkRenderer   *m_RenFront;  ///< pointer to the front renderer
  vtkRenderer   *m_RenBack;   ///< pointer to the back renderer
  vtkRenderer		*m_AlwaysVisibleRenderer; /// < Renderer used to superimpose utility stuff to main render window

  mafView		    *m_View;      ///< pointer to the view

  // Set the flags for creatable vmes type.
	//@@@ virtual void SetCreatableFlag	(mafNodeBaseTypes type,  bool flag = true);

	// Set the flags for mutex vmes.
  //@@@ virtual void SetMutexFlag			(mafNodeBaseTypes type,  bool flag = true);

	// Set the flags for automatic show vme's type.
  //@@@ virtual void SetAutoShowFlag	(mafNodeBaseTypes type,  bool flag = true);

  // Return the gui with the autoshow widgets.
	//@@@ virtual mafGUI *GetGui();

  /**
  Return the selected vme.*/
	virtual mafNode *GetSelectedVme() {return m_SelectedVme;};

  /**
  Used by the mafGUICheckTree - return the status of a SceneNode*/
	virtual int  GetNodeStatus (mafNode *node);

	// Event to show/hide the LandmarkClouds
	//@@@ virtual void	OnOpenCloseEvent( mafSceneNode *node );

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** Set information pipe  enable, if yes PipeVisibilityInformation must be used*/
  void SetInformationPipeModalityEnable(bool enable){m_InformationPipeModality = enable;}

  /** Get flag for information pipe modality. */
  bool GetInformationPipeModalityEnable(){return m_InformationPipeModality;}


protected:
  /** 
  recursively kill all node - starting from the tail of the list. */
  virtual void DeleteNodeList(mafSceneNode *n);
/*	
	bool m_creatable[NUM_OF_BASETYPE];
	bool m_mutex		[NUM_OF_BASETYPE];
	int  m_autoshow	[NUM_OF_BASETYPE];
	mafNode  *m_shown_mutex_vme[NUM_OF_BASETYPE];
*/

	mafSceneNode *NodeAdd(mafNode *vme);

  mafSceneNode *m_List;      ///< list of visualized node
  mafObserver	 *m_Listener;
	mafGUI			 *m_Gui;
	mafNode      *m_SelectedVme;
  bool         m_InformationPipeModality;
};
#endif
