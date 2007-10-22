/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogFindVme.h,v $
  Language:  C++
  Date:      $Date: 2007-10-22 06:39:25 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIDialogFindVme_H__
#define __mafGUIDialogFindVme_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmgDialog.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mmgCheckTree;
class mafVMERoot;

/** mafGUIDialogFindVme - a dialog widget with a find VME GUI.
@sa mmgDialog
*/
class mafGUIDialogFindVme : public mmgDialog
{
public:
	mafGUIDialogFindVme(const wxString& title, long style = mafCLOSEWINDOW);
	virtual ~mafGUIDialogFindVme();

  void OnEvent(mafEventBase *maf_event);

  /** Set the tree on which find the VME.*/
  void SetTree(mmgCheckTree *tree);

  /** Useful for initialize the search string or to use it by code.*/
  bool FindNextNode(const char *text);

  /** Set the flag for the case sensitive search.*/
  void MetchCase(bool mc = false) {m_MatchCase = mc;};

  /** Set the flag for searching node with the whole word matching.*/
  void WholeWord(bool ww = false) {m_WholeWord = ww;};

protected:
  /** Find a node in all the subtrees matching the given VME Name.*/
  mafNode *FindInTreeByName(mafNode *node, const char *name, bool match_case = true, bool whole_word = true);

  /** Check if a node has been found yet.*/
  bool AlreadyFound(mafNode *n);

  mmgGui *m_Gui; ///< Gui variable used to plug custom widgets
  mafString m_SearchString; ///< The string to find into the VME tree.
  int m_MatchCase; ///< Enable the case sensitive search.
  int m_WholeWord; ///< Enable the find only for exact matching VME names.
  mmgCheckTree *m_Tree; ///< The VME tree.
  mafVMERoot *m_Root;
  std::vector<mafNode*> m_NodeFoundList;
};
#endif
