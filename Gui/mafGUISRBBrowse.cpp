/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUISRBBrowse.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-13 13:59:30 $
  Version:   $Revision: 1.8 $
  Authors:   Roberto Mucci
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

#include "mafGUISRBBrowse.h"
#include <wx/tokenzr.h>
#include <wx/string.h>

#include "mafDecl.h"
#include "mmgGui.h"
#include "mafGUISettingsStorage.h"
#include "mmgApplicationSettings.h"
#include "mafPics.h"
#include "mmgTree.h"

#include "SrbStorageWS/soapH.h"
#include "SrbStorageWS/SrbList.nsmap"

#include <list>
#include <stdio.h>

//----------------------------------------------------------------------------
mafGUISRBBrowse::mafGUISRBBrowse(mafObserver *listener,const wxString &title, long style, long dialogStyle)
: mmgDialog(title, style)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  m_DialogStyle = dialogStyle;
  m_Wild = "";
  mafString msfDir = mafGetApplicationDirectory().c_str();
  msfDir.ParsePathName();

  CreateGui();
}
//----------------------------------------------------------------------------
mafGUISRBBrowse::~mafGUISRBBrowse() 
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
//constants:
//----------------------------------------------------------------------------
enum REMOTE_FILE_WIDGET_ID
{
  ID_HOST = MINID,
  ID_USER,
  ID_PWD,
  ID_LIST_FILE,
  ID_SEARCH,

};
//----------------------------------------------------------------------------
void mafGUISRBBrowse::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->Show(true);

  mafGUISettingsStorage *storage_settings = new mafGUISettingsStorage(this);
  m_Host = storage_settings->GetSRBRemoteHostName();
  m_Domain = storage_settings->GetSRBDomain();
  m_AuthSheme = storage_settings->GetetSRBAuth_scheme();
  m_ServerDn = storage_settings->GetSRBServer_dn();
  m_CacheFolder = storage_settings->GetSRBCacheFolder();
  m_Port = storage_settings->GetSRBRemotePort();
  m_User = storage_settings->GetSRBUserName();
  m_Pwd  = storage_settings->GetSRBPwd();
  cppDEL(storage_settings);
  
  m_GuiList = new mmgGui(this);
  m_GuiList->Show(true);

  m_Tree = new mmgTree(m_GuiList,-1,false,true);
  m_Tree->SetAutoSort(FALSE);
  m_Tree->SetSize(290,470);
  m_Tree->SetListener(this);

#include <pic/FOLDER.xpm>
  mafPics.Add("FOLDER", FOLDER_xpm);
  wxImageList *tree_images = new wxImageList(20,20,TRUE,2);
  tree_images->Add(mafPics.GetBmp("FOLDER"));

#include <pic/OP_COPY.xpm>
  mafPics.Add("FILE", OP_COPY_xpm);
  tree_images->Add(mafPics.GetBmp("FILE"));
  m_Tree->SetImageList(tree_images);

  m_Gui->String(ID_HOST,"URL",&m_Host,_("host name"));
  m_Gui->String(ID_USER,_("user"),&m_User);
  m_Gui->String(ID_PWD,_("pwd"),&m_Pwd,"",false,true);
  m_Gui->String(ID_SEARCH, _("search :"),&m_Wild);

  m_Gui->Divider(2);

  m_Gui->Button(ID_LIST_FILE, _("List"),_("SRB files"));
  m_Gui->OkCancel();
  m_Gui->Enable(wxOK, FALSE);

  m_Gui->Divider();
  
  m_GuiList->Divider();

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(m_Gui,0, wxEXPAND);
  sizer->Add(m_GuiList,1, wxEXPAND);

  m_Gui->Reparent(this);
  m_GuiList->Reparent(this);
  
  Add(sizer,1,wxEXPAND);
  SetMinSize(wxSize(500,500));
  SetSize(wxSize(500,500));
  sizer->Fit(this);
}
//----------------------------------------------------------------------------
void mafGUISRBBrowse::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_LIST_FILE:
        m_Tree->Reset();
        if (!m_Host.IsEmpty() && !m_Pwd.IsEmpty() && !m_User.IsEmpty())
        {    
          if (RemoteSRBList() == MAF_OK)
          {
            CreateTree();
            if (m_DialogStyle != 0)
            {
              ShowOnlyFolders();
            }
          }
        }
        m_Gui->Update();
      break;
      /*
      case ID_BROWSE_LOCAL_FILE:
      {
        mafString local_file = "";
        mafString wildc = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
        local_file = mafGetOpenFile("",wildc.GetCStr(),_("Open local file"),this).c_str();
        if (!local_file.IsEmpty())
        {
        //  m_FileName = local_file;
          e->SetId(wxOK);
          mmgDialog::OnEvent(e);
        }
      }
      break;
      */
      case VME_SELECT:
      {
        m_NodeId = e->GetArg();
        if (m_DialogStyle != 0)
        {
          m_Gui->Enable(wxOK, TRUE);
        }
        else
        {
          m_Gui->Enable(wxOK, !m_Tree->NodeHasChildren(m_NodeId));
        }
        m_Gui->Update();
      }
      break;
      case wxOK:
        GetNodeName();
        e->SetId(wxOK);
        mmgDialog::OnEvent(e);
      break;
      case wxCANCEL:
        m_RemoteFileName = "";
        e->SetId(wxCANCEL);
        mmgDialog::OnEvent(e);
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}

//----------------------------------------------------------------------------
int mafGUISRBBrowse::RemoteSRBList()
//----------------------------------------------------------------------------
{
  struct soap soap; // gSOAP runtime environment

  //int portInt = m_Port;
  //char portStr[15];//change!
  //itoa(portInt, portStr, 10);
  mafString portStr = "";
  portStr << m_Port;

  _ns1__SrbList srb_listParams;
  
  ns1__SrbAuth srb_auth;
  srb_listParams.srbAuth = &srb_auth;
  srb_listParams.srbAuth->host = m_Host.GetNonConstCStr();
  srb_listParams.srbAuth->domain = m_Domain.GetNonConstCStr();
  srb_listParams.srbAuth->auth_scheme = m_AuthSheme.GetNonConstCStr();
  srb_listParams.srbAuth->port = portStr.GetNonConstCStr();
  srb_listParams.srbAuth->pwd = m_Pwd.GetNonConstCStr(); 
  srb_listParams.srbAuth->user = m_User.GetNonConstCStr();
  srb_listParams.srbAuth->server_dn = m_ServerDn.GetNonConstCStr();

  _ns1__SrbListResponse lsSRBResult;


  soap_init(&soap); // initialize runtime environment (only once)
  if (soap_call___ns6__SrbList(&soap, "http://ws-lhdl.cineca.it:12000/mafSRBList.cgi", NULL, &srb_listParams, &lsSRBResult) != SOAP_OK)
  {
    mafString errorCode = lsSRBResult.soap->fault->faultstring; //->fault->SOAP_ENV__Reason->SOAP_ENV__Text;
    mafMessage(errorCode.GetCStr());
    soap_print_fault(&soap, stderr); // display the SOAP fault message on the stderr stream
    return MAF_ERROR;
  }
  m_FileList = lsSRBResult.filelist;
   
  soap_destroy(&soap); // delete deserialized class instances (for C++ only)
  soap_end(&soap); // remove deserialized data and clean up
  soap_done(&soap); // detach the gSOAP environment
   
  return MAF_OK;
  
}

//----------------------------------------------------------------------------
void mafGUISRBBrowse::CreateTree()
//----------------------------------------------------------------------------
{
  wxStringTokenizer tkz(m_FileList, "  ");

  mafString domain = m_Domain;
  domain.Append("/");

  //Create the tree
  std::list <wxString> listName;
  wxString nameComplete, root;

  nameComplete = tkz.GetNextToken();

  int pos = nameComplete.Find(domain);
  pos = pos + domain.Length();

  root = nameComplete.Mid(0, pos);
  m_Tree->AddNode(1, 0, root);
  
  nameComplete = nameComplete.Mid(pos);
  listName.push_back(nameComplete);

  int posFolder, posFolderOK;
  m_NodeCounter = 2;
  long root_id;

  while (tkz.HasMoreTokens())
  {
    wxString name;
    name = tkz.GetNextToken();
    
    int pos = name.Find(domain);
    pos = pos + domain.Length();
    name = name.Mid(pos);

    listName.push_back(name);
  }

  std::list <wxString>::iterator list_Iter;

  std::vector <int> nodeID;
  nodeID.resize(listName.size());

  std::vector <wxString> folder;
  folder.resize(listName.size());

  std::vector <wxString> folderOld;
  folderOld.resize(listName.size());

  listName.sort();

  int i, p;
  bool newFolder;
  
  for (list_Iter = listName.begin(); list_Iter != listName.end(); list_Iter++)
  {
    wxString fileName = *list_Iter;
    if (m_Wild.IsEmpty() || fileName.Contains(m_Wild.GetCStr())==1)
    {
      root_id = 1;
      i = 0;
      p = 0;

      posFolder = fileName.Find("/");
      if (posFolder == -1)
      {
        posFolderOK = 0;
      }
      else
      {
        posFolderOK = posFolder;
      }
      while(posFolder != -1) 
      {
        i++;
        folder[p] = fileName.Mid(0, (posFolder + 1));
        fileName = fileName.Mid(posFolder + 1); 
        if (folder[p] != folderOld[p])
        {
          if (p == 0)
          {
            m_Tree->AddNode(m_NodeCounter, root_id, folder[p]);
            nodeID[i] = m_NodeCounter;
          }
          else
          {
            if (newFolder == TRUE)
            {
              m_Tree->AddNode(m_NodeCounter, (m_NodeCounter - 1), folder[p]);
              nodeID[i] = m_NodeCounter; 
            }
            else
            {
              m_Tree->AddNode(m_NodeCounter, nodeID[i-1], folder[p]);
              nodeID[i] = m_NodeCounter;
            }
          }
          m_NodeCounter++;
          newFolder = TRUE;
        }
        else
        {
          newFolder = FALSE;
        }
        folderOld[p] = folder[p];
        posFolder = fileName.Find("/");
        if (posFolder != -1)
        {
          posFolderOK = posFolder;
        }      
        p++;
      }
      if (p==0)
      {
        m_Tree->AddNode(m_NodeCounter, root_id, fileName);
      }
      else
      {
        if (newFolder == TRUE)
        {
          m_Tree->AddNode(m_NodeCounter, (m_NodeCounter - 1), fileName);
        }
        else
        {
          m_Tree->AddNode(m_NodeCounter, nodeID[i], fileName);
          int node = nodeID[i];
        }
      }
      m_NodeCounter++;
    }
    
  }

  //Set the right icon for folders and files
  for (long i = 0; i < m_NodeCounter; i++)
  {
    if (!m_Tree->NodeHasChildren(i))
    {
      m_Tree->SetNodeIcon(i,1);
    }
  }
  m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafGUISRBBrowse::ShowOnlyFolders()
//----------------------------------------------------------------------------
{
  for (long i = 0; i < m_NodeCounter; i++)
  {
    if (!m_Tree->NodeHasChildren(i))
    {
      m_Tree->DeleteNode(i);

    }
  }
}

//----------------------------------------------------------------------------
mafString mafGUISRBBrowse::GetNodeName()
//----------------------------------------------------------------------------
{
  mafString partialName;

  std::vector <long> vectorId;
  vectorId.push_back(m_NodeId);
  int i = 0;
  long id = -1;

  //Fill the vector with the ID of the nodes parents
  while (id != 1 && id != 0)
  {
    i++;
    id = m_Tree->GetNodeParent(vectorId[i-1]);
    if (id != 0)
    {
      vectorId.push_back(id);
    }    
  }

  //Create the node name with the full path
  for (int n = vectorId.size()-1; n >= 0 ; n--)
  {
    partialName = m_Tree->GetNodeLabel(vectorId[n]);
    m_RemoteFileName.Append(partialName);
  }
  return m_RemoteFileName; 
}
