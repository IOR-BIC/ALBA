/*=========================================================================

 Program: MAF2
 Module: mafVMEManager
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
  m_ZipHandler = NULL;
  m_FileSystem = NULL;

  m_TestMode = false;
}
//----------------------------------------------------------------------------
mafVMEManager::~mafVMEManager()
//----------------------------------------------------------------------------
{
  if(m_Storage) 
    NotifyRemove( m_Storage->GetRoot() ); // //SIL. 11-4-2005:  - cast root to node -- maybe to be removed

  m_AppStamp.clear();
  m_Listener = NULL;
  m_FileSystem->CleanUpHandlers(); // Handlers are shared trough file systems.
  cppDEL(m_FileSystem);
  
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
    if (maf_event->GetId() == mafDataVector::GetSingleFileDataId())
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
void mafVMEManager::SetApplicationStamp(mafString &appstamp)
//----------------------------------------------------------------------------
{
  // Add a single application stamp; this is done automatically while creating the application with the application name
  m_AppStamp.push_back(appstamp);
}
//----------------------------------------------------------------------------
void mafVMEManager::SetApplicationStamp(std::vector<mafString> appstamp)
//----------------------------------------------------------------------------
{
  // Add a vector of time stamps; this can be done manually for adding compatibility with other applications. 
  // The application name itself must not be included since it was already added with the other call (see function above).
  for (int i=0; i<appstamp.size();i++)
  {
    m_AppStamp.push_back(appstamp.at(i));
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::SetLocalCacheFolder(mafString cache_folder)
//----------------------------------------------------------------------------
{
  if (m_Storage)
  {
    // Set the local cache directory
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
    if(::wxDirExists(m_TmpDir)) //remove tmp directory due to zip extraction or compression
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
  if (m_Storage)
  {
    m_Storage->GetRoot()->CleanTree();
    mafEventMacro(mafEvent(this,CLEAR_UNDO_STACK)); // ask logic to clear the undo stack
  }

  RemoveTempDirectory(); // remove the temporary directory

  m_Modified = false;

  m_LoadingFlag = true; // set the loading flag to prevent the manager
                        // listen to NODE_ATTACHED_TO_TREE and NODE_DETACHED_TO_TREE events

  mafEvent e(this,CREATE_STORAGE,m_Storage); // ask logic to create a new storage
  mafEventMacro(e);
  m_Storage = (mafVMEStorage *)e.GetMafObject();
  m_LoadingFlag = false;

  if(notify_root_creation)
	{
		//Add the application stamps
		mafTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.at(0).GetCStr());
		m_Storage->GetRoot()->GetTagArray()->SetTag(tag_appstamp); // set the appstamp tag for the root
    AddCreationDate(m_Storage->GetRoot());
		mafEventMacro(mafEvent(this,VME_ADDED,m_Storage->GetRoot())); // raise notification events
		mafEventMacro(mafEvent(this,VME_SELECTED,m_Storage->GetRoot()));
	}

  m_MSFFile = ""; //next MSFSave will ask for a filename
  m_ZipFile = ""; 
}

//----------------------------------------------------------------------------
void mafVMEManager::AddCreationDate(mafNode *vme)
//----------------------------------------------------------------------------
{
  wxString dateAndTime;
  wxDateTime time = wxDateTime::UNow(); // get time with millisecond precision
  dateAndTime  = wxString::Format("%02d/%02d/%02d %02d:%02d:%02d",time.GetDay(), time.GetMonth()+1, time.GetYear(), time.GetHour(), time.GetMinute(),time.GetSecond());
 
  mafTagItem tag_creationDate;
  tag_creationDate.SetName("Creation_Date");
  tag_creationDate.SetValue(dateAndTime);
  vme->GetTagArray()->SetTag(tag_creationDate); // set creation date tag for the specified vme
}

//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(int file_id)
//----------------------------------------------------------------------------
{
  m_FileHistoryIdx = file_id - wxID_FILE1;
	mafString file = m_FileHistory.GetHistoryFile(m_FileHistoryIdx).c_str(); // get the filename from history
	MSFOpen(file);
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(mafString filename)
//----------------------------------------------------------------------------
{
  wxWindowDisabler *disableAll;
  wxBusyCursor *wait_cursor;

  if(!m_TestMode) // Losi 02/16/2010 for test class
  {
    disableAll = new wxWindowDisabler();
    wait_cursor = new wxBusyCursor();
  }
  
  mafString protocol = "";
  bool remote_file = IsRemote(filename, protocol); // check if the specified path refers to a remote location
  
  // open a local msf
  if(!remote_file && !::wxFileExists(filename.GetCStr()))
	{
		mafString msg;
    msg = _("File ");
		msg << filename;
		msg << _(" not found!");
		mafWarningMessage(msg, _("Warning"));
    if(m_FileHistoryIdx != -1)
    {
      m_FileHistory.RemoveFileFromHistory(m_FileHistoryIdx); // remove filename to history
      m_FileHistory.Save(*m_Config); // Save file history to registry
      m_FileHistoryIdx = -1;
    }

    if(!m_TestMode) // Losi 02/16/2010 for test class
    {
      cppDEL(disableAll);
      cppDEL(wait_cursor);
    }
		return;
	}

  MSFNew(false); // insert and select the root - reset m_MSFFile - delete the old storage and create a new one
  
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
    if (remote_file) // download remote zmsf
    {
      // Download the file if it is not present into the cache
      // we are using the remote storage!!
      mafString local_filename, remote_filename;
      remote_filename = filename;
      local_filename = m_Storage->GetTmpFolder();
      local_filename += "\\";
      local_filename += name;
      local_filename += ".zmsf";
      ((mafRemoteStorage *)m_Storage)->GetRemoteFileManager()->DownloadRemoteFile(remote_filename, local_filename); // download the remote file in the download cache
      filename = local_filename;
    }
    unixname = ZIPOpen(filename); // open the zmsf archive and extract it to the temporary directory
    if(unixname.IsEmpty())
    {
    	mafMessage(_("Bad or corrupted zmsf file!"));
      m_Modified = false;
      m_Storage->Delete();
      m_Storage = NULL;
      MSFNew();
      if(!m_TestMode) // Losi 02/16/2010 for test class
      {
        cppDEL(disableAll);
        cppDEL(wait_cursor);
      }
      return;
    }
    wxSetWorkingDirectory(m_TmpDir.GetCStr());
  }
  
  // convert to unix format
  mafString sub_unixname;
  if (unixname.StartsWith("\\\\"))
  {
    // prevent to revert the remote mounted disk back slash characters.
    sub_unixname = unixname;
    sub_unixname.Erase(0,1);
    sub_unixname.ParsePathName();
    unixname = "\\\\";
    unixname += sub_unixname;
  }
  else
    unixname.ParsePathName();

  m_MSFFile = unixname; 
  m_Storage->SetURL(m_MSFFile.GetCStr());
 
  m_LoadingFlag = true; // set m_LoadingFlag to prevent the manager
                        // listen to NODE_ATTACHED_TO_TREE and NODE_DETACHED_TO_TREE events
  int res = m_Storage->Restore(); // restore the tree
  if (res != MAF_OK)
  {
    mafErrorMessage(_("Errors during file parsing! Look the log area for error messages.")); // if some problems occurred during import give feedback to the user
  }
  m_LoadingFlag = false;

  mafTimeStamp b[2];
  mafVMERoot *root_node = m_Storage->GetRoot();
  root_node->GetOutput()->GetTimeBounds(b);
  root_node->SetTreeTime(b[0]); // Set tree time to the starting time
  
	////////////////////////////////  Application Stamp managing ////////////////////
	if(!root_node->GetTagArray()->IsTagPresent("APP_STAMP"))
	{
		//update the old data files to support Application Stamp
		mafTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.at(0).GetCStr());
		root_node->GetTagArray()->SetTag(tag_appstamp); // set appstamp tag of the root
	}
	
	mafString app_stamp;
  app_stamp << root_node->GetTagArray()->GetTag("APP_STAMP")->GetValue();
  // First check for compatibility with all stored App stamps
  bool stamp_found = false;
  bool stamp_data_manager_found = false;
  bool stamp_open_all_found = false;
  for (int k=0; k<m_AppStamp.size(); k++)
  {
    // Check with the Application name
    if (app_stamp.Equals(m_AppStamp.at(k).GetCStr()))
    {
      stamp_found = true;
    }
    // Check with the "Data Manager" tag
    if (m_AppStamp.at(k).Equals("DataManager"))
    {
      stamp_data_manager_found = true;
    }
    // Check with the "OPEN_ALL_DATA" tag
    if (m_AppStamp.at(k).Equals("OPEN_ALL_DATA"))
    {
      stamp_open_all_found = true;
    }
  }
	if(app_stamp.Equals("INVALID") || ((!stamp_found) && (!stamp_data_manager_found) && (!stamp_open_all_found))) 
	{
		//Application stamp not valid
		mafMessage(_("File not valid for this application!"), _("Warning"));
		m_Modified = false;
		m_Storage->Delete();
		m_Storage = NULL;
		MSFNew();
    if(!m_TestMode) // Losi 02/16/2010 for test class
    {
      cppDEL(disableAll);
      cppDEL(wait_cursor);
    }
		return;
	}
	///////////////////////////////////////////////////////////////////////////////// 
  NotifyAdd(root_node); // add the storage root (the tree) with events notification

	mafEventMacro(mafEvent(this,VME_SELECTED, root_node)); // raise notification events (to logic)
  mafEventMacro(mafEvent(this,CAMERA_RESET)); 
  
	if (m_TmpDir != "")
	{
    m_FileHistory.AddFileToHistory(m_ZipFile.GetCStr()); // add the zmsf file to the history
	}
  else if(/*!remote_file && */res == MAF_OK)
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.GetCStr()); // add the msf file to the history
  }
  else if(res != MAF_OK && m_FileHistoryIdx != -1)
  {
    m_FileHistory.RemoveFileFromHistory(m_FileHistoryIdx); // if something get wrong retoring the file remove it from istory
  }
	m_FileHistory.Save(*m_Config); // save file history to registry
  m_FileHistoryIdx = -1;

  mafEventMacro(mafEvent(this,LAYOUT_LOAD)); // ask logic to load the layout

  if(!m_TestMode) // Losi 02/16/2010 for test class
  {
    cppDEL(disableAll);
    cppDEL(wait_cursor);
  }
}
//----------------------------------------------------------------------------
const char *mafVMEManager::ZIPOpen(mafString filename)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  
  m_ZipFile = filename;
  mafString zip_cache = wxPathOnly(filename.GetCStr()); // get the directory
  if (zip_cache.IsEmpty())
  {
    zip_cache = m_Storage->GetTmpFolder();
  }
  
  zip_cache += "/";
  zip_cache = wxFileName::CreateTempFileName(zip_cache.GetCStr()); // used to get a valid temporary name for cache directory
  wxRemoveFile(zip_cache.GetCStr());
  wxSplitPath(zip_cache.GetCStr(),&path,&name,&ext);
  zip_cache = path + "/" + name + ext;
  zip_cache.ParsePathName();
  
  if (!wxDirExists(zip_cache.GetCStr()))
    wxMkdir(zip_cache.GetCStr()); // create a temporary directory in which extract the archive
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
  if(m_FileSystem == NULL)m_FileSystem = new wxFileSystem();
  
  if(m_ZipHandler == NULL)
  {
    m_ZipHandler = new wxZipFSHandler();
    m_FileSystem->AddHandler(m_ZipHandler); // add the handler that manage zip protocol
    // (the handler to manage the local files protocol is already added to wxFileSystem)
  }
  
  m_FileSystem->ChangePathTo(m_ZipFile.GetCStr());
  // extract filename from the zip archive
  zfile = m_FileSystem->FindFirst(complete_name+pkg+name+"\\*.*");
  if (zfile == "")
  {
    enable_mid = true;
    // no files found: try to search inside the archive without filename
    zfile = m_FileSystem->FindFirst(complete_name+pkg+"\\*.*");
  }
  if (zfile == "")
  {
    // no files found inside the archive
    RemoveTempDirectory(); // remove the temporary directory
    return "";
  }
  wxSplitPath(zfile,&path,&name,&ext);
  complete_name = name + "." + ext;
  if (enable_mid)
    complete_name = complete_name.Mid(length_header_name);
  zfileStream = m_FileSystem->OpenFile(zfile);
  if (zfileStream == NULL) // unable to open the file
  {
    RemoveTempDirectory(); // remove the temporary directory
    return "";
  }
  zip_is = (wxZlibInputStream *)zfileStream->GetStream();
  out_file = m_TmpDir + "\\" + complete_name;
  char *buf;
  int s_size;
  std::ofstream out_file_stream;

  if(ext == "msf")
  {
    m_MSFFile = out_file; // the file to extract is the msf
    out_file_stream.open(out_file, std::ios_base::out);
  }
  else
  {
    out_file_stream.open(out_file, std::ios_base::binary); // the file to extract is a binary
  }
  s_size = zip_is->GetSize();
  buf = new char[s_size];
  zip_is->Read(buf,s_size);
  out_file_stream.write(buf, s_size);
  out_file_stream.close();
  delete[] buf;
  
  zfileStream->UnRef();
  delete zfileStream;

  while ((zfile = m_FileSystem->FindNext()) != "")
  {
    zfileStream = m_FileSystem->OpenFile(zfile);
    if (zfileStream == NULL) // unable to open the file
    {
      RemoveTempDirectory(); // remove the temporary directory
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
      m_MSFFile = out_file; // The file to extract is an msf
      out_file_stream.open(out_file, std::ios_base::out);
    }
    else
      out_file_stream.open(out_file, std::ios_base::binary); // The file to extract is a binary
    s_size = zip_is->GetSize();
    buf = new char[s_size];
    zip_is->Read(buf,s_size);
    out_file_stream.write(buf, s_size);
    out_file_stream.close();
    delete[] buf;
    zfileStream->UnRef();
    delete zfileStream;
  }
  
  m_FileSystem->ChangePathTo(m_TmpDir.GetCStr(), TRUE);

  
  if (m_MSFFile == "") // msf file not extracted
  {
    mafMessage(_("compressed archive is not a valid msf file!"), _("Error"));
    return "";
  }

  // return the extracted msf filename
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
  wxDir::GetAllFiles(m_TmpDir.GetCStr(), &files); // get all files in the temporary directory

  if (!MakeZip(m_ZipFile.GetCStr(),&files))
  {
    mafMessage(_("Failed to create compressed archive!"),_("Error"));
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
      if (!zip.PutNextDirEntry(name)) // put the file inside the archive
        return false;
    }
    else 
    {
      wxFFileInputStream in(name);

      if (in.Ok()) 
      {
        wxDateTime dt(wxFileModificationTime(name)); // get the file modification time

        if (!zip.PutNextEntry(short_name, dt, in.GetLength()) || !zip.Write(in) || !in.Eof()) // put the file inside the archive
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
    mafEvent e(this,mafDataVector::GetSingleFileDataId());
    mafEventMacro(e);
    SetSingleBinaryFile(e.GetBool()); // set the save modality for time-varying vme
    
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
  if(wxFileExists(m_MSFFile.GetCStr()) && m_MakeBakFile) // an msf with the same name exists
	{
    mafString bak_filename = m_MSFFile + ".bak";                // create the backup for the saved msf
    wxRenameFile(m_MSFFile.GetCStr(), bak_filename.GetCStr());  // renaming the founded one
	}
	
  if(!m_TestMode) // Losi 02/16/2010 for test class 
  {
	  wxBusyInfo wait(_("Saving MSF: Please wait"));
  }
  m_Storage->SetURL(m_MSFFile.GetCStr());
  if (m_Storage->Store() != MAF_OK) // store the tree
  {
    mafLogMessage(_("Error during MSF saving"));
  }
  // add the msf (or zmsf) to the history
  if (!m_ZipFile.IsEmpty())
  {
    ZIPSave(m_ZipFile);
    m_FileHistory.AddFileToHistory(m_ZipFile.GetCStr()); // add the zmsf to the file history
  }
  else
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.GetCStr()); // add the msf to the file history
  }
	m_FileHistory.Save(*m_Config);
  m_Modified = false;
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFSaveAs()   
//----------------------------------------------------------------------------
{
   m_MSFFile = ""; // set filenames to empty so the MSFSave method will ask for them
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
  if (storage->GetRemoteFileManager()->UploadLocalFile(local_file, remote_file) != MAF_OK) // Upload the local file to the remote repository
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
    wxDir::GetAllFiles(local_dir,&files); // get all files in the temporary directory

    wxString path, short_name, ext, local_file;
    for (size_t i = 0; i < files.GetCount(); i++) 
    {
      local_file = files.Item(i);
      wxSplitPath(local_file.c_str(), &path, &short_name, &ext);
      upload_file = remote_dir + "/" + short_name + "." + ext;
      if (storage->UploadLocalFile(local_file, upload_file) != MAF_OK) // Upload the local file to the remote repository
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
    // check the node's parent
    mafNode *vp = n->GetParent();  
    assert( vp == NULL || m_Storage->GetRoot()->IsInTree(vp) );
    if(vp == NULL) 
			n->ReparentTo(m_Storage->GetRoot()); // reparent the node to the root

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
  if(m_Storage->GetRoot()) m_Storage->GetRoot()->SetTreeTime(time); // set the tree time
}
//----------------------------------------------------------------------------
void mafVMEManager::TimeGetBounds(double *min, double *max)
//----------------------------------------------------------------------------
{
  mafTimeStamp b[2];
  if(m_Storage->GetRoot()) 
  {
    m_Storage->GetRoot()->GetOutput()->GetTimeBounds(b); // get the root's time bounds
    *min = b[0];
    *max = b[1];
  }
  else
  {
    // no msf
    *min = 0;
    *max = 0;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::NotifyRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
  iter->SetTraversalModeToPostOrder(); // traverse is: first the subtree left to right, then the root
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		mafEventMacro(mafEvent(this,VME_REMOVING,node)); // raise notification event (to logic)
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafVMEManager::NotifyAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  bool checkSingleFile = n->IsMAFType(mafVMERoot);
  
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    mafVMERoot *root = GetRoot();
    if (root != NULL)
    {
      mafVME *vme = mafVME::SafeDownCast(node);
      if (vme != NULL)
      {
        // Update the new VME added to the tree with the current time-stamp
        // present in the tree.
        vme->SetTimeStamp(root->GetTimeStamp());
      }
    }

    mafEventMacro(mafEvent(this,VME_ADDED,node)); // raise notification event (to logic)
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
  m_Modified = true;
}
//----------------------------------------------------------------------------
void mafVMEManager::SetFileHistoryMenu(wxMenu *menu)
//----------------------------------------------------------------------------
{
  m_FileHistory.UseMenu(menu);
	m_FileHistory.Load(*m_Config); // Loads file history from registry
}
//----------------------------------------------------------------------------
bool mafVMEManager::AskConfirmAndSave()
//----------------------------------------------------------------------------
{
  bool go = true;
	if (m_Modified) // check if the msf has been modified
	{
		int answer = wxMessageBox(_("your work is modified, would you like to save it?"),_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION,mafGetFrame()); // ask user if will save msf before closing
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
