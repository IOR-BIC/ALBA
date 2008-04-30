/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEDataSetAttributesImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-30 16:55:35 $
  Version:   $Revision: 1.7 $
  Authors:   Stefano Perticoni     
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

const int DEBUG_MODE = true;

#include "mafVMEDataSetAttributesImporter.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkDataSet.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"

#include "vtkDoubleArray.h"

#include "vtkDirectory.h"
#include "vtkDataSet.h"

#include "mafVMEItemVTK.h"
#include "mafVMEGeneric.h"
#include "mafDataVector.h"
#include "mafSmartPointer.h"
#include "mafVMEMesh.h"
#include "mafVMESurface.h"
#include "mafVMEVolume.h"

#include <fstream>
#include <iomanip>

#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_algorithm.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include "vnl/vnl_matrix.h"

mafVMEDataSetAttributesImporter::mafVMEDataSetAttributesImporter()
{

  m_TimeVarying = false;
  m_Input = NULL;
  m_FileName = "";
  m_FileBaseName = "";
  m_ResultsDir = "";
  m_FilePrefix = "";
  m_FileExtension = "";
  SetAttributeTypeToPointData();
  UseTSFileOff();
  UseIdArrayOff();
  m_IdArrayName = "UNDEFINED";
}

mafVMEDataSetAttributesImporter::~mafVMEDataSetAttributesImporter()
{

}


int mafVMEDataSetAttributesImporter::Read()
{  
  if (m_Input == NULL)
  {
    mafWarningMessageMacro("Input vme must be defined!");
    return MAF_ERROR;
  }
  
  int ret = SplitFileName();  
  if (ret == MAF_ERROR)
  {
    return ret;
  }

  vtkDataSet *data = m_Input->GetOutput()->GetVTKData();
  data->Update();

  if (data == NULL)
  {
    mafWarningMessageMacro("No geometry for the current time!")
    return MAF_ERROR;
  }

  // vector holding time stamps
  vnl_matrix<double> tsMatrixWith1Column;
 
  // vector holdin results matrix for each time stamp
  typedef vnl_matrix<double> doubleMatrix; 
  vcl_vector<doubleMatrix> attributesMatrixVector;

  // vector holding file names in current directory
  vcl_vector<vcl_string> fileNamesVector;
  vcl_vector<vcl_string>::iterator fileNamesVectorIterator;

  // holder for result labels
  vcl_vector<vcl_string> labelsVector;

  // number of labels in result file
  int numLabels = -1;

  // number of nodes and elements;
  int numNodes = -1, numElements = -1;

  numNodes = m_Input->GetOutput()->GetVTKData()->GetNumberOfPoints();
  numElements = m_Input->GetOutput()->GetVTKData()->GetNumberOfCells();

  vtkMAFSmartPointer<vtkDirectory> dirAccessor;
  int isDirOpen = dirAccessor->Open(m_ResultsDir.GetCStr());
  
  if (isDirOpen == 0)
  {
    mafWarningMessageMacro("Cannot open directory!");
    return MAF_ERROR;
  }

  if (dirAccessor->GetNumberOfFiles() == 0)
  {
    mafWarningMessageMacro("Empty directory!");
    return MAF_ERROR;
  }

  for (int i = 0; i < dirAccessor->GetNumberOfFiles(); i++)
  {
    vcl_string tmpStr(dirAccessor->GetFile(i));
    fileNamesVector.push_back(tmpStr);
  }

  // search for a file name containing the prefix
  vcl_string vclFilePrefix = "";
  (m_TimeVarying == true) ? vclFilePrefix = m_FilePrefix.GetCStr() : vclFilePrefix = m_FileBaseName  ;
  
  for (fileNamesVectorIterator = fileNamesVector.begin(); fileNamesVectorIterator != fileNamesVector.end(); fileNamesVectorIterator++)
  {
    int pos = (*fileNamesVectorIterator).find(vclFilePrefix,0);
    if (pos != vcl_string::npos) break;
  }
  
  if (fileNamesVectorIterator == fileNamesVector.end())
  {
    vtkGenericWarningMacro("No filename found containing prefix: " << vclFilePrefix.c_str() << endl);
    return MAF_ERROR;
    
  }

  // we found a file name containing the prefix PRNLS_0001.lis
  vcl_string genericFileName = *fileNamesVectorIterator;

  // remove the prefix from the string
  genericFileName.erase(0, m_FilePrefix.Length());

  // get the number of characters composing the increasing index
  int indNumChar = genericFileName.find(m_FileExtension[0]);
  if (m_TimeVarying && indNumChar == 0)
  {
    mafWarningMessageMacro("There is a file with no index!");
    return MAF_ERROR;
  }

  // fill the tsMatrix    
  int nCols = -1;
  int nRows = -1; 

  if (GetUseTSFile()== true)
  {
    vcl_ifstream inputStream(m_TSFileName.GetCStr(), std::ios::in);    
    if (inputStream.is_open())
    { 
      tsMatrixWith1Column.read_ascii(inputStream);
      nCols = tsMatrixWith1Column.cols();
      nRows = tsMatrixWith1Column.rows();
      if (nRows == 0 && nCols == 0)
      {
        inputStream.close();
        mafWarningMessageMacro("No timestamp values found!")
        return MAF_ERROR;   
      }
      else if (nCols != 1) 
      {
        inputStream.close();
        mafWarningMessageMacro("Timestamp file must contain a single column of numbers");
        return MAF_ERROR;
      }
    }
    else
    {
      vtkGenericWarningMacro("TS file:" << m_TSFileName.GetCStr() << "not found; importing modality will be set to UseTSFileOff()" << endl);
      UseTSFileOff();
    }
    inputStream.close();
  }

  // file enumeration is starting from 1
  int fileId = 1;
  
  // create the cell id hash table: 
  vcl_map<int, int> attributeFileAnsysIdToRowIdIMap;

  while (1)
  {
    // build the ith file name
    
    //build the index part
    vcl_ostringstream number;     
    number << std::setfill('0') << std::setw(4) << fileId;
      
    // build the full name
    mafString ithAttributesFileName = m_ResultsDir;
    ithAttributesFileName.Append(vclFilePrefix.c_str());
    if (m_TimeVarying) ithAttributesFileName.Append(number.str().c_str()) ;
    ithAttributesFileName.Append(m_FileExtension);
     
    vtkGenericWarningMacro(<< "reading "<< ithAttributesFileName.GetCStr() << endl);
    
    // open the ith file
    vcl_ifstream ithAttributesFileStream(ithAttributesFileName.GetCStr(), std::ios::in);
    if (ithAttributesFileStream.is_open() == false)
    {
      if (fileId == 1)
      {
        ithAttributesFileStream.close();
        mafWarningMessageMacro("File with index 1 not found!");
        return MAF_ERROR;
      }
      else
      {
        ithAttributesFileStream.close();
        break;
      }
    }

    if (fileId == 1)
    {
      char buf[1000];
      // fill the vector representing the result labels from the file of index 1
      ithAttributesFileStream.getline(buf, 1000, '\n');

      // associate an istringstream with full line
      vcl_istringstream inputStrStream(buf);

      // fill the string vector with columns names
      vcl_string tmpStrSingleWord;
      while (inputStrStream >> tmpStrSingleWord) 
      {
        labelsVector.push_back(tmpStrSingleWord);
      }

      numLabels = labelsVector.size();
    }
    else
    {
      // discard the line containing the labels from the other files
      ithAttributesFileStream.ignore(INT_MAX, '\n');
    }

    // fill the result matrix
    vnl_matrix<double> tmpAttributesMatrix;
    tmpAttributesMatrix.read_ascii(ithAttributesFileStream);
    
    if (GetAttributeType() == POINT_DATA)
    {
      if (tmpAttributesMatrix.rows() != numNodes)
      {
        ithAttributesFileStream.close();
        vtkGenericWarningMacro("Number of nodes is  different from number of results!" << endl 
          << "nodes:" << numNodes << endl << "results:" << tmpAttributesMatrix.rows() << endl );
        vtkGenericWarningMacro(<< vcl_cerr);
        return MAF_ERROR;
      }
    }
    else if (GetAttributeType() == CELL_DATA)
    {
      if (tmpAttributesMatrix.rows() != numElements)
      {
        ithAttributesFileStream.close();
        mafWarningMessageMacro("Number of elements different from number of results!");
        return MAF_ERROR;    
      }
    }

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << std::endl;
      stringStream << "tmpAttributesMatrix" << std::endl;
      stringStream << tmpAttributesMatrix  << std::endl;
      mafLogMessage(stringStream.str().c_str());
    }

    typedef vcl_map<int, int>::const_iterator  Iter;
    

    
    int ansysIdCol = 0;

    
    for (int vtkId = 0; vtkId < tmpAttributesMatrix.rows(); vtkId++)
    {
      int ansysCellId = tmpAttributesMatrix(vtkId, ansysIdCol);
      attributeFileAnsysIdToRowIdIMap.insert(vcl_map<int,int>::value_type(ansysCellId, vtkId));       
      if (DEBUG_MODE)
        {
          std::ostringstream stringStream;
          stringStream << "ansysCellId: " << ansysCellId << " vtkId: " << attributeFileAnsysIdToRowIdIMap[ansysCellId] << std::endl;
          mafLogMessage(stringStream.str().c_str());
        }
    }  
  
    // push the matrix in the matrix vector
    attributesMatrixVector.push_back(tmpAttributesMatrix); 
    ithAttributesFileStream.close();
    
    // exit for single time results
    if (m_TimeVarying == false) break;
    
    fileId++;
  }

  // if UseTSFile is On check if number of ts in the txt file is the
  // same as the dimension of the data matrix vector
  if (GetUseTSFile() == true && tsMatrixWith1Column.rows() != attributesMatrixVector.size())
  {
    vtkGenericWarningMacro("Number of entries in the ts file is different from the number of result files!");
    return MAF_ERROR;
  }
  
  int numTS = attributesMatrixVector.size(); 

  // finished populating result matrix vector

  // fill the input vme
  
  int attributesNumber = (GetAttributeType() == POINT_DATA) ? numNodes : numElements;

  
  // for every time stamp
  for (int tsIndex = 0; tsIndex < numTS; tsIndex++)
  {
    
    vtkDataSet *inputVTKDatasetDeepCopy = NULL;

    // input structure
    
    // get the current time
    float currentTime = GetUseTSFile() == true ? tsMatrixWith1Column(tsIndex, 0) : (double)tsIndex;

    mafVMEItemVTK *currentItem = mafVMEItemVTK::SafeDownCast(m_Input->GetDataVector()->GetItem(currentTime));

    vtkDataSet *currentItemVTKData = currentItem->GetData();

    // check if an item at the current time exists already
    bool itemExist = currentItem ? true : false;

    // the dataset attribute
    vtkDataSetAttributes *targetAttributeData = NULL;

    if (itemExist) // only attach attribute data
    {
      if (GetAttributeType() == POINT_DATA)
      {
        targetAttributeData = currentItem->GetData()->GetPointData();
      }
      else
      {
        targetAttributeData = currentItem->GetData()->GetCellData();
      }

    }
    else // recreate the dataset and attach the data
    {    

      if (m_Input->IsA("mafVMEMesh"))
      {
        inputVTKDatasetDeepCopy = vtkUnstructuredGrid::New();
      }
      else if (m_Input->IsA("mafVMESurface"))
      {
        inputVTKDatasetDeepCopy = vtkPolyData::New();
      }
      else if (m_Input->IsA("mafVMEVolume"))
      {
        inputVTKDatasetDeepCopy = vtkRectilinearGrid::New();
      }
      else
      {
        mafWarningMessageMacro("Vme type not yet supported by the importer!");
        return MAF_ERROR;
      }

      inputVTKDatasetDeepCopy->DeepCopy(m_Input->GetOutput()->GetVTKData());

      numNodes = inputVTKDatasetDeepCopy->GetNumberOfPoints();
      numElements = inputVTKDatasetDeepCopy->GetNumberOfCells();

      if (GetAttributeType() == POINT_DATA)
      {
        targetAttributeData = inputVTKDatasetDeepCopy->GetPointData();
      }
      else
      {
        targetAttributeData = inputVTKDatasetDeepCopy->GetCellData();
      }
    }  

    vtkIntArray *idArray = NULL;

    
    if (m_UseIdArray == true)
    {
      // get the cell id array
      if (GetAttributeType() == POINT_DATA)
      {

        idArray = vtkIntArray::
          SafeDownCast(
          currentItemVTKData->GetPointData()->GetArray(m_IdArrayName.GetCStr())
          );

        if (idArray == NULL)
        {
          // try active scalar
          idArray = vtkIntArray::
            SafeDownCast(
            currentItemVTKData->GetPointData()->GetScalars(m_IdArrayName.GetCStr())
            );

        }
        if (idArray == NULL)
          {
            std::ostringstream stringStream;
            stringStream << "Point Data Array " << m_IdArrayName.GetCStr() << " does not exist!" << std::endl;
            stringStream << "exiting with MAF_ERROR" << std::endl;
            return MAF_ERROR;
            mafLogMessage(stringStream.str().c_str());
          }
        
        assert(idArray);
      }
      else
      {
        idArray = vtkIntArray::
          SafeDownCast(
          currentItemVTKData->GetCellData()->GetArray(m_IdArrayName.GetCStr())
          );

        if (idArray == NULL)
        {
          // try active scalar
          idArray = vtkIntArray::
            SafeDownCast(
            currentItemVTKData->GetCellData()->GetScalars(m_IdArrayName.GetCStr())
            );
        }
        if (idArray == NULL)
        {
          std::ostringstream stringStream;
          stringStream << "Cell Data Array " << m_IdArrayName.GetCStr() << " does not exist!" << std::endl;
          stringStream << "exiting with MAF_ERROR" << std::endl;
          return MAF_ERROR;
          mafLogMessage(stringStream.str().c_str());
        }

        assert(idArray);
      }
      
    }

    // for everery column add the array to the dataset attribute data  
    for (int labelId = 0; labelId < labelsVector.size() - 1; labelId++)
    {
      vtkMAFSmartPointer<vtkDoubleArray> attributeDoubleArray;
      attributeDoubleArray->SetNumberOfComponents(1);
      attributeDoubleArray->SetName(labelsVector[labelId+1].c_str());
      attributeDoubleArray->SetNumberOfTuples(attributesNumber);      

      if (m_UseIdArray == true)
      {     
        // for every row
        for (int vtkId = 0; vtkId < attributesNumber; vtkId++)
        {
          int ansysId = idArray->GetValue(vtkId);
          int inputAttributeTextFileRow = attributeFileAnsysIdToRowIdIMap[ansysId];

          attributeDoubleArray->SetValue(
            vtkId, attributesMatrixVector[tsIndex](inputAttributeTextFileRow, labelId+1));
        }       
      }
      else
      {
        // for every row
        for (int vtkId = 0; vtkId < attributesNumber; vtkId++)
        {
          attributeDoubleArray->SetValue(
            vtkId, attributesMatrixVector[tsIndex](vtkId, labelId+1));
        }
      }

      targetAttributeData->AddArray(attributeDoubleArray); 

    } // for each label
     
    if (inputVTKDatasetDeepCopy)
    {
      m_Input->SetData(inputVTKDatasetDeepCopy,currentTime);
    }

    vtkDEL(inputVTKDatasetDeepCopy);

  } // for every ts  
  
  return MAF_OK;
}

void mafVMEDataSetAttributesImporter::SetFileName( const char *filename )
{
  m_FileName = filename	;
  
}

int mafVMEDataSetAttributesImporter::SplitFileName()
{
  if (m_FileName == "")
  {
    mafWarningMessageMacro("filename not specified!!!")
    return MAF_ERROR;
  }

  // find the last point
  vcl_string fileName = m_FileName.GetCStr();
  int pointPos = fileName.find_last_of('.');
  
  // find the last / on linux
  int slashPos = fileName.find_last_of('/');
  if (slashPos == -1)
  {
    // try on Windows
    slashPos = fileName.find_last_of('\\');
  }

  // extension
  vcl_string ext(fileName, pointPos,fileName.length()-pointPos);

  // fileBaseName
  vcl_string baseFileName(fileName, slashPos+1, pointPos-slashPos-1);

  // path
  vcl_string path(fileName, 0, slashPos+1);

  m_ResultsDir = path.c_str();
  m_FileBaseName = baseFileName.c_str();
  m_FileExtension = ext.c_str();
  
  return MAF_OK;
}
