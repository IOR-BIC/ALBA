/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationLayoutSettings.h,v $
Language:  C++
Date:      $Date: 2006-11-24 16:06:57 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgApplicationLayoutSettings_H__
#define __mmgApplicationLayoutSettings_H__

#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;
class mafViewManager;
class mmaApplicationLayout;

//----------------------------------------------------------------------------
// mmgApplicationLayoutSettings :
//----------------------------------------------------------------------------
/**
*/
class mmgApplicationLayoutSettings : public mafObserver
{
public:
	mmgApplicationLayoutSettings(mafObserver *listener);
	~mmgApplicationLayoutSettings(); 

  enum APPLICATION_LAYOUT_WIDGET_ID
  {
    SAVE_LAYOUT_ID = MINID,
    LAYOUT_NAME_ID
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Set the listener to which send events.*/
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  void SetViewManager(mafViewManager *view_manager) {m_ViewManager = view_manager;};

  /** Return the Settings GUI.*/
  mmgGui* GetGui() {return m_Gui;};

protected:
  /** Initialize Application layout used into the application.*/
  void InitializeLayout();

  /** Save current application layout into the attribute mmaApplicationLayout. */
  void SaveLayout();

  mafString              m_DefaultLayout;
  mmgGui                *m_Gui;
  mafObserver           *m_Listener;
  mafViewManager        *m_ViewManager;
  mmaApplicationLayout  *m_Layout;
};
#endif
