/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEManager.cpp,v $
  Language:  C++
  Date:      $Date: 2007-05-23 12:39:31 $
  Version:   $Revision: 1.33 $
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


#include <wx/busyinfo.h>
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>
#include <wx/dir.h>

#include "mafDecl.h"
#include "mafVMEManager.h"
#include "mafNode.h"
#include "mafVMEStorage.h"
#include "mafRemoteStorage.h"
#include "mafRemoteFileManager.h"
#include "mmgApplicationSettings.h"

#include "mafNodeIterator.h"
#include "mafTagArray.h"

#include <fstream>

//----------------------------------------------------------------------------
mafVMEManager::mafVMEManager()
//----------------------------------------------------------------------------
{
  m_Modified    = false;
	m_MakeBakFile = true;
	m_Listener    = NULL;
	m_Storage     = NULL;
  m_Crypting    = false;
  m_LoadingFlag = false;

  /*
  mmgApplicationSettings *app_settings = new mmgApplicationSettings(this);
  m_Host = app_settings->GetRemoteHostName();
  m_Port = app_settings->GetRemotePort();
  m_User = app_settings->GetUserName();
  m_Pwd  = app_settings->GetPassword();
  m_LocalCacheFolder = app_settings->GetCacheFolder();
  cppDEL(app_settings);
  */

  m_FileHistoryIdx = -1;

  mafString MSFDir = mafGetApplicationDirectory().c_str();
  MSFDir.ParsePathName();
	m_MSFDir   = MSFDir;
	m_MSFFile   = "";
	m_ZipFile   = "";
  m_TmpDir   = "";
	m_MergeFile = "";

  m_Config = wxConfigBase::Get();
}
//----------------------------------------------------------------------------
mafVMEManager::~mafVMEManager()
//----------------------------------------------------------------------------
{
  if(m_Storage) 
    NotifyRemove( m_Storage->GetRoot() ); // //SIL. 11-4-2005:  - cast root to node -- maybe to be removed

  m_Listener = NULL;
  mafDEL(m_Storage);
  cppDEL(m_Config);
}
//----------------------------------------------------------------------------
void mafVMEManager::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (maf_event->GetChannel()==MCH_UP)
  {
    // events coming from the tree...

    switch (maf_event->GetId())
    {
      case NODE_ATTACHED_TO_TREE:
      {
        if (!m_LoadingFlag)
        {
          NotifyAdd((mafNode *)maf_event->GetSender());
          MSFModified(true);
        }
      }
      break;
      case NODE_DETACHED_FROM_TREE:
      {
        if (!m_LoadingFlag)
        {
          NotifyRemove((mafNode *)maf_event->GetSender());
          MSFModified(true);
        }
      }
      break;
      default:
        mafEventMacro(*maf_event);
    }
  }
}

//----------------------------------------------------------------------------
mafVMERoot *mafVMEManager::GetRoot()
//----------------------------------------------------------------------------
{
  return (m_Storage?m_Storage->GetRoot():NULL);
}
//----------------------------------------------------------------------------
mafVMEStorage *mafVMEManager::GetStorage()
//----------------------------------------------------------------------------
{
  return m_Storage;
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFNew(bool notify_root_creation)
//----------------------------------------------------------------------------
{
  if (m_TmpDir != "")
  {
    wxString working_dir;
    working_dir = mafGetApplicationDirectory().c_str();
    wxSetWorkingDirectory(working_dir);
    //remove tmp directory due to zip extraction or compression
    if(::wxDirExists(m_TmpDir))
    {
      wxString file_match = m_TmpDir + "/*.*";
      wxString f = wxFindFirstFile(file_match);
      while ( !f.IsEmpty() )
      {
        ::wxRemoveFile(f);
        f = wxFindNextFile();
      }
      ::wxRmdir(m_TmpDir);
    }
    m_TmpDir = "";
  }

  m_Modified = false;
  
  m_LoadingFlag = true;
  mafEvent e(this,CREATE_STORAGE,m_Storage);
  mafEventMacro(e);
  m_Storage = (mafVMEStorage *)e.GetMafObject();
  m_LoadingFlag = false;

  if(notify_root_creation)
	{
		//Add the application stamps
		mafTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.GetCStr());
		m_Storage->GetRoot()->GetTagArray()->SetTag(tag_appstamp);
		mafEventMacro(mafEvent(this,VME_ADDED,m_Storage->GetRoot()));
		mafEventMacro(mafEvent(this,VME_SELECTED,m_Storage->GetRoot()));
	}

  m_MSFFile = ""; //next MSFSave will ask for a filename
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(int file_id)   
//----------------------------------------------------------------------------
{
  m_FileHistoryIdx = file_id - wxID_FILE1;
	wxString file = "";
	file = m_FileHistory.GetHistoryFile(m_FileHistoryIdx);
	MSFOpen(file);
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(wxString filename)
//----------------------------------------------------------------------------
{
  wxWindowDisabler disableAll;
  wxBusyCursor wait_cursor;
  
  mafString protocol = "";
  bool remote_file = IsRemote(filename.c_str(),protocol);
  
  if(!remote_file && !::wxFileExists(filename))
	{
		wxString msg("File ");
		msg += filename;
		msg += " not found!";
		mafWarningMessage( msg,"Warning");
    if(m_FileHistoryIdx != -1)
    {
      m_FileHistory.RemoveFileFromHistory(m_FileHistoryIdx);
      m_FileHistory.Save(*m_Config);
      m_FileHistoryIdx = -1;
    }
		return;
	}

  // insert and select the root - reset m_MSFFile - delete the old storage and create a new one
  MSFNew(false);
  
  mafString unixname = filename;

  if (remote_file)
  {
    // set parameters for remote storage according to the remote file.
    ((mafRemoteStorage *)m_Storage)->SetHostName(m_Host);
    ((mafRemoteStorage *)m_Storage)->SetRemotePort(m_Port);
    ((mafRemoteStorage *)m_Storage)->SetUsername(m_User);
    ((mafRemoteStorage *)m_Storage)->SetPassword(m_Pwd);
  }
  
  wxString path, name, ext;
  wxSplitPath(filename,&path,&name,&ext);
  if(ext == "zmsf")
  {
    if (remote_file)
    {
      // Download the file if it is not present into the cache
      // we are using the remote storage!!
      mafString local_filename, remote_filename;
      remote_filename = filename.c_str();
      local_filename = m_LocalCacheFolder;
      local_filename += "\\";
      local_filename += name;
      local_filename += ".zmsf";
      ((mafRemoteStorage *)m_Storage)->GetRemoteFileManager()->DownloadRemoteFile(remote_filename, local_filename);
      filename = local_filename;
    }
    unixname = ZIPOpen(filename);
    wxSetWorkingDirectory(m_TmpDir);
  }
  
  mafString sub_unixname;
  if (unixname.StartsWith("\\\\"))
  {
    sub_unixname = unixname;
    sub_unixname.Erase(0,1);
    sub_unixname.ParsePathName();
    unixname = "\\\\";
    unixname += sub_unixname;
  }
  else
    unixname.ParsePathName(); // convert to unix format

  m_MSFFile = unixname; 
  m_Storage->SetURL(m_MSFFile.c_str());
 
  m_LoadingFlag = true;
  int res = m_Storage->Restore();
  if (res != MAF_OK)
  {
    // if some problems occurred during import give feedback to the user
    mafErrorMessage("Errors during file parsing! Look the log area for error messages.");
  }
  m_LoadingFlag = false;

  mafTimeStamp b[2];
  m_Storage->GetRoot()->GetOutput()->GetTimeBounds(b);
  m_Storage->GetRoot()->SetTreeTime(b[0]);
  
	////////////////////////////////  Application Stamp managing ////////////////////
	if(!m_Storage->GetRoot()->GetTagArray()->IsTagPresent("APP_STAMP"))
	{
		//update the old data files to support Application Stamp
		mafTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.GetCStr());
		m_Storage->GetRoot()->GetTagArray()->SetTag(tag_appstamp);
	}
	
	mafString app_stamp;
  app_stamp << m_Storage->GetRoot()->GetTagArray()->GetTag("APP_STAMP")->GetValue();
	if(app_stamp.Equals("INVALID") || ((!app_stamp.Equals(m_AppStamp.GetCStr())) && (!m_AppStamp.Equals("DataManager")) && (!m_AppStamp.Equals("OPEN_ALL_DATA"))))
	{
		//Application stamp not valid
		mafMessage("File not valid for this application!","Warning");
		m_Modified = false;
		m_Storage->Delete();
		m_Storage = NULL;
		MSFNew();
		return;
	}
	///////////////////////////////////////////////////////////////////////////////// 
  NotifyAdd(m_Storage->GetRoot());

	mafEventMacro(mafEvent(this,VME_SELECTED,m_Storage->GetRoot())); 
  mafEventMacro(mafEvent(this,CAMERA_RESET)); 
  
	if (m_TmpDir != "")
	{
    m_FileHistory.AddFileToHistory(m_ZipFile);
	}
  else if(/*!remote_file && */res == MAF_OK)
  {
    m_FileHistory.AddFileToHistory(m_MSFFile);
  }
  else if(res != MAF_OK && m_FileHistoryIdx != -1)
  {
    m_FileHistory.RemoveFileFromHistory(m_FileHistoryIdx);
  }
	m_FileHistory.Save(*m_Config);
  m_FileHistoryIdx = -1;

  mafEventMacro(mafEvent(this,LAYOUT_LOAD));
}
//----------------------------------------------------------------------------
const char *mafVMEManager::ZIPOpen(wxString filename)
//----------------------------------------------------------------------------
{
  m_ZipFile = filename;
  wxString zip_cache = wxPathOnly(filename);
  if (zip_cache.IsEmpty())
  {
    zip_cache = ::wxGetCwd();
  }
  zip_cache = zip_cache + "\\~TmpData";
  if (!wxDirExists(zip_cache))
    wxMkdir(zip_cache);
  m_TmpDir = zip_cache;

  wxString path, name, ext, complete_name, zfile, out_file;
  wxSplitPath(m_ZipFile,&path,&name,&ext);
  complete_name = name + "." + ext;
  
  wxFSFile *zfileStream;
  wxZlibInputStream *zip_is;
  wxString pkg = "#zip:";
  wxString header_name = complete_name + pkg;
  int length_header_name = header_name.Length();
  bool enable_mid = false;
  wxFileSystem *zip_fs = new wxFileSystem();
  zip_fs->AddHandler(new wxZipFSHandler);
  zip_fs->ChangePathTo(m_ZipFile);
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
  out_file = m_TmpDir + "\\" + complete_name;
  char *buf;
  int s_size;
  std::ofstream out_file_stream;

  if(ext == "msf")
  {
    m_MSFFile = out_file;
    out_file_stream.open(out_file, std::ios_base::out);
  }
  else
  {
    out_file_stream.open(out_file, std::ios_base::binary);
  }
  s_size = zip_is->GetSize();
  buf = new char[s_size];
  zip_is->Read(buf,s_size);
  out_file_stream.write(buf, s_size);
  out_file_stream.close();
  delete[] buf;
  
  zfileStream->UnRef();
  delete zfileStream;

  while ((zfile = zip_fs->FindNext()) != "")
  {
    zfileStream = zip_fs->OpenFile(zfile);
    zip_is = (wxZlibInputStream *)zfileStream->GetStream();
    wxSplitPath(zfile,&path,&name,&ext);
    complete_name = name + "." + ext;
    if (enable_mid)
      complete_name = complete_name.Mid(length_header_name);
    out_file = m_TmpDir + "\\" + complete_name;
    if(ext == "msf")
    {
      m_MSFFile = out_file;
      out_file_stream.open(out_file, std::ios_base::out);
    }
    else
    out_file_stream.open(out_file, std::ios_base::binary);
    s_size = zip_is->GetSize();
    buf = new char[s_size];
    zip_is->Read(buf,s_size);
    out_file_stream.write(buf, s_size);
    out_file_stream.close();
    delete[] buf;
    zfileStream->UnRef();
    delete zfileStream;
  }
  
  zip_fs->ChangePathTo(m_TmpDir,TRUE);
  zip_fs->CleanUpHandlers();
  delete zip_fs;
  
  if (m_MSFFile == "")
  {
    wxMessageBox("compressed archive is not a valid msf file!", "Error");
    return "";
  }

  return m_MSFFile.c_str();
}
//----------------------------------------------------------------------------
void mafVMEManager::ZIPSave(wxString filename)
//----------------------------------------------------------------------------
{
  if (filename != "")
  {
    m_ZipFile = filename;
  }
  if (m_ZipFile == "")
  {
    return;
  }

  wxArrayString files;
  wxDir::GetAllFiles(m_TmpDir,&files);

  if (!MakeZip(m_ZipFile,&files))
  {
    wxMessageBox(_("Filed to create compressed archive!"),_("Error"));
  }
}
//----------------------------------------------------------------------------
bool mafVMEManager::MakeZip(const wxString& zipname, wxArrayString *files)
//----------------------------------------------------------------------------
{
  wxString name, path, short_name, ext;
  wxFileOutputStream out(zipname);
  wxZipOutputStream zip(out);

  if (!out || !zip)
    return false;

  for (size_t i = 0; i < files->GetCount(); i++) 
  {
    name = files->Item(i);
    wxSplitPath(name.c_str(), &path, &short_name, &ext);
    short_name += ".";
    short_name += ext;
    //mafLogMessage(_T("adding %s"), short_name.c_str());

    if (wxDirExists(name)) 
    {
      if (!zip.PutNextDirEntry(name))
        return false;
    }
    else 
    {
      wxFFileInputStream in(name);

      if (in.Ok()) 
      {
        wxDateTime dt(wxFileModificationTime(name));

        if (!zip.PutNextEntry(short_name, dt, in.GetLength()) || !zip.Write(in) || !in.Eof())
          return false;
      }
    }
  }

  return zip.Close() && out.Close();
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFSave()
//----------------------------------------------------------------------------
{
  if(m_MSFFile == "") 
  {
    wxString wildc    = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
    mafString file = mafGetSaveFile(m_MSFDir, wildc.c_str()).c_str();
    if(file == "") return;
   
    wxString path, name, ext, file_dir;
    wxSplitPath(file.GetCStr(),&path,&name,&ext);

    if(!wxFileExists(file.GetCStr()))
		{
			file_dir = path + "/" + name;
			if(!wxDirExists(file_dir))
				wxMkdir(file_dir);
      if (ext == "zmsf")
      {
        m_ZipFile = file.GetCStr();
        m_TmpDir = path + "/" + name;
        ext = "msf";
      }
			file = file_dir + "/" + name + "." + ext;
		}

    // convert to unix format
    file.ParsePathName();
    m_MSFFile = file.GetCStr();
  }
  if(wxFileExists(m_MSFFile) && m_MakeBakFile)
	{
		wxString bak_filename = m_MSFFile + ".bak";
    wxRenameFile(m_MSFFile,bak_filename);
	}
	
	wxBusyInfo wait("Saving MSF: Please wait");
  m_Storage->SetURL(m_MSFFile.c_str());
  if (m_Storage->Store()!=0)
  {
    mafLogMessage("Error during MSF saving");
  }
  if (m_TmpDir != "")
  {
    ZIPSave();
    m_FileHistory.AddFileToHistory(m_ZipFile);
  }
  else
  {
    m_FileHistory.AddFileToHistory(m_MSFFile);
  }
	m_FileHistory.Save(*m_Config);
  m_Modified = false;
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFSaveAs()   
//----------------------------------------------------------------------------
{
   m_MSFFile = "";
   m_MakeBakFile = false;
   MSFSave();
}
//----------------------------------------------------------------------------
void mafVMEManager::Upload(mafString local_file, mafString remote_file)
//----------------------------------------------------------------------------
{
  if (m_Storage == NULL)
  {
    wxMessageBox(_("Some problem occourred, MAF storage is NULL!!"), _("Warning"));
    return;
  }
  mafRemoteStorage *storage = (mafRemoteStorage *)m_Storage;
  if (storage->GetRemoteFileManager()->UploadLocalFile(local_file, remote_file) != MAF_OK)
  {

  }
  /*
  if (upload_flag == UPLOAD_TREE)
  {
    wxString local_dir = wxPathOnly(m_MSFFile);
    wxString remote_dir = remote_file.GetCStr();
    remote_dir = wxPathOnly(remote_dir);
    wxString upload_file;

    wxArrayString files;
    wxDir::GetAllFiles(local_dir,&files);

    wxString path, short_name, ext, local_file;
    for (size_t i = 0; i < files.GetCount(); i++) 
    {
      local_file = files.Item(i);
      wxSplitPath(local_file.c_str(), &path, &short_name, &ext);
      upload_file = remote_dir + "/" + short_name + "." + ext;
      if (storage->UploadLocalFile(local_file, upload_file) != MAF_OK)
      {
        break;
      }
    }
  }*/
}
//----------------------------------------------------------------------------
void mafVMEManager::VmeAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  if(n != NULL)
  {
    mafNode *vp = n->GetParent();  
    assert( vp == NULL || m_Storage->GetRoot()->IsInTree(vp) );
    if(vp == NULL) 
			n->ReparentTo(m_Storage->GetRoot());

    m_Modified = true;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::VmeRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  if(n != NULL && m_Storage->GetRoot() /*&& m_Storage->GetRoot()->IsInTree(n)*/) 
  {
    assert(m_Storage->GetRoot()->IsInTree(n));
    n->ReparentTo(NULL);
    m_Modified = true;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::TimeSet(double time)
//----------------------------------------------------------------------------
{
  if(m_Storage->GetRoot()) m_Storage->GetRoot()->SetTreeTime(time);
}
//----------------------------------------------------------------------------
void mafVMEManager::TimeGetBounds(double *min, double *max)
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
void mafVMEManager::NotifyRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true);
  iter->SetTraversalModeToPostOrder();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		mafEventMacro(mafEvent(this,VME_REMOVING,node));
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafVMEManager::NotifyAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true);
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    mafEventMacro(mafEvent(this,VME_ADDED,node));
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
void mafVMEManager::UpdateFromTag(mafNode *n)
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
    mafNodeIterator *iter = m_Storage->GetRoot()->NewIterator();
    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      mafVmeData *vd = (mafVmeData *)node->GetClientData();
      if (vd)
        vd->UpdateFromTag();
    }
    iter->Delete();
  }
  */
}
