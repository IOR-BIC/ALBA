/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaZipUtility
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <wx/zipstrm.h>
#include <wx/wfstream.h>

#include "albaZipUtility.h"
#include "albaString.h"

#include <fstream>
#include "wx/filesys.h"
#include "wx/zstream.h"
#include "wx/fs_zip.h"


//----------------------------------------------------------------------------
std::vector<albaString> ZIPOpen(albaString file)
//----------------------------------------------------------------------------
{
  std::vector<albaString> filesCreated;

  albaString ZipFile,tmpDir,MSFFile;
  ZipFile = file;
  albaString zip_cache = wxPathOnly(file.GetCStr());
  if (zip_cache.IsEmpty())
  {
    return filesCreated;
  }

  if (!wxDirExists(zip_cache.GetCStr()))
    wxMkdir(zip_cache.GetCStr());
  tmpDir = zip_cache;

  wxString path, name, ext, complete_name, zfile, out_file;
	wxFileName::SplitPath(ZipFile.GetCStr(),&path,&name,&ext);
	complete_name = name + "." + ext;

	wxFSFile *zfileStream;
	wxZlibInputStream *zip_is;
	wxString pkg = "#zip:";
	wxString header_name = complete_name + pkg;
	int length_header_name = header_name.Length();
	bool enable_mid = false;
	
	wxFileSystem *fileSystem = new wxFileSystem();
	wxZipFSHandler *zipHandler = new wxZipFSHandler();

	fileSystem->AddHandler(zipHandler); // add the handler that manage zip protocol

	fileSystem->ChangePathTo(ZipFile.GetCStr());
	// extract filename from the zip archive
	zfile = fileSystem->FindFirst(complete_name + pkg + name + "\\*.*");

	if (zfile == "")
	{
		enable_mid = true;
		// no files found: try to search inside the archive without filename
		zfile = fileSystem->FindFirst(complete_name + pkg + "\\*.*");
	}
	if (zfile == "")
	{
		// no files found inside the archive
		return filesCreated;
	}

	wxFileName::SplitPath(zfile, &path, &name, &ext);
	complete_name = name + "." + ext;

	if (enable_mid)
		complete_name = complete_name.Mid(length_header_name);
	
	zfileStream = fileSystem->OpenFile(zfile);
	if (zfileStream == NULL) // unable to open the file
	{
		return filesCreated;
	}

	zip_is = (wxZlibInputStream *)zfileStream->GetStream();
	out_file = tmpDir + "\\" + complete_name;
	char *buf;
	int s_size;
	std::ofstream out_file_stream;

	out_file_stream.open(out_file.char_str(), std::ios_base::binary);

	s_size = zip_is->GetSize();
	buf = new char[s_size];
	zip_is->Read(buf, s_size);
	out_file_stream.write(buf, s_size);

	filesCreated.push_back(name);

	out_file_stream.close();
	delete[] buf;

	zfileStream->UnRef();
	delete zfileStream;

	while ((zfile = fileSystem->FindNext()) != "")
	{
		zfileStream = fileSystem->OpenFile(zfile);

		zip_is = (wxZlibInputStream *)zfileStream->GetStream();
		wxFileName::SplitPath(zfile, &path, &name, &ext);
		complete_name = name + "." + ext;
		if (enable_mid)
			complete_name = complete_name.Mid(length_header_name);
		out_file = tmpDir + "\\" + complete_name;

		out_file_stream.open(out_file.char_str(), std::ios_base::binary); // The file to extract is a binary
		s_size = zip_is->GetSize();
		buf = new char[s_size];
		zip_is->Read(buf, s_size);
		out_file_stream.write(buf, s_size);

		filesCreated.push_back(name);

		out_file_stream.close();
		delete[] buf;
		zfileStream->UnRef();
		delete zfileStream;
	}

	fileSystem->ChangePathTo(tmpDir.GetCStr(), TRUE);

  return filesCreated;
}