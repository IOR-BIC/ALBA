/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIListCtrlBitmap
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIListCtrlBitmap_H__
#define __albaGUIListCtrlBitmap_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/hash.h>
#include "albaEvent.h"
#include "albaGUINamedPanel.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaObserver;

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
/// possible values for icon
enum ITEM_ICONS
{
   ITEM_ERROR  = -1, //returned by GetItemIcon(id) if 'id' doesn't exist
   ITEM_YELLOW = 0,
   ITEM_GRAY,
   ITEM_RED,
   ITEM_BLUE
};
//----------------------------------------------------------------------------
// albaGUIListCtrlBitmap :
//----------------------------------------------------------------------------
/** albaGUIListCtrlBitmap allows a simplified use of a wxWindows ListCtrl widget. */
class albaGUIListCtrlBitmap: public albaGUINamedPanel
{
public:
                 albaGUIListCtrlBitmap (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  virtual       ~albaGUIListCtrlBitmap();
  
	void SetListener(albaObserver *listener)   {m_Listener = listener;}; 

  /** Clear the control list. */
  void Reset();

  /** Add one item to the list. */
  bool AddItem(long item_id, wxString label, wxBitmap *bmp = NULL);

  /** Remove the item from the list. */
  bool DeleteItem(long item_id);

  /** Set the item's label. */
  bool SetItemLabel(long item_id, wxString label);

  /** Get the item's label. */
  wxString GetItemLabel  (long item_id);

  /** Set the item's icon, return true on success. */
  bool SetItemIcon(long item_id, wxBitmap *bmp);

  /** Get the item's icon. */
  ITEM_ICONS GetItemIcon(long item_id);

  /** Select the item 'id'. */
  bool SelectItem(long item_id);

  /** Set the label for the list's column. */
  void SetColumnLabel(int col, wxString label);

protected:
  /** Notify the Listener of item selection and deselection. */
  void OnSelectionChanged(wxListEvent& event);

  bool         m_PreventNotify;
  wxListCtrl  *m_List;         
  wxImageList *m_Images;       
  albaObserver *m_Listener;     

DECLARE_EVENT_TABLE()
}; // end of albaGUIListCtrlBitmap
#endif
