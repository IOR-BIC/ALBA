/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSceneGraph.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:18:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafSceneGraph_H__
#define __mafSceneGraph_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDecl.h"
#include "mafEvent.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mmgGui;
class mafView;
class mafSceneNode;
class vtkRenderer;

//----------------------------------------------------------------------------
// mafSceneGraph :
//----------------------------------------------------------------------------
class mafSceneGraph  /*: public mafEventListener*/
{
public:
								mafSceneGraph(mafView	*view, vtkRenderer *ren1, vtkRenderer *ren2=NULL);
	virtual			 ~mafSceneGraph();
  virtual void	SetListener(mafEventListener *Listener) {m_listener = Listener;};
	//virtual void	OnEvent(mafEvent& e);

	/** Add a vme to the scene graph. */
	virtual void					VmeAdd						(mafNode *vme);

	/** Remove a vme from the scene graph. */
	virtual void					VmeRemove					(mafNode *vme);

	/** Select a vme and store a reference to it into m_selected_vme variable. */
	virtual void					VmeSelect					(mafNode *vme,					bool select);

	/** Change the visibility of the vme and if necessary create the vme releted pipe. */
	virtual void					VmeShow						(mafNode *vme,					bool show);

	/** Show/Hide the Vme with the same type of the passed vme */
  //SIL. 18-4-2005: - question: Show by type should consider the derived classes or not ?
	//@@@ virtual void					VmeShowByType			(mafNode *vme,					bool show);

	/** Show all the same vme type. */
	//@@@ virtual void					VmeShowByType			(mafNodeBaseTypes type,bool show);

	/** Show all the vme subtree. */
	//@@@ virtual void					VmeShowSubTree		(mafNode *vme,					bool show);

	/** Update the vme's properties. */
	virtual void					VmeUpdateProperty	(mafNode *vme,					bool fromTag = false);

	/** Find the corresponding vme's node. */
	virtual mafSceneNode *Vme2Node					(mafNode *vme);

  mafSceneNode	*m_list;
  vtkRenderer		*m_ren1;
  vtkRenderer		*m_ren2;
  mafView		    *m_view;

  // Set the flags for creatable vmes type.
	//@@@ virtual void SetCreatableFlag	(mafNodeBaseTypes type,  bool flag = true);

	// Set the flags for mutex vmes.
  //@@@ virtual void SetMutexFlag			(mafNodeBaseTypes type,  bool flag = true);

	// Set the flags for automatic show vme's type.
  //@@@ virtual void SetAutoShowFlag	(mafNodeBaseTypes type,  bool flag = true);

  // Return the gui with the autoshow widgets.
	//@@@ virtual mmgGui *GetGui();

  // Return the selected vme.
	virtual mafNode *GetSelectedVme() {return m_selected_vme;};

  // Used by the mmgCheckTree - return the status of a SceneNode
	virtual int  GetNodeStatus (mafNode *vme);

	// Event to show/hide the LandmarkClouds
	//@@@ virtual void	OnOpenCloseEvent( mafSceneNode *node );

protected:
  /** recursively kill all node - starting from the tail of the list. */
  virtual void DeleteNodeList(mafSceneNode *n);
/*	
	bool m_creatable[NUM_OF_BASETYPE];
	bool m_mutex		[NUM_OF_BASETYPE];
	int  m_autoshow	[NUM_OF_BASETYPE];
	mafNode  *m_shown_mutex_vme[NUM_OF_BASETYPE];
*/

	mafSceneNode			*NodeAdd	(mafNode *vme);
  mafEventListener	*m_listener;
	mmgGui						*m_gui;

	mafNode            *m_selected_vme;
};
#endif
