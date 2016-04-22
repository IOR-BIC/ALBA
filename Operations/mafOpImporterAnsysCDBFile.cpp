/*=========================================================================

Program: MAF2
Module:  mafOpImporterAnsysCDBFile.cpp
Authors: Nicola Vanella

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

#include "mafOpImporterAnsysCDBFile.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMEMeshAnsysTextImporter.h"
#include "mafProgressBarHelper.h"

#include "vtkMAFSmartPointer.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterAnsysCDBFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterAnsysCDBFile::mafOpImporterAnsysCDBFile(const wxString &label) :
mafOpImporterAnsysCommon(label)
{ 

}
//----------------------------------------------------------------------------
mafOpImporterAnsysCDBFile::~mafOpImporterAnsysCDBFile()
{
  //mafDEL(m_ImportedVmeMesh);
}

//----------------------------------------------------------------------------
mafOp* mafOpImporterAnsysCDBFile::Copy()   
{
  mafOpImporterAnsysCDBFile *cp = new mafOpImporterAnsysCDBFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
mafString mafOpImporterAnsysCDBFile::GetWildcard()
{
  return "cdb files (*.cdb)|*.cdb|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
int mafOpImporterAnsysCDBFile::ParseAnsysFile(mafString fileName)
{
	m_ProgressHelper = new mafProgressBarHelper(m_Listener);
	m_ProgressHelper->SetTextMode(m_TestMode);
	m_ProgressHelper->InitProgressBar("Please wait parsing CDBAnsys File...");
	
  ReadInit(fileName);

  FILE *nodesFile, *materialsFile;
  nodesFile = fopen(m_NodesFileName, "w");
	if (!nodesFile)
	{
		mafLogMessage("Cannot Open: %s",m_NodesFileName.c_str());
		return MAF_ERROR;
	}

  materialsFile = fopen(m_MaterialsFileName, "w");
	if (!materialsFile)
	{
		mafLogMessage("Cannot Open: %s",m_MaterialsFileName.c_str());
		return MAF_ERROR;
	}

	int lineLenght;

  m_CurrentMatId = -1;

  while ((lineLenght = GetLine(m_FilePointer, m_Line)) != 0) 
  {
    if(strncmp (m_Line,"NBLOCK,",7) == 0)
    {
      ReadNBLOCK(nodesFile);
    }

    if(strncmp (m_Line,"EBLOCK,",7) == 0)
    {
      ReadEBLOCK();
    }

    if(strncmp (m_Line,"CMBLOCK,",8) == 0)
    {
      ReadCMBLOCK();
    }

    if(strncmp (m_Line,"MPDATA,",7) == 0)
    {
      ReadMPDATA(materialsFile);
    }
  }

  fclose(nodesFile);
  fclose(materialsFile);

	ReadFinalize();
  cppDEL(m_ProgressHelper);

  return MAF_OK;
}

