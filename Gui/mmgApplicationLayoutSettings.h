/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationLayoutSettings.h,v $
Language:  C++
Date:      $Date: 2006-12-07 14:42:02 $
Version:   $Revision: 1.5 $
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
class mmgMDIFrame;
class mafVMEStorage;

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
    SAVE_TREE_LAYOUT_ID = MINID,
    APPLY_TREE_LAYOUT_ID,
    LAYOUT_NAME_ID,
    LAYOUT_VISIBILITY_VME,
    ID_LIST_LAYOUT,
    OPEN_LAYOUT_ID,
    APPLY_LAYOUT_ID,
    ADD_LAYOUT_ID,
    REMOVE_LAYOUT_ID,
    SAVE_APPLICATION_LAYOUT_ID,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Set the listener to which send events.*/
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  void SetViewManager(mafViewManager *view_manager) {m_ViewManager = view_manager;};

  /** Set a reference to the interface elements to store their visibility.*/
  void SetApplicationFrame(mmgMDIFrame *win) {m_Win = win;};

  /** Return the Settings GUI.*/
  mmgGui* GetGui() {return m_Gui;};

  /** Return Layout Modification Flag*/
  bool GetModifiedLayouts(){return m_ModifiedLayouts;};
 
  /** Store in file Layout List*/
  void SaveApplicationLayout();

   /** Save current application layout into the attribute mmaApplicationLayout, 
  inside current MSF data*/
  void SaveTreeLayout();

  /** Add current layout to the list*/
  void AddLayout();

  /** Add current layout to the list*/
  void ApplyTreeLayout();

protected:
  /** Initialize Application layout used into the application.*/
  void InitializeLayout();

  /** Load layouts stored in a file*/
  void LoadLayout(bool fileDefault = false);

  /** Remove Selected Layout from the list*/
  void RemoveLayout();

  /** Apply selected layout */
  void ApplyLayout();

  mmgMDIFrame           *m_Win;
  mafString              m_DefaultLayout;
  mafString              m_DefaultLayoutFile;
  
  mafString              m_LayoutFileSave;
  mmgGui                *m_Gui;
  mafObserver           *m_Listener;
  mafViewManager        *m_ViewManager;
  mmaApplicationLayout  *m_Layout;
  mafVMEStorage         *m_Storage;
  int                    m_VisibilityVme;
  wxListBox				      *m_List;
  bool                   m_ModifiedLayouts;
};
#endif
