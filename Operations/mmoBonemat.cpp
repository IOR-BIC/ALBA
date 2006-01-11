/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoBonemat.cpp,v $
  Language:  C++
  Date:      $Date: 2006-01-11 14:05:24 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
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

#include "mmoBonemat.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMERoot.h"
#include "mafString.h"
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include "mesh.h"
#include "hashtable.h"

#include <stdlib.h>

typedef struct {
  ID_TYPE id;  
  double ro;
  double E;
} ElementProp;

int compar(const void *p1, const void *p2) {
  double result;

  result = ((ElementProp *) p2)->E -  ((ElementProp *) p1)->E; // decreasing order  
  if (result < 0)
    return -1;
  if (result > 0)
    return 1;
  return 0;  
}


//----------------------------------------------------------------------------
mmoBonemat::mmoBonemat(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = false;
  m_Input   = NULL;
  m_ConfigurationFileName = "";
  m_FrequencyFileName = "frequency.txt";
  m_InputCTFileName = "";   
  m_InputCTFileName = "";
  m_OutputMeshFileName = "output.ntr";

  m_CalibrationFirstPoint[0] = 0;
  m_CalibrationFirstPoint[1] = 0;
  m_CalibrationSecondPoint[0] = 1;
  m_CalibrationSecondPoint[1] = 1;

  m_FirstExponentialCoefficientsVector[0] = 0;
  m_FirstExponentialCoefficientsVector[1] = 1;
  m_FirstExponentialCoefficientsVector[2] = 1;

  m_GapValue = 1;
  m_StepsNumber = 4;

}
//----------------------------------------------------------------------------
mmoBonemat::~mmoBonemat()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmoBonemat::Accept(mafNode *node)
//----------------------------------------------------------------------------
{ 
  return true;
}
//----------------------------------------------------------------------------
mafOp* mmoBonemat::Copy()   
//----------------------------------------------------------------------------
{
  mmoBonemat *cp = new mmoBonemat(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum BONEMAT_ID
{
  ID_FIRST = MINID,
  ID_OPEN_CONFIGURATION_FILE,
  ID_SAVE_CONFIGURATION_FILE,
  ID_SAVE_CONFIGURATION_FILE_AS,
  ID_OPEN_INPUT_MESH,
  ID_OPEN_INPUT_TAC,
  ID_OUTPUT_MESH_NAME,
  ID_OUTPUT_FREQUENCY_FILE_NAME,
  ID_EXECUTE,
  ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR,
  ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR,
  ID_HU_THRESHOLD,
  ID_CALIBRATION_FIRST_POINT,
  ID_CALIBRATION_SECOND_POINT,
  ID_STEPS_NUMBER,
  ID_GAP_VALUE,
};

//----------------------------------------------------------------------------
void mmoBonemat::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildcVTK = "vtk Data (*.vtk)|*.vtk";
  mafString wildcNeutral = "neutral Data (*.ntr)|*.ntr";


  m_Gui = new mmgGui(this);
  //m_Gui->FileSave(ID_CHOOSE_FILENAME,"vtk file", &m_File, wildc);
  /*m_Gui->Label("file type",true);
  m_Gui->Label("absolute matrix",true);
  if (m_Input->IsA("mafVMESurface") || m_Input->IsA("mafVMEPointSet") || m_Input->IsA("mafVMEGroup"))
  m_Gui->Enable(ID_ABS_MATRIX,true);
  else
  m_Gui->Enable(ID_ABS_MATRIX,false);
  */

  m_Gui->Label(""); 
//   m_Gui->Button(ID_OPEN_CONFIGURATION_FILE, "open configuration file");
  //m_Gui->Button(ID_SAVE_CONFIGURATION_FILE, "save configuration file");
  //m_Gui->Button(ID_SAVE_CONFIGURATION_FILE_AS, "save configuration file as");
  m_Gui->Label("open input mesh");
  m_Gui->FileOpen(ID_OPEN_INPUT_MESH, "", &m_InputMeshFileName, wildcNeutral);
  m_Gui->Label("open input tac");
  m_Gui->FileOpen(ID_OPEN_INPUT_TAC, "", &m_InputCTFileName, wildcVTK);
  m_Gui->Label("output mesh name");
  m_Gui->String(ID_OUTPUT_MESH_NAME, "", &m_OutputMeshFileName);
  m_Gui->Label("output frequency fine name");
  m_Gui->String(ID_OUTPUT_FREQUENCY_FILE_NAME,"", &m_FrequencyFileName);
  m_Gui->Divider();
  m_Gui->Label("coefficients vector: a, b, c");
  m_Gui->VectorN(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR, "", m_FirstExponentialCoefficientsVector, 3);
  
  /*m_Gui->Label("second coefficients vector");
  m_Gui->VectorN(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR, "", m_SecondExponentialCoefficientsVector);
  m_Gui->Label("hu threshold");
  m_Gui->Double(::ID_HU_THRESHOLD, "", &m_HUThreshold);*/

  m_Gui->Label("first calibration point: x0, y0");
  m_Gui->VectorN(::ID_CALIBRATION_FIRST_POINT, "",m_CalibrationFirstPoint,2);
  m_Gui->Label("second calibration point: x1, y1");
  m_Gui->VectorN(::ID_CALIBRATION_SECOND_POINT, "",m_CalibrationSecondPoint,2);
  m_Gui->Label("gap value");
  m_Gui->Double(::ID_GAP_VALUE, "", &m_GapValue);
  m_Gui->Divider();
  m_Gui->Button(ID_EXECUTE, "execute");

  m_Gui->OkCancel();

}

//----------------------------------------------------------------------------
void mmoBonemat::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
	ShowGui();
}

//----------------------------------------------------------------------------
int mmoBonemat::SaveConfigurationFileAs()
//----------------------------------------------------------------------------
{
  mafString initialFileName;
  initialFileName = mafGetApplicationDirectory().c_str();
  initialFileName.Append("\\newConfigurationFile.conf");

  mafString wildc = "configuration file (*.conf)|*.conf";
  mafString newFileName = mafGetSaveFile(initialFileName.GetCStr(), wildc).c_str();
 
  if (newFileName = "") return 1;

  return SaveConfigurationFile(newFileName.GetCStr());
}

//----------------------------------------------------------------------------
int mmoBonemat::SaveConfigurationFile(const char *fileName)
//----------------------------------------------------------------------------
{

  //char *param_filename, dataset_filename[256], mesh_input_filename[256], mesh_output_filename[256], freq_filename[256];
  FILE *param_fp;
  //*dataset_fp, *mesh_input_fp, *mesh_output_fp, *freq_fp;
  //double  ROa, ROb;
  //ID_TYPE numNodes, numElements, numMats, id, i, key, matKey, numElementNodes;
  //char line[256];
  //float x[3];
  //fpos_t file_loc;
  //int ElementShape; 
  //Mesh *mesh;
  //Element *element;  
  //Node *node;
  //unsigned int xdim, ydim, zdim;
  //DataSet *dataset;
  //int type;
  //double HU, E, G, ro, Ni = 0.3;
  //// ElementProp *propsWithIntegralAtStartup, **matProp;
  //ID_TYPE freq;
  //char *header = "%2d%8d%8d%8d%8d%8d%8d%8d%8d\n";
  //char *nodedata = "%16.8E%16.8E%16.8E\n";
  //char *matdata = "%16.9E%16.9E%16.9E%16.9E%16.9E\n" ;
  //char *elemdata1 = "%8d%8d%8d%8d%16.9E%16.9E%16.9E\n";
  //HashTable table;

  if ((param_fp = fopen(fileName, "w")) == NULL) {

    std::cerr << "Error opening " << fileName << "\n";
    return 1;
  }

  fprintf(param_fp, "%s %s %s %s\n", m_InputMeshFileName.GetCStr(), m_InputCTFileName.GetCStr(), m_OutputMeshFileName.c_str(), m_FrequencyFileName.c_str());
  fprintf(param_fp, "%lf %lf %lf %lf\n", &m_CalibrationFirstPoint[0], &m_CalibrationFirstPoint[1], &m_CalibrationSecondPoint[0], &m_CalibrationSecondPoint[1]);
  fprintf(param_fp, "%lf %lf %lf\n", &m_FirstExponentialCoefficientsVector[0], &m_FirstExponentialCoefficientsVector[1], &m_FirstExponentialCoefficientsVector[2]);
  fprintf(param_fp, "%lf\n", &m_GapValue);
  fprintf(param_fp, "%u\n", &m_StepsNumber);

  fclose(param_fp);

  return 0;
}
//----------------------------------------------------------------------------
void mmoBonemat::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case wxOK:
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
  
      
      case ID_OPEN_CONFIGURATION_FILE:
      {
        // OpenConfigurationFile();
      }  
      break;

      //case ID_SAVE_CONFIGURATION_FILE:
      //{
      //  SaveConfigurationFile(m_ConfigurationFileName.GetCStr());
      //}
      //break;

      //case ID_SAVE_CONFIGURATION_FILE_AS	:
      //{
      //  SaveConfigurationFileAs();
      //}
      //break;
          
      case ID_EXECUTE	:
      {
        Execute();
      }
      break;

      case VME_ADD:
        {
          //trap the VME_ADD of the mmoCollapse and mmoExplode to update the
          //m_Input, then forward the message to mafDMLlogicMDI
          this->m_Input = e->GetVme();
          mafEventMacro(mafEvent(this,VME_ADD,this->m_Input));
        }
        break;
      default:
        mafEventMacro(*e);
      break;
    }
	}
}
//----------------------------------------------------------------------------
void mmoBonemat::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}

//----------------------------------------------------------------------------
const char* mmoBonemat::GetConfigurationFileName()
//----------------------------------------------------------------------------
{
  return m_ConfigurationFileName.GetCStr();
}

//----------------------------------------------------------------------------
void mmoBonemat::SetConfigurationFileName(const char* name)
//----------------------------------------------------------------------------
{
  m_ConfigurationFileName = name;  
}

//----------------------------------------------------------------------------
const char* mmoBonemat::GetInputCTFileName()
//----------------------------------------------------------------------------
{
  return m_InputCTFileName.GetCStr();
}

//----------------------------------------------------------------------------
void mmoBonemat::SetInputCTFileName(const char* name)
//----------------------------------------------------------------------------
{
  m_InputCTFileName = name;  
}

//----------------------------------------------------------------------------
const char* mmoBonemat::GetOutputMeshFileName()
//----------------------------------------------------------------------------
{
  return m_OutputMeshFileName.c_str();
}

//----------------------------------------------------------------------------
void mmoBonemat::SetOutputMeshFileName(const char* name)
//----------------------------------------------------------------------------
{
  m_OutputMeshFileName = name;  
}

//----------------------------------------------------------------------------
const char* mmoBonemat::GetInputMeshFileName()
//----------------------------------------------------------------------------
{
  return m_InputMeshFileName.GetCStr();
}

//----------------------------------------------------------------------------
void mmoBonemat::SetInputMeshFileName(const char* name)
//----------------------------------------------------------------------------
{
  m_InputMeshFileName = name;  
}

//----------------------------------------------------------------------------
const char* mmoBonemat::GetFrequencyFileName()
//----------------------------------------------------------------------------
{
  return m_FrequencyFileName.c_str();
}

//----------------------------------------------------------------------------
void mmoBonemat::SetFrequencyFileName(const char* name)
//----------------------------------------------------------------------------
{
  m_FrequencyFileName = name;  
}


//----------------------------------------------------------------------------
int mmoBonemat::OpenConfigurationFile()
//----------------------------------------------------------------------------
{
  /*char *param_filename, */
  //, *dataset_fp, *mesh_input_fp, *mesh_output_fp, *freq_fp;
  //double ROa, ROb;
  //ID_TYPE numNodes, numElements, numMats, id, i, key, matKey, numElementNodes;
  //char line[256];
  //float x[3];
  //fpos_t file_loc;
  //int ElementShape; 
  //Mesh *mesh;
  //Element *element;  
  //Node *node;
  //unsigned int xdim, ydim, zdim;
  //DataSet *dataset;
  //int type;
  //double   HU, E, G, ro, Ni = 0.3;
  //// ElementProp *propsWithIntegralAtStartup, **matProp;
  //ID_TYPE freq;
  //char *header = "%2d%8d%8d%8d%8d%8d%8d%8d%8d\n";
  //char *nodedata = "%16.8E%16.8E%16.8E\n";
  //char *matdata = "%16.9E%16.9E%16.9E%16.9E%16.9E\n" ;
  //char *elemdata1 = "%8d%8d%8d%8d%16.9E%16.9E%16.9E\n";
  //HashTable table;

  // PARSING ARGUMENTS
  //if (argc !=2) {
  //  std::cerr << "USAGE: " << argv[0] << " param_file\n";
  //  return 1;
  //}

  std::string initial = mafGetApplicationDirectory().c_str();

  std::string returnString = mafGetOpenFile(initial.c_str(), "");

  if (returnString == "")
  {
    return 1;
  }

  m_ConfigurationFileName = returnString.c_str();  
  
  std::ifstream inputFile(m_ConfigurationFileName.GetCStr(), std::ios::in);

  if (inputFile == NULL) {
    std::cerr << "Error opening " << m_ConfigurationFileName.GetCStr() << "\n";
    return 1;
  }

  
  
  /*fscanf(param_fp, "%s %s %s %s\n", mesh_input_filename, dataset_filename, mesh_output_filename, freq_filename);*/
  
  std::string meshInputFilename, datasetFilename, meshOutputFilename, frequencyFilename;
  inputFile  >> meshInputFilename >> datasetFilename >> meshOutputFilename >> frequencyFilename;

  /*fscanf(param_fp, "%lf %lf %lf %lf\n", &m_CalibrationFirstPoint[0], &m_CalibrationFirstPoint[1], &m_CalibrationSecondPoint[0], &m_CalibrationSecondPoint[1]);*/
  inputFile >> m_CalibrationFirstPoint[0] >> m_CalibrationFirstPoint[1] >> m_CalibrationSecondPoint[0] >> m_CalibrationSecondPoint[1];
  
  /*fscanf(param_fp, "%lf %lf %lf\n", &m_FirstExponentialCoefficientsVector[0], &m_FirstExponentialCoefficientsVector[1], &m_FirstExponentialCoefficientsVector[2]);*/
  inputFile >> m_FirstExponentialCoefficientsVector[0] >> m_FirstExponentialCoefficientsVector[1] >> m_FirstExponentialCoefficientsVector[2];
  
  //fscanf(param_fp, "%lf\n", &m_GapValue);
  inputFile >> m_GapValue; 

  /*fscanf(param_fp, "%u\n", &m_StepsNumber);*/
  inputFile >> m_StepsNumber;

  /*fclose(param_fp);*/
  inputFile.close();

  m_InputMeshFileName = meshInputFilename.c_str();
  m_InputCTFileName = datasetFilename.c_str();
  m_OutputMeshFileName = meshOutputFilename.c_str();
  m_FrequencyFileName = frequencyFilename.c_str();
  
  return 0;

}

//----------------------------------------------------------------------------
int mmoBonemat::Execute()
//----------------------------------------------------------------------------
{
  char *param_filename, dataset_filename[256], mesh_input_filename[256], mesh_output_filename[256], freq_filename[256];
  FILE *param_fp, *dataset_fp, *mesh_input_fp, *mesh_output_fp, *freq_fp;
  double ROa, ROb;
  int m_StepsNumber;
  ID_TYPE numNodes, numElements, numMats, id, i, key, matKey, numElementNodes;
  char line[256];
  float x[3];
  fpos_t file_loc;
  int ElementShape; 
  Mesh *mesh;
  Element *element;  
  Node *node;
  unsigned int xdim, ydim, zdim;
  DataSet *dataset;
  int type;
  double HU, E, G, ro, Ni = 0.3;
  ElementProp *propsWithIntegralAtStartup, **matProp;
  ID_TYPE freq;
  char *header = "%2d%8d%8d%8d%8d%8d%8d%8d%8d\n";
  char *nodedata = "%16.8E%16.8E%16.8E\n";
  char *matdata = "%16.9E%16.9E%16.9E%16.9E%16.9E\n" ;
  char *elemdata1 = "%8d%8d%8d%8d%16.9E%16.9E%16.9E\n";
  HashTable table;

  if ((mesh_input_fp = fopen(m_InputMeshFileName.GetCStr(), "r")) == NULL) {
    std::cerr << "Error opening " << m_InputMeshFileName.GetCStr() << "\n";
    return 1;
  }

  if ((dataset_fp = fopen(m_InputCTFileName.GetCStr(), "r")) == NULL) {
    std::cerr << "Error opening " << m_InputCTFileName.GetCStr() << "\n";
    return 1;
  }

  mafString fullOutputMeshFileName = mafGetApplicationDirectory().c_str() ;
  fullOutputMeshFileName.Append("\\");
  fullOutputMeshFileName.Append(m_OutputMeshFileName.c_str());

  if ((mesh_output_fp = fopen(fullOutputMeshFileName.GetCStr(), "w")) == NULL) {
    std::cerr << "Error opening " << fullOutputMeshFileName.GetCStr() << "\n";
    return 1;
  }

  mafString fullFrequencyFileName = mafGetApplicationDirectory().c_str();
  fullFrequencyFileName.Append("\\");
  fullFrequencyFileName.Append(m_FrequencyFileName.c_str());

  if ((freq_fp = fopen(fullFrequencyFileName.GetCStr(), "w")) == NULL) {
    std::cerr << "Error opening " << fullFrequencyFileName.GetCStr() << "\n";
    return 1;
  }


  //
  // READING MESH
  //

  std::cout << "-- Reading mesh" << std::endl ;

  // read model summary data

  fgets(line,82,mesh_input_fp);
  fgets(line,82,mesh_input_fp);

  fscanf(mesh_input_fp,"%*d%*d%*d%*d%d%d%d%*d%*d\n",&numNodes,&numElements,&numMats);
  fgets(line,82,mesh_input_fp);

  std::cout << "Number of nodes: " << numNodes << std::endl;
  std::cout << "Number of elements: " << numElements << std::endl;
  std::cout << "Number of materials: "<< numMats << std::endl << std::endl;

  mesh = Mesh::New();
  mesh->SetNumberOfNodes(numNodes);
  mesh->SetNumberOfElements(numElements);


  // Read point data

  HashTableInit(&table);

  for (id=0; id < numNodes; id++) {
    fgetpos(mesh_input_fp, &file_loc);
    fscanf(mesh_input_fp, "%*d %d", &key);    
    fsetpos(mesh_input_fp, &file_loc);
    fgets(line, 82, mesh_input_fp);

    fscanf(mesh_input_fp, "%16E%E%E\n", &x[0], &x[1], &x[2]);
    //if (id ==0) {
    //	std::cout<<"x0"<<x[0]<<"x1"<<x[1]<<"x2"<<x[2]<<"\n";
    //}//flu
    mesh->SetNode(id, key, x);
    HashTableSetValue(&table, key, mesh->GetNode(id));

    fgets(line, 82, mesh_input_fp);          
  }

  //HashTablePrint(&table, printnode);

  // Read element data

  for(id=0; id < numElements; id++) {
    fscanf(mesh_input_fp,"%*d %d %d %*d %*d %*d %*d %*d %*d\n", &key ,&ElementShape);   
    fscanf(mesh_input_fp,"%d %*d %d %*d %*lE %*lE %*lE\n", &numElementNodes, &matKey);

    switch (ElementShape) {
    case 5: 
      if (numElementNodes == 4) 
        element = Tetra::New();
      else 
        element = Tetra10::New();
      break;      
    case 7: element = Wedge::New(); break;
    case 8: element = Hexa::New(); break;
    default: 
      std::cerr << "Element type  not supported";
      return 1;
    }

    element->SetKey(key);
    //element->SetMaterial(matKey);

    for(i=0; i < numElementNodes; i++) {
      fscanf(mesh_input_fp,"%u", &key);   
      element->SetNode(i, (Node *) HashTableGetValue(&table, key));
    }

    mesh->SetElement(id, element);

    fscanf(mesh_input_fp,"\n");
  }

  HashTableDelete(&table);

  /*
  // Read material data

  for (i =0; i < numMats ; i++) {  
  fscanf(mesh_input_fp,"%*d %d %*d %*d %*d %*d %*d %*d %*d\n", &matId);
  fscanf(mesh_input_fp,"%*E %E %*E %*E %*E\n", &value);

  mats[i]->SetMaterial(matId, value);

  for(j=0; j<18 ; j++) 
  fscanf(mesh_input_fp,"%*E %*E %*E %*E %*E\n");
  fscanf(mesh_input_fp,"%*E\n");

  }
  */

  fclose(mesh_input_fp);


  // READING DATASET

  fgets(line, 255, dataset_fp); 


  //
  // read title
  //

  fgets(line, 255, dataset_fp); 
  std::cout << "-- Reading dataset entitled: " << line << "\n";

  //
  // read type
  //

  fgets(line, 255, dataset_fp);

  if ( !strncmp(line,"ASCII",5) )
  {
    type = 0;
  }
  else if ( !strncmp(line,"BINARY",6) )
  {
    type = 1;
  }
  else
  {
    std::cerr << "Unrecognized file type: "<< line << " for file: " 
      << dataset_filename << "\n";      
    return 1;
  }

  fscanf(dataset_fp, "%s", line);

  if ( !strncmp(line, "DATASET", 7) ) {
    fscanf(dataset_fp, "%s\n", line);
    if  ( ! strncmp(line,"STRUCTURED_POINTS",17) )
      dataset = SP::New();
    else if ( ! strncmp(line,"RECTILINEAR_GRID",16) )
      dataset = RG::New();
    else if ( ! strncmp(line,"CT_SCAN",7) )
      dataset = CT::New();
    else {
      std::cerr << "Unrecognized dataset type: "<< line << " for file: " 
        << dataset_filename << "\n";      
      return 1;
    }
  }
  else {
    std::cerr << "Error reading  dataset type: "<< line << " for file: " 
      << dataset_filename << "\n";      
    return 1;
  }

  fscanf(dataset_fp, "%s %u %u %u\n", line, &xdim, &ydim, &zdim);
  dataset->SetDimensions(xdim, ydim, zdim); 
  if (dataset->ReadData(dataset_fp, type))
    return 1;

  fclose(dataset_fp);

  //  COMPUTE ELEMENTS DATA using integration startup

  std::cout << "-- Mapping data\n";

  propsWithIntegralAtStartup = new ElementProp[numElements];
  matProp = new ElementProp*[numElements];

  ROb = (m_CalibrationSecondPoint[1] - m_CalibrationFirstPoint[1]) / double(m_CalibrationSecondPoint[0] - m_CalibrationFirstPoint[0]);
  ROa = m_CalibrationFirstPoint[1] - m_CalibrationFirstPoint[0] * ROb;

  
  for (id=0; id < numElements; id++) {  
    HU = mesh->GetElement(id)->ComputeScalar(dataset, m_StepsNumber);
    // ro = a + b * HU
    propsWithIntegralAtStartup[id].ro = ROa + ROb * HU;
    if (propsWithIntegralAtStartup[id].ro <= 0) {
      propsWithIntegralAtStartup[id].ro = 1e-6;
    } 
    // E = a + b * ro ^ c
    propsWithIntegralAtStartup[id].E = m_FirstExponentialCoefficientsVector[0] + m_FirstExponentialCoefficientsVector[1] * pow(propsWithIntegralAtStartup[id].ro, m_FirstExponentialCoefficientsVector[2]);
    if (propsWithIntegralAtStartup[id].E <= 0) {
      propsWithIntegralAtStartup[id].E = 1e-6;
    } 

    propsWithIntegralAtStartup[id].id = id;
  }
   

  //  COMPUTE ELEMENTS DATA using integration at the end

  std::cout << "-- Mapping data\n";
 
  ElementProp *propsWithIntegralAtTheAnd; // , **matProp;
  propsWithIntegralAtTheAnd = new ElementProp[numElements];

  int pointsNumber = dataset->GetDimensions()[0] * dataset->GetDimensions()[1] * dataset->GetDimensions()[2]; 
  for (int pointID = 0; pointID < pointsNumber; pointID++)
  {
    Scalars *datasetScalars  = dataset->GetScalars();
    HU =  datasetScalars->GetScalar(pointID);
    // ro = a + b * HU
    double density = ROa + ROb * HU;
    if (density <= 0) {
      density = 1e-6;
    } 
    // E = a + b * ro ^ c
    double youngModule = m_FirstExponentialCoefficientsVector[0] + m_FirstExponentialCoefficientsVector[1] * pow(density, m_FirstExponentialCoefficientsVector[2]);
    if (youngModule <= 0) {
      youngModule = 1e-6;
    } 
    
    datasetScalars->SetScalar(pointID, youngModule);
  }

  for (id=0; id < numElements; id++) {  
    double meanYoungModule = mesh->GetElement(id)->ComputeScalar(dataset, m_StepsNumber);
    
    propsWithIntegralAtTheAnd[id].E = meanYoungModule;
    propsWithIntegralAtTheAnd[id].id = id;
    propsWithIntegralAtTheAnd[id].ro = 0;
  }

  // COMPUTE MATERIALS & WRITE FREQUENCY FILE

  // Sorting propsWithIntegralAtTheAnd according to E value in decreasing order
  qsort(propsWithIntegralAtTheAnd, numElements, sizeof(ElementProp), compar);
  qsort(propsWithIntegralAtStartup, numElements, sizeof(ElementProp), compar);

  std::cout << "ROmax: " << propsWithIntegralAtStartup[0].ro << "\n";
  std::cout << "ROmin: " << propsWithIntegralAtStartup[numElements - 1].ro << "\n";
  std::cout << "Emax: " << propsWithIntegralAtTheAnd[0].E << "\n";
  std::cout << "Emin: " << propsWithIntegralAtTheAnd[numElements - 1].E << "\n\n";

  std::cout << "-- Writing frequency file\n";

  fprintf(freq_fp, "ro \t\t E \t\t NUMBER OF ELEMENTS\n\n");

  freq = 0;
  numMats = 1;
  ro = propsWithIntegralAtStartup[0].ro;
  E = propsWithIntegralAtTheAnd[0].E;  
  matProp[0] = &propsWithIntegralAtTheAnd[0];
  for (id = 0; id < numElements; id++) {
    if (E - propsWithIntegralAtTheAnd[id].E > m_GapValue) {
      fprintf(freq_fp, "%f \t %f \t %d\n", ro, E, freq); 
      matProp[numMats] = &propsWithIntegralAtTheAnd[id];
      numMats++;
      E = propsWithIntegralAtTheAnd[id].E;
      ro = propsWithIntegralAtStartup[id].ro;
      freq = 1;
    }
    else
      freq++;
    mesh->GetElement(propsWithIntegralAtTheAnd[id].id)->SetMatKey(numMats);
  }
  fprintf(freq_fp, "%f \t %f \t %d\n\n", ro, E, freq);
  fclose(freq_fp);

  std::cout <<"Number of materials: " << numMats << std::endl << std::endl;

  // WRITE MESH

  std::cout << "-- Writing mesh\n\n";

  // Title
  fprintf(mesh_output_fp, header, 25, 0, 0, 1, 0, 0, 0, 0, 0);
  fprintf(mesh_output_fp, ".... Output from Bonemat\n");      

  // Summary data
  fprintf(mesh_output_fp, header, 26, 0, 0, 1, numNodes, numElements, numMats, 4, 0);
  fprintf(mesh_output_fp,"date        time    version\n");

  // Node data

  for (id = 0; id < numNodes; id++) {
    node = mesh->GetNode(id);
    fprintf(mesh_output_fp, header, 1, node->key, 0, 2, 0, 0, 0, 0, 0);    
    fprintf(mesh_output_fp, nodedata,  node->x[0],  node->x[1],  node->x[2]);
    fprintf(mesh_output_fp,"1G       8       0       0  000000\n");
  }

  // Element data  

  for (id = 0; id < numElements; id++) {
    element = mesh->GetElement(id);
    numElementNodes = element->GetNumberOfNodes();

    fprintf(mesh_output_fp, header, 2, element->GetKey(), element->GetType(), 1 + (numElementNodes+9)/10, 0, 0, 0, 0, 0);
    fprintf(mesh_output_fp, elemdata1, numElementNodes, 0, element->GetMatKey() , 0, 0, 0 ,0);

    for(i=0; i < numElementNodes; i++) {
      node = element->GetNode(i);      
      fprintf(mesh_output_fp,"%8d", node->key);
    }
    fprintf(mesh_output_fp,"\n");
  }

  // Material properties

  for(id = 0; id < numMats; id++) {
    ro = matProp[id]->ro;
    if ( ro < 0) 
      std::cerr << "WARNING: Negative density for material" << id + 1 << std::endl;
    E = matProp[id]->E;
    G = E / (2.0 * (1 + Ni) );

    fprintf(mesh_output_fp, header, 3, id + 1, 1, 20, 0, 0, 0, 0, 0);
    fprintf(mesh_output_fp, matdata, 0.0, ro, 0.0, 0.0, 0.0);
    fprintf(mesh_output_fp, matdata, 0.0, 0.0, 1.0, 0.0, 0.0);
    fprintf(mesh_output_fp, matdata, 0.0, 0.0, 0.0, 0.0, 0.0);
    fprintf(mesh_output_fp, matdata, 0.0, 0.0, 0.0, 0.0, 0.0);
    fprintf(mesh_output_fp, matdata, 0.0, 0.0, 0.0, 0.0, 0.0);
    fprintf(mesh_output_fp, matdata, 0.0, E, E, E, Ni);
    fprintf(mesh_output_fp, matdata, Ni, Ni, G, G, G);

    for(i=0; i<12 ; i++)
      fprintf(mesh_output_fp, matdata, 0.0, 0.0, 0.0, 0.0, 0.0);
    fprintf(mesh_output_fp, "%16.9E\n", 0.0);
  }

  // Element properties

  for(id = 0; id < numMats; id++)
  {   
    fprintf(mesh_output_fp,header, 4, id + 1, id + 1, 1, 8, 8, 0, 1, 0);
    fprintf(mesh_output_fp,"%16.9E\n",(float) id + 1);
  }

  // End of file
  fprintf(mesh_output_fp, header, 99, 0, 0, 1, 0, 0, 0, 0, 0); 

  fclose (mesh_output_fp);

  delete [] matProp;
  delete [] propsWithIntegralAtStartup;

  std::cout << "-- End" << std::endl;
  return 0;
} 