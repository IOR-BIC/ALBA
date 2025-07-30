/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterSTL
 Authors: Paolo Quadrani
 
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

#include "albaOpImporterSTL.h"
#include "albaGUIBusyInfo.h"

#include "albaDecl.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaSmartPointer.h"
#include "vtkALBASmartPointer.h"

#include "vtkSTLReader.h"
#include "vtkPolyData.h"

#include "wx\filename.h"

#include <fstream>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterSTL);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterSTL::albaOpImporterSTL(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Files.clear();
  m_Swaps.clear();
}
//----------------------------------------------------------------------------
albaOpImporterSTL::~albaOpImporterSTL()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedSTLs.size(); i++)
    albaDEL(m_ImportedSTLs[i]);
}
//----------------------------------------------------------------------------
bool albaOpImporterSTL::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterSTL::Copy()   
//----------------------------------------------------------------------------
{
  albaOpImporterSTL *cp = new albaOpImporterSTL(m_Label);
  cp->m_Files = m_Files;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterSTL::OpRun()   
//----------------------------------------------------------------------------
{
	albaString	fileDir = albaGetLastUserFolder();

  if (!m_TestMode && m_Files.size() == 0)
  {
    albaString wildc = "Stereo Litography (*.stl)|*.stl";
    std::vector<wxString> files;
    albaString f;

    m_Files.clear();
    albaGetOpenMultiFiles(fileDir.GetCStr(),wildc.GetCStr(), files);
    for(unsigned i = 0; i < files.size(); i++)
    {
      f = files[i];
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

	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterSTL::CheckSwap(const char *file_name, int &swapFlag)
//----------------------------------------------------------------------------
{
  //check if the file is binary
  if(IsFileBinary(file_name))
  {
    std::ifstream f_in;
    f_in.open (file_name, std::ifstream::in| std::ifstream::binary);

    //check if the file needs swapping
    //reading the header
    albaGUIBusyInfo wait("Checking if the file needs swapping: ...",m_TestMode);
    
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
void albaOpImporterSTL::OpDo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedSTLs.size(); i++)
  {
    if (m_ImportedSTLs[i])
    {
      m_ImportedSTLs[i]->ReparentTo(m_Input);
    }
  }
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpImporterSTL::OpUndo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedSTLs.size(); i++)
  {
    if (m_ImportedSTLs[i])
    {
      GetLogicManager()->VmeRemove(m_ImportedSTLs[i]);
    }
  }
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpImporterSTL::ImportSTL()
//----------------------------------------------------------------------------
{
  albaGUIBusyInfo wait("Loading file: ...",m_TestMode);  
  
  unsigned int i;
  for(i = 0; i < m_ImportedSTLs.size(); i++)
    albaDEL(m_ImportedSTLs[i]);
  m_ImportedSTLs.clear();

  for(unsigned kk = 0; kk < m_Files.size(); kk++)
  {
    albaString fn;
    fn = m_Files[kk];
    if (m_Swaps[kk] != 0)
	  { //swapping the file
		  std::ifstream f_in;
      albaString swapped;				
		  f_in.open (m_Files[kk].GetCStr(), std::ifstream::in| std::ifstream::binary);
		  int dot_pos = m_Files[kk].FindLastChr('.');
      swapped.NCopy(m_Files[kk].GetCStr(),dot_pos);
		  swapped = swapped + "_swapped";
		  swapped = swapped + ".stl";
		  std::ofstream f_out;
		  f_out.open(swapped.GetCStr(), std::ofstream::out | std::ofstream::binary);
  		
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

    vtkALBASmartPointer<vtkSTLReader> reader;
	  albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,reader));
    reader->SetFileName(fn);
	  reader->Update();

    wxString path, name, ext;
    wxFileName::SplitPath(fn.GetCStr(),&path,&name,&ext);

    albaVMESurface *importedSTL;
    albaNEW(importedSTL);
    importedSTL->SetName(name);
	  importedSTL->SetDataByDetaching(reader->GetOutput(),0);

    albaTagItem tag_Nature;
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
void albaOpImporterSTL::Swap_Four(unsigned int *value)
//----------------------------------------------------------------------------
{ 
	unsigned int r; 
	r =  *value;
	*value = ((r & 0xff) << 24) | ((r & 0xff00) << 8) | ((r & 0xff0000) >> 8) | ((r & 0xff000000) >> 24);
}

//----------------------------------------------------------------------------
bool albaOpImporterSTL::IsFileBinary(const char *name_file)
//  it is similar to the protected member vtkSTLReader::GetSTLFileType(FILE *fp)
//----------------------------------------------------------------------------
{
  unsigned char header[80];
  int i;
  int numChars;

  bool binary = true;
  FILE * pFile;
  pFile = albaTryOpenFile(name_file,"r");

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
void albaOpImporterSTL::SetFileName(const char *file_name)
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
void albaOpImporterSTL::GetImportedSTL(std::vector<albaVMESurface*> &importedSTL)
//----------------------------------------------------------------------------
{
  importedSTL.clear();
  importedSTL.resize(m_ImportedSTLs.size());
  for (unsigned int i=0; i< m_ImportedSTLs.size(); i++)
  {
    importedSTL[i] = m_ImportedSTLs[i];
  }
}

//----------------------------------------------------------------------------
char ** albaOpImporterSTL::GetIcon()
{
#include "pic/MENU_IMPORT_STL.xpm"
	return MENU_IMPORT_STL_xpm;
}