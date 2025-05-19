/*=========================================================================

Program: ALBA
Module: albaOpImporterAnsysCommon.cpp
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

#include "albaOpImporterAnsysCommon.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSmartPointer.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEMeshAnsysTextImporter.h"

#include "vtkALBASmartPointer.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/busyinfo.h"
#define min(a,b)  (((a) < (b)) ? (a) : (b))

//----------------------------------------------------------------------------
albaOpImporterAnsysCommon::albaOpImporterAnsysCommon(const wxString &label) :
albaOp(label)
{ 
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_Output=NULL;

  m_NodesFileName = "";
  m_MaterialsFileName = "";

	wxString userPath= wxStandardPaths::Get().GetUserDataDir();

	m_DataDir = userPath + "\\Data";
  m_CacheDir = m_DataDir + "\\AnsysReaderCache";
  m_AnsysInputFileNameFullPath		= "";
  
  m_BusyInfo = NULL;
}

//----------------------------------------------------------------------------
albaOpImporterAnsysCommon::~albaOpImporterAnsysCommon()
{
	for (int e = 0; e<m_Elements.size(); e++)
	{
		delete[] m_Elements[e].Nodes;
	}
	m_Elements.clear();

	for (int c = 0; c<m_Components.size(); c++)
  {
    if(m_Components[c].Ranges)
      delete[] m_Components[c].Ranges;
  }
  m_Components.clear();

	m_Materials.clear();
}
//----------------------------------------------------------------------------
bool albaOpImporterAnsysCommon::InternalAccept(albaVME *node)
{
  return true;
}

//----------------------------------------------------------------------------
void albaOpImporterAnsysCommon::OpRun()   
{  
  albaString wildcard = GetWildcard();

  int result = OP_RUN_CANCEL;
  m_AnsysInputFileNameFullPath = "";

  wxString f;
  f = albaGetOpenFile("", wildcard).ToAscii(); 
  if(!f.IsEmpty() && wxFileExists(f))
  {
    m_AnsysInputFileNameFullPath = f;
    Import();
    result = OP_RUN_OK;
  }
  albaEventMacro(albaEvent(this,result));  
}

//----------------------------------------------------------------------------
void albaOpImporterAnsysCommon::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        this->Import();
        this->OpStop(OP_RUN_OK);
      }
      break;
    case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
      }
      break;
    default:
      albaEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
int albaOpImporterAnsysCommon::Import()
{
  m_NodesFileName = m_CacheDir + "\\nodes.lis" ;
  m_MaterialsFileName = m_CacheDir + "\\materials.lis" ;

  // Create tmp path
  mkdir(m_DataDir);
	
	if(!wxDirExists(m_DataDir.GetCStr()))
	{
		albaLogMessage("Cloud not create \"Data\" Directory");
		return ALBA_ERROR;
	}

	mkdir(m_CacheDir);
	if(!wxDirExists(m_DataDir.GetCStr()))
	{
		albaLogMessage("Cloud not create Read Cache Directory");
		return ALBA_ERROR;
	}
  
  // Parsing Ansys File
  if(ParseAnsysFile(m_AnsysInputFileNameFullPath) == ALBA_ERROR)
  {
    return ALBA_ERROR;
  }

  if (GetTestMode() == false)
  {
    m_BusyInfo = new wxBusyInfo("Please wait importing VME Mesh AnsysText...");
  }

  wxString name, path, ext;
  wxFileName::SplitPath(m_AnsysInputFileNameFullPath, &path, &name, &ext);

  int returnValue = ALBA_OK;  
  
  if(m_Components.size()==0)
  {
    // Create dafault component
    AnsysComponent comp;
    comp.Name=name;
    comp.RangeNum=1;
    comp.Ranges=NULL;

    m_Components.push_back(comp);
  }

  for (int c=0; c<m_Components.size(); c++)
  {
    albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
    reader->SetNodesFileName(m_NodesFileName.ToAscii());
		if (m_Materials.size() > 0)
			reader->SetMaterialsFileName(m_MaterialsFileName.ToAscii());
		else
			reader->SetMode(albaVMEMeshAnsysTextImporter::WITHOUT_MAT_MODE);
		
		WriteElements(c);
		
		reader->SetElementsFileName(m_Components[c].ElementsFileName.c_str());

		returnValue = reader->Read();

		if (returnValue == ALBA_ERROR)
		{
			if (!m_TestMode)
        albaMessage(_("Error parsing input files! See log window for details..."),_("Error"));
		  else
			  printf("Error parsing input files!");
    } 
    else if (returnValue == ALBA_OK)
    {
			albaVMEMesh *importedVmeMesh;
      albaNEW(importedVmeMesh);

      importedVmeMesh->SetName(m_Components[c].Name.c_str());
	    importedVmeMesh->SetDataByDetaching(reader->GetOutput()->GetUnstructuredGridOutput()->GetVTKData(),0);

      albaTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      importedVmeMesh->GetTagArray()->SetTag(tag_Nature);

      importedVmeMesh->ReparentTo(m_Input);
      importedVmeMesh->Update();

			albaDEL(importedVmeMesh);
    }
    delete reader;
  }
  
  if (GetTestMode() == false)
  {
    cppDEL(m_BusyInfo);
  }
 
  return returnValue;
}

//----------------------------------------------------------------------------
int albaOpImporterAnsysCommon::ReadNBLOCK(FILE *outFile)
{
  char blockName[254];
  int maxId = 0, numElements = 0, nodeId, nodeSolidModelEntityId, nodeLine;
  double nodeX, nodeY, nodeZ;

  // NBLOCK,6,SOLID,   2596567,    25   or
  // NBLOCK,6,SOLID
  int nReaded = sscanf(m_Line, "%s %d, %d", blockName, &maxId, &numElements);

  GetLine(); // (3i8,6e20.13) Line ignored

  if(nReaded == 1)
  {
    // CDB from Hypermesh
	  while (GetLine() != 0 && strncmp(m_Line, "N,R5", 4) != 0 && strncmp(m_Line, "-1", 2) != 0)
	  {
		  nodeId = nodeSolidModelEntityId = nodeLine = 0;
		  nodeX = nodeY = nodeZ = 0;

		  //15239 0 0 112.48882247215 -174.4868225037 -378.3886770441 0.0 0.0 0.0
		  int nReaded = sscanf(m_Line, "%d %d %d %lf %lf %lf", &nodeId, &nodeSolidModelEntityId, &nodeLine, &nodeX, &nodeY, &nodeZ);

		  if (nReaded == 2)
			  nReaded = sscanf(m_Line, "%d %lf %lf %lf", &nodeId, &nodeX, &nodeY, &nodeZ);

		  if (nReaded < 2)
			  break;

		  fprintf(outFile, "%d\t%.13E\t%.13E\t%.13E\n", nodeId, nodeX, nodeY, nodeZ);
	  }
  }
  else
  {
    // CDB from Ansys
    for (int i = 0; i < numElements; i++)
    {
      if(GetLine() != 0)
      {
        nodeId = nodeSolidModelEntityId = nodeLine = 0;
        nodeX = nodeY = nodeZ = 0;

        sscanf(m_Line, "%d %d %d %lf %lf %lf", &nodeId, &nodeSolidModelEntityId, &nodeLine, &nodeX, &nodeY, &nodeZ);
        fprintf(outFile,"%d\t%.13E\t%.13E\t%.13E\n", nodeId, nodeX, nodeY, nodeZ);
      }
    } 
  }

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAnsysCommon::ReadEBLOCK()
{
  char blockName[254];
  int maxId = 0, numElements = 0;
  int idMaterial,idTypeElement,idConstants,nNodes, elementNumber, nodes[20], unused;

  // EBLOCK,19,SOLID,   3436163,    234932   / EBLOCK,19,SOLID,   3436163     or 
  // EBLOCK,19,SOLID,
  int nReaded = sscanf(m_Line, "%s %d, %d", blockName, &maxId, &numElements);

  GetLine(); //(19i8) Line ignored
  
  if(nReaded < 3)
  {
    // CDB from Hypermesh
    while (GetLine() != 0 )
    {
      int readedElem=sscanf(m_Line, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &idMaterial,&idTypeElement,&idConstants,&unused,&unused,&unused,&unused,&unused,&nNodes,&unused,&elementNumber,nodes+0,nodes+1,nodes+2,nodes+3,nodes+4,nodes+5,nodes+6,nodes+7);

      if (readedElem < 2)
        break;

      if(nNodes > 8)
      {
        GetLine();
        sscanf(m_Line, "%d %d %d %d %d %d %d %d %d %d %d %d", nodes+8,nodes+9,nodes+10,nodes+11,nodes+12,nodes+13,nodes+14,nodes+15,nodes+16,nodes+17,nodes+18,nodes+19);
      }

      AddElement(elementNumber, nNodes, idTypeElement, idMaterial, nodes);
    }
  }
  else
  {
    // CDB from Ansys
    for (int index = 0; index < numElements; index++)
    {
      if(GetLine() != 0)
      {
        sscanf(m_Line, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &idMaterial,&idTypeElement,&idConstants,&unused,&unused,&unused,&unused,&unused,&nNodes,&unused,&elementNumber,nodes+0,nodes+1,nodes+2,nodes+3,nodes+4,nodes+5,nodes+6,nodes+7);

        if(nNodes > 8)
        {
          GetLine();
          sscanf(m_Line, "%d %d %d %d %d %d %d %d", nodes+8,nodes+9,nodes+10,nodes+11,nodes+12,nodes+13,nodes+14,nodes+15);
        }

        AddElement(elementNumber, nNodes, idTypeElement, idMaterial, nodes);
      }
    } 
  }  

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAnsysCommon::ReadMPDATA()
{
	char matName[254], unusedStr[254];
	float matValue;
  int matId;

  //MPDATA,EX  ,2,1,          1000.0  or MP,EX  ,2,1,          1000.0
  //MPDATA,R5.0, 1,EX  ,       1, 1,  26630.9000 
  int commaNum = ReplaceInString(m_Line, ',', ' ');

  if(commaNum <= 4)
  {
    //MPDATA EX   2 1           1000.0  or //MP EX   2 1           1000.0   
    sscanf(m_Line, "%s %s %d %f", unusedStr, matName, &matId, &matValue);
  }
  else
  {
    //MPDATA R5.0  1 EX          1  1   26630.9000    
    sscanf(m_Line, "%s %s %s %s %d %s %f", unusedStr, unusedStr, unusedStr, matName, &matId, unusedStr, &matValue);
  }

  if(matId != m_CurrentMatId)
  {  
		AnsysMaterial newMat = { matId,0,0.3,0 };
		m_Materials.push_back(newMat);
    m_CurrentMatId = matId;
  }
	
	albaString matNames[4] = { "EX","NUXY","DENS" };

	if (matNames[0].Equals(matName))
		m_Materials[m_Materials.size() - 1].Ex = matValue;
	else if (matNames[1].Equals(matName))
		m_Materials[m_Materials.size() - 1].Nuxy = matValue;
	else if (matNames[2].Equals(matName))
		m_Materials[m_Materials.size() - 1].Dens = matValue;


  return ALBA_OK; 
}
//----------------------------------------------------------------------------
int albaOpImporterAnsysCommon::ReadCMBLOCK()
{
	char compHeader[254], unusedStr[254];
	int compNum;

	// CMBLOCK,NAME,ELEM,       2  ! comment OR
	// CMBLOCK,NAME,NODE,       100  ! comment
	sscanf(m_Line, "%s %d %s", compHeader, &compNum, unusedStr);

	std::string line = compHeader;
	std::string name = line.substr(8).c_str();
	std::size_t pos = name.find(",");

	std::string compName = name.substr(0, pos).c_str();
	std::string compType = name.substr(pos + 1, 5).c_str();

	if (compType == "ELEM,")
	{
		wxString fname, fpath, fext;
		wxFileName::SplitPath(m_AnsysInputFileNameFullPath, &fpath, &fname, &fext);

		// Create Component
		AnsysComponent comp;
		comp.Name = fname + "-" + compName.c_str();

		GetLine(); // (8i10) Line ignored

		// Create range array
		int *idList = new int[compNum];

		int nReaded = 0, totReaded = 0;
		int value;
		char *linePointer;

		GetLine();
		linePointer = m_Line;

		for (int i = 0; i < compNum; i++)
		{
			nReaded = sscanf(linePointer, "%d", &value);

			if (nReaded <= 0)
			{
				GetLine(); // New Line
				linePointer = m_Line;
				nReaded = sscanf(linePointer, "%d", &value);
			}

			if (value < 0) value *= -1;
			idList[i] = value;
			linePointer += 10;
		}

		comp.Ranges = idList;
		comp.RangeNum = compNum;

		m_Components.push_back(comp);
	}

	return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaOpImporterAnsysCommon::WriteElements(int comp)
{
  albaString elementsfileName = m_CacheDir + "\\elements_part"<<comp<<".lis";
  m_Components[comp].ElementsFileName = elementsfileName;

  FILE* elementsFile = albaTryOpenFile(elementsfileName, "w");
  if (!elementsFile)
  {
    albaLogMessage("Cannot Open: %s",elementsfileName);
    return ALBA_ERROR;
  }

  int elemId, matId = 0, elemType = 0, idConstants = 0, nNodes, nodes[16];
	bool hasMaterials = m_Materials.size() > 0;

  for (int e=0; e<m_Elements.size(); e++)
  {
    AnsysElement myElem = m_Elements[e];

    if(m_Components[comp].RangeNum==1 || IsInRange(myElem.Id, comp))
    {
      elemId = myElem.Id;
      matId = myElem.MatId;
      elemType = myElem.Type;
      nNodes = myElem.NodesNumber;

//			fprintf(elementsFile, "%d\t%d\t%d\t%d\t%d\t%d", elemId, matId, elemType, idConstants, 0, 1);
			if(hasMaterials)
				fprintf(elementsFile, "%d\t%d", elemId, matId);
			else
				fprintf(elementsFile, "%d", elemId);

      for (int j=0;j<nNodes;j++)
      {
        fprintf(elementsFile,"\t%d",myElem.Nodes[j]);
      }

      fprintf(elementsFile,"\n");
    }
  }
  
  fclose(elementsFile);
  return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaOpImporterAnsysCommon::WriteMaterials()
{
	FILE *materialsFile = albaTryOpenFile(m_MaterialsFileName, "w");
	if (!materialsFile)
	{
		albaLogMessage("Cannot Open: %s", m_MaterialsFileName.ToAscii());
		return ALBA_ERROR;
	}
	for (int i = 0; i < m_Materials.size(); i++)
	{
		fprintf(materialsFile, "%d %f %f %f\n", m_Materials[i].Id, m_Materials[i].Ex, m_Materials[i].Nuxy, m_Materials[i].Dens);
	}
	fclose(materialsFile);
	
	return ALBA_OK;
}
//----------------------------------------------------------------------------
void albaOpImporterAnsysCommon::AddElement(int Id, int nNodes, int type, int matId, int *nodes)
{
  AnsysElement elem;

  elem.Id=Id;
  elem.NodesNumber=nNodes;
  elem.MatId=matId;
  elem.Type=type;

  int *newNodes=new int[nNodes];

  memcpy(newNodes,nodes,nNodes*sizeof(int));
  elem.Nodes=newNodes;  

  m_Elements.push_back(elem);
}
//----------------------------------------------------------------------------
bool albaOpImporterAnsysCommon::IsInRange(int elemId, int partId)
{
  AnsysComponent comp = m_Components[partId];

  for (int i=0; i<comp.RangeNum/2; i++)
  {
    if(elemId>=comp.Ranges[(i*2)] && elemId<=comp.Ranges[(i*2)+1]) 
      return true;
  }

  return false;
}

