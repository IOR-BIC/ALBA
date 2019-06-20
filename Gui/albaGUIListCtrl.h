/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIListCtrl
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIListCtrl_H__
#define __albaGUIListCtrl_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/hash.h>

#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUINamedPanel.h"
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
/// possible values for icon
enum ITEM_ICONS
{
   ITEM_ERROR  =-1, //restituito da GetItemIcon(id) se id non esiste


   ITEM_YELLOW =0,  //todo: to be verified (after changes in albaGUIBitmaps)  //SIL. 7-4-2005: 
   ITEM_GRAY,
   ITEM_RED,
   ITEM_BLUE
};
//----------------------------------------------------------------------------
// albaGUIListCtrl :
//----------------------------------------------------------------------------
/** albaGUIListCtrl allows a simplified use of a wxWindows ListCtrl widget. */
class albaGUIListCtrl: public albaGUINamedPanel
{
public:
                 albaGUIListCtrl (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  virtual       ~albaGUIListCtrl();
  void           SetListener  (albaObserver *listener)   {m_Listener=listener;}; 

  /** Clear the control list. */
	void Reset();

  /** Add one item to the list. */
  bool AddItem(wxString label, ITEM_ICONS icon = ITEM_GRAY);

  /** Remove the item from the list. */
  bool DeleteItem(long index);

  /**Deselect an item from the list. */
  void DeselectItem(long index);

  /** Set the item's label. */
  bool SetItemLabel(long index, wxString label);

  /** Get the item's label. */
  wxString GetItemLabel  (long index);

  /** Set the item's icon, return true on success. */
  bool SetItemIcon(long index, ITEM_ICONS icon);

  /** Get the item's icon. */
  ITEM_ICONS GetItemIcon   (long index);

  /** Select the item 'id'. */
  bool SelectItem(long index);

  /** Set the label for the list's column. */
  void SetColumnLabel(int col, wxString label);

protected:
  /** Notify the Listener of item selection and deselection. */
  void OnSelectionChanged(wxListEvent& event);

  bool          m_PreventNotify;
  wxListCtrl   *m_List;         
  wxImageList  *m_Images;       
  albaObserver  *m_Listener;     

DECLARE_EVENT_TABLE()
}; // end of albaGUIListCtrl
#endif
