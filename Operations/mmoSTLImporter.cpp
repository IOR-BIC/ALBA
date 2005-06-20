/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoSTLImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-20 09:16:46 $
  Version:   $Revision: 1.1 $
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
#include "vtkMAFSmartPointer.h"

#include "vtkSTLReader.h"
#include "vtkPolyData.h"

#include <fstream>
//----------------------------------------------------------------------------
mmoSTLImporter::mmoSTLImporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Vme     = NULL;
  m_Swap    = 0;
  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoSTLImporter::~mmoSTLImporter( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Vme);
}
//----------------------------------------------------------------------------
bool mmoSTLImporter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
void mmoSTLImporter::OpRun()   
//----------------------------------------------------------------------------
{
	m_File = "";

	mafString wildc = "Stereo Litography (*.stl)|*.stl";
	mafString f = mafGetOpenFile(m_FileDir.GetCStr(),wildc.GetCStr()).c_str(); 	
	
	int result = OP_RUN_CANCEL;

	if(f != "") 
	{
		m_File = f;
		result = OP_RUN_OK;
    CheckSwap(m_File.GetCStr());
	}

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoSTLImporter::CheckSwap(const char *file_name)
//----------------------------------------------------------------------------
{
  //check if the file is binary
  if(IsFileBinary(file_name))
  {
    std::ifstream f_in;
    f_in.open (file_name, ifstream::in| ifstream::binary);

    //check if the file needs swapping
    //reading the header
    wxBusyInfo wait("Checking if the file needs swapping: ...");

    char ch;
    unsigned int number, v;
    number = 0;
    v = 0;

    for (int i = 0; i< 80; i++)
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
      m_Swap = 1;
    }
  }
}
//----------------------------------------------------------------------------
void mmoSTLImporter::OpDo()   
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("Loading file: ...");  
	assert(!m_Vme);
	if (m_Swap != 0)
	{ //swapping the file
		mafString swapped;				
		std::ifstream f_in;
		f_in.open (m_File.GetCStr(), ifstream::in| ifstream::binary);
		int dot_pos = m_File.FindLastChr('.');
    swapped.NCopy(m_File.GetCStr(),dot_pos);
		swapped = swapped + "_swapped";
		swapped = swapped + ".stl";
		std::ofstream f_out;
		f_out.open(swapped.GetCStr(), ofstream::out | ofstream::binary);
		
		char ch;
		unsigned int number = 0, v = 0;
		//reading the header and copying it without swapping
		for (int i = 0; i< 80; i++)
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
		m_File = swapped;
	}

	vtkMAFSmartPointer<vtkSTLReader> reader;
	mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,reader));
	reader->SetFileName(m_File);
	reader->Update();

  wxString path, name, ext;
  wxSplitPath(m_File.GetCStr(),&path,&name,&ext);

  mafNEW(m_Vme);
  m_Vme->SetName(name);
	m_Vme->SetDataByDetaching(reader->GetOutput(),0);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");
  m_Vme->GetTagArray()->SetTag(tag_Nature);

	mafEventMacro(mafEvent(this,VME_ADD,m_Vme));
	
	//delete the swapped file
	if (m_Swap != 0)
	{
		const char* file_name = (m_File);
		remove(file_name);
	}
}
//----------------------------------------------------------------------------
void mmoSTLImporter::OpUndo()
//----------------------------------------------------------------------------
{
	assert(m_Vme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme)); // unreference the vme
  mafDEL(m_Vme);
}
//----------------------------------------------------------------------------
mafOp* mmoSTLImporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoSTLImporter *cp = new mmoSTLImporter(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mmoSTLImporter::OpStop(int result)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));  	   
}
//----------------------------------------------------------------------------
void mmoSTLImporter::Swap_Four(unsigned int *value)
/**  */
//----------------------------------------------------------------------------
{ 
	unsigned int r; 
	r =  *value;
	*value = ((r & 0xff) << 24) | ((r & 0xff00) << 8) | ((r & 0xff0000) >> 8) | ((r & 0xff000000) >> 24);
}

//----------------------------------------------------------------------------
bool mmoSTLImporter::IsFileBinary(const char *name_file)
/**  it is similar to the protected member vtkSTLReader::GetSTLFileType(FILE *fp) */
//----------------------------------------------------------------------------
{
  unsigned char header[256];
  int i;
  int numChars;

  bool binary = false;
  FILE * pFile;
  pFile = fopen(name_file,"r"); 

  //  Read a little from the file to figure what type it is.
  // skip 255 characters so we are past any first line comment 
  numChars = static_cast<int>(fread ((unsigned char *)header, 1, 255, pFile));
  for (i = 0; i< numChars; i++) 
  {
		if (header[i] > 127)
		{
			binary = true;
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
  m_File = file_name;
}
