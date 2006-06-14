/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDictionaryWidget.h,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgDictionaryWidget_H__
#define __mmgDictionaryWidget_H__

// mmgDictionaryWidget :
/**
- mmgDictionaryWidget is the class that treats dictionaries.
*/
#include "mafObserver.h"
#include "mafStorable.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVME;
class mmgListCtrl;
class mafEvent;
class mafObserver;


//----------------------------------------------------------------------------
// mmgDictionaryWidget :
//----------------------------------------------------------------------------
class mmgDictionaryWidget : public mafObserver
{

public:
            mmgDictionaryWidget(wxWindow *parent, int id);
           ~mmgDictionaryWidget();
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
  
	/** Return dictionary gui. */
  wxWindow *GetWidget() {return (wxWindow *)m_List;};

protected:
	/** Set the icon of the item to red or gray according to valid. */
  void ValidateItem(wxString item, bool valid = true);
  
	/** Set the icon of all the items present into the cloud to red or gray according to valid. */
  void ValidateAllItem(bool valid = true);

  mafObserver   *m_Listener;
	static wxString			m_File;
  int									m_NumItem;
	wxString					**m_Items;
  mmgListCtrl				 *m_List;
  mafVME						 *m_Vme;
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
