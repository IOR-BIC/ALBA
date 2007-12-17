/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-17 13:40:37 $
  Version:   $Revision: 1.18 $
  Authors:   Marco Petrone
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


#include "mafVMEItemVTK.h"

#include "mafEventIO.h"
#include "mafIndent.h"
#include "mafStorage.h"
#include "mafStorageElement.h"
#include "mafCrypt.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridReader.h"
#include "vtkStructuredGrid.h"
#include "vtkStructuredGridReader.h"
#include "vtkStructuredPoints.h"
#include "vtkStructuredPointsReader.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkDataSetReader.h"
#include "vtkDataSetWriter.h"

#include <assert.h>

const char *file_extension="vtk";

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEItemVTK)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEItemVTK::mafVMEItemVTK()
//-------------------------------------------------------------------------
{
  m_IOStatus    = MAF_OK;
  m_Data        = NULL;
  m_DataReader  = NULL;
  m_DataWriter  = NULL;
}

//-------------------------------------------------------------------------
mafVMEItemVTK::~mafVMEItemVTK()
//-------------------------------------------------------------------------
{
  vtkDEL(m_DataWriter);
  vtkDEL(m_DataReader);
}

//-------------------------------------------------------------------------
void mafVMEItemVTK::DeepCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  mafVMEItemVTK *vtk_item=mafVMEItemVTK::SafeDownCast(a);
  assert(vtk_item);
  Superclass::DeepCopy(vtk_item);
  if (vtk_item->GetData())
  {
    m_Data = vtk_item->GetData()->NewInstance();
    m_Data->Delete(); // decrease reference count since VTK set it to 1 by default
    m_Data->DeepCopy(vtk_item->GetData());
  }
  else
  {
    m_Data = NULL;
  }
  
}

//-------------------------------------------------------------------------
void mafVMEItemVTK::ShallowCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  mafVMEItemVTK *vtk_item=mafVMEItemVTK::SafeDownCast(a);
  assert(vtk_item);
  m_Data=vtk_item->GetData();
}

//-------------------------------------------------------------------------
bool mafVMEItemVTK::Equals(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  if (!Superclass::Equals(a))
    return false;

  if (m_GlobalCompareDataFlag)
  {
    mafVMEItemVTK *item=(mafVMEItemVTK *)a;

    vtkDataSet *data1=GetData();
    vtkDataSet *data2=item->GetData();

    data1->Update();
    data2->Update();

    if (data1&&data2)
    {
      // We test for equivalence of data types. It could happen that the written 
      // dataset is inherited from a standard dataset type, thus when we read it 
      // back the data type is changed but data is the same. E.g. vtkImageData
      // are always written as vtkStructuredPoints by vtkDataSetWriter, thus when 
      // read back it becomes a vtkStructuredPoints with the same data.
      if (data1->SafeDownCast(data2)==NULL&&data2->SafeDownCast(data1)==NULL)
        return false;

      int pnt1=data1->GetNumberOfPoints();
      int pnt2=data2->GetNumberOfPoints();
      int cell1=data1->GetNumberOfCells();
      int cell2=data2->GetNumberOfCells();

      if (pnt1!=pnt2 || cell1!=cell2)
        return false;
      
      data1->ComputeBounds();
      data2->ComputeBounds();

      double bounds1[6],bounds2[6];
      data1->GetBounds(bounds1);
      data2->GetBounds(bounds2);

      if (bounds1[0]!=bounds2[0]||bounds1[1]!=bounds2[1]||bounds1[2]!=bounds2[2]|| \
        bounds1[3]!=bounds2[3]||bounds1[4]!=bounds2[4]||bounds1[5]!=bounds2[5])
      {
        return false;
      }

      double range1[2],range2[2];
      data1->GetScalarRange(range1);
      data2->GetScalarRange(range2);

      if (range1[0]!=range2[0]||range1[1]!=range2[1])
      {
        return false;
      }
    }
    else
    {
      if (data1!=data2)
      {
        return false;
      }
    }
  }

  return true;
}


//-------------------------------------------------------------------------
vtkDataSet *mafVMEItemVTK::GetData()
//-------------------------------------------------------------------------
{
  UpdateData();
  return m_Data.GetPointer();
}

//-------------------------------------------------------------------------
void mafVMEItemVTK::SetData(vtkDataSet *data)
//-------------------------------------------------------------------------
{
  if (m_Data!=data)
  {
    if (data)
    {
      //m_DataType=data->GetClassName();
      this->SetDataType(data->GetClassName());

      double bounds[6];
      data->Update();
      data->ComputeBounds();
      data->GetBounds(bounds);
      m_Bounds.DeepCopy(bounds);
    }
    else
    {
      //m_DataType="";
      this->SetDataType("");
      m_Bounds.Reset();
    }

    m_Data=data;

    m_UpdateTime.Modified();  //modified by Marco. 3-2-2004

    Modified();

    if (!m_IsLoadingData)
      SetDataModified(true);
  }
}

//-------------------------------------------------------------------------
// Update the internally stored dataset.
void mafVMEItemVTK::UpdateData()
//-------------------------------------------------------------------------
{
  // If the data has been externally modified, and is not NULL ask the
  // pipeline to update.
  if (IsDataModified()&&m_Data.GetPointer())
  {
    m_Data->Update();
    //this->UpdateBounds();
    return;
  }
   
  // At present... if data is already present, simply return
  // otherwise make it be read from disk. Notice that when read
  // from this SetData() is called: Bounds are updated but we need 
  // to reset the DataModified flag.
  if (m_Data.GetPointer()==NULL)
  {
    if (RestoreData() == MAF_OK)
    {
    }
  }
}


//-------------------------------------------------------------------------
void mafVMEItemVTK::UpdateBounds()
//-------------------------------------------------------------------------
{
  if (m_Data.GetPointer())
  {
    if (GetMTime()>m_Bounds.GetMTime() || \
      m_Data->GetMTime()>m_Bounds.GetMTime() || \
      !m_Bounds.IsValid())
    {
      double bounds[6];

      m_Data->Update();
      //m_Data->Modified();
      m_Data->GetBounds(bounds);

      m_Bounds.DeepCopy(bounds);
    }
  }
  else
  {
    if (!m_Bounds.IsValid())
    {
      // If no data is present and bounds are not valid call UpdateData()
      // to force reading the data. 
      UpdateData();
      
      if (m_Data.GetPointer())
        this->UpdateBounds(); // if new data loaded try to update bounds
    }
  }
}

//-------------------------------------------------------------------------
const char * mafVMEItemVTK::GetDataFileExtension()
//-------------------------------------------------------------------------
{
  return file_extension;
}

//-------------------------------------------------------------------------
int mafVMEItemVTK::InternalRestoreData()
//-------------------------------------------------------------------------
{
  if (!IsDataPresent())
  {
    if (m_URL.IsEmpty())
    {
      return MAF_NO_IO;
    }
    
    mafEventIO e(this,NODE_GET_STORAGE);
    mafEventMacro(e);
    mafStorage *storage = e.GetStorage();
    assert(storage);
    
    int resolvedURL = MAF_OK;
    mafString filename;
    if (m_IOMode != MEMORY)
    {
      resolvedURL = storage->ResolveInputURL(m_URL, filename, m_DataObserver);
    }
    else
    {
      resolvedURL = storage->ResolveInputURL(m_ArchiveFileName, filename, m_DataObserver);
    }
    if (resolvedURL == MAF_WAIT)
    {
      return MAF_WAIT;
    }
    return ReadData(filename, resolvedURL);
  }
  return MAF_NO_IO;
}
//-------------------------------------------------------------------------
int mafVMEItemVTK::ReadData(mafString &filename, int resolvedURL)
//-------------------------------------------------------------------------
{
  vtkDataSet *data;
  vtkDataReader *reader;

  mafCString datatype = GetDataType();

  if (resolvedURL == MAF_OK)
  {
    m_DecryptedFileString.clear();
    if (GetCrypting() && m_IOMode != MEMORY)
    {
#ifdef MAF_USE_CRYPTO
      mafDefaultDecryptFileInMemory(filename, m_DecryptedFileString);
#else
      mafErrorMacro("Encrypted data not supported: MAF not linked to Crypto library.");
      return MAF_ERROR;
#endif
    }

    // Workaround for double read bug of the vtkDataSetReader class
    // Read immediately the data and destroy the reader to close the input file
    if (datatype == "vtkPolyData")
    {
      reader = vtkPolyDataReader::New();
      UpdateReader(reader, filename);
      data = ((vtkPolyDataReader *)reader)->GetOutput();
    }
    else if (datatype == "vtkStructuredPoints" || datatype == "vtkImageData")
    {
      reader = vtkStructuredPointsReader::New();
      UpdateReader(reader, filename);
      data = ((vtkStructuredPointsReader *)reader)->GetOutput();
    }
    else if (datatype == "vtkStructuredGrid")
    {
      reader = vtkStructuredGridReader::New();
      UpdateReader(reader, filename);
      data = ((vtkStructuredGridReader *)reader)->GetOutput();
    }
    else if (datatype == "vtkRectilinearGrid")
    {
      reader = vtkRectilinearGridReader::New();
      UpdateReader(reader, filename);
      data = ((vtkRectilinearGridReader *)reader)->GetOutput();
    }
    else if (datatype == "vtkUnstructuredGrid")
    {
      reader = vtkUnstructuredGridReader::New();
      UpdateReader(reader, filename);
      data = ((vtkUnstructuredGridReader *)reader)->GetOutput();
    }
    else
    {
      // using generic vtkDataSet reader...
      mafWarningMacro("Unknown data type, using generic VTK dataset reader");
      reader = vtkDataSetReader::New();
      UpdateReader(reader, filename);
      data = ((vtkDataSetReader *)reader)->GetOutput();
    }

    m_DataReader = reader;
    m_DataReader->Register(NULL);

    if (data==NULL)
    {
      mafErrorMacro("Cannot read data file " << filename);
      return MAF_ERROR;
    }
    else
    {
      SetData(data);
      m_IsLoadingData = false;
    }
    reader->Delete();
  }
  else if (resolvedURL == MAF_ERROR)
  {
    return MAF_NO_IO;
  }
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVMEItemVTK::UpdateReader(vtkDataReader *reader, mafString &filename)
//-------------------------------------------------------------------------
{
  reader->SetProgressText("Loading data...");
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,reader));
  if (m_IOMode != MEMORY)
  {
    if (GetCrypting())
    {
#ifdef MAF_USE_CRYPTO
      reader->ReadFromInputStringOn();
      reader->SetInputString(m_DecryptedFileString.c_str(),m_DecryptedFileString.size());
#else
      mafErrorMacro("Encrypted data not supported: MAF not linked to Crypto library.");
      reader->Delete();
      return MAF_ERROR;
#endif
    }
    else
    {
      reader->SetFileName(filename);
    }
    reader->Update();
  }
  else
  {
    // Extract the file from the archive before passing the string to the reader and get the VTK dataset.
    int res = ExtractFileFromArchive(filename, m_URL);
    if (res != MAF_OK)
    {
      mafErrorMacro("Error extracting item from the archive!");
      return MAF_ERROR;
    }
    reader->ReadFromInputStringOn();
    reader->SetInputString(m_InputMemory, m_InputMemorySize);
    reader->Update();
    delete m_InputMemory;
    m_InputMemory = NULL;
    m_InputMemorySize = 0;
  }
  return MAF_OK;
}
//-------------------------------------------------------------------------
int mafVMEItemVTK::InternalStoreData(const char *url)
//-------------------------------------------------------------------------
{
  if (GetData())
  {
    bool found = false;
    mafString filename;
          
    mafEventIO e(this,NODE_GET_STORAGE);
    mafEventMacro(e);
    mafStorage *storage=e.GetStorage();
    assert(storage);

    switch (m_IOMode)
    {
    case MEMORY:
      found = false;
      break;
    case TMP_FILE:
      found = false;
      filename = url; // use directly the url as a tmp file
      mafErrorMacro("Unsupported I/O Mode");
      return MAF_ERROR;
    case DEFAULT:
      if (mafString::IsEmpty(url))
      {
        mafWarningMacro("No filename specified: cannot write data to disk");
        return MAF_ERROR;
      }

      found=storage->IsFileInDirectory(url);
      storage->GetTmpFile(filename);
      break;
    default:
      mafErrorMacro("Unsupported I/O Mode");
      return MAF_ERROR;
    };

    /*********************************
    // Possible cases for writing DATA
    // Updated = Non-NULL Data is present in memory
    // Found = Data file for specified VME-Item found on disk
    // Modified = Data associated to VME Item has been changed by means of SetData()
    // Write = Whether the data file has to be written on disk
    Updated	Found	Modified	Write	ResetFilename   Type
    0	      0	    0	        1		  1               No Data (e.g.SaveAs)
    0	      0	    1	        0	    1               Data Removed
    0       1	    0	        0		  0               Missing Data (not yet loaded) 
    0	      1	    1	        0     1               Data Removed (should also delete file)
    1	      0	    0	        1	    0               Missing File (e.g. SaveAs)
    1	      0	    1	        1	    0               Modified Data & Missing File (Save with new data)
    1	      1	    0	        0		  0               No Changes
    1	      1	    1	        1		  0               Modified Data (save data changes)

    **********************************/

    m_IOStatus=0;
    int ret=MAF_OK; // value returned by StoreToURL() function at the end of saving to file
    if ((IsDataPresent()&&(!found||(m_URL!=url)))||((IsDataPresent()==found)&&(found==IsDataModified())))
    {       
      //if (!item->IsDataModified()&&found)
      //  return;

      vtkDataSet *data=GetData();

      // problems retrieving data... (e.g. when a file has been erroneously deleted or corrupted...)
      if (data==NULL)
      {
        return 0;
      }

      // force release old writer if present
      ReleaseOutputMemory();

      vtkMAFSmartPointer<vtkDataSetWriter> writer;

      // this is to catch possible I/O errors
      //unsigned long tag=mflAgent::PlugEventSource(writer,mflMSFWriter::ErrorHandler,this,vtkCommand::ErrorEvent);
      writer->SetInput(data);
      writer->SetFileTypeToBinary();
      writer->SetHeader("# MAF data file - mafVMEItemVTK output\n");

      if (m_IOMode == MEMORY || GetCrypting())
      {
        writer->WriteToOutputStringOn();
        writer->Write();
      }

      if (m_IOMode == MEMORY)
      {
        if (GetCrypting())
        {
#ifdef MAF_USE_CRYPTO
          std::string encrypted_output;
          mafDefaultEncryptFromMemory(writer->GetOutputString(), writer->GetOutputStringLength(), encrypted_output);
          m_OutputMemory = encrypted_output.c_str();
          m_OutputMemorySize = encrypted_output.size();
#else
          mafErrorMacro("Encrypted data is not supported: Crypto library not linked to MAF!");
          return MAF_ERROR;
#endif
        }
        else
        {
          m_OutputMemory = writer->GetOutputString();
          m_OutputMemorySize = writer->GetOutputStringLength();
        }
        m_DataWriter = writer;
        m_DataWriter->Register(NULL);
        return MAF_OK;
      }
      else
      {
        if (GetCrypting())
        {
#ifdef MAF_USE_CRYPTO
          mafDefaultEncryptFileFromMemory(writer->GetOutputString(), writer->GetOutputStringLength(),filename);
#else
          mafErrorMacro("Encrypted data is not supported: Crypto library not linked to MAF!");
          return MAF_ERROR;
#endif
        }
        else
        {
          writer->SetFileName(filename);
          writer->Write();
        }
      }
      //writer->RemoveObserver(tag);

      if (m_IOMode == DEFAULT)
      {
        SetURL(url);
        ret = storage->StoreToURL(filename, m_URL);
      }
    }
    else
    {
      // if data has been set to NULL reset the filename
      if ((!IsDataPresent() && !found) || (!IsDataPresent() && found && IsDataModified()))
      {
        if (found)
          storage->ReleaseURL(m_URL); // delete the file from the storage

        SetURL(""); // if data has been set to NULL reset the filename
      }
      UpdateBounds(); // force updating the bounds
    }

    if (m_IOStatus != MAF_OK)
      return MAF_ERROR;

    // reset modified data flag
    SetDataModified(false);
    return ret;
  }
  return MAF_NO_IO;
}

//-------------------------------------------------------------------------
void mafVMEItemVTK::ReleaseData()
//-------------------------------------------------------------------------
{
  vtkDEL(m_DataReader); // destroy reader
  m_Data = NULL;        // unregister dataset
}

/*
//-------------------------------------------------------------------------
void mafVMEItemVTK::ErrorHandler(void *ptr)
//-------------------------------------------------------------------------
{
  mafVMEItemVTK *self=(mafVMEItemVTK *)ptr;
  self->m_IOStatus = MAF_ERROR;
}
*/

//-------------------------------------------------------------------------
void mafVMEItemVTK::GetOutputMemory(const char *&out_str, int &size)
//-------------------------------------------------------------------------
{
  if (m_DataWriter)
  {
    out_str = m_DataWriter->GetOutputString();
    size = m_DataWriter->GetOutputStringLength();
  }
  else
  {
    out_str = NULL;
    size = 0;
  }
}
//-------------------------------------------------------------------------
bool mafVMEItemVTK::StoreToArchive(wxZipOutputStream &zip)
//-------------------------------------------------------------------------
{
  if (!zip.PutNextEntry(m_URL.GetCStr(), wxDateTime::Now(), m_OutputMemorySize) || !zip.Write(m_OutputMemory, m_OutputMemorySize))
    return false;
  return true;
}
//-------------------------------------------------------------------------
void mafVMEItemVTK::ReleaseOutputMemory()
//-------------------------------------------------------------------------
{
  vtkDEL(m_DataWriter);
  vtkDEL(m_DataReader);

  m_OutputMemory = NULL;
  m_OutputMemorySize = 0;
}
//-------------------------------------------------------------------------
void mafVMEItemVTK::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  // to do: implement DUMP of internally stored data
  strstream ostr;
}
