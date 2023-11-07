/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEManager
 Authors: Silvano Imboden
 
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


#include "albaVMEManager.h"

#include <wx/busyinfo.h>
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>
#include <wx/dir.h>
#include <wx/filename.h>

#include "albaDecl.h"
#include "albaVME.h"
#include "albaVMEStorage.h"
#include "albaVMEGenericAbstract.h"
#include "albaDataVector.h"

#include "albaVMEIterator.h"
#include "albaTagArray.h"

#include <fstream>

//----------------------------------------------------------------------------
albaVMEManager::albaVMEManager()
//----------------------------------------------------------------------------
{
  m_Modified    = false;
	m_MakeBakFile = true;
	m_Listener    = NULL;
	m_Storage     = NULL;
  m_Crypting    = false;
  m_LoadingFlag = false;
  m_FileHistoryIdx = -1;

	m_MSFDir   = albaGetLastUserFolder();
	m_MSFFile  = "";
	m_ZipFile  = "";
  m_TmpDir   = "";
  m_MsfFileExtension = "msf";

  m_SingleBinaryFile = false;

  m_Config = wxConfigBase::Get();
  m_ZipHandler = NULL;
  m_FileSystem = NULL;

	m_IgnoreAppStamp = false;
  
	m_TestMode = false;
}
//----------------------------------------------------------------------------
albaVMEManager::~albaVMEManager()
//----------------------------------------------------------------------------
{
  if(m_Storage) 
    NotifyRemove( m_Storage->GetRoot() ); // //SIL. 11-4-2005:  - cast root to node -- maybe to be removed

  m_AppStamp.clear();
  m_Listener = NULL;
  m_FileSystem->CleanUpHandlers(); // Handlers are shared trough file systems.
  cppDEL(m_FileSystem);
  
  albaDEL(m_Storage);
  cppDEL(m_Config);
}
//----------------------------------------------------------------------------
void albaVMEManager::SetSingleBinaryFile(bool singleFile)
//----------------------------------------------------------------------------
{
  m_SingleBinaryFile = singleFile;
}
//----------------------------------------------------------------------------
void albaVMEManager::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (alba_event->GetChannel()==MCH_UP)
  {
    if (alba_event->GetId() == albaDataVector::GetSingleFileDataId())
    {
      ((albaEvent *)alba_event)->SetBool(m_SingleBinaryFile);
      return;
    }
    // events coming from the tree...
    switch (alba_event->GetId())
    {
      case NODE_ATTACHED_TO_TREE:
      {
        if (!m_LoadingFlag)
        {
          NotifyAdd((albaVME *)alba_event->GetSender());
          MSFModified(true);
        }
      }
      break;
      case NODE_DETACHED_FROM_TREE:
      {
        if (!m_LoadingFlag)
        {
          NotifyRemove((albaVME *)alba_event->GetSender());
          MSFModified(true);
        }
      }
      break;
      default:
        albaEventMacro(*alba_event);
    }
  }
}

//----------------------------------------------------------------------------
albaVMERoot *albaVMEManager::GetRoot()
//----------------------------------------------------------------------------
{
  return (m_Storage?m_Storage->GetRoot():NULL);
}
//----------------------------------------------------------------------------
albaVMEStorage *albaVMEManager::GetStorage()
//----------------------------------------------------------------------------
{
  return m_Storage;
}
//----------------------------------------------------------------------------
void albaVMEManager::SetApplicationStamp(albaString &appstamp)
//----------------------------------------------------------------------------
{
  // Add a single application stamp; this is done automatically while creating the application with the application name
  m_AppStamp.push_back(appstamp);
}
//----------------------------------------------------------------------------
void albaVMEManager::SetApplicationStamp(std::vector<albaString> appstamp)
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
void albaVMEManager::RemoveTempDirectory()
//----------------------------------------------------------------------------
{
  if (m_TmpDir != "")
  {
    wxString working_dir;
    working_dir = albaGetAppDataDirectory().char_str();
    wxSetWorkingDirectory(working_dir);
    if(::wxDirExists(m_TmpDir.GetCStr())) //remove tmp directory due to zip extraction or compression
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
void albaVMEManager::MSFNew()
//----------------------------------------------------------------------------
{
  if (m_Storage)
  {
    m_Storage->GetRoot()->CleanTree();
    albaEventMacro(albaEvent(this,CLEAR_UNDO_STACK)); // ask logic to clear the undo stack
  }

  RemoveTempDirectory(); // remove the temporary directory

  m_Modified = false;

  m_LoadingFlag = true; // set the loading flag to prevent the manager
                        // listen to NODE_ATTACHED_TO_TREE and NODE_DETACHED_TO_TREE events

  albaEvent e(this,CREATE_STORAGE,m_Storage); // ask logic to create a new storage
  albaEventMacro(e);
  m_Storage = (albaVMEStorage *)e.GetMafObject();
  m_LoadingFlag = false;
	  
	//Add the application stamps
	albaTagItem tag_appstamp;
	tag_appstamp.SetName("APP_STAMP");
	tag_appstamp.SetValue(this->m_AppStamp.at(0).GetCStr());
	m_Storage->GetRoot()->GetTagArray()->SetTag(tag_appstamp); // set the appstamp tag for the root
  AddCreationDate(m_Storage->GetRoot());
	albaEventMacro(albaEvent(this,VME_ADDED,m_Storage->GetRoot())); // raise notification events
	albaEventMacro(albaEvent(this,VME_SELECTED,m_Storage->GetRoot()));

  m_MSFFile = ""; //next MSFSave will ask for a filename
  m_ZipFile = ""; 
}

//----------------------------------------------------------------------------
void albaVMEManager::AddCreationDate(albaVME *vme)
//----------------------------------------------------------------------------
{
  wxString dateAndTime;
  wxDateTime time = wxDateTime::UNow(); // get time with millisecond precision
  dateAndTime  = albaString::Format("%02d/%02d/%02d %02d:%02d:%02d",time.GetDay(), time.GetMonth()+1, time.GetYear(), time.GetHour(), time.GetMinute(),time.GetSecond());
 
  albaTagItem tag_creationDate;
  tag_creationDate.SetName("Creation_Date");
  tag_creationDate.SetValue(dateAndTime);
  vme->GetTagArray()->SetTag(tag_creationDate); // set creation date tag for the specified vme
}

//----------------------------------------------------------------------------
int  albaVMEManager::MSFOpen(int file_id)
//----------------------------------------------------------------------------
{
  m_FileHistoryIdx = file_id - wxID_FILE1;
	albaString file = m_FileHistory.GetHistoryFile(m_FileHistoryIdx); // get the filename from history
	return MSFOpen(file);
}
//----------------------------------------------------------------------------
int albaVMEManager::MSFOpen(albaString filename)
//----------------------------------------------------------------------------
{
  wxWindowDisabler *disableAll;
  wxBusyCursor *wait_cursor;

  if(!m_TestMode) // Losi 02/16/2010 for test class
  {
    disableAll = new wxWindowDisabler();
    wait_cursor = new wxBusyCursor();
  }
   
  // open a local msf
  if(!wxFileExists(filename.GetCStr()))
	{
		albaString msg;
    msg = _("File ");
		msg << filename;
		msg << _(" not found!");
		albaWarningMessage(msg, _("Warning"));
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
		return ALBA_ERROR; 
	}

  MSFNew(); // insert and select the root - reset m_MSFFile - delete the old storage and create a new one
  
  albaString unixname = filename;
  
  wxString path, name, ext;
  wxFileName::SplitPath(filename.GetCStr(),&path,&name,&ext);

	if(ext.IsSameAs("z" + m_MsfFileExtension,true)) 
  {
    unixname = ZIPOpen(filename); // open the zmsf archive and extract it to the temporary directory
    if(unixname.IsEmpty())
    {
    	albaMessage(_("Bad or corrupted z" + m_MsfFileExtension + " file!"));
      m_Modified = false;
      m_Storage->Delete();
      m_Storage = NULL;
      MSFNew();
      if(!m_TestMode) // Losi 02/16/2010 for test class
      {
        cppDEL(disableAll);
        cppDEL(wait_cursor);
      }
      return ALBA_ERROR;
    }
    wxSetWorkingDirectory(m_TmpDir.GetCStr());
  }
  
  // convert to unix format
  albaString sub_unixname;
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
  if (res != ALBA_OK)
  {
    albaErrorMessage(_("Errors during file parsing! Look the log area for error messages.")); // if some problems occurred during import give feedback to the user
  }
  m_LoadingFlag = false;

  albaTimeStamp b[2];
  albaVMERoot *root_node = m_Storage->GetRoot();
  root_node->GetOutput()->GetTimeBounds(b);
  root_node->SetTreeTime(b[0]); // Set tree time to the starting time
  
	////////////////////////////////  Application Stamp managing ////////////////////
	if(!root_node->GetTagArray()->IsTagPresent("APP_STAMP"))
	{
		//update the old data files to support Application Stamp
		albaTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.at(0).GetCStr());
		root_node->GetTagArray()->SetTag(tag_appstamp); // set appstamp tag of the root
	}

	albaString app_stamp;
  app_stamp << root_node->GetTagArray()->GetTag("APP_STAMP")->GetValue();
  // First check for compatibility with all stored App stamps
  bool stamp_found = false;
  for (int k=0; k<m_AppStamp.size(); k++)
  {
    // Check with the Application name
    if (app_stamp.Equals(m_AppStamp.at(k).GetCStr()))
    {
      stamp_found = true;
    }
   
  }
	if((app_stamp.Equals("INVALID") || !stamp_found) && !m_IgnoreAppStamp) 
	{
		int answer = wxMessageBox("This file was not created under this application\nYou can lose data.\n\nAre you sure to open this file?","Confirm", wxYES_NO|wxICON_QUESTION, albaGetFrame());
		if(answer != wxYES)
		{
			//Application stamp not valid
			m_Modified = false;
    
			MSFNew();
			if(!m_TestMode) // Losi 02/16/2010 for test class
			{
				cppDEL(disableAll);
				cppDEL(wait_cursor);
			}
			return ALBA_ERROR;
		}
	}
	///////////////////////////////////////////////////////////////////////////////// 
  NotifyAdd(root_node); // add the storage root (the tree) with events notification

	albaEventMacro(albaEvent(this,VME_SELECTED, root_node)); // raise notification events (to logic)
  albaEventMacro(albaEvent(this,CAMERA_RESET));
  
	if (m_TmpDir != "")
	{
    m_FileHistory.AddFileToHistory(m_ZipFile.GetCStr()); // add the zmsf file to the history
	}
  else
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.GetCStr()); // add the msf file to the history
  }

	m_FileHistory.Save(*m_Config); // save file history to registry
  m_FileHistoryIdx = -1;

  if(!m_TestMode) // Losi 02/16/2010 for test class
  {
    cppDEL(disableAll);
    cppDEL(wait_cursor);
  }
  return res;
}
//----------------------------------------------------------------------------
const char *albaVMEManager::ZIPOpen(albaString filename)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  
  m_ZipFile = filename;
  albaString zip_cache = wxPathOnly(filename.GetCStr()); // get the directory
  if (zip_cache.IsEmpty())
  {
    zip_cache = m_Storage->GetTmpFolder();
  }
  
  zip_cache += "/";
  zip_cache = wxFileName::CreateTempFileName(zip_cache.GetCStr()); // used to get a valid temporary name for cache directory
  wxRemoveFile(zip_cache.GetCStr());
  wxFileName::SplitPath(zip_cache.GetCStr(),&path,&name,&ext);
  zip_cache = path + "/" + name + ext;
  zip_cache.ParsePathName();
  
  if (!wxDirExists(zip_cache.GetCStr()))
    wxMkdir(zip_cache.GetCStr()); // create a temporary directory in which extract the archive
  m_TmpDir = zip_cache;

  wxString complete_name, zfile, out_file;
  wxFileName::SplitPath(m_ZipFile.GetCStr(),&path,&name,&ext);
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
    m_ZipHandler = new wxArchiveFSHandler();
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
  wxFileName::SplitPath(zfile,&path,&name,&ext);
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

  //if(ext == "msf")
  if(ext.IsSameAs(m_MsfFileExtension,true)) 
  {
    m_MSFFile = out_file; // the file to extract is the msf
    out_file_stream.open(out_file.char_str(), std::ios_base::out);
  }
  else
  {
    out_file_stream.open(out_file.char_str(), std::ios_base::binary); // the file to extract is a binary
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
    wxFileName::SplitPath(zfile,&path,&name,&ext);
    complete_name = name + "." + ext;
    if (enable_mid)
      complete_name = complete_name.Mid(length_header_name);
    out_file = m_TmpDir + "\\" + complete_name;
    //if(ext == "msf")
	if(ext.IsSameAs(m_MsfFileExtension,true))
    {
      m_MSFFile = out_file; // The file to extract is an msf
      out_file_stream.open(out_file.char_str(), std::ios_base::out);
    }
    else
      out_file_stream.open(out_file.char_str(), std::ios_base::binary); // The file to extract is a binary
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
    albaMessage(_("compressed archive is not a valid " + m_MsfFileExtension + " file!"), _("Error"));
    return "";
  }

  // return the extracted msf filename
  return m_MSFFile.GetCStr();
}
//----------------------------------------------------------------------------
void albaVMEManager::ZIPSave(albaString filename)
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
    albaMessage(_("Failed to create compressed archive!"),_("Error"));
  }
}
//----------------------------------------------------------------------------
bool albaVMEManager::MakeZip(const albaString &zipname, wxArrayString *files)
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
    wxFileName::SplitPath(name, &path, &short_name, &ext);
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
int albaVMEManager::MSFSave()
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  int ret=ALBA_OK;

	bool fromDifferentApp = false;
	if (m_Storage)
	{
		albaVMERoot *root_node = m_Storage->GetRoot();
		albaString app_stamp;
		app_stamp << root_node->GetTagArray()->GetTag("APP_STAMP")->GetValue();
		fromDifferentApp = !app_stamp.Equals(m_AppStamp.at(0).GetCStr());
	}

  if(m_MSFFile.IsEmpty() || fromDifferentApp) 
  {
    // new file to save: ask to the application which is the default
    // modality to save binary files.
    albaEvent e(this,albaDataVector::GetSingleFileDataId());
    albaEventMacro(e);
    SetSingleBinaryFile(e.GetBool()); // set the save modality for time-varying vme
    
    // ask for the new file name.
    wxString wildc = _("Project file (*."+ m_MsfFileExtension +")|*."
		              + m_MsfFileExtension +"|Compressed file (*.z"+ m_MsfFileExtension +")|*.z" + m_MsfFileExtension + "");
    albaString file = albaGetSaveFile(m_MSFDir, wildc.char_str());
    if(file.IsEmpty())
      return ALBA_ERROR;
   
    wxString path, name, ext, file_dir;
    wxFileName::SplitPath(file.GetCStr(),&path,&name,&ext);

		if (!wxFileExists(file.GetCStr()))
		{
			file_dir = path + "/" + name;  //Put the file in a folder with the same name
			if (!wxDirExists(file_dir))
				wxMkdir(file_dir);

			if (ext.IsSameAs("z" + m_MsfFileExtension, true))
			{
				m_ZipFile = file;
				m_TmpDir = path + "/" + name;
				//ext = "msf";
				ext = m_MsfFileExtension;
			}
			file = file_dir + "/" + name + "." + ext;
		}

    // convert to unix format
    albaString sub_unixname;
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
    albaString bak_filename = m_MSFFile + ".bak";                // create the backup for the saved msf
    wxRenameFile(m_MSFFile.GetCStr(), bak_filename.GetCStr());  // renaming the founded one
	}
	
  if(!m_TestMode) 
    wait=new wxBusyInfo(_("Saving Project: Please wait"));
  
	//Update the application stamps
	albaTagItem tag_appstamp;
	tag_appstamp.SetName("APP_STAMP");
	tag_appstamp.SetValue(this->m_AppStamp.at(0).GetCStr());
	m_Storage->GetRoot()->GetTagArray()->SetTag(tag_appstamp); // set the appstamp tag for the root
	AddCreationDate(m_Storage->GetRoot());

  m_Storage->SetURL(m_MSFFile.GetCStr());
  if (m_Storage->Store() != ALBA_OK) // store the tree
  {
    ret=false;
    albaLogMessage(_("Error during saving"));
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

  if(!m_TestMode)
    delete wait;

  return ret;
}
//----------------------------------------------------------------------------
int albaVMEManager::MSFSaveAs()   
//----------------------------------------------------------------------------
{
  int retValue;
  albaString oldFileName;
  oldFileName=m_MSFFile;
  m_MSFFile = ""; // set filenames to empty so the MSFSave method will ask for them
  m_ZipFile = "";
  m_MakeBakFile = false;
  retValue=MSFSave();
  //if the user cancel save operation the name will be empty 
  if (m_MSFFile=="")
    m_MSFFile=oldFileName;
  
  return retValue;
}

//----------------------------------------------------------------------------
void albaVMEManager::VmeAdd(albaVME *vme)
//----------------------------------------------------------------------------
{
  if(vme != NULL)
  {
    // check the node's parent
    albaVME *vp = vme->GetParent();  
    assert( vp == NULL || m_Storage->GetRoot()->IsInTree(vp) );
    if(vp == NULL) 
			vme->ReparentTo(m_Storage->GetRoot()); // reparent the node to the root

    m_Modified = true;
  }
}
//----------------------------------------------------------------------------
void albaVMEManager::VmeRemove(albaVME *vme)
//----------------------------------------------------------------------------
{
  if(vme != NULL && m_Storage->GetRoot() /*&& m_Storage->GetRoot()->IsInTree(n)*/) 
  {
    assert(m_Storage->GetRoot()->IsInTree(vme));
    vme->ReparentTo(NULL);
    m_Modified = true;
  }
}
//----------------------------------------------------------------------------
void albaVMEManager::TimeSet(double time)
//----------------------------------------------------------------------------
{
  if(m_Storage->GetRoot()) m_Storage->GetRoot()->SetTreeTime(time); // set the tree time
}
//----------------------------------------------------------------------------
void albaVMEManager::TimeGetBounds(double *min, double *max)
//----------------------------------------------------------------------------
{
  albaTimeStamp b[2];
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
void albaVMEManager::NotifyRemove(albaVME *vme)
//----------------------------------------------------------------------------
{
  albaVMEIterator *iter = vme->NewIterator();
  iter->IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
  iter->SetTraversalModeToPostOrder(); // traverse is: first the subtree left to right, then the root
  for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		albaEventMacro(albaEvent(this,VME_REMOVING,node)); // raise notification event (to logic)
  iter->Delete();
}
//----------------------------------------------------------------------------
void albaVMEManager::NotifyAdd(albaVME *vme)
//----------------------------------------------------------------------------
{
  bool checkSingleFile = vme->IsALBAType(albaVMERoot);
  
  albaVMEIterator *iter = vme->NewIterator();
  iter->IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
  for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    albaVMERoot *root = GetRoot();
    if (root != NULL)
    {
      if (node)
      {
        // Update the new VME added to the tree with the current time-stamp
        // present in the tree.
        node->SetTimeStamp(root->GetTimeStamp());
      }
    }

    albaEventMacro(albaEvent(this,VME_ADDED,node)); // raise notification event (to logic)
    if (checkSingleFile)
    {
      // if checkSingleFile == true this method is called by the MSFOpen
      // so we have to check if the msf has been saved in single binary file or not.
      albaVMEGenericAbstract *vmeWithDataVector = albaVMEGenericAbstract::SafeDownCast(node);
      if (vmeWithDataVector)
      {
        albaDataVector *dv = vmeWithDataVector->GetDataVector();
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
void albaVMEManager::SetFileHistoryMenu(wxMenu *menu)
//----------------------------------------------------------------------------
{
  m_FileHistory.UseMenu(menu);
	m_FileHistory.Load(*m_Config); // Loads file history from registry
}
//----------------------------------------------------------------------------
bool albaVMEManager::AskConfirmAndSave()
//----------------------------------------------------------------------------
{
  bool go = true;
	if (m_Modified) // check if the msf has been modified
	{
		int answer = wxMessageBox(_("Your work is modified, would you like to save it?"),_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION,albaGetFrame()); // ask user if will save msf before closing
		if(answer == wxCANCEL) go = false;
		if(answer == wxYES)    MSFSave();
	}
	return go;
}

