/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMSF
 Authors: Roberto Mucci
 
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

#include "albaOpImporterMSF.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/fs_zip.h>

#include "albaEvent.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEGroup.h"
#include "albaVMEGeneric.h"
#include "albaDataVector.h"
#include "albaAbsLogicManager.h"

#include "vtkALBASmartPointer.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterMSF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterMSF::albaOpImporterMSF(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Group   = NULL;
}
//----------------------------------------------------------------------------
albaOpImporterMSF::~albaOpImporterMSF()
//----------------------------------------------------------------------------
{
  albaDEL(m_Group);
  RemoveTempDirectory();
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterMSF::Copy()   
//----------------------------------------------------------------------------
{
  albaOpImporterMSF *cp = new albaOpImporterMSF(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterMSF::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    albaString fileDir = albaGetLastUserFolder();
		
		albaString wildc;
		const	char *ext = GetLogicManager()->GetMsfFileExtension();

		wildc = albaString::Format("All Supported File (*.%s;*.z%s;msf;zmsf)|*.%s;*.%s;*.msf;*.zmsf", ext, ext, ext, ext);
		wildc += albaString::Format("|Alba Project File (*.%s)|*.%s", ext, ext);
		wildc += albaString::Format("|Alba Compressed File (*.z%s)|*.z%s", ext, ext);
		wildc += albaString::Format("|MAF Project File (*.msf)|*.msf");
		wildc += albaString::Format("|MAF Compressed File (*.zmsf)|*.zmsf", ext, ext);

    m_File = albaGetOpenFile(fileDir, wildc, _("Choose Project file"));
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty()) 
  {
    if (ImportMSF() == ALBA_OK)
    {
      result = OP_RUN_OK;
    }
  }
  albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
int albaOpImporterMSF::ImportMSF()
//----------------------------------------------------------------------------
{
  albaString unixname = m_File;
  wxString path, name, ext;
  wxFileName::SplitPath(m_File.GetCStr(),&path,&name,&ext);

	albaString zippedExt;
	const char *archExt = GetLogicManager()->GetMsfFileExtension();
	zippedExt.Printf("z%s", archExt);

  if(ext == zippedExt.GetCStr())
  {
    unixname = ZIPOpen(m_File);
    if(unixname.IsEmpty())
    {
      if (!m_TestMode)
        albaMessage(_("Bad or corrupted file!"));
      return ALBA_ERROR;
    }
    wxSetWorkingDirectory(m_TmpDir.GetCStr());
  }

  albaString sub_unixname;
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
  albaVMEStorage *storage;
  storage = albaVMEStorage::New();
  storage->SetURL(m_MSFFile.GetCStr());
  albaVMERoot *root;
  root = storage->GetRoot();
  root->Initialize();
  root->SetName("RootB");

  int res = storage->Restore();
  if (res != ALBA_OK)
  {
    // if some problems occurred during import give feedback to the user
    if (!m_TestMode)
      albaErrorMessage(_("Errors during file parsing! Look the log area for error messages."));
    //return ALBA_ERROR;
  }
      
  wxString group_name = albaString::Format("imported from %s.%s",name.ToAscii(),ext.ToAscii());
 
  albaNEW(m_Group);
  m_Group->SetName(group_name);
  m_Group->ReparentTo(m_Input);
  
  while (albaVME *node = root->GetFirstChild())
  {
    node->ReparentTo(m_Group);

    // Losi 03/16/2010 Bug #2049 fix
    albaVMEGeneric *vme = albaVMEGeneric::SafeDownCast(node);
    if(vme)
    {
      // Update data vector id to avoid duplicates
      albaDataVector *dataVector = vme->GetDataVector();
      if(dataVector)
      {
        dataVector->UpdateVectorId();
      }
    }
  }
  m_Group->Update();
  m_Output = m_Group;

  albaDEL(storage);
  return ALBA_OK;
}
//----------------------------------------------------------------------------
const char *albaOpImporterMSF::ZIPOpen(albaString m_File)
//----------------------------------------------------------------------------
{
  albaString ZipFile;
  ZipFile = m_File;
  albaString zip_cache = wxPathOnly(m_File.GetCStr());
  if (zip_cache.IsEmpty())
  {
    zip_cache = albaGetAppDataDirectory();
  }
  zip_cache = zip_cache + "\\~TmpData";
  if (!wxDirExists(zip_cache.GetCStr()))
    wxMkdir(zip_cache.GetCStr());
  m_TmpDir = zip_cache;

  wxString path, name, ext, complete_name, zfile, out_file;
  wxFileName::SplitPath(ZipFile.GetCStr(),&path,&name,&ext);
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
    cppDEL(zip_fs);
    RemoveTempDirectory();
    return "";
  }
  wxFileName::SplitPath(zfile,&path,&name,&ext);
  complete_name = name + "." + ext;
  if (enable_mid)
    complete_name = complete_name.Mid(length_header_name);
  zfileStream = zip_fs->OpenFile(zfile);
  if (zfileStream == NULL)
  {
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
    out_file_stream.open(out_file.ToAscii(), std::ios_base::out);
  }
  else
  {
    out_file_stream.open(out_file.ToAscii(), std::ios_base::binary);
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
      cppDEL(zip_fs);
      RemoveTempDirectory();
      return "";
    }
    zip_is = (wxZlibInputStream *)zfileStream->GetStream();
    wxFileName::SplitPath(zfile,&path,&name,&ext);
    complete_name = name + "." + ext;
    if (enable_mid)
      complete_name = complete_name.Mid(length_header_name);
    out_file = m_TmpDir + "\\" + complete_name;
    if(ext == "msf")
    {
      m_MSFFile = out_file;
      out_file_stream.open(out_file.ToAscii(), std::ios_base::out);
    }
    else
      out_file_stream.open(out_file.ToAscii(), std::ios_base::binary);
    s_size = zip_is->GetSize();
    buf = new char[s_size];
    zip_is->Read(buf,s_size);
    out_file_stream.write(buf, s_size);
    out_file_stream.close();
    delete[] buf;
    zfileStream->UnRef();
    delete zfileStream;
  }

  zip_fs->ChangePathTo(m_TmpDir.GetCStr(), true);
  cppDEL(zip_fs);

  if (m_MSFFile == "")
  {
    if (!m_TestMode)
      albaMessage(_("compressed archive is not a valid msf file!"), _("Error"));
    return "";
  }

  return m_MSFFile.GetCStr();
}
//----------------------------------------------------------------------------
void albaOpImporterMSF::RemoveTempDirectory()
//----------------------------------------------------------------------------
{
  if (m_TmpDir != "")
  {
    wxString working_dir;
    working_dir = albaGetAppDataDirectory().ToAscii();
    wxSetWorkingDirectory(working_dir);
    //remove tmp directory due to zip extraction or compression
    if(::wxDirExists(m_TmpDir.GetCStr()))
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
char ** albaOpImporterMSF::GetIcon()
{
#include "pic/MENU_IMPORT_ALBA.xpm"
	return MENU_IMPORT_ALBA_xpm;
}
