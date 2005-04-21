/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 16:37:43 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafView_H__
#define __mafView_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafDecl.h"
#include "mafNode.h"
#include "mmgGuiHolder.h"
#include "mmgGui.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// const :
//----------------------------------------------------------------------------
/** status of a SceneNode -- do not change, it is used by mafCheckTree also */
enum 
{
  NODE_NON_VISIBLE =0,
  NODE_VISIBLE_OFF,
  NODE_VISIBLE_ON,
  NODE_MUTEX_OFF,
  NODE_MUTEX_ON,
};
//----------------------------------------------------------------------------
// mafView :
//----------------------------------------------------------------------------
/** 
mafView is the base class for Views in maf.

The member function GetNodeStatus(vme) will be called by the mmgCheckTree 
in order to update it's icons. (Note that mmgCheckTree doesn't know about
the SceneGraph anymore)

mafView doesn't have a Scenegraph, nor knowledge about VTK /sa mafViewVTK.

The External Flag has been removed

mafView can be the base class for composed Views.

*/
class mafView: public mafEventListener
{
public:
  mafView(wxString label, bool external = false);
  virtual ~mafView(); 

  virtual void			SetListener(mafEventListener *Listener) {m_Listener = Listener;};
  virtual void			OnEvent(mafEvent& e);
  virtual mafView*  Copy(mafEventListener *Listener) {return NULL;};
  virtual void      Create() {};

  virtual void			VmeAdd(mafNode *vme)																		{};
  virtual void			VmeRemove(mafNode *vme)																	{};
  virtual void			VmeSelect(mafNode *vme, bool select)										{};
  virtual void			VmeShow(mafNode *vme, bool show)												{};
	virtual void      VmeUpdateProperty(mafNode *vme, bool fromTag = false)		{};

  virtual void	    VmeCreatePipe(mafNode *vme)													    {};
  virtual void	    VmeDeletePipe(mafNode *vme)													    {};

  virtual void			CameraReset()		                  											{};
  virtual void			CameraUpdate()																					{};

  /** return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... */
  //having mafView::GetNodeStatus allow mmgCheckTree to not know about mafSceneGraph
  virtual int 	    GetNodeStatus(mafNode *vme) {return NODE_NON_VISIBLE;};

  virtual wxString        GetLabel() {return m_label;};
  virtual wxString        GetName()  {return m_name;};
  virtual wxWindow*	      GetWindow(){return m_win;};
  virtual wxFrame*		    GetFrame() {return m_frame;};
  virtual mmgGui*		      GetGui()   {return m_gui;};
  virtual mmgGuiHolder*		GetGuih()  {return m_guih;};
  virtual void		  SetFrame(wxFrame* f) {m_frame = f;};

  virtual void			HideGui();
  virtual void			ShowGui();
  virtual void			ShowSettings()							{};
  virtual void			OnSize(wxSizeEvent &event)	{};

protected:
  mafEventListener	*m_Listener;
  wxString           m_label; 
  wxString           m_name;
  wxWindow					*m_win;
  wxFrame					  *m_frame;
  mmgGui      			*m_gui;
  mmgGuiHolder			*m_guih;

public:
  int                m_id;       // forget it - it is used from outside 
  bool               m_plugged;  // forget it - it is used from outside 
  mafView           *m_next;     // forget it - it is used from outside 
};
#endif
