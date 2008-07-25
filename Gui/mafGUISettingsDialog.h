/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUISettingsDialog.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUISettingsDialog_H__
#define __mafGUISettingsDialog_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
//#include "mafString.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUITree;
class mafGUI;
class mafGUIHolder;
class mafGUIBitmaps;
class mafGUIDialog;
//----------------------------------------------------------------------------
// mafGUISettingsDialog :
//----------------------------------------------------------------------------
/**  */  
class mafGUISettingsDialog : public mafObserver
{

public:
	mafGUISettingsDialog(wxString dialog_title = _("Application Settings"));
 ~mafGUISettingsDialog();  

  void OnEvent(mafEventBase *maf_event);
  
  /** AddPage */	
  void AddPage(mafGUI *ui, wxString label, wxString parent_label= "");

  /** RemovePage -- kills the specified page and all its children */	
  void RemovePage(wxString label);

  /** Show the Settings Dialog. */
	bool ShowModal();

protected:  

  struct Page {
    Page*    parent;
    wxString label;
    mafGUI*  ui;
  } ;
  
  std::vector<Page*> m_List;
  mafGUIDialog			*m_Dialog;
	mafGUITree       *m_Tree;
	mafGUIHolder	*m_Guih;

  void  AddRoot();
  Page* GetPageByName(wxString name);
};
#endif // _mafGUISettingsDialog_H_
