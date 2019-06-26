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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGUIDialogFindVme.h"
#include "albaDecl.h"
#include "albaGUIValidator.h"
#include "albaGUICheckTree.h"

#include "albaVMERoot.h"

enum FIND_VME_ID
{
  FIND_STRING = MINID,
  MATCH_CASE_ID,
  WHOLE_WORD_ID,
  FIND_NEXT
};
//----------------------------------------------------------------------------
albaGUIDialogFindVme::albaGUIDialogFindVme(const wxString& title, long style)
: albaGUIDialog(title, style)
//----------------------------------------------------------------------------
{
  m_SearchString = "";
  m_MatchCase = 0;
  m_WholeWord = 0;
  m_Root = NULL;
  m_Tree = NULL;
  m_NodeFoundList.clear();

  m_Gui = new albaGUI(this);
  m_Gui->String(FIND_STRING,_("Find vme: "), &m_SearchString);
  m_Gui->Bool(MATCH_CASE_ID, _("Match case"), &m_MatchCase, 1);
  m_Gui->Bool(WHOLE_WORD_ID, _("Match whole word"), &m_WholeWord, 1);

  wxBoxSizer *buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
  wxButton *findNextButton = new wxButton(this,FIND_NEXT,_("Find next"));
  findNextButton->SetValidator(albaGUIValidator(this, FIND_NEXT, findNextButton));
  wxButton *cancelButton = new wxButton(this,wxID_CANCEL,_("Cancel"));
  buttons_sizer->Add(findNextButton);
  buttons_sizer->Add(cancelButton);
  
  Add(m_Gui,1);
  Add(buttons_sizer,0);
}
//----------------------------------------------------------------------------
albaGUIDialogFindVme::~albaGUIDialogFindVme()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIDialogFindVme::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
  {
    case FIND_STRING:
    break;
    case FIND_NEXT:
    {
      if (!FindNextNode(m_SearchString))
      {
        if (m_NodeFoundList.size() == 0)
        {
          albaMessage(_("No VME found named %s"), m_SearchString);
        }
        else
        {
          albaMessage(_("End of the tree!!"));
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
      albaGUIDialog::OnEvent(alba_event);
  }
}
//----------------------------------------------------------------------------
bool albaGUIDialogFindVme::FindNextNode(const char *text)
//----------------------------------------------------------------------------
{
  if (m_Root == NULL || m_Tree == NULL)
  {
    return false;
  }
  m_SearchString = text;
  albaVME *nodeFound = FindInTreeByName(m_Root, m_SearchString, m_MatchCase != 0, m_WholeWord != 0);
  if (nodeFound == NULL)
  {
    return false;
  }
  return m_Tree->SelectNode((long long)nodeFound);
}
//----------------------------------------------------------------------------
void albaGUIDialogFindVme::SetTree(albaGUICheckTree *tree)
//----------------------------------------------------------------------------
{
  m_Tree = tree;
  if (m_Tree != NULL)
  {
    albaVME *n = m_Tree->GetSelectedNode();
    if (n != NULL)
    {
      m_Root = albaVMERoot::SafeDownCast(n->GetRoot());
    }
  }
}
//-------------------------------------------------------------------------
albaVME *albaGUIDialogFindVme::FindInTreeByName(albaVME *node, const char *name, bool match_case, bool whole_word)
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
    if (albaVME *n = FindInTreeByName(node->GetChild(i), name, match_case, whole_word))
      return n;
  }
  return NULL;
}
//-------------------------------------------------------------------------
bool albaGUIDialogFindVme::AlreadyFound(albaVME *n)
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
