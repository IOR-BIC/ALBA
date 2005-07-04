/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView.h,v $
  Language:  C++
  Date:      $Date: 2005-07-04 16:01:27 $
  Version:   $Revision: 1.19 $
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
#include "mafObserver.h"
#include "mafObject.h"
#include "mafDecl.h"
#include "mafNode.h"
#include "mmgGuiHolder.h"
#include "mmgGui.h"
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmdMouse;
class mafRWIBase;

//----------------------------------------------------------------------------
// const :
//----------------------------------------------------------------------------
/** status of a SceneNode -- do not change, it is used by mafCheckTree also */
enum NODE_STATUS_ID
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
class mafView: public mafObserver, public mafObject
{
public:
  mafView(wxString label = "View", bool external = false);
  virtual ~mafView(); 

  mafTypeMacro(mafView, mafObject);

  virtual void			SetListener(mafObserver *Listener) {m_Listener = Listener;};
  virtual void			OnEvent(mafEventBase *maf_event);
  virtual mafView*  Copy(mafObserver *Listener) {return NULL;};
  virtual void      Create() {};

  virtual void			VmeAdd(mafNode *vme)																		{};
  virtual void			VmeRemove(mafNode *vme)																	{};
  virtual void			VmeSelect(mafNode *vme, bool select)										{};
  virtual void			VmeShow(mafNode *vme, bool show)												{};
	virtual void      VmeUpdateProperty(mafNode *vme, bool fromTag = false)		{};

  virtual void	    VmeCreatePipe(mafNode *vme)													    {};
  virtual void	    VmeDeletePipe(mafNode *vme)													    {};

  virtual void			CameraReset(mafNode *node = NULL)  											{};
  virtual void			CameraUpdate()																					{};
  virtual void      SetMouse(mmdMouse *mouse)                               {};
  virtual mafRWIBase *GetRWI()                                              {return NULL;};

  /** return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... */
  //having mafView::GetNodeStatus allow mmgCheckTree to not know about mafSceneGraph
  virtual int 	    GetNodeStatus(mafNode *vme) {return NODE_NON_VISIBLE;};
  
  /** return the current pipe for the specified vme (if any exist at this moment) */
  virtual mafPipe*  GetNodePipe(mafNode *vme) {return NULL;};

  virtual wxString        GetLabel() {return m_Label;};
  virtual wxString        GetName()  {return m_Name;};
  virtual wxWindow*	      GetWindow(){return m_Win;};
  virtual wxFrame*		    GetFrame() {return m_Frame;};
  virtual mmgGui*		      GetGui()   {if(m_Gui == NULL) CreateGui(); assert(m_Gui); return m_Gui;};
//  virtual mmgGuiHolder*		GetGuih()  {return m_Guih;};
  virtual void		  SetFrame(wxFrame* f) {m_Frame = f;};

  //virtual void			HideGui();
  //virtual void			ShowGui();
//  virtual void			ShowSettings()							{};
  virtual void			OnSize(wxSizeEvent &maf_event)	{};

protected:
  mafObserver   *m_Listener;
  wxString       m_Label; 
  wxString       m_Name;
  wxWindow			*m_Win;
  wxFrame				*m_Frame;
  mmgGui      	*m_Gui;
  mmgGuiHolder	*m_Guih;

public:
  int            m_Mult;    ///< Used to store the multiplicity of the view type created (e.g. the 3rd view surface created).
  int            m_Id;      ///< Used to store the view type created (e.g. view surface).
  bool           m_ExternalFlag;
  bool           m_Plugged; // forget it - it is used from outside 
  mafView       *m_Next;    // forget it - it is used from outside 

  /** IDs for the GUI */
  enum VIEW_WIDGET_ID
  {
    ID_FIRST = MINID,
    ID_LAST
  };

  /** destroy the Gui */
  void DeleteGui();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mmgGui  *CreateGui();
};
#endif
