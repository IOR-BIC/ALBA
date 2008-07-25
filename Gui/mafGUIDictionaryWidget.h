/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDictionaryWidget.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUIDictionaryWidget_H__
#define __mafGUIDictionaryWidget_H__

// mafGUIDictionaryWidget :
/**
- mafGUIDictionaryWidget is the class that treats dictionaries.
*/
#include "mafObserver.h"
#include "mafStorable.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVME;
class mafGUIListCtrl;
class mafEvent;
class mafObserver;


//----------------------------------------------------------------------------
// mafGUIDictionaryWidget :
//----------------------------------------------------------------------------
class mafGUIDictionaryWidget : public mafObserver
{

public:
            mafGUIDictionaryWidget(wxWindow *parent, int id);
           ~mafGUIDictionaryWidget();
  void			SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void      OnEvent(mafEventBase *event);

  /** Set the reference cloud. */
  void SetCloud(mafVME *vme);
  
	/** Return choosed item. */
  wxString ChooseItem();
  
	/** Load dictionary opening file dialog. */
  void LoadDictionary();
  
	/** Load dictionary from file. */
  void LoadDictionary(wxString file);
  
	/** Return dictionary GUI. */
  wxWindow *GetWidget() {return (wxWindow *)m_List;};

  /** Return file name dictionary */
  const char* GetDictionaryFileName(){return m_File;};

protected:
	/** Set the icon of the item to red or gray according to valid. */
  void ValidateItem(wxString item, bool valid = true);
  
	/** Set the icon of all the items present into the cloud to red or gray according to valid. */
  void ValidateAllItem(bool valid = true);

  mafObserver *m_Listener;
	wxString     m_File;
  int          m_NumItem;
	wxString   **m_Items;
  mafGUIListCtrl *m_List;
  mafVME      *m_Vme;
};

//------------------------------------------------------------------------------
class mafStorableDictionary: public mafObject, public mafStorable
//------------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafStorableDictionary,mafObject);
  mafStorableDictionary();
  ~mafStorableDictionary();
  virtual int InternalStore(mafStorageElement *parent){return MAF_OK;}
  virtual int InternalRestore(mafStorageElement *node);
  std::vector<mafString> m_StrVector;
};
#endif
