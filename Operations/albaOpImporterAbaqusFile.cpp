/*=========================================================================
  Program:   Bonemat
  Module:    albaOpImporterAbaqusFile.cpp
  Language:  C++
  Date:      $Date: 2009-05-19 14:29:53 $
  Version:   $Revision: 1.1 $
  Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"

#include "albaOpImporterAbaqusFile.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSmartPointer.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEMeshAnsysTextImporter.h"
#include "albaMatrix.h"
#include "albaProgressBarHelper.h"

#include "vtkALBASmartPointer.h"
#include "vtkMath.h"
#include "vtkTransform.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "albaGUIBusyInfo.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"


//buffer size 1024*1024 
#define READ_BUFFER_SIZE 1048576 

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterAbaqusFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterAbaqusFile::albaOpImporterAbaqusFile(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;
  m_Output=NULL;

  wxString userPath= wxStandardPaths::Get().GetUserDataDir();

  m_DataDir = userPath + "\\Data";
  m_CacheDir = m_DataDir + "\\AbaqusReaderCache";
  m_AbaqusInputFileNameFullPath		= "";
}
//----------------------------------------------------------------------------
albaOpImporterAbaqusFile::~albaOpImporterAbaqusFile()
{
  for (int i = 0; i < m_Parts.size(); i++)
  {  
    AbaqusPart currentPart=m_Parts[i];

    for (int j=0; j<currentPart.elementsVector.size(); j++)
    {
      delete [] currentPart.elementsVector[j].nodes;
    }

    currentPart.elementsVector.clear();
    currentPart.elsetsVector.clear();
		currentPart.elementsToElset.clear();
  }
		
  m_Parts.clear();
  m_MatIDMap.clear();
  
  albaDEL(m_ImportedVmeMesh);
}
//----------------------------------------------------------------------------
bool albaOpImporterAbaqusFile::InternalAccept(albaVME *node)
{
  return true;
}

//----------------------------------------------------------------------------
albaOp* albaOpImporterAbaqusFile::Copy()   
{
  albaOpImporterAbaqusFile *cp = new albaOpImporterAbaqusFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
albaString albaOpImporterAbaqusFile::GetWildcard()
{
  return "inp files (*.inp)|*.inp|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
void albaOpImporterAbaqusFile::OpRun()   
{  
  albaString wildcard = GetWildcard();

  int result = OP_RUN_CANCEL;
  m_AbaqusInputFileNameFullPath = "";

  wxString f;
  f = albaGetOpenFile("", wildcard).ToAscii(); 
  if(!f.IsEmpty() && wxFileExists(f))
  {
    m_AbaqusInputFileNameFullPath = f;
    Import();
    result = OP_RUN_OK;
  }
  albaEventMacro(albaEvent(this,result));  
}
//----------------------------------------------------------------------------
void albaOpImporterAbaqusFile::OnEvent(albaEventBase *alba_event) 
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
int albaOpImporterAbaqusFile::Import()
{
  int returnValue=ALBA_OK;

  m_LastMatId = 1;
  m_MatIDMap.clear();
	m_Parts.clear();

  albaLogMessage("Current working directory is: '%s' ", wxGetCwd().ToAscii());

  // Create tmp path
  mkdir(m_DataDir);

  if(!wxDirExists(m_DataDir.GetCStr()))
  {
    albaLogMessage("Cloud not create \"Data\" Directory");
    return ALBA_ERROR;
  }

  mkdir(m_CacheDir);
  if(!wxDirExists(m_CacheDir.GetCStr()))
  {
    albaLogMessage("Cloud not create Read Cache Directory");
    return ALBA_ERROR;
  }

  // Parsing Abaqus File
  if(ParseAbaqusFile(m_AbaqusInputFileNameFullPath) == ALBA_ERROR)
  {
    return ALBA_ERROR;
  }

  for (int p=0; p<m_Parts.size(); p++)
  {
    if(m_Parts[p].elementsVector.empty())
      continue;

    albaString nodesFileName = m_CacheDir + "\\nodes_part"<<p<<".lis";
    albaString elementsFileName = m_CacheDir + "\\elements_part"<<p<<".lis";
    albaString materialsFileName = m_CacheDir + "\\materials.lis" ;

    albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
    reader->SetNodesFileName(nodesFileName);
    reader->SetElementsFileName(elementsFileName);
    reader->SetMaterialsFileName(materialsFileName);
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
      wxString name, path, ext;
      wxFileName::SplitPath(m_AbaqusInputFileNameFullPath, &path, &name, &ext);

      albaNEW(m_ImportedVmeMesh);

      if(m_Parts[p].name.length()>0)
        name = name + "-" + m_Parts[p].name;

      m_ImportedVmeMesh->SetName(name);
      m_ImportedVmeMesh->SetDataByDetaching(reader->GetOutput()->GetUnstructuredGridOutput()->GetVTKData(),0);

      albaTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_ImportedVmeMesh->GetTagArray()->SetTag(tag_Nature);
      
      m_ImportedVmeMesh->ReparentTo(m_Input);

      for (int t=0; t<m_Transforms.size();t++)
      {
        if(m_Transforms[t].partName.compare(m_Parts[p].name))
        {                 
          m_ImportedVmeMesh->SetAbsMatrix(m_Transforms[t].matrix);
          
          break;
        }
      }

      m_ImportedVmeMesh->Update();
    }

    delete reader;
  }

  return returnValue;
}

//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::ParseAbaqusFile(albaString fileName)
{

	if (ReadInit(fileName, m_TestMode, true, "Please wait parsing Abaqus File...", m_Listener) == ALBA_ERROR)
	{
		albaLogMessage("Cannot Open: %s", fileName);
		ReadFinalize();
		return ALBA_ERROR;
	}

  // Init Files
  FILE *nodesFile=NULL, *partNodesFile=NULL, *materialsFile=NULL;
  
  albaString nodesFileName = m_CacheDir + "\\nodes_part0.lis";
  nodesFile = albaTryOpenFile(nodesFileName, "w");
  if (!nodesFile)
  {
    albaLogMessage("Cannot Open: %s",*nodesFileName);
    return ALBA_ERROR;
  }

  albaString materialsFileName =  m_CacheDir + "\\materials.lis";
  materialsFile = albaTryOpenFile(materialsFileName, "w");
  if (!materialsFile)
  {
    albaLogMessage("Cannot Open: %s",*materialsFileName);
    return ALBA_ERROR;
  }

  // Create default Part
  int currentPart = SetPart();
  int readnewline=true;

  while (!readnewline || (GetLine(true)) != 0) 
  {
    //if(strncmp (m_Line,"*HEADING",8) == 0)
    //{
    //  ReadHeader(nodesFile);
    //}
    readnewline=true;

    if(strncmp (m_Line,"*PART,",6) == 0)
    {      
      char partName[254];
      
      //*Part, name=Part1
      sscanf(m_Line, "*PART, NAME=%s", partName);
      currentPart = SetPart(partName);
      
      // Open files
      albaString partNodesFileName =  m_CacheDir + "\\nodes_part"<<currentPart<<".lis";
      partNodesFile = albaTryOpenFile(partNodesFileName, "w");
      if (!partNodesFile)
      {
        albaLogMessage("Cannot Open: %s",*partNodesFileName);
        return ALBA_ERROR;
      }
    }

		if (strncmp(m_Line, "*INSTANCE", 9) == 0)
		{
			ReadInstance();
		}

    if(strncmp (m_Line,"*NODE,",5) == 0)
    {
      if(currentPart==0) 
        ReadNodes(nodesFile);
      else
        ReadNodes(partNodesFile);
      readnewline=false;
    }

    if(strncmp (m_Line,"*ELEMENT,",8) == 0)
    {
      ReadElements();
      readnewline=false;
    }

    if(strncmp (m_Line,"*ELSET,",7) == 0)
    {
      ReadElset();
      readnewline=false;
    }

    if(strncmp (m_Line,"*SOLID",6) == 0)
    {
      ReadSolid();
    }
    
    if(strncmp (m_Line,"*MATERIAL,",10) == 0)
    {
      ReadMaterials(materialsFile);
			readnewline = false;
    }

    if(strncmp (m_Line,"*END PART",9) == 0)
    {
      //*End Part
      std::string line = m_Line;

      //Close files
      fclose(partNodesFile);

      currentPart = SetPart("",0);
    }
  }

  WriteElements();

  fclose(nodesFile);
  fclose(materialsFile);

  ReadFinalize();
  cppDEL(m_ProgressHelper);

  return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::SetPart(std::string name, int part)
{
  int currentPart =-1;

  if(part==-1)
  {
    // Add New Part
    AbaqusPart newPart;
    newPart.name=name;

    m_Parts.push_back(newPart);

    int nParts=m_Parts.size();
    currentPart=nParts-1;

    m_Elements = &m_Parts[currentPart].elementsVector;
    m_Elsets = &m_Parts[currentPart].elsetsVector;
		m_ElementsToElset = &m_Parts[currentPart].elementsToElset;
  }
  else
  {
    if(part<m_Parts.size())
    {
      // Rename Part
      if(name.compare("")!=0)
        m_Parts[part].name=name;

      m_Elements = &m_Parts[part].elementsVector;
      m_Elsets = &m_Parts[part].elsetsVector;
			m_ElementsToElset = &m_Parts[part].elementsToElset;

      currentPart=part;
    }
    else return ALBA_ERROR;
  }

  return currentPart;
}

//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::ReadHeader(FILE *outFile)
{
  int lineLenght;

  char jobkName[254];
  char projectkName[254];
  char echo[4], model[4], history[4], contact[4];

  while ((lineLenght = GetLine(true)) != 0 && strncmp (m_Line,"*PART,",6) != 0) 
  {
    if(strncmp (m_Line,"** JOB NAME:",12) == 0)
    {
      //** Job name:  Model name: 
      sscanf(m_Line, "** JOB NAME: %s MODEL NAME: %s", jobkName,projectkName);
    }

    //** Generated by: Bonemat

    if(strncmp (m_Line,"*PREPRINT,",10) == 0)
    {
      //*Preprint, echo=NO, model=NO, history=NO, contact=NO
      sscanf(m_Line, "*PREPRINT, ECHO=%s, MODEL=%s, HISTORY=%s, CONTACT=%s", echo,model,history,contact);
    }

    //**
    //** PARTS
    //**
  }

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::ReadNodes(FILE *outFile)
{
  int nodeId;
  double nodes[3];

  int nReaded=0, totReaded=0, indexNode=0;
  char *linePointer;

  // *Node
  //      1,         -1.0,          1.0,         -1.0
  while (GetLine(true) != 0)
  {
    if(strncmp (m_Line,"*NODE,",5) == 0)
     GetLine(true); 

    linePointer=m_Line;

    nReaded = sscanf(linePointer, "%d", &nodeId);
    totReaded+=nReaded;

    indexNode=0;

    while (nReaded && (linePointer = strchr(linePointer,',')))
    {
      linePointer++;

      nReaded = sscanf(linePointer, "%lf", nodes+indexNode);
      totReaded+=nReaded;

      indexNode++;
    }

    if(totReaded!=4) 
      break;

    totReaded = 0;
    fprintf(outFile,"%d\t%.13E\t%.13E\t%.13E\n", nodeId, nodes[0], nodes[1], nodes[2]);
  }

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::ReadElements()
{
  int elementId,nodes[20];
  int numElements=0,nNodes=0,elementMatId=0;
	int nReaded=0, totReaded=0;
	bool hasType;

	int oldElementsNum = (*m_Elements).size();

  // *Element, type=C3D4               or
  // *Element, type=C3D4, elset=name
 
  std::string line = m_Line;
  std::size_t pos = line.find("TYPE=");
  std::string typeElem = line.substr(pos+5);
  
  std::string elsetName="";
  bool hasElset=false;
  if((pos = line.find("ELSET=")) != std::string::npos)
  {
    elsetName = line.substr(pos+6,line.size()-1);
    hasElset=true;

    if((pos = elsetName.find(",")) != std::string::npos)
    {
      elsetName = elsetName.substr(0, pos);
    }
  }

	int numElemStart;

	if(typeElem [2]=='D') //ex. C3D3
  	numElemStart=3;
	else if (typeElem[3]=='D') //ex. RG3D3
		numElemStart=4;
	else 
		numElemStart=-1;

	if(numElemStart > 0)
	{
		char nNodesChar[10];
		int i=0;

		do 
		{
			nNodesChar[i]=typeElem[i+numElemStart];
			i++;
		} while (typeElem[i+numElemStart]>='0' && typeElem[i+numElemStart]<='9');
		
		nNodesChar[i]='\0';

		//if i can read correctly nNodes i set hasType to true to enable fast reading
		hasType = sscanf(nNodesChar,"%d",&nNodes);
	}
	else 
		hasType = false;

  while (GetLine(true) != 0)
  {
    if(hasType)
    {
      //  1,      1,      2,      3,      4,      5,      6    (one line) or

      //  1,      1,      2,      3,      4,                   (two lines)
      //  5,      6

      // Read first line
      int nReaded = sscanf(m_Line, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", &elementId, nodes+0,nodes+1,nodes+2,nodes+3,nodes+4,nodes+5,nodes+6,nodes+7,nodes+8,nodes+9);
      totReaded+=nReaded;

      if(nReaded != 0 && nReaded < nNodes+1)
      {
        // Read second line
        GetLine(true);
        int shift = nReaded-1;
        nReaded = sscanf(m_Line, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d", nodes+shift+0,nodes+shift+1,nodes+shift+2,nodes+shift+3,nodes+shift+4,nodes+shift+5,nodes+shift+6,nodes+shift+7);
        totReaded+=nReaded;
      }
    }
    else
    {
      //  1,      1,      2,      3,      4,      5,      6, ...    (no type - one line)

      char *linePointer=m_Line;

      nReaded = sscanf(linePointer, "%d", &elementId);
      totReaded=nReaded;

      int indexNode=0;

      while (nReaded && (linePointer = strchr(linePointer,',')))
      {
        linePointer++;

        nReaded = sscanf(linePointer, "%d", nodes+indexNode);
        totReaded+=nReaded;

        indexNode++;
      }

      nNodes = indexNode;
    }

    // Create Abaqus Element
    if( totReaded==nNodes+1 && nNodes>=3 )
    {
      AbaqusElement elem;

      elem.nodesNumber=nNodes;
      elem.Id=elementId;
      elem.type=3;

      int *newNodes=new int[nNodes];

      memcpy(newNodes,nodes,nNodes*sizeof(int));
      elem.nodes=newNodes;      

      (*m_Elements).push_back(elem);

      totReaded=0;
    }
    else
    {
      break;
    }
  } 

  // Create Abaqus Elset
  if(hasElset)
  {
    int elementsNum = (*m_Elements).size();
    
    AbaqusElset elset;
    elset.name=elsetName;

    int currentElsetIndex = (*m_Elsets).size();

    for (int j=oldElementsNum; j<elementsNum; j++)
    {
			int elementId = (*m_Elements)[j].Id;
			(*m_ElementsToElset)[elementId]=currentElsetIndex;
    }

    (*m_Elsets).push_back(elset);
  }

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::ReadElset()
{
  int currentElsetIndex = (*m_Elsets).size();
	int nReaded=1;
	char *linePointer;
	int elemId;

  //*Elset, elset=Set_Part1_1, ...
  std::string line = m_Line;
  std::size_t pos = line.find("ELSET=");
  std::string elsetName = line.substr(pos+6, line.size()-1);

   if((pos = elsetName.find(",")) != std::string::npos)
     elsetName = elsetName.substr(0, pos);

	 //*Elset, elset=Set_Part1_1, ..., generate
	 if (line.find("GENERATE") != std::string::npos)
	 {
		 int start=0, end=0, inc=0;
		 // Start, End, Incr.
		 //    1,  651,    1
		 GetLine(true);

		///RemoveInString(m_Line, ' ');		 
		//ReplaceInString(m_Line, ',', ' ');

		 int nReaded = sscanf(m_Line, "%d,%d,%d", &start, &end, &inc);

		 for (int i = start; i <= end; i += inc)
		 {
			 (*m_ElementsToElset)[i] = currentElsetIndex;
		 }
	 }
	 else
	 {
		 // Read element list
		 // 1, 2, 3, 4,  ...
		 while (GetLine(true) != 0)
		 {
			 linePointer = m_Line;

			 nReaded = sscanf(linePointer, "%d", &elemId);
			 if (nReaded)
			 {
				 (*m_ElementsToElset)[elemId] = currentElsetIndex;
			 }
			 else
				 break;

			 while (nReaded && (linePointer = strchr(linePointer, ',')))
			 {
				 linePointer++;
				 nReaded = sscanf(linePointer, "%d", &elemId);
				 (*m_ElementsToElset)[elemId] = currentElsetIndex;
			 }
		 }
	 }

  //*Solid Section, elset=Set_Part1_1, material=Mat_1 

  //Read Material name
  line = m_Line;
  pos = line.find("MATERIAL=");
  std::string matName ="";

  if(pos < line.length())
    matName = line.substr(pos+9);

  AbaqusElset elset;
  elset.name=elsetName;
  elset.matName=matName;
	
	(*m_Elsets).push_back(elset);

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::ReadSolid()
{
  //*Solid Section, elset=Set_Part1_1, material=Mat_1   
  std::string line = m_Line;

  // Read Elset name
  std::size_t pos = line.find("ELSET=");
  std::string elsetName = line.substr(pos+6);
  pos = elsetName.find(",");
  elsetName = elsetName.substr(0,pos);

  //Read Material name
  pos = line.find("MATERIAL=");
  std::string matName ="";

  if(pos < line.length())
    matName = line.substr(pos+9);  
  int res=0;

  // Assign matName at Elset
  for (int i=0; i<(*m_Elsets).size(); i++)
  {
    std::string cmpName = (*m_Elsets)[i].name;

		if (cmpName.size() > 0 && cmpName[cmpName.size() - 1] == '\n')
			cmpName = cmpName.substr(0, cmpName.size() - 1);
			
    int equalRes = cmpName.compare(elsetName);

    if(equalRes==0)
    {
      (*m_Elsets)[i].matName=matName;
      break;
    }
  }

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::ReadMaterials(FILE *outFile)
{
	char matEx[254], matNuxy[254], matDens[254];

	// *Material, name=Mat_1
	std::string line = m_Line;
	std::size_t pos = line.find("NAME=");
	std::string matName = line.substr(pos + 5);

	while (GetLine(true) != 0)
	{
		// Default Values
		sprintf(matEx, "0");
		sprintf(matNuxy, "0.3");
		sprintf(matDens, "0");

		// *Density
		if (strncmp(m_Line, "*DENSITY", 8) == 0)
		{
			// 1e-12,
			GetLine(true);
			ReplaceInString(m_Line, ',', ' ');

			int nReaded = sscanf(m_Line, "%s", matDens);
			if (nReaded == 0) sprintf(matDens, "0");

			GetLine(true); // Next Line
		}

		// *Elastic
		if (strncmp(m_Line, "*ELASTIC", 8) == 0)
		{
			line = m_Line;
			if(line.find("TYPE=") != std::string::npos)
			{
				// Not Supported
			}
			else
			{
				// 3.296537922150794, 0.35
				GetLine(true);
				ReplaceInString(m_Line, ',', ' ');

				int nReaded = sscanf(m_Line, "%s %s", matEx, matNuxy);
				if (nReaded == 1) sprintf(matNuxy, "0");
			}

			GetLine(true); // Next Line
		}
		
		if (m_MatIDMap.find(matName) == m_MatIDMap.end())
		{
			// Write Material file
			fprintf(outFile, "%d %s %s %s\n", m_LastMatId, matEx, matNuxy, matDens);

			m_MatIDMap[matName] = m_LastMatId;
			m_LastMatId++;
		}

		if (strncmp(m_Line, "*MATERIAL,", 10) == 0)
			break;
	}

	return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::ReadInstance()
  {
    //*Instance, name=Part1, part=Part1
    //  -1, 0, 0
    //  -1, 0, 0, -2, 0, 0, 90

    std::string line = m_Line;
    std::size_t pos = line.find("PART=");
    std::string partName = line.substr(pos+5);

    GetLine(true);

    AbaqusTransform myTransform;  
    myTransform.partName = partName;

    vtkTransform *tr = vtkTransform::New();
    tr->PostMultiply();

    float values[10];

    int nReaded = sscanf(m_Line, "%f, %f, %f", values+0,values+1,values+2);

    if(nReaded==3)
    {
      // Translation
      tr->Translate(values);
    }
		else
		{
			return ALBA_OK;
		}

    GetLine(true);

    nReaded = sscanf(m_Line, "%f, %f, %f, %f, %f, %f, %f", values+3,values+4,values+5,values+6,values+7,values+8,values+9);
    if(nReaded==7)
    {
      albaMatrix rotMatrix= RotationMatrixUnnormalizedDirVector(values[3],values[4],values[5],values[6],values[7],values[8],values[9]);
      // Rotation
      tr->Concatenate(rotMatrix.GetVTKMatrix());
    }

    tr->Update();
    myTransform.matrix=tr->GetMatrix();

    m_Transforms.push_back(myTransform);

    return ALBA_OK;
  }
//----------------------------------------------------------------------------
int albaOpImporterAbaqusFile::WriteElements()
{
  int elemId, matId; 
  int numElements=0,nNodes=0;

  FILE *elementsPartFile;

  for (int p=0; p<m_Parts.size(); p++)
  {
    m_Elements = &m_Parts[p].elementsVector;
    m_Elsets = &m_Parts[p].elsetsVector;
		m_ElementsToElset = &m_Parts[p].elementsToElset;

    albaString elementsfileName = m_CacheDir + "\\elements_part"<<p<<".lis";

    elementsPartFile = albaTryOpenFile(elementsfileName, "w");

    if (!elementsPartFile)
    {
      albaLogMessage("Cannot Open: %s",elementsfileName);
      return ALBA_ERROR;
    }

    //////
    for (int e=0; e<(*m_Elements).size(); e++)
    {
      AbaqusElement myElem = (*m_Elements)[e];
				
      elemId = myElem.Id;

			matId=0;
			std::map<int,int>::iterator elsetIndexIterator = m_ElementsToElset->find(elemId);
			if(elsetIndexIterator!=m_ElementsToElset->end())
			{
				int elsetIndex=elsetIndexIterator->second;
				AbaqusElset elset = ((*m_Elsets)[elsetIndex]);

				std::string matName = elset.matName;
				std::map<std::string,int>::iterator matNameIter = m_MatIDMap.find(matName);
				
				if(matNameIter != m_MatIDMap.end())
					matId=matNameIter->second;
			}
      
      fprintf(elementsPartFile,"%d\t%d", elemId,matId);

			nNodes = myElem.nodesNumber;
			for (int j = 0; j < nNodes; j++)
      {
        fprintf(elementsPartFile,"\t%d",myElem.nodes[j]);
      }

      fprintf(elementsPartFile,"\n");
    }
    
    fclose(elementsPartFile);
  }

  return ALBA_OK;
}

//----------------------------------------------------------------------------
albaMatrix albaOpImporterAbaqusFile::RotationMatrixUnnormalizedDirVector(double a, double b, double c, double d, double e, double f, double theta) 
{
  // References
  // http://www.egr.msu.edu/software/abaqus/Documentation/docs/v6.7/books/key/default.htm?startat=ch09abk19.html#usb-kws-minstance
  //
  // http://inside.mines.edu/fs_home/gmurray/ArbitraryAxisRotation/ArbitraryAxisRotation.pdf
  
  albaMatrix matrix;

  double u= d-a;
  double v= e-b;
  double w= f-c;

  double thetaRad=vtkMath::DegreesToRadians()*theta;

  double l = sqrt(u*u + v*v + w*w);

  // Set some intermediate values.
  double u2 = u*u;
  double v2 = v*v;
  double w2 = w*w;
  double cosT = cos(thetaRad);
  double oneMinusCosT = 1 - cosT;
  double sinT = sin(thetaRad);
  double l2 = u2 + v2 + w2;

  // Build the matrix entries element by element.
  matrix.SetElement(0,0, (u2 + (v2 + w2) * cosT)/l2);
  matrix.SetElement(0,1,(u*v * oneMinusCosT - w*l*sinT)/l2);
  matrix.SetElement(0,2,(u*w * oneMinusCosT + v*l*sinT)/l2);
  matrix.SetElement(0,3,((a*(v2 + w2) - u*(b*v + c*w)) * oneMinusCosT + (b*w - c*v)*l*sinT)/l2);

  matrix.SetElement(1,0,(u*v * oneMinusCosT + w*l*sinT)/l2);
  matrix.SetElement(1,1,(v2 + (u2 + w2) * cosT)/l2);
  matrix.SetElement(1,2,(v*w * oneMinusCosT - u*l*sinT)/l2);
  matrix.SetElement(1,3,((b*(u2 + w2) - v*(a*u + c*w)) * oneMinusCosT + (c*u - a*w)*l*sinT)/l2);

  matrix.SetElement(2,0,(u*w * oneMinusCosT - v*l*sinT)/l2);
  matrix.SetElement(2,1,(v*w * oneMinusCosT + u*l*sinT)/l2);
  matrix.SetElement(2,2,(w2 + (u2 + v2) * cosT)/l2);
  matrix.SetElement(2,3,((c*(u2 + v2) - w*(a*u + b*v)) * oneMinusCosT + (a*v - b*u)*l*sinT)/l2);

  return matrix;
}