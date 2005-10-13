/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-13 13:39:05 $
  Version:   $Revision: 1.18 $
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
#include <wx/fs_zip.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>

#include "mafDecl.h"
#include "mafVMEManager.h"
#include "mafNode.h"
#include "mafVMEStorage.h"
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

  mafString MSFDir = mafGetApplicationDirectory().c_str();
  MSFDir.ParsePathName();
	m_MSFDir   = MSFDir;
  m_MSFDir   += "/Data/MSF/";
	m_MSFFile   = "";
	m_ZipFile   = "";
  m_TmpDir   = "";
	m_MergeFile = "";
	m_Wildcard  = "Multimod Storage Format file |*.msf;*.zmsf";

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
          NotifyAdd((mafNode *)maf_event->GetSender());
      }
      break;
      case NODE_DETACHED_FROM_TREE:
      {
        if (!m_LoadingFlag)
          NotifyRemove((mafNode *)maf_event->GetSender());
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
void mafVMEManager::CreateNewStorage()
//----------------------------------------------------------------------------
{
  if (m_Storage)
  {
    NotifyRemove(m_Storage->GetRoot());
    m_LoadingFlag = true;
    m_Storage->Delete();
    m_LoadingFlag = false;
  }
  m_Storage = mafVMEStorage::New();
  m_Storage->GetRoot()->SetName("root");
  m_Storage->SetListener(this);
  m_Storage->GetRoot()->Initialize();
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
  
  CreateNewStorage();

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
  int idx = file_id - wxID_FILE1;
	wxString file = "";
	file = m_FileHistory.GetHistoryFile(idx);
	MSFOpen(file);
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFOpen(wxString filename)   
//----------------------------------------------------------------------------
{
  wxWindowDisabler disableAll;
  wxBusyCursor wait_cursor;
  
  if(!::wxFileExists(filename))
	{
		wxString msg("File ");
		msg += filename;
		msg += " not found!";
		mafWarningMessage( msg,"Warning");
		return;
	}

  // insert and select the root - reset m_MSFFile 
  MSFNew(false);
  
  mafString unixname = filename;

  wxString path, name, ext;
  wxSplitPath(filename,&path,&name,&ext);
  if(ext == "zmsf")
  {
    unixname = ZIPOpen(filename);
    wxSetWorkingDirectory(m_TmpDir);
  }
  
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
  else
  {
    m_FileHistory.AddFileToHistory(m_MSFFile);
  }
	m_FileHistory.Save(*m_Config);
}
//----------------------------------------------------------------------------
const char *mafVMEManager::ZIPOpen(wxString filename)
//----------------------------------------------------------------------------
{
  m_ZipFile = filename;
  wxString file, ext, working_dir = "";
  wxSplitPath(m_ZipFile,&working_dir,&file,&ext);
  if (working_dir == "")
    working_dir = ::wxGetCwd();
  
  working_dir = working_dir + "\\~TmpData";
  if (!wxDirExists(working_dir))
    wxMkdir(working_dir);
  m_TmpDir = working_dir;

  wxString path, name, complete_name, zfile, out_file;
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
  out_file = working_dir + "\\" + complete_name;
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
      m_MSFFile = out_file;
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
  
  zip_fs->ChangePathTo(working_dir,TRUE);
  zip_fs->CleanUpHandlers();
  zfileStream->UnRef();
  delete zfileStream;
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
/* Not working with 2.4.2 (wxZipOutputStream is available only on wxWidgets 2.6.2)
  if (filename != "")
  {
    m_ZipFile = filename;
  }
  if (m_ZipFile == "")
  {
    return;
  }

  wxString path, name, ext, complete_name, zfile, input_file;
  wxSplitPath(m_ZipFile,&path,&name,&ext);
  complete_name = name + "." + ext + "#zip";

  wxFileOutputStream fileOutStream(m_ZipFile);
  wxZlibOutputStream zipostr(fileOutStream);

  char *buf;
  int s_size;
  wxFileInputStream infile(m_MSFFile);
  s_size = infile.StreamSize();
  buf = new char[s_size];
  infile.Read(buf,s_size);
  zipostr.Write(buf,s_size);
  zipostr.GetFilterOutputStream()->Sync();
  delete[] buf;
*/
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFSave()
//----------------------------------------------------------------------------
{
  if(m_MSFFile == "") 
  {
    mafString save_wildcard  = "Multimod Storage Format file |*.msf";
    mafString file = mafGetSaveFile(m_MSFDir, save_wildcard.GetCStr()).c_str();
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
        m_TmpDir = path;
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
    wxLogMessage("Error during MSF saving");
  }
  if (m_TmpDir != "")
  {
    wxString path, name, ext, file_dir;
    wxSplitPath(m_MSFFile,&path,&name,&ext);
    ext = "zmsf";
    m_ZipFile = m_TmpDir + "/" + name + "." + ext;
    ZIPSave();
    m_FileHistory.AddFileToHistory(m_ZipFile);
  }
  else
  {
    m_FileHistory.AddFileToHistory(m_MSFFile );
  }
	m_FileHistory.Save(*m_Config);
  m_Modified = false;
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFSaveAs()   
//----------------------------------------------------------------------------
{
   m_MSFFile = "";
   MSFSave();
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
