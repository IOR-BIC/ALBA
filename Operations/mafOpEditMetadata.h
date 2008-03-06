/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpEditMetadata.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpEditMetadata_H__
#define __mafOpEditMetadata_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafTagArray;
class mafTagItem;

//----------------------------------------------------------------------------
// mafOpEditMetadata :
//----------------------------------------------------------------------------
/** */
class mafOpEditMetadata: public mafOp
{
public:
  mafOpEditMetadata(const wxString &label = "EditMetadata");
 ~mafOpEditMetadata(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpEditMetadata, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node) {return true;};
  void OpRun();
  void OpUndo();

  /** Select the tag into the tag array and initialize the variables associated with the gui.*/
  void SelectTag(const char *tag_name);

  /** Create a new mafTagItem and initialize it with standard parameters.*/
  void AddNewTag(mafString &name);

protected:
  enum TAG_TYPE
  {
    NUMERIC_TAG = 0,
    STRING_TAG
  };

  /** Enable-Disable editing widgets.*/
  void EnableWidgets();

  wxListBox   *m_MetadataList;
  mafTagArray *m_TagArray;
  mafTagArray *m_OldTagArray;
  mafTagItem  *m_SelectedTag;

  mafString    m_TagName;
  int          m_TagType;
  int          m_TagMulteplicity;
  int          m_TagComponent;
  mafString    m_TagValueAsString;
  double       m_TagValueAsDouble;
};
#endif
