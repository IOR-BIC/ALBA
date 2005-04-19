/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-19 08:25:21 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
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


#ifdef _MSC_FULL_VER
#include <xiosbase>
#endif

#include <wx/busyinfo.h>
#include <wx/fs_zip.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include "mafDecl.h"
//#include "mafVmeData.h"
#include "mafVMEManager.h"
#include "mafNode.h"
#include "mafVMEStorage.h"
#include "mafNodeIterator.h"
#include "mafTagArray.h"

//----------------------------------------------------------------------------
mafVMEManager::mafVMEManager()
//----------------------------------------------------------------------------
{
  m_modified  = false;
	m_make_bak_file = true;
	m_Listener  = NULL;
	m_Storage   = NULL;

	m_msf_dir   = mafGetApplicationDirectory().c_str();
  m_msf_dir   += "/Data/MSF/";
	m_msffile   = "";
	m_zipfile   = "";
	m_mergefile = "";
	m_wildc     = "Multimod Storage Format file (*.msf)|*.msf";

  m_Config = wxConfigBase::Get();
}
//----------------------------------------------------------------------------
mafVMEManager::~mafVMEManager( ) 
//----------------------------------------------------------------------------
{
  if(m_Storage) NotifyRemove( m_Storage->GetRoot() ); // //SIL. 11-4-2005:  - cast root to node -- maybe to be removed

  if(m_Storage)	m_Storage->Delete();
  delete m_Config;  
}
//----------------------------------------------------------------------------
void mafVMEManager::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
   e.Log();
}

//----------------------------------------------------------------------------
mafVMERoot *mafVMEManager::GetRoot()
//----------------------------------------------------------------------------
{
  return (m_Storage?m_Storage->GetRoot():NULL);
}

//----------------------------------------------------------------------------
void mafVMEManager::CreateNewStorage()
//----------------------------------------------------------------------------
{
  if(m_Storage) m_Storage->Delete(); 
  m_Storage = mafVMEStorage::New();
  m_Storage->GetRoot()->SetName("root");
}

//----------------------------------------------------------------------------
void mafVMEManager::MSFNew(bool notify_root_creation)   
//----------------------------------------------------------------------------
{
  m_modified = false;
  
  if (m_Storage) NotifyRemove(m_Storage->GetRoot());
  CreateNewStorage();

  if(notify_root_creation)
	{
		//Add the application stamps
		mafTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.c_str());
		m_Storage->GetRoot()->GetTagArray()->SetTag(tag_appstamp);
		mafEventMacro(mafEvent(this,VME_ADDED,m_Storage->GetRoot()));
		mafEventMacro(mafEvent(this,VME_SELECTED,m_Storage->GetRoot())); 
	}

  m_msffile = ""; //next MSFSave will ask for a filename
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(int file_id)   
//----------------------------------------------------------------------------
{
  int idx = file_id - wxID_FILE1;
	wxString file = "";
	file = m_FileHistory.GetHistoryFile(idx);
	MSFOpen(file);
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(wxString filename)   
//----------------------------------------------------------------------------
{
	
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
  m_Storage->SetURL(m_msffile.c_str());
 
  int res = m_Storage->Restore();  //modified by Stefano 29-10-2004

  mafTimeStamp b[2];
  m_Storage->GetRoot()->GetOutput()->GetTimeBounds(b);
  m_Storage->GetRoot()->SetTreeTime(b[0]);
  //m_Storage->GetRoot()->SetName("root"); ///?????

	////////////////////////////////  Application Stamp managing ////////////////////
	if(!m_Storage->GetRoot()->GetTagArray()->IsTagPresent("APP_STAMP"))
	{
		//update the old data files to support Application Stamp
		mafTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.c_str());
		m_Storage->GetRoot()->GetTagArray()->SetTag(tag_appstamp);
	}
	
	wxString app_stamp(m_Storage->GetRoot()->GetTagArray()->GetTag("APP_STAMP")->GetValue());
	if(app_stamp == "INVALID" || ((app_stamp != m_AppStamp) && (m_AppStamp != "DataManager") && (m_AppStamp != "OPEN_ALL_DATA")))
	{
		//Application stamp not valid
		wxMessageBox("File not valid for this application!","Warning", wxOK, NULL);
		m_modified = false;
		m_Storage->Delete();
		m_Storage = NULL;
		MSFNew();
		return;
	}
	///////////////////////////////////////////////////////////////////////////////// 

  NotifyAdd(m_Storage->GetRoot());

  // modified by Stefano 29-10-2004 (beg)
  // if some problems occurred during import give feedback to the user
  if (res == 1)
  {
    wxString tmpStr("Errors during file parsing! Look the log area for error messages.");
    wxMessageBox(tmpStr, "Error", wxOK | wxICON_ERROR);
  }
  //modified by Stefano 29-10-2004 (end)


	mafEventMacro(mafEvent(this,VME_SELECTED,m_Storage->GetRoot())); 
  mafEventMacro(mafEvent(this,CAMERA_RESET)); 

	m_FileHistory.AddFileToHistory(m_msffile);
	m_FileHistory.Save(*m_Config);
}
//----------------------------------------------------------------------------
void mafVMEManager::ZIPOpen(wxString filename)
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
void mafVMEManager::MSFSave()   
//----------------------------------------------------------------------------
{
  if(m_msffile == "") 
  {
    std::string file = mafGetSaveFile(m_msf_dir, m_wildc);
    if(file == "") return;
		if(!wxFileExists(file.c_str()))
		{
			wxString path, name, ext, file_dir;
			wxSplitPath(file.c_str(),&path,&name,&ext);
			file_dir = path + "\\" + name;
			if(!wxDirExists(file_dir))
				wxMkdir(file_dir);
			file = file_dir + "\\" + name + "." + ext;
		}
    m_msffile = file.c_str();
  }
  if(wxFileExists(m_msffile) && m_make_bak_file)
	{
		wxString bak_filename = m_msffile + ".bak";
    wxRenameFile(m_msffile,bak_filename);
		//ifstream old_msf_file(m_msffile.c_str(), ios::in|ios::binary);
		//ofstream bak_msf_file(bak_filename.c_str(),ios::out|ios::binary);
		//bak_msf_file << old_msf_file.rdbuf();
		//old_msf_file.close();
		//bak_msf_file.close();
	}
	
	wxBusyInfo wait("Saving MSF: Please wait");
  m_Storage->SetURL(m_msffile.c_str());
  if (m_Storage->Store()!=0)
  {
    wxLogMessage("Error during MSF saving");
  }
	m_FileHistory.AddFileToHistory(m_msffile );
	m_FileHistory.Save(*m_Config);
  m_modified = false;
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFSaveAs()   
//----------------------------------------------------------------------------
{
   m_msffile = "";
   MSFSave();
}
//----------------------------------------------------------------------------
void mafVMEManager::VmeAdd(mafNode *v)
//----------------------------------------------------------------------------
{
  if(v != NULL)
  {
    mafNode *vp = v->GetParent();  
    assert( vp == NULL || m_Storage->GetRoot()->IsInTree(vp) );
    if(vp == NULL) 
			v->ReparentTo(m_Storage->GetRoot());
     //v->SetTreeTime(m_Storage->GetRoot()->GetTime()); //spostare in mfl
    NotifyAdd(v);
    m_modified = true;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::VmeRemove(mafNode *v)
//----------------------------------------------------------------------------
{
  if(v != NULL && m_Storage->GetRoot() /*&& m_Storage->GetRoot()->IsInTree(v)*/) 
  {
    NotifyRemove(v);
    v->ReparentTo(NULL); // may kill the vme
    m_modified = true;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::TimeSet(float time)
//----------------------------------------------------------------------------
{
  if(m_Storage->GetRoot()) m_Storage->GetRoot()->SetTreeTime(time);
}
//----------------------------------------------------------------------------
void mafVMEManager::TimeGetBounds(float *min, float *max)
//----------------------------------------------------------------------------
{
  
  mafTimeStamp b[2];
  if(m_Storage->GetRoot()) 
  {
    m_Storage->GetRoot()->GetOutput()->GetTimeBounds(b);
    *min = b[0];
    *max = b[1];
  }
  else
  {
    *min = 0;
    *max = 0;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::NotifyRemove(mafNode *v)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter=v->NewIterator();
  iter->SetTraversalModeToPostOrder();
  for (mafNode *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
		mafEventMacro(mafEvent(this,VME_REMOVING,vme));
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafVMEManager::NotifyAdd(mafNode *v)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter=v->NewIterator();
  for (mafNode *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
    mafEventMacro(mafEvent(this,VME_ADDED,vme));
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafVMEManager::SetFileHistoryMenu(wxMenu *menu)
//----------------------------------------------------------------------------
{
	m_FileHistory.UseMenu(menu);
	m_FileHistory.Load(*m_Config);
}
//----------------------------------------------------------------------------
bool mafVMEManager::AskConfirmAndSave()
//----------------------------------------------------------------------------
{
  bool go = true;
	if (m_modified)
	{
		int answer = wxMessageBox("your work is modified, would you like to save it?","Confirm",
			                     wxYES_NO|wxCANCEL|wxICON_QUESTION , mafGetFrame());
		if(answer == wxCANCEL) go = false;
		if(answer == wxYES)    MSFSave();
	}
	return go;
}
//----------------------------------------------------------------------------
void mafVMEManager::UpdateFromTag(mafNode *vme)
//----------------------------------------------------------------------------
{
  /*
  if (vme)
  {
    mafVmeData *vd = (mafVmeData *)vme->GetClientData();
    if (vd)
      vd->UpdateFromTag();
  }
  else
  {
    mafNodeIterator *iter = m_Storage->GetRoot()->NewIterator();
    for (mafNode *v = iter->GetFirstNode(); v; v = iter->GetNextNode())
    {
      mafVmeData *vd = (mafVmeData *)v->GetClientData();
      if (vd)
        vd->UpdateFromTag();
    }
    iter->Delete();
  }
  */
}
