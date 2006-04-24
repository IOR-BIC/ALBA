/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewHTML.h,v $
  Language:  C++
  Date:      $Date: 2006-04-24 08:14:55 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewHTML_H__
#define __mafViewHTML_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class wxMozillaBrowser;

//----------------------------------------------------------------------------
// mafViewHTML :
//----------------------------------------------------------------------------
/** 
mafViewHTML is a View that build a web browser using wxMozilla
*/
class mafViewHTML: public mafView
{
public:
  mafViewHTML(wxString label = "htmlView", bool external = false);
  virtual ~mafViewHTML(); 

  mafTypeMacro(mafViewHTML, mafView);

  virtual mafView*  Copy(mafObserver *Listener);
  virtual void      Create();
  virtual void			OnEvent(mafEventBase *maf_event);

  /** 
  Add the vme to the view's scene-graph*/
  virtual void VmeAdd(mafNode *vme);
  /** 
  Remove the vme from the view's scene-graph*/
  virtual void VmeRemove(mafNode *vme);

  virtual void VmeSelect(mafNode *vme, bool select);

  /** 
  Called to show/hide vme*/
  virtual void VmeShow(mafNode *vme, bool show);

  /** 
  Called to update visual pipe properties of the vme passed as argument. If the 'fromTag' flag is true,
  the update is done by reading the visual parameters from tags.*/
  virtual void VmeUpdateProperty(mafNode *vme, bool fromTag = false);

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph*/
  virtual int GetNodeStatus(mafNode *vme);
  
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

	/** 
    Set the vtk RenderWindow size. Used only for Linux (not necessary for Windows) */
  void SetWindowSize(int w, int h);

  /** 
    Set the mouse device to use inside the view */
  void SetMouse(mmdMouse *mouse);

protected:
  virtual mmgGui *CreateGui();

  wxMozillaBrowser *m_MozillaBrowser;

  /** 
  Return the visual pipe's name.*/
  void GetVisualPipeName(mafNode *node, mafString &pipe_name);
};
#endif
