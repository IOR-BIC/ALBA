/*=========================================================================

 Program: MAF2
 Module: mafOpEditMetadata
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafOpEditMetadata: public mafOp
{
public:
  mafOpEditMetadata(const wxString &label = "EditMetadata");
 ~mafOpEditMetadata(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpEditMetadata, mafOp);

  mafOp* Copy();

  bool Accept(mafVME*node) {return true;};
  void OpRun();
  void OpUndo();

  /** Select the tag into the tag array and initialize the variables associated with the gui.*/
  void SelectTag(const char *tag_name);

  /** Set the name of the selected mafTagItem.*/
  void SetTagName(const char *name);

  /** Remove the selected mafTagItem.*/
  void RemoveTag();

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
