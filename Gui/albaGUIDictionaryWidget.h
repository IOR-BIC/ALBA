/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDictionaryWidget
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIDictionaryWidget_H__
#define __albaGUIDictionaryWidget_H__

// albaGUIDictionaryWidget :
/**
- albaGUIDictionaryWidget is the class that treats dictionaries.
*/
#include "albaObserver.h"
#include "albaStorable.h"
#include "albaDefines.h"
#include "albaGUIListCtrl.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaVME;
class albaGUIListCtrl;
class albaEvent;
class albaObserver;


/**
  class name : albaGUIDictionaryWidget
  Widget that handle a dictionary object. 
*/
class ALBA_EXPORT albaGUIDictionaryWidget : public albaObserver
{

public:
  /** constructor*/
            albaGUIDictionaryWidget(wxWindow *parent, int id);
  /** destructor */
           ~albaGUIDictionaryWidget();
  /** Set the listener of the events launched */
  void			SetListener(albaObserver *Listener) {m_Listener = Listener;};
  /** Answer to the messages coming from interface. */
  void      OnEvent(albaEventBase *event);

  /** Set the reference cloud. */
  void SetCloud(albaVME *vme);
  
	void AddItem(wxString item);

	int RemoveItem(long itemId);

	int RemoveItem(albaString itemName);

	int GetItemIndex(albaString itemName);

	albaString GetItemByIndex(int index);

	int UpdateItem(albaString oldItemName, albaString newItemName);

	int SelectItem(albaString itemName, bool select=true);

	int DeselectItem(albaString itemName);
	
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

  albaObserver *m_Listener;
	wxString     m_File;
	std::vector<albaString> m_Items;
	std::vector<bool> m_Selections;
  albaGUIListCtrl *m_List;
  albaVME      *m_Vme;
};

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaString);
#endif
/**
 class name: albaStorableDictionary
  Utility object that supply the  InternalStore  and InternalRestore method for serialization.
*/
class ALBA_EXPORT albaStorableDictionary: public albaObject, public albaStorable
{
public:
  /** RTTI macro*/
  albaTypeMacro(albaStorableDictionary,albaObject);
  /** constructor */
  albaStorableDictionary();
  /** destructor */
  ~albaStorableDictionary();
  /** serialize  object attributes */
  virtual int InternalStore(albaStorageElement *parent){return ALBA_OK;}
  /** restore  object attributes */
  virtual int InternalRestore(albaStorageElement *node);
  std::vector<albaString> m_StrVector;
};
#endif
