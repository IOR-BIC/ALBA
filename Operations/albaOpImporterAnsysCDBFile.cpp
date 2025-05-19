/*=========================================================================

Program: ALBA
Module:  albaOpImporterAnsysCDBFile.cpp
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

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

#include "albaOpImporterAnsysCDBFile.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSmartPointer.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEMeshAnsysTextImporter.h"
#include "albaProgressBarHelper.h"

#include "vtkALBASmartPointer.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterAnsysCDBFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterAnsysCDBFile::albaOpImporterAnsysCDBFile(const wxString &label) :
albaOpImporterAnsysCommon(label)
{ 

}
//----------------------------------------------------------------------------
albaOpImporterAnsysCDBFile::~albaOpImporterAnsysCDBFile()
{
  //albaDEL(m_ImportedVmeMesh);
}

//----------------------------------------------------------------------------
albaOp* albaOpImporterAnsysCDBFile::Copy()   
{
  albaOpImporterAnsysCDBFile *cp = new albaOpImporterAnsysCDBFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
albaString albaOpImporterAnsysCDBFile::GetWildcard()
{
  return "cdb files (*.cdb)|*.cdb|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
int albaOpImporterAnsysCDBFile::ParseAnsysFile(albaString fileName)
{
	
	if (ReadInit(fileName, GetTestMode(), true, "Please wait parsing CDBAnsys File...", m_Listener) == ALBA_ERROR)
	{
		albaLogMessage("Cannot Open: %s", fileName);
		ReadFinalize();
		return ALBA_ERROR;
	}

  FILE *nodesFile;
  nodesFile = albaTryOpenFile(m_NodesFileName, "w");
	if (!nodesFile)
	{
		albaLogMessage("Cannot Open: %s",m_NodesFileName.ToAscii());
		ReadFinalize();
		return ALBA_ERROR;
	}

	int lineLenght;

  m_CurrentMatId = -1;

  while ((lineLenght = GetLine(true)) != 0) 
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
      ReadMPDATA();
    }
  }
	fclose(nodesFile);

	if (WriteMaterials() == ALBA_ERROR)
	{
		ReadFinalize();
		return ALBA_ERROR;
	}
  
	ReadFinalize();

  return ALBA_OK;
}

