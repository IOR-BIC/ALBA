/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoSTLImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-11-05 10:39:10 $
  Version:   $Revision: 1.13 $
  Authors:   Paolo Quadrani
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

#include "mmoSTLImporter.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"

#include "vtkSTLReader.h"
#include "vtkPolyData.h"

#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoSTLImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoSTLImporter::mmoSTLImporter(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Files.clear();
  m_Swaps.clear();
  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoSTLImporter::~mmoSTLImporter()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedSTLs.size(); i++)
    mafDEL(m_ImportedSTLs[i]);
}
//----------------------------------------------------------------------------
bool mmoSTLImporter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* mmoSTLImporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoSTLImporter *cp = new mmoSTLImporter(m_Label);
  cp->m_Files = m_Files;
  return cp;
}
//----------------------------------------------------------------------------
void mmoSTLImporter::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode && m_Files.size() == 0)
  {
    mafString wildc = "Stereo Litography (*.stl)|*.stl";
    std::vector<std::string> files;
    mafString f;

    m_Files.clear();
    mafGetOpenMultiFiles(m_FileDir.GetCStr(),wildc.GetCStr(), files);
    for(unsigned i = 0; i < files.size(); i++)
    {
      f = files[i].c_str();
      m_Files.push_back(f);
    }
  }
	
	int result = OP_RUN_CANCEL;

	if(m_Files.size() != 0) 
	{
		result = OP_RUN_OK;
    m_Swaps.resize(m_Files.size());
    for(unsigned i = 0; i < m_Swaps.size(); i++)
    {
      m_Swaps[i] = 0;
      CheckSwap(m_Files[i].GetCStr(), m_Swaps[i]);
    }
    ImportSTL();
	}

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoSTLImporter::CheckSwap(const char *file_name, int &swapFlag)
//----------------------------------------------------------------------------
{
  //check if the file is binary
  if(IsFileBinary(file_name))
  {
    std::ifstream f_in;
    f_in.open (file_name, std::ifstream::in| std::ifstream::binary);

    //check if the file needs swapping
    //reading the header
    if (!m_TestMode)
    {
      wxBusyInfo wait("Checking if the file needs swapping: ...");
    }

    char ch;
    unsigned int number, v;
    number = 0;
    v = 0;
    
    int i;
    for (i = 0; i< 80; i++)
    {
      f_in.read (&ch,1);
    } 
    //reading the integer representing the number of polygons	
    f_in.read ((char*) &number,4);
    int j = 0;

    while (!f_in.eof()) 
    {
      for (i = 0; i < 12; i++)
      {
        f_in.read ((char*) &v,4);
      }
      f_in.read (&ch,1);
      f_in.read (&ch,1);
      j++;
    } 	

    f_in.close();
    if ((j-1) != number)
    {
      swapFlag = 1;
    }
  }
}
//----------------------------------------------------------------------------
void mmoSTLImporter::OpDo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedSTLs.size(); i++)
  {
    if (m_ImportedSTLs[i])
    {
      m_ImportedSTLs[i]->ReparentTo(m_Input);
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mmoSTLImporter::OpUndo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedSTLs.size(); i++)
  {
    if (m_ImportedSTLs[i])
    {
      mafEventMacro(mafEvent(this, VME_REMOVE, m_ImportedSTLs[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mmoSTLImporter::ImportSTL()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Loading file: ...");  
  }

  unsigned int i;
  for(i = 0; i < m_ImportedSTLs.size(); i++)
    mafDEL(m_ImportedSTLs[i]);
  m_ImportedSTLs.clear();

  for(unsigned kk = 0; kk < m_Files.size(); kk++)
  {
    mafString fn;
    fn = m_Files[kk];
    if (m_Swaps[kk] != 0)
	  { //swapping the file
		  std::ifstream f_in;
      mafString swapped;				
		  f_in.open (m_Files[kk].GetCStr(), ifstream::in| ifstream::binary);
		  int dot_pos = m_Files[kk].FindLastChr('.');
      swapped.NCopy(m_Files[kk].GetCStr(),dot_pos);
		  swapped = swapped + "_swapped";
		  swapped = swapped + ".stl";
		  std::ofstream f_out;
		  f_out.open(swapped.GetCStr(), ofstream::out | ofstream::binary);
  		
		  char ch;
		  unsigned int number = 0, v = 0;
		  //reading the header and copying it without swapping
		  for (i = 0; i < 80; i++)
		  {
			  f_in.read (&ch,1);
			  f_out.write(&ch,1);
		  } 
  			
		  //copying and swapping the integer representing the number of polygons	
		  f_in.read ((char*) &number,4);
		  Swap_Four(&number);
		  f_out.write((char*) &number,4);

		  int j = 0;
		  while (!f_in.eof()) 
		  {
			  for (i = 0; i < 12; i++)
			  {
				  if (!f_in.eof())
				  {
					  f_in.read ((char*) &v,4);
					  Swap_Four(&v);
					  f_out.write((char*) &v,4);
				  }
			  }
			  if (!f_in.eof())
			  {
				  f_in.read (&ch,1);
				  f_out.write(&ch,1);

				  f_in.read (&ch,1);
				  f_out.write(&ch,1);
			  }
			  j++;
		  } 							
		  f_out.close();
		  f_in.close();		
		  fn = swapped;
	  }

    vtkMAFSmartPointer<vtkSTLReader> reader;
	  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,reader));
    reader->SetFileName(fn);
	  reader->Update();

    wxString path, name, ext;
    wxSplitPath(fn.GetCStr(),&path,&name,&ext);

    mafVMESurface *importedSTL;
    mafNEW(importedSTL);
    importedSTL->SetName(name);
	  importedSTL->SetDataByDetaching(reader->GetOutput(),0);

    mafTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");
    importedSTL->GetTagArray()->SetTag(tag_Nature);

	  //delete the swapped file
	  if (m_Swaps[kk] != 0)
	  {
		  const char* file_name = (fn);
		  remove(file_name);
	  }
    m_ImportedSTLs.push_back(importedSTL);
  }
}
//----------------------------------------------------------------------------
void mmoSTLImporter::Swap_Four(unsigned int *value)
//----------------------------------------------------------------------------
{ 
	unsigned int r; 
	r =  *value;
	*value = ((r & 0xff) << 24) | ((r & 0xff00) << 8) | ((r & 0xff0000) >> 8) | ((r & 0xff000000) >> 24);
}

//----------------------------------------------------------------------------
bool mmoSTLImporter::IsFileBinary(const char *name_file)
//  it is similar to the protected member vtkSTLReader::GetSTLFileType(FILE *fp)
//----------------------------------------------------------------------------
{
  unsigned char header[80];
  int i;
  int numChars;

  bool binary = true;
  FILE * pFile;
  pFile = fopen(name_file,"r"); 

  // From Wikipedia: A binary STL file has an 80 character header 
  // (which is generally ignored - but which should never begin with 'solid' 
  // because that will lead most software to assume that this is an ASCII STL file)
  numChars = static_cast<int>(fread ((unsigned char *)header, 1, 80, pFile));
  for (i = 0; i < numChars - 5; i++) // don't test \0
  {
    if (header[i] == 's' &&
      header[i+1] == 'o' &&
      header[i+2] == 'l' &&
      header[i+3] == 'i' &&
      header[i+4] == 'd')
    {
      binary = false;
      break;
    }
  }
  
  // Reset file for reading
  //
  fclose(pFile);
  return binary;
}
//----------------------------------------------------------------------------
void mmoSTLImporter::SetFileName(const char *file_name)
//----------------------------------------------------------------------------
{
  m_Files.resize(1);
  m_Files[0] = file_name;
  m_Swaps.resize(m_Files.size());
  for(unsigned i = 0; i < m_Swaps.size(); i++)
  {
    m_Swaps[i] = 0;
    CheckSwap(m_Files[i].GetCStr(), m_Swaps[i]);
  }
}
//----------------------------------------------------------------------------
void mmoSTLImporter::GetImportedSTL(std::vector<mafVMESurface*> &importedSTL)
//----------------------------------------------------------------------------
{
  importedSTL.clear();
  importedSTL.resize(m_ImportedSTLs.size());
  for (unsigned int i=0; i< m_ImportedSTLs.size(); i++)
  {
    importedSTL[i] = m_ImportedSTLs[i];
  }
}
