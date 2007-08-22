/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEManager.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-22 10:57:28 $
  Version:   $Revision: 1.39 $
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


#include "mafVMEManager.h"

#include <wx/busyinfo.h>
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>
#include <wx/dir.h>
#include <wx/filename.h>

#include "mafDecl.h"
#include "mafNode.h"
#include "mafVMEStorage.h"
#include "mafRemoteStorage.h"
#include "mmdRemoteFileManager.h"
#include "mafVMEGenericAbstract.h"
#include "mafDataVector.h"

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
  m_FileHistoryIdx = -1;

  mafString msfDir = mafGetApplicationDirectory().c_str();
  msfDir.ParsePathName();
	m_MSFDir   = msfDir;
	m_MSFFile  = "";
	m_ZipFile  = "";
  m_TmpDir   = "";

  m_SingleBinaryFile = false;

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
void mafVMEManager::SetSingleBinaryFile(bool singleFile)
//----------------------------------------------------------------------------
{
  m_SingleBinaryFile = singleFile;
}
//----------------------------------------------------------------------------
void mafVMEManager::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (maf_event->GetChannel()==MCH_UP)
  {
    if (maf_event->GetId() == mafDataVector::SINGLE_FILE_DATA)
    {
      ((mafEvent *)maf_event)->SetBool(m_SingleBinaryFile);
      return;
    }
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
void mafVMEManager::SetLocalCacheFolder(mafString cache_folder)
//----------------------------------------------------------------------------
{
  if (m_Storage)
  {
    m_Storage->SetTmpFolder(cache_folder.GetCStr());
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::RemoveTempDirectory()
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
      ::wxRmdir(m_TmpDir.GetCStr());
    }
    m_TmpDir = "";
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFNew(bool notify_root_creation)
//----------------------------------------------------------------------------
{
  RemoveTempDirectory();

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
  m_ZipFile = ""; 
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(int file_id)
//----------------------------------------------------------------------------
{
  m_FileHistoryIdx = file_id - wxID_FILE1;
	mafString file = m_FileHistory.GetHistoryFile(m_FileHistoryIdx).c_str();
	MSFOpen(file);
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(mafString filename)
//----------------------------------------------------------------------------
{
  wxWindowDisabler disableAll;
  wxBusyCursor wait_cursor;
  
  mafString protocol = "";
  bool remote_file = IsRemote(filename, protocol);
  
  if(!remote_file && !::wxFileExists(filename.GetCStr()))
	{
		mafString msg;
    msg = _("File ");
		msg << filename;
		msg << _(" not found!");
		mafWarningMessage(msg, _("Warning"));
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
  wxSplitPath(filename.GetCStr(),&path,&name,&ext);
  if(ext == "zmsf")
  {
    if (remote_file)
    {
      // Download the file if it is not present into the cache
      // we are using the remote storage!!
      mafString local_filename, remote_filename;
      remote_filename = filename;
      local_filename = m_Storage->GetTmpFolder();
      local_filename += "\\";
      local_filename += name;
      local_filename += ".zmsf";
      ((mafRemoteStorage *)m_Storage)->GetRemoteFileManager()->DownloadRemoteFile(remote_filename, local_filename);
      filename = local_filename;
    }
    unixname = ZIPOpen(filename);
    if(unixname.IsEmpty())
    {
    	mafMessage(_("Bad or corrupted zmsf file!"));
      m_Modified = false;
      m_Storage->Delete();
      m_Storage = NULL;
      MSFNew();
      return;
    }
    wxSetWorkingDirectory(m_TmpDir.GetCStr());
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
  m_Storage->SetURL(m_MSFFile.GetCStr());
 
  m_LoadingFlag = true;
  int res = m_Storage->Restore();
  if (res != MAF_OK)
  {
    // if some problems occurred during import give feedback to the user
    mafErrorMessage(_("Errors during file parsing! Look the log area for error messages."));
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
		mafMessage(_("File not valid for this application!"), _("Warning"));
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
    m_FileHistory.AddFileToHistory(m_ZipFile.GetCStr());
	}
  else if(/*!remote_file && */res == MAF_OK)
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.GetCStr());
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
const char *mafVMEManager::ZIPOpen(mafString filename)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  
  m_ZipFile = filename;
  mafString zip_cache = wxPathOnly(filename.GetCStr());
  if (zip_cache.IsEmpty())
  {
    zip_cache = m_Storage->GetTmpFolder();
  }
  
  zip_cache += "/";
  zip_cache = wxFileName::CreateTempFileName(zip_cache.GetCStr());
  wxRemoveFile(zip_cache.GetCStr());
  wxSplitPath(zip_cache.GetCStr(),&path,&name,&ext);
  zip_cache = path + "/" + name + ext;
  zip_cache.ParsePathName();
  
  if (!wxDirExists(zip_cache.GetCStr()))
    wxMkdir(zip_cache.GetCStr());
  m_TmpDir = zip_cache;

  wxString complete_name, zfile, out_file;
  wxSplitPath(m_ZipFile.GetCStr(),&path,&name,&ext);
  complete_name = name + "." + ext;
  
  wxFSFile *zfileStream;
  wxZlibInputStream *zip_is;
  wxString pkg = "#zip:";
  wxString header_name = complete_name + pkg;
  int length_header_name = header_name.Length();
  bool enable_mid = false;
  wxFileSystem *zip_fs = new wxFileSystem();
  zip_fs->AddHandler(new wxZipFSHandler);
  zip_fs->ChangePathTo(m_ZipFile.GetCStr());
  // extract filename from the zip archive
  zfile = zip_fs->FindFirst(complete_name+pkg+name+"\\*.*");
  if (zfile == "")
  {
    enable_mid = true;
    zfile = zip_fs->FindFirst(complete_name+pkg+"\\*.*");
  }
  if (zfile == "")
  {
    zip_fs->CleanUpHandlers();
    cppDEL(zip_fs);
    RemoveTempDirectory();
    return "";
  }
  wxSplitPath(zfile,&path,&name,&ext);
  complete_name = name + "." + ext;
  if (enable_mid)
    complete_name = complete_name.Mid(length_header_name);
  zfileStream = zip_fs->OpenFile(zfile);
  if (zfileStream == NULL)
  {
    zip_fs->CleanUpHandlers();
    cppDEL(zip_fs);
    RemoveTempDirectory();
    return "";
  }
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
    if (zfileStream == NULL)
    {
      zip_fs->CleanUpHandlers();
      cppDEL(zip_fs);
      RemoveTempDirectory();
      return "";
    }
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
  
  zip_fs->ChangePathTo(m_TmpDir.GetCStr(), TRUE);
  zip_fs->CleanUpHandlers();
  cppDEL(zip_fs);
  
  if (m_MSFFile == "")
  {
    mafMessage(_("compressed archive is not a valid msf file!"), _("Error"));
    return "";
  }

  return m_MSFFile.GetCStr();
}
//----------------------------------------------------------------------------
void mafVMEManager::ZIPSave(mafString filename)
//----------------------------------------------------------------------------
{
  m_ZipFile = filename.IsEmpty() ? "" : filename;

  if (m_ZipFile.IsEmpty())
  {
    return;
  }

  wxArrayString files;
  wxDir::GetAllFiles(m_TmpDir.GetCStr(), &files);

  if (!MakeZip(m_ZipFile.GetCStr(),&files))
  {
    mafMessage(_("Filed to create compressed archive!"),_("Error"));
  }
}
//----------------------------------------------------------------------------
bool mafVMEManager::MakeZip(const mafString &zipname, wxArrayString *files)
//----------------------------------------------------------------------------
{
  wxString name, path, short_name, ext;
  wxFileOutputStream out(zipname.GetCStr());
  wxZipOutputStream zip(out);

  if (!out || !zip)
    return false;

  for (size_t i = 0; i < files->GetCount(); i++) 
  {
    name = files->Item(i);
    wxSplitPath(name, &path, &short_name, &ext);
    short_name += ".";
    short_name += ext;

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
  if(m_MSFFile.IsEmpty()) 
  {
    // new file to save: ask to the application which is the default
    // modality to save binary files.
    mafEvent e(this,mafDataVector::SINGLE_FILE_DATA);
    mafEventMacro(e);
    SetSingleBinaryFile(e.GetBool());
    
    // ask for the new file name.
    wxString wildc = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
    mafString file = mafGetSaveFile(m_MSFDir, wildc.c_str()).c_str();
    if(file.IsEmpty())
      return;
   
    wxString path, name, ext, file_dir;
    wxSplitPath(file.GetCStr(),&path,&name,&ext);

    if(!wxFileExists(file.GetCStr()))
		{
			file_dir = path + "/" + name;
			if(!wxDirExists(file_dir))
				wxMkdir(file_dir);
      if (ext == "zmsf")
      {
        m_ZipFile = file;
        m_TmpDir = path + "/" + name;
        ext = "msf";
      }
			file = file_dir + "/" + name + "." + ext;
		}

    // convert to unix format
    mafString sub_unixname;
    if (file.StartsWith("\\\\"))
    {
      // prevent to revert the remote mounted disk back slash characters.
      sub_unixname = file;
      sub_unixname.Erase(0,1);
      sub_unixname.ParsePathName();
      file = "\\\\";
      file += sub_unixname;
    }
    else
      file.ParsePathName();

    m_MSFFile = file.GetCStr();
  }
  if(wxFileExists(m_MSFFile.GetCStr()) && m_MakeBakFile)
	{
		mafString bak_filename = m_MSFFile + ".bak";
    wxRenameFile(m_MSFFile.GetCStr(), bak_filename.GetCStr());
	}
	
	wxBusyInfo wait(_("Saving MSF: Please wait"));
  m_Storage->SetURL(m_MSFFile.GetCStr());
  if (m_Storage->Store() != MAF_OK)
  {
    mafLogMessage(_("Error during MSF saving"));
  }
  if (!m_ZipFile.IsEmpty())
  {
    ZIPSave(m_ZipFile);
    m_FileHistory.AddFileToHistory(m_ZipFile.GetCStr());
  }
  else
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.GetCStr());
  }
	m_FileHistory.Save(*m_Config);
  m_Modified = false;
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFSaveAs()   
//----------------------------------------------------------------------------
{
   m_MSFFile = "";
   m_ZipFile = "";
   m_MakeBakFile = false;
   MSFSave();
}
//----------------------------------------------------------------------------
void mafVMEManager::Upload(mafString local_file, mafString remote_file)
//----------------------------------------------------------------------------
{
  if (m_Storage == NULL)
  {
    mafMessage(_("Some problem occourred, MAF storage is NULL!!"), _("Warning"));
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
  bool checkSingleFile = n->IsMAFType(mafVMERoot);
  
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true);
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    mafEventMacro(mafEvent(this,VME_ADDED,node));
    if (checkSingleFile)
    {
      // if checkSingleFile == true this method is called by the MSFOpen
      // so we have to check if the msf has been saved in single binary file or not.
      mafVMEGenericAbstract *vmeWithDataVector = mafVMEGenericAbstract::SafeDownCast(node);
      if (vmeWithDataVector)
      {
        mafDataVector *dv = vmeWithDataVector->GetDataVector();
        if (dv != NULL)
        {
          SetSingleBinaryFile(dv->GetSingleFileMode());
          checkSingleFile = false;
        }
      }
    }
  }
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
		int answer = wxMessageBox(_("your work is modified, would you like to save it?"),_("Confirm"),
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
