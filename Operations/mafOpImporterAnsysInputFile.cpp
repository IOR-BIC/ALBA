/*=========================================================================

Program: MAF2
Module: mafOpImporterAnsysInputFile.cpp
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

#include "mafOpImporterAnsysInputFile.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMEMeshAnsysTextImporter.h"
#include "mafProgressBarHelper.h"

#include "vtkMAFSmartPointer.h"

#include "wx/busyinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterAnsysInputFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterAnsysInputFile::mafOpImporterAnsysInputFile(const wxString &label) :
mafOpImporterAnsysCommon(label)
{

}
//----------------------------------------------------------------------------
mafOpImporterAnsysInputFile::~mafOpImporterAnsysInputFile()
{

}

//----------------------------------------------------------------------------
mafOp* mafOpImporterAnsysInputFile::Copy()   
{
  mafOpImporterAnsysInputFile *cp = new mafOpImporterAnsysInputFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
mafString mafOpImporterAnsysInputFile::GetWildcard()
{
  return "inp files (*.inp)|*.inp|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
int mafOpImporterAnsysInputFile::ParseAnsysFile(mafString fileName)
{
	m_ProgressHelper = new mafProgressBarHelper(m_Listener);
	m_ProgressHelper->SetTextMode(m_TestMode);
	m_ProgressHelper->InitProgressBar("Please wait parsing Input Ansys File...");

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
      ReadMPDATA(materialsFile);
    } 
    
    if(strncmp (m_Line,"TYPE,",5) == 0)
    {
      UpdateElements();
    }

    if(strncmp (m_Line,"EBLOCK,",7) == 0)
    {
      ReadEBLOCK();
    }

    if(strncmp (m_Line,"CMBLOCK,",8) == 0)
    {
      ReadCMBLOCK();
    }
  }

  fclose(nodesFile);
  fclose(materialsFile);

  ReadFinalize();
  cppDEL(m_ProgressHelper);

  return MAF_OK;
}

//----------------------------------------------------------------------------
int mafOpImporterAnsysInputFile::UpdateNodesFile(FILE *outFile)
{
  char blockName[254];
  int nodeId = 0;
  double nodeX = 0, nodeY = 0, nodeZ = 0;

  //N,209819,             -110.145699,             172.905502,             -333.099487
  ReplaceInString(m_Line, ',', ' ');
  //N 209819              -110.145699              172.905502              -333.099487
  sscanf(m_Line, "%s %d %lf %lf %lf", blockName, &nodeId, &nodeX, &nodeY, &nodeZ);

  int result = fprintf(outFile,"%d\t%lf\t%lf\t%lf\n", nodeId, nodeX, nodeY, nodeZ);

  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafOpImporterAnsysInputFile::UpdateElements()
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

  GetLine(m_FilePointer, m_Line);

  if(strncmp (m_Line,"EBLOCK,",7) == 0)
  {
    ReadEBLOCK();    
  }
  else
  {
    // INP from Ansys
    while (strncmp (m_Line,"CM,",3) != 0) 
    {    
      if(strncmp (m_Line,"EN,",3) == 0)
      {
        //EN,       2,       3,       1,       2,       4,       7,       6,       9,      12
        ReplaceInString(m_Line, ',', ' ');
        sscanf(m_Line, "%s %d %d %d %d %d %d %d %d %d", blockName, &idMaterial, nodes+0,nodes+1,nodes+2,nodes+3,nodes+4,nodes+5,nodes+6,nodes+7);

        GetLine(m_FilePointer, m_Line);
      }

      if(strncmp (m_Line,"EMORE,",6) == 0)
      {
        //EMORE,       8,      10
        ReplaceInString(m_Line, ',', ' ');
        int readed = sscanf(m_Line, "%s %d %d %d %d %d %d %d %d", blockName, nodes+8,nodes+9,nodes+10,nodes+11,nodes+12,nodes+13,nodes+14,nodes+15);
           
        int nNodes= 8 + (readed-1);
        AddElement(idMaterial, nNodes, idTypeElement, matCard, nodes);
      }
        
      GetLine(m_FilePointer, m_Line);
    }
  }

  return MAF_OK;
}