/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogFindVme.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:23 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUIDialogFindVme.h"
#include "mafDecl.h"
#include "mafGUIValidator.h"
#include "mafGUICheckTree.h"

#include "mafVMERoot.h"

enum FIND_VME_ID
{
  FIND_STRING = MINID,
  MATCH_CASE_ID,
  WHOLE_WORD_ID,
  FIND_NEXT
};
//----------------------------------------------------------------------------
mafGUIDialogFindVme::mafGUIDialogFindVme(const wxString& title, long style)
: mafGUIDialog(title, style)
//----------------------------------------------------------------------------
{
  m_SearchString = "";
  m_MatchCase = 0;
  m_WholeWord = 0;
  m_Root = NULL;
  m_Tree = NULL;
  m_NodeFoundList.clear();

  m_Gui = new mafGUI(this);
  m_Gui->String(FIND_STRING,_("find vme: "), &m_SearchString);
  m_Gui->Bool(MATCH_CASE_ID, _("match case"), &m_MatchCase, 1);
  m_Gui->Bool(WHOLE_WORD_ID, _("match whole word"), &m_WholeWord, 1);

  wxBoxSizer *buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
  wxButton *findNextButton = new wxButton(this,FIND_NEXT,_("find next"));
  findNextButton->SetValidator(mafGUIValidator(this, FIND_NEXT, findNextButton));
  wxButton *cancelButton = new wxButton(this,wxID_CANCEL,_("cancel"));
  buttons_sizer->Add(findNextButton);
  buttons_sizer->Add(cancelButton);
  
  Add(m_Gui,1);
  Add(buttons_sizer,0);
}
//----------------------------------------------------------------------------
mafGUIDialogFindVme::~mafGUIDialogFindVme()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIDialogFindVme::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case FIND_STRING:
    break;
    case FIND_NEXT:
    {
      if (!FindNextNode(m_SearchString))
      {
        if (m_NodeFoundList.size() == 0)
        {
          mafMessage(_("No VME found named %s"), m_SearchString);
        }
        else
        {
          mafMessage(_("End of the tree!!"));
          m_NodeFoundList.clear();
        }
      }
    }
    break;
    case MATCH_CASE_ID:
    break;
    case WHOLE_WORD_ID:
    break;
    default:
      mafGUIDialog::OnEvent(maf_event);
  }
}
//----------------------------------------------------------------------------
bool mafGUIDialogFindVme::FindNextNode(const char *text)
//----------------------------------------------------------------------------
{
  if (m_Root == NULL || m_Tree == NULL)
  {
    return false;
  }
  m_SearchString = text;
  mafNode *nodeFound = FindInTreeByName(m_Root, m_SearchString, m_MatchCase != 0, m_WholeWord != 0);
  if (nodeFound == NULL)
  {
    return false;
  }
  return m_Tree->SelectNode((long)nodeFound);
}
//----------------------------------------------------------------------------
void mafGUIDialogFindVme::SetTree(mafGUICheckTree *tree)
//----------------------------------------------------------------------------
{
  m_Tree = tree;
  if (m_Tree != NULL)
  {
    mafNode *n = m_Tree->GetSelectedNode();
    if (n != NULL)
    {
      m_Root = mafVMERoot::SafeDownCast(n->GetRoot());
    }
  }
}
//-------------------------------------------------------------------------
mafNode *mafGUIDialogFindVme::FindInTreeByName(mafNode *node, const char *name, bool match_case, bool whole_word)
//-------------------------------------------------------------------------
{
  wxString word_to_search;
  word_to_search = name;
  wxString myName = node->GetName();

  if (!match_case)
  {
    word_to_search.MakeLower();
    myName.MakeLower();
  }

  if (whole_word)
  {
    if (myName == word_to_search)
    {
      if (!AlreadyFound(node))
      {
        m_NodeFoundList.push_back(node);
        return node;
      }
    }
  }
  else
  {
    if (myName.Find(word_to_search) != -1)
    {
      if (!AlreadyFound(node))
      {
        m_NodeFoundList.push_back(node);
        return node;
      }
    }
  }

  for (int i = 0; i < node->GetNumberOfChildren(); i++)
  {
    if (mafNode *n = FindInTreeByName(node->GetChild(i), name, match_case, whole_word))
      return n;
  }
  return NULL;
}
//-------------------------------------------------------------------------
bool mafGUIDialogFindVme::AlreadyFound(mafNode *n)
//-------------------------------------------------------------------------
{
  int checkNode = 0;
  bool alreadyFound = false;

  for (; checkNode < m_NodeFoundList.size(); checkNode++)
  {
    if (n == m_NodeFoundList[checkNode])
    {
      alreadyFound = true;
    }
  }
  return alreadyFound;
}
