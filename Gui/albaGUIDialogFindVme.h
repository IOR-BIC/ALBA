/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogFindVme
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIDialogFindVme_H__
#define __albaGUIDialogFindVme_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaGUICheckTree;
class albaVMERoot;

/** albaGUIDialogFindVme - a dialog widget with a find VME GUI.
@sa albaGUIDialog
*/
class albaGUIDialogFindVme : public albaGUIDialog
{
public:
	albaGUIDialogFindVme(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~albaGUIDialogFindVme();

  void OnEvent(albaEventBase *alba_event);

  /** Set the tree on which find the VME.*/
  void SetTree(albaGUICheckTree *tree);

  /** Useful for initialize the search string or to use it by code.*/
  bool FindNextNode(const char *text);

  /** Set the flag for the case sensitive search.*/
  void MetchCase(bool mc = false) {m_MatchCase = mc;};

  /** Set the flag for searching node with the whole word matching.*/
  void WholeWord(bool ww = false) {m_WholeWord = ww;};

protected:
  /** Find a node in all the subtrees matching the given VME Name.*/
  albaVME *FindInTreeByName(albaVME *node, const char *name, bool match_case = true, bool whole_word = true);

  /** Check if a node has been found yet.*/
  bool AlreadyFound(albaVME *n);

  albaGUI *m_Gui; ///< Gui variable used to plug custom widgets
  albaString m_SearchString; ///< The string to find into the VME tree.
  int m_MatchCase; ///< Enable the case sensitive search.
  int m_WholeWord; ///< Enable the find only for exact matching VME names.
  albaGUICheckTree *m_Tree; ///< The VME tree.
  albaVMERoot *m_Root;
  std::vector<albaVME*> m_NodeFoundList;
};
#endif
