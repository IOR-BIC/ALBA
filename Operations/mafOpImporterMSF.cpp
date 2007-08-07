/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterMSF.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-07 14:32:44 $
  Version:   $Revision: 1.4 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpImporterMSF.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/fs_zip.h>

#include "mafEvent.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGroup.h"

#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterMSF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterMSF::mafOpImporterMSF(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Group   = NULL;
}
//----------------------------------------------------------------------------
mafOpImporterMSF::~mafOpImporterMSF()
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterMSF::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterMSF *cp = new mafOpImporterMSF(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterMSF::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    mafString fileDir = mafGetApplicationDirectory().c_str();
    mafString wildc  = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
    m_File = mafGetOpenFile(fileDir, wildc, _("Choose MSF file")).c_str();
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty()) 
  {
    if (ImportMSF() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
  }
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
int mafOpImporterMSF::ImportMSF()
//----------------------------------------------------------------------------
{
  mafString unixname = m_File;
  wxString path, name, ext;
  wxSplitPath(m_File.GetCStr(),&path,&name,&ext);

  if(ext == "zmsf")
  {
    unixname = ZIPOpen(m_File);
    if(unixname.IsEmpty())
    {
      if (!m_TestMode)
        mafMessage(_("Bad or corrupted zmsf file!"));
      return MAF_ERROR;
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
  mafVMEStorage *storage;
  storage = mafVMEStorage::New();
  storage->SetURL(m_MSFFile.GetCStr());
  mafVMERoot *root;
  root = storage->GetRoot();
  root->Initialize();
  root->SetName("RootB");

  int res = storage->Restore();
  if (res != MAF_OK)
  {
    // if some problems occurred during import give feedback to the user
    if (!m_TestMode)
      mafErrorMessage(_("Errors during file parsing! Look the log area for error messages."));
    //return MAF_ERROR;
  }
      
  wxString group_name = wxString::Format("imported from %s.%s",name,ext);
 
  mafNEW(m_Group);
  m_Group->SetName(group_name);
  
  while (mafNode *node = root->GetFirstChild())
  {
    node->ReparentTo(m_Group);
  }
  m_Group->Update();
  m_Output = m_Group;

  mafDEL(storage);
  return MAF_OK;
}
//----------------------------------------------------------------------------
const char *mafOpImporterMSF::ZIPOpen(mafString m_File)
//----------------------------------------------------------------------------
{
  mafString ZipFile;
  ZipFile = m_File;
  mafString zip_cache = wxPathOnly(m_File.GetCStr());
  if (zip_cache.IsEmpty())
  {
    zip_cache = ::wxGetCwd();
  }
  zip_cache = zip_cache + "\\~TmpData";
  if (!wxDirExists(zip_cache.GetCStr()))
    wxMkdir(zip_cache.GetCStr());
  m_TmpDir = zip_cache;

  wxString path, name, ext, complete_name, zfile, out_file;
  wxSplitPath(ZipFile.GetCStr(),&path,&name,&ext);
  complete_name = name + "." + ext;

  wxFSFile *zfileStream;
  wxZlibInputStream *zip_is;
  wxString pkg = "#zip:";
  wxString header_name = complete_name + pkg;
  int length_header_name = header_name.Length();
  bool enable_mid = false;
  wxFileSystem *zip_fs = new wxFileSystem();
  zip_fs->AddHandler(new wxZipFSHandler);
  zip_fs->ChangePathTo(ZipFile.GetCStr());
  // extract m_File from the zip archive
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
    if (!m_TestMode)
      mafMessage(_("compressed archive is not a valid msf file!"), _("Error"));
    return "";
  }

  return m_MSFFile.GetCStr();
}
//----------------------------------------------------------------------------
void mafOpImporterMSF::RemoveTempDirectory()
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
