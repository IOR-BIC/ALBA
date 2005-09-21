/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewCompound.h,v $
  Language:  C++
  Date:      $Date: 2005-09-21 10:11:29 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewCompound_H__
#define __mafViewCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmdMouse;
class mafSceneGraph;
class mmgGui;
class mafRWIBase;

//----------------------------------------------------------------------------
// mafViewCompound :
//----------------------------------------------------------------------------
/** 
mafViewCompound is the base class for Compound Views in maf.

The member function GetNodeStatus(vme) will be called by the mmgCheckTree 
in order to update it's icons. (Note that mmgCheckTree doesn't know about
the SceneGraph anymore)

mafViewCompound doesn't have a Scenegraph, nor knowledge about VTK /sa mafViewVTK.
*/
class mafViewCompound: public mafView
{
public:
  mafViewCompound(wxString label = "View Compound", int num_row = 1, int num_col = 2, bool external = false);
  virtual ~mafViewCompound(); 

  mafTypeMacro(mafViewCompound, mafView);

  /** Plug the child view into the compound view.
  If the number of child view is less then row x col, the last plugged view will fill the remaining holes.*/
  virtual void PlugChildView(mafView *child);

  virtual mafView *Copy(mafObserver *Listener);
  virtual void     Create();
  virtual void     OnEvent(mafEventBase *maf_event);

  virtual void VmeAdd(mafNode *node);
  virtual void VmeRemove(mafNode *node);
  virtual void VmeSelect(mafNode *node, bool select);
  virtual void VmeShow(mafNode *node, bool show);
	virtual void VmeUpdateProperty(mafNode *node, bool fromTag = false)		{};

  virtual void VmeCreatePipe(mafNode *node);
  virtual void VmeDeletePipe(mafNode *node);

  virtual void CameraReset(mafNode *node = NULL);
  virtual void CameraUpdate();

  /** return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... */
  //having mafViewCompound::GetNodeStatus allow mmgCheckTree to not know about mafSceneGraph
  virtual int  GetNodeStatus(mafNode *vme);
  virtual void OnSize(wxSizeEvent &event);

  /** IDs for the GUI */
  enum VIEW_WIDGET_ID
  {
    ID_DEFAULT_CHILD_VIEW = Superclass::ID_LAST,
    ID_LAST
  };

  /** Set the size of the windowing double slider according to the size of the view. */
  void OnLayout();

  void SetMouse(mmdMouse *mouse);

  virtual mafSceneGraph *GetSceneGraph();
  virtual mafRWIBase    *GetRWI();

  /** return the current pipe for the specified vme (if any exist at this moment) */
  virtual mafPipe*  GetNodePipe(mafNode *vme);

  /** Return the gui for the visual pipe corresponding to the default child view. */
  virtual mmgGui *GetNodePipeGUI(mafNode *vme);

  /** Return the gui for the visual pipe corresponding to the n-th child view. */
  virtual mmgGui *GetNodePipeGUI(mafNode *vme, int view_idx);

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mmgGui  *CreateGui();

  int m_ViewRowNum; ///< number of rows to divide the compound view
  int m_ViewColNum; ///< number of cols to divide the compound view
  int m_NumOfPluggedChildren; ///< number of plugged children view
  int m_NumOfChildView; ///< number of child view (is equal or greater then m_NumOfPluggedChildren)

  int m_DefauldChildView;

  wxSize m_Size; ///< size of the compound view

  std::vector<mafView *> m_ChildViewList; ///< Child views vector
  std::vector<mafView *> m_PluggedChildViewList; ///< Plugged Child views vector
};
#endif
