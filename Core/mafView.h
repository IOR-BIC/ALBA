/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:19 $
  Version:   $Revision: 1.2 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mafEvent.h"
//#include "mafRWIBase.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
//class mafVME;
//class mafSceneGraph;
class mmgGuiHolder;
class mmgGui;
//----------------------------------------------------------------------------
// mafView :
//----------------------------------------------------------------------------
/** 
mafView is the anchestor af all the Views for the LAL.
Any mafView has a 
> m_listener  (someone to which send events)
> m_label     (usually appearing in the choose view menu) 
> m_win       (a window that can be put on the screen - usually carring one or mode mafVTKWindow)

The system notify changing in the vme set by calling:
> VmeAdd     
> VmeRemove
> VmeSelect
> VmeShow

The system may ask the view to perform a Render or a
Fit-All + Render by calling
> Camera_Update
> Camera_Reset

mafView also has two public member
> m_plugged
> m_next
mafView must initialize these to NULL and then forget them at all

Responsability of a view:
-------------------------
Any usual mafView is supposed to work in this way:
in the constructor: set-up m_win, and place on it one mafVTKWindow
in VmeAdd :         create a mafSceneNode connected to the vme, put that Actor in the RenderWindow
in VmeRemove :      remove the corresponding mafSceneNode from the RenderWindow, destroy him
in Vmeshow :        pass notification to the corresponding mafSceneNode
VmeSelected :          "                "

mafView has to keep an hash table to indexing vme->mafSceneNodes.
and perform the lookup in response to GetActor.
There is no other way to find the mafSceneNodes related to a certain vme:

Handling mouse movements is not responsability of mafView
these duties are accomplished by the collaboration of mafISV,mafVTKWindow, and mafViewManager
 
    
\sa mafSceneNode mafVTKWindow
*/
class mafView: public mafEventListener
{
public:
  mafView(wxString label, bool external = false);
  virtual ~mafView(); 

  virtual mafView  *Copy() {return NULL;};
  virtual void			SetListener(mafEventListener *Listener) {m_Listener = Listener;};
  virtual void			OnEvent(mafEvent& e);

//  virtual void			VmeAdd(mafVME *vme)																			{};
//  virtual void			VmeRemove(mafVME *vme)																	{};
//  virtual void			VmeSelect(mafVME *vme, bool select)											{};
//  virtual void			VmeShow(mafVME *vme, bool show)													{};
//	virtual void      VmeUpdateProperty(mafVME *vme, bool fromTag = false)		{};
//  virtual void	    VmeCreatePipe(mafVME *vme)													    {};
//  virtual void	    VmeDeletePipe(mafVME *vme)													    {};

	virtual void			CameraReset()		                  											{};
  virtual void			CameraUpdate()																					{};

  virtual void			HideGui();
  virtual void			ShowGui();
  virtual void			ShowSettings()																					{};
  virtual void			OnSize(wxSizeEvent &event)															{};
  wxString           m_label;
  wxString           m_name;
  wxWindow					*m_win;
  wxFrame					  *m_frame;

//  virtual mafSceneGraph *GetSceneGraph()	{return NULL;}; 
//  virtual mafRWIBase    *GetDefaultRWI()    {return NULL;}; 

	bool IsExternal() {return m_external_flag;};

protected:
  mafEventListener	*m_Listener;
	bool							m_external_flag;		

public:
  int                m_id;       // forget it - it is used from outside 
  bool               m_plugged;  // forget it - it is used from outside 
  mafView           *m_next;     // forget it - it is used from outside 
	mmgGui      			*m_gui;
	mmgGuiHolder			*m_guih;
};
#endif
























