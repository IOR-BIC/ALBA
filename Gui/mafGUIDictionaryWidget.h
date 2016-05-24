/*=========================================================================

 Program: MAF2
 Module: mafGUIDictionaryWidget
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIDictionaryWidget_H__
#define __mafGUIDictionaryWidget_H__

// mafGUIDictionaryWidget :
/**
- mafGUIDictionaryWidget is the class that treats dictionaries.
*/
#include "mafObserver.h"
#include "mafStorable.h"
#include "mafDefines.h"
#include "mafGUIListCtrl.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVME;
class mafGUIListCtrl;
class mafEvent;
class mafObserver;


/**
  class name : mafGUIDictionaryWidget
  Widget that handle a dictionary object. 
*/
class MAF_EXPORT mafGUIDictionaryWidget : public mafObserver
{

public:
  /** constructor*/
            mafGUIDictionaryWidget(wxWindow *parent, int id);
  /** destructor */
           ~mafGUIDictionaryWidget();
  /** Set the listener of the events launched */
  void			SetListener(mafObserver *Listener) {m_Listener = Listener;};
  /** Answer to the messages coming from interface. */
  void      OnEvent(mafEventBase *event);

  /** Set the reference cloud. */
  void SetCloud(mafVME *vme);
  
	void AddItem(wxString item);

	int RemoveItem(long itemId);

	int RemoveItem(mafString itemName);

	int GetItemIndex(mafString itemName);

	mafString GetItemByIndex(int index);

	int UpdateItem(mafString oldItemName, mafString newItemName);

	int SelectItem(mafString itemName, bool select=true);

	int DeselectItem(mafString itemName);
	
	int GetSize() { return m_Items.size(); };

	void SetTitle(wxString title){ m_List->SetColumnLabel(0, title); };

	/** Return choosed item. */
  wxString ChooseItem();
  
	/** Load dictionary opening file dialog. */
  void LoadDictionary();
  
	/** Load dictionary from file. */
  void LoadDictionary(wxString file);
  
	void InitDictionary(std::vector<wxString> *strVect);

	/** Return dictionary GUI. */
  wxWindow *GetWidget() {return (wxWindow *)m_List;};

  /** Return file name dictionary */
  const char* GetDictionaryFileName(){return m_File;};

protected:
	/** Set the icon of all the items present into the cloud to red or gray according to valid. */
  void ValidateAllItem(bool valid = true);

  mafObserver *m_Listener;
	wxString     m_File;
	std::vector<mafString> m_Items;
	std::vector<bool> m_Selections;
  mafGUIListCtrl *m_List;
  mafVME      *m_Vme;
};

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafString);
#endif
/**
 class name: mafStorableDictionary
  Utility object that supply the  InternalStore  and InternalRestore method for serialization.
*/
class MAF_EXPORT mafStorableDictionary: public mafObject, public mafStorable
{
public:
  /** RTTI macro*/
  mafTypeMacro(mafStorableDictionary,mafObject);
  /** constructor */
  mafStorableDictionary();
  /** destructor */
  ~mafStorableDictionary();
  /** serialize  object attributes */
  virtual int InternalStore(mafStorageElement *parent){return MAF_OK;}
  /** restore  object attributes */
  virtual int InternalRestore(mafStorageElement *node);
  std::vector<mafString> m_StrVector;
};
#endif
