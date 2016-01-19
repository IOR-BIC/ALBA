/*=========================================================================

 Program: MAF2
 Module: mafGUIDialogRemoteFile
 Authors: Paolo Quadrani
 
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

#define DEFAULT_REMOTE_PATH "/mafstorage/"

#include "mafGUIDialogRemoteFile.h"
#include <wx/tokenzr.h>

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUISettingsStorage.h"

#include "mafCurlUtility.h"
#include <curl/types.h>
#include <curl/easy.h>

//----------------------------------------------------------------------------
mafGUIDialogRemoteFile::mafGUIDialogRemoteFile(mafObserver *listener,const wxString &title, long style)
: mafGUIDialog(title, style)
//----------------------------------------------------------------------------
{
	m_Listener	      = listener;
  m_RemoteFilesList = NULL;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  m_Curl = curl_easy_init();

  CreateGui();
}
//----------------------------------------------------------------------------
mafGUIDialogRemoteFile::~mafGUIDialogRemoteFile() 
//----------------------------------------------------------------------------
{
  curl_easy_cleanup(m_Curl);
  curl_global_cleanup();
}
//----------------------------------------------------------------------------
//constants:
//----------------------------------------------------------------------------
enum REMOTE_FILE_WIDGET_ID
{
	ID_HOST = MINID,
  ID_PORT,
  ID_USER,
  ID_PWD,
  ID_PATH,
  ID_BROWSE_REMOTE_FILE,
  ID_BROWSE_LOCAL_FILE,
  ID_LIST_FILES
};
//----------------------------------------------------------------------------
void mafGUIDialogRemoteFile::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
	m_Gui->Show(true);

  mafGUISettingsStorage *storage_settings = new mafGUISettingsStorage(this);
  m_Host = storage_settings->GetRemoteHostName();
  m_Port = storage_settings->GetRemotePort();
  m_User = storage_settings->GetUserName();
  m_Pwd  = storage_settings->GetPassword();
  cppDEL(storage_settings);

  m_RemotePath = DEFAULT_REMOTE_PATH;
  m_RemoteFilename = m_Host;
  m_RemoteFilename += m_RemotePath;
  m_Gui->String(ID_HOST,"host",&m_Host,_("hostname including the protocol: http://..."));
  m_Gui->Integer(ID_PORT,_("port"),&m_Port,1);
  m_Gui->String(ID_USER,_("user"),&m_User);
  m_Gui->String(ID_PWD,_("pwd"),&m_Pwd,"",false,true);
  m_Gui->Divider(2);
  m_Gui->String(ID_PATH,_("path"),&m_RemotePath);
  m_Gui->Button(ID_BROWSE_REMOTE_FILE,_("browse"),_("remote files"));
  m_Gui->Button(ID_BROWSE_LOCAL_FILE, _("browse"),_("local files"));
  m_Gui->OkCancel();

	m_Gui->Divider();
  
  m_GuiList = new mafGUI(this);
  m_GuiList->Show(true);
  m_RemoteFilesList = m_GuiList->ListBox(ID_LIST_FILES,"",155);

	m_GuiList->Divider();

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(m_Gui,0, wxEXPAND);
  sizer->Add(m_GuiList,1, wxEXPAND);

  m_Gui->Reparent(this);
  m_GuiList->Reparent(this);
  
  Add(sizer,1,wxEXPAND);
  SetMinSize(wxSize(200,100));
  SetSize(wxSize(200,100));
  sizer->Fit(this);

  EnableWidgets();
}
//----------------------------------------------------------------------------
void mafGUIDialogRemoteFile::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_HOST:
      case ID_PATH:
        EnableWidgets();
      break;
      case ID_PORT:
      case ID_USER:
      case ID_PWD:
      break;
      case ID_BROWSE_REMOTE_FILE:
        RemoteFileBrowsing();
      break;
      case ID_BROWSE_LOCAL_FILE:
      {
        wxString lastFolder = mafGetLastUserFolder().c_str();
        mafString local_file = "";
        mafString wildc = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
        local_file = mafGetOpenFile(lastFolder,wildc.GetCStr(),_("Open local file"),this).c_str();
        if (!local_file.IsEmpty())
        {
          m_RemoteFilename = local_file;
          e->SetId(wxOK);
          mafGUIDialog::OnEvent(e);
        }
      }
      break;
      case ID_LIST_FILES:
      {
        int sel = m_RemoteFilesList->GetSelection();
        m_RemotePath = m_RemoteFilesList->GetString(sel);
        m_Gui->Update();
      }
      break;
      case wxOK:
        if (m_RemoteFilename.IsEmpty())
        {
          e->SetId(wxCANCEL);
        }
        m_RemoteFilename = m_Host;
        m_RemoteFilename += m_RemotePath;
        mafGUIDialog::OnEvent(e);
      break;
      case wxCANCEL:
        m_RemoteFilename = "";
        mafGUIDialog::OnEvent(e);
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIDialogRemoteFile::RemoteFileBrowsing()
//----------------------------------------------------------------------------
{
  struct msfTreeSearchReult chunk;
  chunk.memory=NULL; // we expect realloc(NULL, size) to work 
  chunk.size = 0;    // no data at this point 

  mafString auth = m_User;
  auth += ":";
  auth += m_Pwd;
  curl_easy_reset(m_Curl);
  mafString mafTree = m_Host; 
  mafTree += m_RemotePath + "msfTree";
  curl_easy_setopt(m_Curl, CURLOPT_URL, mafTree.GetCStr());
  curl_easy_setopt(m_Curl, CURLOPT_PORT, m_Port);
  curl_easy_setopt(m_Curl, CURLOPT_USERPWD, auth.GetCStr());
  curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, &chunk);

  m_Result = curl_easy_perform(m_Curl);

  if (m_Result == CURLE_OK)
  {
    wxString port;
    port = "";
    port << m_Port;
    wxString msf_list = chunk.memory;
    wxStringTokenizer tkz(msf_list, "\n");
    wxString remoteMsf = "";
    while (tkz.HasMoreTokens())
    {
      remoteMsf = tkz.GetNextToken();
      if (remoteMsf.Find(m_Host.GetCStr()) != -1)
      {
        remoteMsf = remoteMsf.SubString(m_Host.Length(),remoteMsf.Length());
        if (remoteMsf.Find(':') == 0)
        {
          remoteMsf = remoteMsf.SubString(port.Length()+1,remoteMsf.Length());
        }
      }
      m_RemoteFilesList->Insert(remoteMsf,m_RemoteFilesList->GetCount());
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIDialogRemoteFile::EnableWidgets()
//----------------------------------------------------------------------------
{
  mafString protocol;
  bool enable = IsRemote(m_RemoteFilename, protocol);
  m_Gui->Enable(ID_PORT,enable);
  m_Gui->Enable(ID_USER,enable);
  m_Gui->Enable(ID_PWD,enable);
  m_Gui->Enable(ID_BROWSE_REMOTE_FILE,enable /*&& protocol.Equals("ftp")*/);
}
