/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditMetadata
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpEditMetadata_H__
#define __albaOpEditMetadata_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaTagArray;
class albaTagItem;

//----------------------------------------------------------------------------
// albaOpEditMetadata :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpEditMetadata: public albaOp
{
public:
  albaOpEditMetadata(const wxString &label = "EditMetadata");
 ~albaOpEditMetadata(); 
	virtual void OnEvent(albaEventBase *alba_event);

  albaTypeMacro(albaOpEditMetadata, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpUndo();

  /** Select the tag into the tag array and initialize the variables associated with the gui.*/
  void SelectTag(const char *tag_name);

  /** Set the name of the selected albaTagItem.*/
  void SetTagName(const char *name);

  /** Remove the selected albaTagItem.*/
  void RemoveTag();

  /** Create a new albaTagItem and initialize it with standard parameters.*/
  void AddNewTag(albaString &name);

protected:

  enum TAG_TYPE
  {
    NUMERIC_TAG = 0,
    STRING_TAG
  };

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  /** Enable-Disable editing widgets.*/
  void EnableWidgets();

  wxListBox   *m_MetadataList;
  albaTagArray *m_TagArray;
  albaTagArray *m_OldTagArray;
  albaTagItem  *m_SelectedTag;

  albaString    m_TagName;
  int          m_TagType;
  int          m_TagMulteplicity;
  int          m_TagComponent;
  albaString    m_TagValueAsString;
  double       m_TagValueAsDouble;
};
#endif
