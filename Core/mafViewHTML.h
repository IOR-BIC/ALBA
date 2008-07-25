/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewHTML.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:56:04 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani    Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewHTML_H__
#define __mafViewHTML_H__


#include "mafView.h"

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------
class mafRWI;
class mafSceneGraph;
class mafSceneGraph;
class mafGUICheckListBox;
class wxHtmlWindow;
//----------------------------------------------------------------------------
// mafViewHTML :
//----------------------------------------------------------------------------
/**   */
class mafViewHTML: public mafView
{
public:
	          mafViewHTML(const wxString &label = "vtkViewHTML", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = true, bool show_grid = false, int stereo = 0);
	virtual	 ~mafViewHTML();

  mafTypeMacro(mafViewHTML, mafView);

	mafView				*Copy(mafObserver *Listener = NULL);
  void           Create();
	mafSceneGraph *GetSceneGraph();
  mafRWIBase    *GetDefaultRWI();

  void VmeAdd   (mafNode *vme);
	void VmeRemove(mafNode *vme);
	void VmeSelect(mafNode *vme, bool select);
	void VmeShow  (mafNode *vme, bool show);
	void VmeUpdateProperty(mafNode*n, bool fromTag = false) 	{};
  void VmeCreatePipe(mafNode *vme);
  void VmeDeletePipe(mafNode *vme)                           {};

	void OnEvent(mafEventBase *maf_event);

    /** IDs for the GUI */
  enum VIEW_HTML_WIDGET_ID
  {
	  ID_LOAD = MINID,
	  ID_FORWARD,
	  ID_BACK,
    ID_URL,
  };

protected:
  mafRWI	      *m_Rwi;
  mafSceneGraph *m_Sg;
  mafNode        *m_ActiveNote;

  void  OnLoad();
  void  OnForward();
  void  OnBack();
  
	mafGUI *CreateGui();

  wxHtmlWindow *m_Html;
  wxString m_Url;
};
#endif
