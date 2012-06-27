/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDictionaryWidget.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 10:08:10 $
  Version:   $Revision: 1.1.2.2 $
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
#include "mafDefines.h"
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
