/*=========================================================================

Program: ALBA
Module: albaOpImporterAnsysInputFile.cpp
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

#include "albaOpImporterAnsysInputFile.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSmartPointer.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEMeshAnsysTextImporter.h"
#include "albaProgressBarHelper.h"

#include "vtkALBASmartPointer.h"

#include "albaGUIBusyInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterAnsysInputFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterAnsysInputFile::albaOpImporterAnsysInputFile(const wxString &label) :
albaOpImporterAnsysCommon(label)
{

}
//----------------------------------------------------------------------------
albaOpImporterAnsysInputFile::~albaOpImporterAnsysInputFile()
{

}

//----------------------------------------------------------------------------
albaOp* albaOpImporterAnsysInputFile::Copy()   
{
  albaOpImporterAnsysInputFile *cp = new albaOpImporterAnsysInputFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
albaString albaOpImporterAnsysInputFile::GetWildcard()
{
  return "inp files (*.inp)|*.inp|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
int albaOpImporterAnsysInputFile::ParseAnsysFile(albaString fileName)
{
 
	if (ReadInit(fileName, m_TestMode, true, "Please wait parsing Input Ansys File...", m_Listener) == ALBA_ERROR)
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
	
  m_CurrentMatId = -1;

	int lineLenght = GetLine();
  while (lineLenght != 0) 
  {
    if(strncmp (m_Line,"N,",2) == 0)
    {
      UpdateNodesFile(nodesFile);
    }

    if(strncmp (m_Line,"NBLOCK,",7) == 0)
    {
      ReadNBLOCK(nodesFile);
    }

    if(strncmp (m_Line,"MP,",3) == 0 || strncmp (m_Line,"MPDATA,",7) == 0)
    {
      ReadMPDATA();
    } 
    
    if(strncmp (m_Line,"TYPE,",5) == 0)
    {
      UpdateElements();
			continue;
    }

    if(strncmp (m_Line,"EBLOCK,",7) == 0)
    {
      ReadEBLOCK();
    }

    if(strncmp (m_Line,"CMBLOCK,",8) == 0)
    {
      ReadCMBLOCK();
    }

		lineLenght = GetLine();
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

//----------------------------------------------------------------------------
int albaOpImporterAnsysInputFile::UpdateNodesFile(FILE *outFile)
{
  char blockName[254];
  int nodeId = 0;
  double nodeX = 0, nodeY = 0, nodeZ = 0;

  //N,209819,             -110.145699,             172.905502,             -333.099487
  ReplaceInString(m_Line, ',', ' ');
  //N 209819              -110.145699              172.905502              -333.099487
  sscanf(m_Line, "%s %d %lf %lf %lf", blockName, &nodeId, &nodeX, &nodeY, &nodeZ);

  int result = fprintf(outFile,"%d\t%lf\t%lf\t%lf\n", nodeId, nodeX, nodeY, nodeZ);

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAnsysInputFile::UpdateElements()
{
  char blockName[254], unusedStr[254];
  int matCard = 0;
  int idMaterial, idTypeElement = 0, idConstants = 0, nodes[16];

  for (int j=0;j<16;j++)
  {
    nodes[j] = -1;
  }

  //TYPE, 50  $ MAT, 440  $ REAL, 2
  ReplaceInString(m_Line, ',', ' ');
  ReplaceInString(m_Line, '$', ' ');
  //TYPE  50    MAT  440    REAL  2
  sscanf(m_Line, "%s %d %s %d %s %d", blockName, &idTypeElement, unusedStr, &matCard, unusedStr, &idConstants);

  GetLine();

  if(strncmp (m_Line,"EBLOCK,",7) == 0)
  {
    ReadEBLOCK();    
  }
  else
  {
    // INP from Ansys
    while (strncmp(m_Line, "EN,", 3) == 0 || strncmp(m_Line, "EMORE,", 6) == 0 || strncmp(m_Line, "ESYS,", 5) == 0 || strncmp(m_Line,"\n",1) == 0 )
    {    
      if(strncmp (m_Line,"EN,",3) == 0)
      {
        //EN,       2,       3,       1,       2,       4,       7,       6,       9,      12
        ReplaceInString(m_Line, ',', ' ');
        sscanf(m_Line, "%s %d %d %d %d %d %d %d %d %d", blockName, &idMaterial, nodes+0,nodes+1,nodes+2,nodes+3,nodes+4,nodes+5,nodes+6,nodes+7);

        GetLine();
      }

      if(strncmp (m_Line,"EMORE,",6) == 0)
      {
        //EMORE,       8,      10
        ReplaceInString(m_Line, ',', ' ');
        int readed = sscanf(m_Line, "%s %d %d %d %d %d %d %d %d", blockName, nodes+8,nodes+9,nodes+10,nodes+11,nodes+12,nodes+13,nodes+14,nodes+15);
           
        int nNodes= 8 + (readed-1);
        AddElement(idMaterial, nNodes, idTypeElement, matCard, nodes);
      }
        
      GetLine();
    }
  }

  return ALBA_OK;
}