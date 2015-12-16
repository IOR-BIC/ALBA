/*=========================================================================

 Program: MAF2
 Module: mafNodeManager
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


#ifdef _MSC_FULL_VER
#include <xiosbase>
#endif

#include <wx/busyinfo.h>
#include <wx/fs_zip.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include "mafDecl.h"
//#include "mafVmeData.h"
#include "mafNodeManager.h"
#include "mafNode.h"
//#include "mafNodeStorage.h"
#include "mafNodeIterator.h"
//#include "vtkTagItem.h"

//----------------------------------------------------------------------------
mafNodeManager::mafNodeManager()
//----------------------------------------------------------------------------
{
  m_Modified      = false;
	m_MakeBakFile = true;
	m_Listener      = NULL;
//@@	m_storage   = NULL;
	m_Root          = NULL;

  m_MsfDir   = mafGetLastUserFolder().c_str();
	m_MsfFile   = "";
	m_ZipFile   = "";
	m_MergeFile = "";
	m_Wildc     = "Multimod Storage Format file (*.msf)|*.msf";

  m_Config = wxConfigBase::Get();
}
//----------------------------------------------------------------------------
mafNodeManager::~mafNodeManager()
//----------------------------------------------------------------------------
{
  if(m_Root) NotifyRemove( (mafNode*)m_Root ); // //SIL. 11-4-2005:  - cast root to node -- maybe to be removed
  mafDEL(m_Root);

  m_AppStamp.clear();

  //@@ if(m_storage)	m_storage->Delete();
  delete m_Config;  
}
//----------------------------------------------------------------------------
void mafNodeManager::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
   //maf_event->Log();
}
//----------------------------------------------------------------------------
void mafNodeManager::MSFNew(bool notify_root_creation)   
//----------------------------------------------------------------------------
{
  m_Modified = false;
  if(m_Root) NotifyRemove( (mafNode*) m_Root);  //SIL. 11-4-2005: cast root to node - maybe to be removed
  mafDEL(m_Root);
  mafNEW(m_Root);
  m_Root->SetName("root");

  //@@ if(m_storage) m_storage->Delete(); 
  //@@ m_storage = mafNodeStorage::New();
  //@@ m_Root = m_storage->GetOutput();
  //@@ m_Root->SetName("root");

  if(notify_root_creation)
	{
		//Add the application stamps
		//vtkTagItem tag_appstamp;
		//tag_appstamp.SetName("APP_STAMP");
		//tag_appstamp.SetValue(this->m_AppStamp.c_str());
		//m_Root->GetTagArray()->AddTag(tag_appstamp);
		mafEventMacro(mafEvent(this,VME_ADDED,m_Root));
		mafEventMacro(mafEvent(this,VME_SELECTED,m_Root)); 
	}

  m_MsfFile = ""; //next MSFSave will ask for a filename
}
//----------------------------------------------------------------------------
void mafNodeManager::MSFOpen(int file_id)   
//----------------------------------------------------------------------------
{
  /*
  int idx = file_id - wxID_FILE1;
	wxString file = "";
	file = m_FileHistory.GetHistoryFile(idx);
	MSFOpen(file);
  */
}
//----------------------------------------------------------------------------
void mafNodeManager::MSFOpen(wxString filename)   
//----------------------------------------------------------------------------
{
	/*
  wxWindowDisabler disableAll; //SIL. 17-9-2004: 

  wxBusyCursor wait_cursor;
  
  if(!::wxFileExists(filename))
	{
		wxString msg("File ");
		msg += filename;
		msg += " not found!";
		wxMessageBox( msg,"Warning", wxOK|wxICON_WARNING , NULL);
		return;
	}

  // insert and select the root - reset m_msffile 
  MSFNew(false);

  wxString path, name, ext;
  wxSplitPath(filename,&path,&name,&ext);
  if(ext == "zip")
  {
    ZIPOpen(filename);
    return;
  }
  
  m_msffile = filename; 

  wxBusyInfo wait("Loading MSF: Please wait");
  m_storage->SetFileName(m_msffile.c_str());
 
  int res = m_storage->Load();  //modified by Stefano 29-10-2004

  mflTimeStamp b[2];
  m_Root->GetTimeBounds(b);
  m_Root->SetTreeTime(b[0]);
  m_Root->SetName("root");

	////////////////////////////////  Application Stamp managing ////////////////////
	if(m_Root->GetTagArray()->FindTag("APP_STAMP") == -1)
	{
		//update the old data files to support Application Stamp
		vtkTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.c_str());
		m_Root->GetTagArray()->AddTag(tag_appstamp);
	}
	
	wxString app_stamp(m_Root->GetTagArray()->GetTag("APP_STAMP")->GetValue());
	if(app_stamp == "INVALID" || ((app_stamp != m_AppStamp) && (m_AppStamp != "DataManager") && (m_AppStamp != "OPEN_ALL_DATA")))
	{
		//Application stamp not valid
		wxMessageBox("File not valid for this application!","Warning", wxOK, NULL);
		m_modified = false;
		m_storage->Delete();
		m_Root = NULL;
		m_storage = NULL;
		MSFNew();
		return;
	}
	///////////////////////////////////////////////////////////////////////////////// 

  NotifyAdd(m_Root);

  // modified by Stefano 29-10-2004 (beg)
  // if some problems occurred during import give feedback to the user
  if (res == 1)
  {
    wxString tmpStr("Errors during file parsing! Look the log area for error messages.");
    wxMessageBox(tmpStr, "Error", wxOK | wxICON_ERROR);
  }
  //modified by Stefano 29-10-2004 (end)


	mafEventMacro(mafEvent(this,VME_SELECTED,m_Root)); 
  mafEventMacro(mafEvent(this,CAMERA_RESET)); 

	m_FileHistory.AddFileToHistory(m_msffile);
	m_FileHistory.Save(*m_Config);
  */
}
//----------------------------------------------------------------------------
void mafNodeManager::ZIPOpen(wxString filename)
//----------------------------------------------------------------------------
{
  /*
  wxBusyInfo wait("Unzipping file: Please wait");

  m_zipfile = filename;
  wxString file, ext, working_dir = "";
  wxSplitPath(m_zipfile,&working_dir,&file,&ext);
  if (working_dir == "")
    working_dir = ::wxGetCwd();
  
  working_dir = working_dir + "\\~TmpData";
  if (!wxDirExists(working_dir))
    wxMkdir(working_dir);
  wxSetWorkingDirectory(working_dir);

  wxString path, name, complete_name, zfile, out_file;
  wxSplitPath(m_zipfile,&path,&name,&ext);
  complete_name = name + "." + ext;
  
  wxFSFile *zfileStream;
  wxZlibInputStream *zip_is;
  wxString pkg = "#zip:";
  wxString header_name = complete_name + pkg;
  int length_header_name = header_name.Length();
  bool enable_mid = false;
  wxFileSystem *zip_fs = new wxFileSystem();
  zip_fs->AddHandler(new wxZipFSHandler);
  zip_fs->ChangePathTo(m_zipfile);
  // extract filename from the zip archive
  zfile = zip_fs->FindFirst(complete_name+pkg+name+"\\*.*");
  if (zfile == "")
  {
    enable_mid = true;
    zfile = zip_fs->FindFirst(complete_name+pkg+"\\*.*");
  }
  wxSplitPath(zfile,&path,&name,&ext);
  complete_name = name + "." + ext;
  if (enable_mid)
    complete_name = complete_name.Mid(length_header_name);
  zfileStream = zip_fs->OpenFile(zfile);
  zip_is = (wxZlibInputStream *)zfileStream->GetStream();
  out_file = working_dir + "\\" + complete_name;
  char *buf;
  int s_size;
#ifdef _MSC_FULL_VER
  ofstream out_file_stream;
#else
  std::ofstream out_file_stream;
#endif
  if(ext == "msf")
  {
    m_msffile = out_file;
    out_file_stream.open(out_file, std::ios_base::out);
  }
  else
    out_file_stream.open(out_file, std::ios_base::binary);
  s_size = zip_is->StreamSize();
  buf = new char[s_size];
  zip_is->Read(buf,s_size);
  out_file_stream.write(buf, s_size);
  out_file_stream.close();
  delete[] buf;
  
  while ((zfile = zip_fs->FindNext()) != "")
  {
    zfileStream = zip_fs->OpenFile(zfile);
    zip_is = (wxZlibInputStream *)zfileStream->GetStream();
    wxSplitPath(zfile,&path,&name,&ext);
    complete_name = name + "." + ext;
    if (enable_mid)
      complete_name = complete_name.Mid(length_header_name);
    out_file = working_dir + "\\" + complete_name;
    if(ext == "msf")
    {
      m_msffile = out_file;
      out_file_stream.open(out_file, std::ios_base::out);
    }
    else
      out_file_stream.open(out_file, std::ios_base::binary);
    s_size = zip_is->StreamSize();
    buf = new char[s_size];
    zip_is->Read(buf,s_size);
    out_file_stream.write(buf, s_size);
    out_file_stream.close();
    delete[] buf;
  }
  
  zfileStream->UnRef();
  delete zfileStream;
  zip_fs->CleanUpHandlers();
  delete zip_fs;
  
  if (m_msffile == "")
  {
    wxMessageBox("zip archive is not a valid msf file!", "Error");
    return;
  }

  wxSetWorkingDirectory(working_dir);
  MSFOpen(m_msffile);
  */
}
//----------------------------------------------------------------------------
void mafNodeManager::MSFSave()   
//----------------------------------------------------------------------------
{
  /*
  if(m_msffile == "") 
  {
    wxString file = mafGetSaveFile(m_msf_dir, m_wildc);
    if(file == "") return;
		if(!wxFileExists(file))
		{
			wxString path, name, ext, file_dir;
			wxSplitPath(file,&path,&name,&ext);
			file_dir = path + "\\" + name;
			if(!wxDirExists(file_dir))
				wxMkdir(file_dir);
			file = file_dir + "\\" + name + "." + ext;
		}
    m_msffile = file;
  }
  if(wxFileExists(m_msffile) && m_make_bak_file)
	{
		wxString bak_filename = m_msffile + ".bak";
		ifstream old_msf_file(m_msffile.c_str(), ios::in|ios::binary);
		ofstream bak_msf_file(bak_filename.c_str(),ios::out|ios::binary);
		bak_msf_file << old_msf_file.rdbuf();
		old_msf_file.close();
		bak_msf_file.close();
	}
	
	wxBusyInfo wait("Saving MSF: Please wait");
  m_storage->SetFileName(m_msffile.c_str());
  if (m_storage->Write()!=0)
  {
    mafLogMessage("Error during MSF saving");
  }
	m_FileHistory.AddFileToHistory(m_msffile );
	m_FileHistory.Save(*m_Config);
  m_modified = false;
  */
}
//----------------------------------------------------------------------------
void mafNodeManager::MSFSaveAs()   
//----------------------------------------------------------------------------
{
  /**
   m_msffile = "";
   MSFSave();
   */
}
//----------------------------------------------------------------------------
void mafNodeManager::VmeAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  if(n != NULL)
  {
    mafNode *vp = n->GetParent();  
    assert( vp == NULL || m_Root->IsInTree(vp) );
    if(vp == NULL) 
			n->ReparentTo(m_Root);
    NotifyAdd(n);
    m_Modified = true;
  }
}
//----------------------------------------------------------------------------
void mafNodeManager::VmeRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  if(n != NULL && m_Root /*&& m_Root->IsInTree(n)*/) 
  {
    NotifyRemove(n);
    n->ReparentTo(NULL); // may kill the vme
    m_Modified = true;
  }
}
//----------------------------------------------------------------------------
void mafNodeManager::TimeSet(double time)
//----------------------------------------------------------------------------
{
  //if(m_Root) m_Root->SetTreeTime(time);
}
//----------------------------------------------------------------------------
void mafNodeManager::TimeGetBounds(double *min, double *max)
//----------------------------------------------------------------------------
{
  /*
  mflTimeStamp b[2];
  if(m_Root) 
  {
    m_Root->GetTimeBounds(b);
    *min = b[0];
    *max = b[1];
  }
  else
  
  {*/
    *min = 0;
    *max = 0;
  //}
}
//----------------------------------------------------------------------------
void mafNodeManager::NotifyRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = n->NewIterator();
  iter->SetTraversalModeToPostOrder();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		mafEventMacro(mafEvent(this, VME_REMOVING, node));
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafNodeManager::NotifyAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = n->NewIterator();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    mafEventMacro(mafEvent(this,VME_ADDED,node));
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafNodeManager::SetFileHistoryMenu(wxMenu *menu)
//----------------------------------------------------------------------------
{
	m_FileHistory.UseMenu(menu);
	m_FileHistory.Load(*m_Config);
}
//----------------------------------------------------------------------------
bool mafNodeManager::AskConfirmAndSave()
//----------------------------------------------------------------------------
{
  bool go = true;
	if (m_Modified)
	{
		int answer = wxMessageBox("your work is modified, would you like to save it?","Confirm",
			                     wxYES_NO|wxCANCEL|wxICON_QUESTION , mafGetFrame());
		if(answer == wxCANCEL) go = false;
		if(answer == wxYES)    MSFSave();
	}
	return go;
}
//----------------------------------------------------------------------------
void mafNodeManager::SetApplicationStamp(wxString appstamp)
//----------------------------------------------------------------------------
{
  m_AppStamp.push_back(appstamp);
}
//----------------------------------------------------------------------------
void mafNodeManager::SetApplicationStamp(std::vector<wxString> appstamp)
//----------------------------------------------------------------------------
{
  for (int i=0; i<appstamp.size();i++)
  {
    m_AppStamp.push_back(appstamp.at(i));
  }
}
//----------------------------------------------------------------------------
void mafNodeManager::UpdateFromTag(mafNode *n)
//----------------------------------------------------------------------------
{
  /*
  if (n)
  {
    mafVmeData *vd = (mafVmeData *)n->GetClientData();
    if (vd)
      vd->UpdateFromTag();
  }
  else
  {
    mafNodeIterator *iter = m_Root->NewIterator();
    for (mafNode *n = iter->GetFirstNode(); n; n = iter->GetNextNode())
    {
      mafVmeData *vd = (mafVmeData *)n->GetClientData();
      if (vd)
        vd->UpdateFromTag();
    }
    iter->Delete();
  }
  */
}
