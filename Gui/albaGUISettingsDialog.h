/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettingsDialog
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUISettingsDialog_H__
#define __albaGUISettingsDialog_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
//#include "albaString.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaGUITree;
class albaGUI;
class albaGUIHolder;
class albaGUIBitmaps;
class albaGUIDialog;

/** 
  class name: albaGUISettingsDialog.
  Represent the settings collector dialog. It generates a tree (left parto of the dialogue) with all the settings appended to it.
In the right part ofg the dialog there is the specific gui setting gui.
 */  
class ALBA_EXPORT albaGUISettingsDialog : public albaObserver
{

public:
  /** constructor */
	albaGUISettingsDialog(wxString dialog_title = _("Application Settings"));
  /** destructor */
 ~albaGUISettingsDialog();  

  /** Answer to the messages coming from interface. */
  void OnEvent(albaEventBase *alba_event);
  
  /** AddPage  to the dialog*/	
  void AddPage(albaGUI *ui, wxString label, wxString parent_label= "");

  /** RemovePage -- kills the specified page and all its children */	
  void RemovePage(wxString label);

  /** Show the Settings Dialog. */
	bool ShowModal();

protected:  

  struct Page {
    Page*    parent;
    wxString label;
    albaGUI*  ui;
  } ;
  
  std::vector<Page*> m_List;
  albaGUIDialog			*m_Dialog;
	albaGUITree       *m_Tree;
	albaGUIHolder	*m_Guih;

  /** add a root to  the gui setting tree  */
  void  AddRoot();
  /** retrieve the page through the name */
  Page* GetPageByName(wxString name);
};
#endif // _albaGUISettingsDialog_H_
