/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgListCtrlBitmap.h,v $
  Language:  C++
  Date:      $Date: 2005-05-30 15:51:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgListCtrlBitmap_H__
#define __mmgListCtrlBitmap_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/hash.h>
#include "mafEvent.h"
#include "mmgNamedPanel.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafObserver;

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
// mmgListCtrlBitmap :
//----------------------------------------------------------------------------
/** mmgListCtrlBitmap allows a simplified use of a wxWindows ListCtrl widget. */
class mmgListCtrlBitmap: public mmgNamedPanel
{
public:
                 mmgListCtrlBitmap (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  virtual       ~mmgListCtrlBitmap();
  
	void SetListener(mafObserver *listener)   {m_Listener = listener;}; 

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

  /** Return true if the item exists. */
  bool ItemExist(long item_id);
    
  bool         m_PreventNotify;
  wxListCtrl  *m_List;         
  wxImageList *m_Images;       
  mafObserver *m_Listener;     

DECLARE_EVENT_TABLE()
}; // end of mmgListCtrlBitmap
#endif
