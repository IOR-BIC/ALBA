/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCheckListBox.h,v $
  Language:  C++
  Date:      $Date: 2005-06-22 07:02:28 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgCheckListBox_H__
#define __mmgCheckListBox_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmgPanel.h"
//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mafObserver;
//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
#define mmgCheckListBox_ArraySize 50

enum {
	MODE_CHECK,
	MODE_RADIO
};

//----------------------------------------------------------------------------
// mmgCheckListBox :
/**
a CheckList that send mafEventNotification in the form
mafEvent(this, widget_id, item_id, checked)
*/
//----------------------------------------------------------------------------
class mmgCheckListBox: public mmgPanel
{
public:
  mmgCheckListBox(wxWindow* parent, 
                  wxWindowID id, 
                  const wxPoint& pos = wxDefaultPosition, 
                  const wxSize& size = wxDefaultSize, 
                  long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

  virtual ~mmgCheckListBox();

  /** Clear the checked listbox. */
  void Clear();

  /** Add the item to the checked listbox. */
  void AddItem(int id, wxString label);

  /** Add the item to the checked listbox and allow to check it. */
  void AddItem(int id, wxString label, bool checked);

  /** Remove the item from the checked listbox. */
  void RemoveItem(int id);

  /** Check/Uncheck the item into the checked listbox. */
  void CheckItem(int id, bool check);

  /** Set the item label. */
  void SetItemLabel(int id, wxString label);

  /** Get the item label. */
	wxString GetItemLabel(int id);

  /** Select the item. */
	void Select(int id);

  /** Get the selected item. */
	int GetSelection() {return m_selectedItem;};

  /** Return true if the item is checked. */
  bool IsItemChecked(int id);

  /** Set the check mode of the checked listbox to be MODE_RADIO or MODE_CHECK. */
  void SetCheckMode(int mode) {m_check_mode = mode;};

  /** Set the check mode of the checked listbox to be MODE_RADIO. */
	void SetCheckModeToRadio() {m_check_mode = MODE_RADIO;};

  /** Set the check mode of the checked listbox to be MODE_CHECK. */
	void SetCheckModeToCheck() {m_check_mode = MODE_CHECK;};

  void SetListener(mafObserver *listener) {m_Listener = listener;};

  /** Return the index of the item into the checked listbox, otherwise -1 if the item is not present. */
	int FindItem(wxString label);

protected:
  /** Send an event to the listener to notify that an item has been selected. */
  void OnSelect(wxCommandEvent &event);

  /** Send an event to the listener to notify that an item has been checked. */
  void OnCheck(wxCommandEvent &event);

  /** Resize the checked lisbox according to the new parent panel size. */
  void OnSize(wxSizeEvent& event);

  /** Return the index of the item into the array, otherwise -1 if the item is not present. */
  int FindItemIndex(int id);

  mafObserver*      m_Listener;
  wxCheckListBox   *m_clb;
  bool              m_prevent_notify;
	int               m_check_mode;
  int               m_array[mmgCheckListBox_ArraySize];   // item_position_in_CheckListBox -> id
  int               m_selectedItem;

  DECLARE_EVENT_TABLE()
};
#endif
