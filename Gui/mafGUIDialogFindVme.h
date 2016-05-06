/*=========================================================================

 Program: MAF2
 Module: mafGUIDialogFindVme
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIDialogFindVme_H__
#define __mafGUIDialogFindVme_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUIDialog.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafGUICheckTree;
class mafVMERoot;

/** mafGUIDialogFindVme - a dialog widget with a find VME GUI.
@sa mafGUIDialog
*/
class mafGUIDialogFindVme : public mafGUIDialog
{
public:
	mafGUIDialogFindVme(const wxString& title, long style = mafCLOSEWINDOW);
	virtual ~mafGUIDialogFindVme();

  void OnEvent(mafEventBase *maf_event);

  /** Set the tree on which find the VME.*/
  void SetTree(mafGUICheckTree *tree);

  /** Useful for initialize the search string or to use it by code.*/
  bool FindNextNode(const char *text);

  /** Set the flag for the case sensitive search.*/
  void MetchCase(bool mc = false) {m_MatchCase = mc;};

  /** Set the flag for searching node with the whole word matching.*/
  void WholeWord(bool ww = false) {m_WholeWord = ww;};

protected:
  /** Find a node in all the subtrees matching the given VME Name.*/
  mafVME *FindInTreeByName(mafVME *node, const char *name, bool match_case = true, bool whole_word = true);

  /** Check if a node has been found yet.*/
  bool AlreadyFound(mafVME *n);

  mafGUI *m_Gui; ///< Gui variable used to plug custom widgets
  mafString m_SearchString; ///< The string to find into the VME tree.
  int m_MatchCase; ///< Enable the case sensitive search.
  int m_WholeWord; ///< Enable the find only for exact matching VME names.
  mafGUICheckTree *m_Tree; ///< The VME tree.
  mafVMERoot *m_Root;
  std::vector<mafVME*> m_NodeFoundList;
};
#endif
