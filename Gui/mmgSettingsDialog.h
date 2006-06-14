/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgSettingsDialog.h,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgSettingsDialog_H__
#define __mmgSettingsDialog_H__

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
class mmgTree;
class mmgGui;
class mmgGuiHolder;
class mmgBitmaps;
class mmgDialog;
//----------------------------------------------------------------------------
// mmgSettingsDialog :
//----------------------------------------------------------------------------
/**  */  
class mmgSettingsDialog : public mafObserver
{

public:
	mmgSettingsDialog(wxString dialog_title = "Application Settings");  
 ~mmgSettingsDialog();  

  void OnEvent(mafEventBase *maf_event);
  
  /** AddPage */	
  void AddPage(mmgGui *ui, wxString label, wxString parent_label= "");

  /** RemovePage -- kills the specified page and all its children */	
  void RemovePage(wxString label);

  /** Show the Settings Dialog. */
	bool ShowModal();

protected:  

  struct Page {
    Page*    parent;
    wxString label;
    mmgGui*  ui;
  } ;
  
  std::vector<Page*> m_List;
  mmgDialog			*m_Dialog;
	mmgTree       *m_Tree;
	mmgGuiHolder	*m_Guih;

  void  AddRoot();
  Page* GetPageByName(wxString name);
};
#endif // _mmgSettingsDialog_H_
