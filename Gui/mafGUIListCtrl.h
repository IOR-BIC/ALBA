/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIListCtrl.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIListCtrl_H__
#define __mafGUIListCtrl_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/hash.h>

#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUINamedPanel.h"
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
/// possible values for icon
enum ITEM_ICONS
{
   ITEM_ERROR  =-1, //restituito da GetItemIcon(id) se id non esiste


   ITEM_YELLOW =0,  //todo: to be verified (after changes in mafGUIBitmaps)  //SIL. 7-4-2005: 
   ITEM_GRAY,
   ITEM_RED,
   ITEM_BLUE
};
//----------------------------------------------------------------------------
// mafGUIListCtrl :
//----------------------------------------------------------------------------
/** mafGUIListCtrl allows a simplified use of a wxWindows ListCtrl widget. */
class mafGUIListCtrl: public mafGUINamedPanel
{
public:
                 mafGUIListCtrl (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  virtual       ~mafGUIListCtrl();
  void           SetListener  (mafObserver *listener)   {m_Listener=listener;}; 

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

  bool          m_PreventNotify;
  wxListCtrl   *m_List;         
  wxImageList  *m_Images;       
  mafObserver  *m_Listener;     

DECLARE_EVENT_TABLE()
}; // end of mafGUIListCtrl
#endif
