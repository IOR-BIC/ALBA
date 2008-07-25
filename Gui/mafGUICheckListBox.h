/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUICheckListBox.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUICheckListBox_H__
#define __mafGUICheckListBox_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUIPanel.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mafObserver;

//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
#define mafGUICheckListBox_ArraySize 4096

enum CHECKLIST_MODE_ID
{
	MODE_CHECK,
	MODE_RADIO
};

//----------------------------------------------------------------------------
// mafGUICheckListBox :
/**
a CheckList that send mafEventNotification in the form
mafEvent(this, widget_id, item_id, checked)
*/
//----------------------------------------------------------------------------
class mafGUICheckListBox: public mafGUIPanel
{
public:
  mafGUICheckListBox(wxWindow* parent, 
                  wxWindowID id, 
                  const wxPoint& pos = wxDefaultPosition, 
                  const wxSize& size = wxDefaultSize, 
                  long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

  virtual ~mafGUICheckListBox();

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
  void SetItemLabel(int id, mafString label);

  /** Get the item label. */
	mafString GetItemLabel(int id);

  /** Select the item. */
	void Select(int id);

  /** Get the selected item. */
	int GetSelection() {return m_SelectedItem;};

  /** Return true if the item is checked. */
  bool IsItemChecked(int id);

  /** Set the check mode of the checked listbox to be MODE_RADIO or MODE_CHECK. */
  void SetCheckMode(int mode) {m_CheckMode = mode;};

  /** Set the check mode of the checked listbox to be MODE_RADIO. */
	void SetCheckModeToRadio() {m_CheckMode = MODE_RADIO;};

  /** Set the check mode of the checked listbox to be MODE_CHECK. */
	void SetCheckModeToCheck() {m_CheckMode = MODE_CHECK;};

  void SetListener(mafObserver *listener) {m_Listener = listener;};

  /** Return the index of the item into the checked listbox, otherwise -1 if the item is not present. */
	int FindItem(wxString label);

  /** Return the index of the item into the array, otherwise -1 if the item is not present. */
  int FindItemIndex(int id);

  /** Return true if the event raised is coming from a check/uncheck item.*/
  bool IsCheckEvent() {return m_CheckEvent;};

  /** Return the number of inserted items.*/
  int GetNumberOfItems() {return m_CheckListBox->GetCount();};

protected:
  /** Send an event to the listener to notify that an item has been selected. */
  void OnSelect(wxCommandEvent &event);

  /** Send an event to the listener to notify that an item has been checked. */
  void OnCheck(wxCommandEvent &event);

  /** Resize the checked listbox according to the new parent panel size. */
  void OnSize(wxSizeEvent& event);

  bool              m_CheckEvent; ///< Flag used to distinguish between Check item and Select item event
  mafObserver*      m_Listener;
  wxCheckListBox   *m_CheckListBox;
  bool              m_PreventNotify;
	int               m_CheckMode;
  int               m_Array[mafGUICheckListBox_ArraySize];   // item_position_in_CheckListBox -> id
  int               m_SelectedItem;

  DECLARE_EVENT_TABLE()
};
#endif
