/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgListCtrl.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 11:39:46 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgListCtrl_H__
#define __mmgListCtrl_H__

#include "mafDefines.h" //important: mafDefines should always be included as first

#include <wx/laywin.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/hash.h>

#include "mafEvent.h"
#include "mmgNamedPanel.h"
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
/// possible values for icon
enum ITEM_ICONS
{
   ITEM_ERROR  =-1, //restituito da GetItemIcon(id) se id non esiste


   ITEM_YELLOW =0,  //todo: to be verified (after changes in mmgBitmaps)  //SIL. 7-4-2005: 
   ITEM_GRAY,
   ITEM_RED,
   ITEM_BLUE
};
//----------------------------------------------------------------------------
// mmgListCtrl :
//----------------------------------------------------------------------------
/** mmgListCtrl allows a simplified use of a wxWindows ListCtrl widget. */
class mmgListCtrl: public mmgNamedPanel
{
public:
                 mmgListCtrl (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  virtual       ~mmgListCtrl();
  void           SetListener  (mafEventListener *listener)   {m_Listener=listener;}; 

  /** Clear the control list. */
	void Reset();

  /** Add one item to the list. */
  bool AddItem(long item_id, wxString label, ITEM_ICONS icon = ITEM_GRAY);

  /** Remove the item from the list. */
  bool DeleteItem(long item_id);

  /** Set the item's label. */
  bool SetItemLabel(long item_id, wxString label);

  /** Get the item's label. */
  wxString GetItemLabel  (long item_id);

  /** Set the item's icon, return true on success. */
  bool SetItemIcon(long item_id, ITEM_ICONS icon);

  /** Get the item's icon. */
  ITEM_ICONS GetItemIcon   (long item_id);

  /** Select the item 'id'. */
  bool SelectItem(long item_id);

  /** Set the label for the list's column. */
  void SetColumnLabel(int col, wxString label);

protected:
  /** Notify the Listener of item selection and deselection. */
  void OnSelectionChanged(wxListEvent& event);

  /** Return true if the item exists. */
  bool ItemExist(long item_id);
    
  bool               m_prevent_notify;
  wxListCtrl        *m_list;         
  wxImageList       *m_images;       
  mafEventListener  *m_Listener;     

DECLARE_EVENT_TABLE()
}; // end of mmgListCtrl
#endif
