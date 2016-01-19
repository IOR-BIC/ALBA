/*=========================================================================

 Program: MAF2
 Module: mafGUIFileHistory
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUIFileHistory.h"
#include "mafDecl.h"
//----------------------------------------------------------------------------
// local const
//----------------------------------------------------------------------------
static const wxChar *s_MRUEntryFormat = wxT("&%d %s");
#define NUMBER_OF_RECENT_FILE 9

//----------------------------------------------------------------------------
mafGUIFileHistory::mafGUIFileHistory() :wxFileHistory(NUMBER_OF_RECENT_FILE)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIFileHistory::AddFileToHistory(const wxString& file)
//----------------------------------------------------------------------------
{
  int i;
  // if we have already have this file we will remove it and reinsert in first position
  for(i = 0; i < m_fileHistoryN; i++)
  {
    if(m_fileHistory[i] && wxString(m_fileHistory[i]) == file)
		{
			RemoveFileFromHistory(i);
			break;
		}
  }

  // Add to the project file history:
  // Move existing files (if any) down so we can insert file at beginning.

  // First delete filename that has popped off the end of the array (if any)
  if(m_fileHistoryN == m_fileMaxFiles)
  {
		delete[] m_fileHistory[m_fileMaxFiles-1];
		m_fileHistory[m_fileMaxFiles-1] = (wxChar *) NULL;
  }

  if(m_fileHistoryN < m_fileMaxFiles)
  {
    wxNode* node = m_fileMenus.GetFirst();
    while (node)
    {
      wxMenu* menu = (wxMenu*) node->GetData();
      menu->Append(wxID_FILE1+m_fileHistoryN, _("[EMPTY]"));
      node = node->GetNext();
    }
    m_fileHistoryN ++;
  }

  // Shuffle filenames down
  for (i = (m_fileHistoryN-1); i > 0; i--)
  {
    m_fileHistory[i] = m_fileHistory[i-1];
  }
  m_fileHistory[0] = copystring(file);

  for(i = 0; i < m_fileHistoryN; i++)
    if(m_fileHistory[i])
    {
      wxString buf;
      buf.Printf(s_MRUEntryFormat, i+1, m_fileHistory[i]);
      wxNode* node = m_fileMenus.GetFirst();
      while (node)
      {
        wxMenu* menu = (wxMenu*) node->GetData();
        menu->SetLabel(wxID_FILE1+i, buf);
        node = node->GetNext();
      }
    }
}
//----------------------------------------------------------------------------
void mafGUIFileHistory::RemoveFileFromHistory(int i)
//----------------------------------------------------------------------------
{
  wxCHECK_RET( i < m_fileHistoryN, _("invalid index in mafGUIFileHistory::RemoveFileFromHistory") );

  wxNode* node = m_fileMenus.GetFirst();
  while ( node )
  {
    wxMenu* menu = (wxMenu*) node->GetData();

    // delete the element from the array (could use memmove() too...)
    delete [] m_fileHistory[i];

    int j;
    for ( j = i; j < m_fileHistoryN - 1; j++ )
    {
      m_fileHistory[j] = m_fileHistory[j + 1];
    }

    // shuffle filenames up
    wxString buf;
    for( j = i; j < m_fileHistoryN - 1; j++ )
    {
      buf.Printf(s_MRUEntryFormat, j + 1, m_fileHistory[j]);
      menu->SetLabel(wxID_FILE1 + j, buf);
    }

    node = node->GetNext();

    // delete the last menu item which is unused now
    menu->Delete(wxID_FILE1 + m_fileHistoryN - 1);
  }

  m_fileHistoryN--;
}
//----------------------------------------------------------------------------
void mafGUIFileHistory::AddFilesToMenu()
//----------------------------------------------------------------------------
{
  if (m_fileHistoryN > 0)
  {
    wxNode* node = m_fileMenus.GetFirst();
    while(node)
    {
      wxMenu* menu = (wxMenu*) node->GetData();
      int i;
      for(i = 0; i < m_fileHistoryN; i++)
      {
        if(m_fileHistory[i])
        {
          wxString buf;
          buf.Printf(s_MRUEntryFormat, i+1, m_fileHistory[i]);
          if (menu->FindItem(buf) == -1)
          {
            menu->Append(wxID_FILE1+i, buf);
          }
        }
      }
      node = node->GetNext();
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIFileHistory::AddFilesToMenu(wxMenu* menu)
//----------------------------------------------------------------------------
{
  if(m_fileHistoryN > 0)
  {
    int i;
    for(i = 0; i < m_fileHistoryN; i++)
    {
      if(m_fileHistory[i])
      {
        wxString buf;
        buf.Printf(s_MRUEntryFormat, i+1, m_fileHistory[i]);
        if (menu->FindItem(buf) == -1)
        {
          menu->Append(wxID_FILE1+i, buf);
        }
      }
    }
  }
}
