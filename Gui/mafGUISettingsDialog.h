/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUISettingsDialog.h,v $
  Language:  C++
  Date:      $Date: 2009-12-01 14:36:33 $
  Version:   $Revision: 1.1.2.1 $
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

/** 
  class name: mafGUISettingsDialog.
  Represent the settings collector dialog. It generates a tree (left parto of the dialogue) with all the settings appended to it.
In the right part ofg the dialog there is the specific gui setting gui.
 */  
class mafGUISettingsDialog : public mafObserver
{

public:
  /** constructor */
	mafGUISettingsDialog(wxString dialog_title = _("Application Settings"));
  /** destructor */
 ~mafGUISettingsDialog();  

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);
  
  /** AddPage  to the dialog*/	
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

  /** add a root to  the gui setting tree  */
  void  AddRoot();
  /** retrieve the page through the name */
  Page* GetPageByName(wxString name);
};
#endif // _mafGUISettingsDialog_H_
