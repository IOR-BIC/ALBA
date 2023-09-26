/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItemVTK
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


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


#include "albaVMEItemVTK.h"

#include "albaEventIO.h"
#include "albaIndent.h"
#include "albaStorage.h"
#include "albaStorageElement.h"
#include "albaCrypt.h"
#include "albaTagArray.h"

#include "vtkALBASmartPointer.h"
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
#include "vtkCharArray.h"

#include <assert.h>

const char *file_extension="vtk";

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEItemVTK)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEItemVTK::albaVMEItemVTK()
//-------------------------------------------------------------------------
{
  m_IOStatus    = ALBA_OK;
  m_Data        = NULL;
  m_DataReader  = NULL;
  m_DataWriter  = NULL;
}

//-------------------------------------------------------------------------
albaVMEItemVTK::~albaVMEItemVTK()
//-------------------------------------------------------------------------
{
  vtkDEL(m_DataWriter);
  vtkDEL(m_DataReader);
}

//-------------------------------------------------------------------------
void albaVMEItemVTK::DeepCopy(albaVMEItem *a)
//-------------------------------------------------------------------------
{
  albaVMEItemVTK *vtk_item=albaVMEItemVTK::SafeDownCast(a);
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
void albaVMEItemVTK::DeepCopyVmeLarge(albaVMEItem *a)
//-------------------------------------------------------------------------
{
  albaVMEItemVTK *vtk_item=albaVMEItemVTK::SafeDownCast(a);
  assert(vtk_item);
  Superclass::DeepCopy(vtk_item);
  if (vtk_item->GetData())
  {
      vtkDataSetWriter *w;
      vtkNEW(w);
      w->SetFileName("TMP.vtk");
      w->SetInput(vtk_item->GetData());
      w->SetFileTypeToBinary();
      w->Write();
      vtkDEL(w);

      m_Data = vtk_item->GetData()->NewInstance();
      m_Data->Delete(); // decrease reference count since VTK set it to 1 by default

      vtk_item->ReleaseData();

      albaString filename = albaGetAppDataDirectory().c_str();
      filename<<"/TMP.vtk";
      albaLogMessage("<<<<<Creating temp file : %s",filename);
      this->ReadData(filename);
      albaLogMessage("<<<<<Read temp file : %s",filename);

      remove(filename);
  }
  else
  {
    m_Data = NULL;
  }

}


//-------------------------------------------------------------------------
void albaVMEItemVTK::ShallowCopy(albaVMEItem *a)
//-------------------------------------------------------------------------
{
  Superclass::ShallowCopy(a); // Added by Losi 09.24.09 ShallowCopy must copy timestamp too
  albaVMEItemVTK *vtk_item=albaVMEItemVTK::SafeDownCast(a);
  assert(vtk_item);
  m_Data=vtk_item->GetData();
}

//-------------------------------------------------------------------------
bool albaVMEItemVTK::Equals(albaVMEItem *a)
//-------------------------------------------------------------------------
{
  if (!Superclass::Equals(a))
    return false;

  if ((*GetGlobalCompareDataFlag()))
  {
    albaVMEItemVTK *item=(albaVMEItemVTK *)a;

    vtkDataSet *data1=GetData();
    vtkDataSet *data2=item->GetData();

    data1->Update();
    data2->Update();

    if (data1&&data2)
    {
      // We test for equivalence of data types. It could happen that the written 
      // dataset is inherited from a standard dataset type, thus when we read it 
      // back the data type is changed but data is the same. E.g. vtkImageData
      // are always written as vtkImageData by vtkDataSetWriter, thus when 
      // read back it becomes a vtkImageData with the same data.
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
vtkDataSet *albaVMEItemVTK::GetData()
//-------------------------------------------------------------------------
{
  UpdateData();
  return m_Data.GetPointer();
}

//-------------------------------------------------------------------------
void albaVMEItemVTK::SetData(vtkDataSet *data)
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
void albaVMEItemVTK::UpdateData()
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
    if (RestoreData() == ALBA_OK)
    {
    }
  }
}


//-------------------------------------------------------------------------
void albaVMEItemVTK::UpdateBounds()
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
const char * albaVMEItemVTK::GetDataFileExtension()
//-------------------------------------------------------------------------
{
  return file_extension;
}

//-------------------------------------------------------------------------
int albaVMEItemVTK::InternalRestoreData()
//-------------------------------------------------------------------------
{
  if (!IsDataPresent())
  {
    if (m_URL.IsEmpty())
    {
      return ALBA_NO_IO;
    }
    
    albaEventIO e(this,NODE_GET_STORAGE);
    albaEventMacro(e);
    albaStorage *storage = e.GetStorage();
    
		if (!storage)
			return ALBA_NO_IO;
    
    int resolvedURL = ALBA_OK;
    albaString filename;
    if (m_IOMode != MEMORY)
    {
      resolvedURL = storage->ResolveInputURL(m_URL, filename, m_DataObserver);
    }
    else
    {
      resolvedURL = storage->ResolveInputURL(m_ArchiveFileName, filename, m_DataObserver);
    }
    if (resolvedURL == ALBA_WAIT)
    {
      return ALBA_WAIT;
    }
    return ReadData(filename, resolvedURL);
  }
  return ALBA_NO_IO;
}
//-------------------------------------------------------------------------
int albaVMEItemVTK::ReadData(albaString &filename, int resolvedURL)
//-------------------------------------------------------------------------
{
  vtkDataSet *data;
  vtkDataReader *reader;

  albaCString datatype = GetDataType();

  if (resolvedURL == ALBA_OK)
  {
    m_DecryptedFileString.clear();
    if (GetCrypting() && m_IOMode != MEMORY)
    {
#ifdef ALBA_USE_CRYPTO
      albaDefaultDecryptFileInMemory(filename, m_DecryptedFileString);
#else
      albaErrorMacro("Encrypted data not supported: ALBA not linked to Crypto library.");
      return ALBA_ERROR;
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
    else if (datatype == "vtkImageData" || datatype == "vtkStructuredPoint")
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
      albaWarningMacro("Unknown data type, using generic VTK dataset reader");
      reader = vtkDataSetReader::New();
      UpdateReader(reader, filename);
      data = ((vtkDataSetReader *)reader)->GetOutput();
    }

    //BES: 23.5.2008 - m_DataReader is apparently not necessary, however,
    //it may consume lot of memory (especially, if the data is loaded from memory)
    //we do not need it => the following code is commented

    //m_DataReader = reader;
    //m_DataReader->Register(NULL);

    unsigned long err = reader->GetErrorCode();

    if (data == NULL || err == VTK_ERROR)
    {
      albaErrorMacro("Cannot read data file " << filename);
      reader->Delete(); //BES: 23.5.2008 - added to avoid memory leaks
      return ALBA_ERROR;
    }
    else
    {
      //BES: 23.5.2008 - detach data from its reader, so we can destroy the reader
      data->SetSource(NULL);

      SetData(data);
      m_IsLoadingData = false;
    }
    reader->Delete();
  }
  else if (resolvedURL == ALBA_ERROR)
  {
    return ALBA_NO_IO;
  }
  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMEItemVTK::UpdateReader(vtkDataReader *reader, albaString &filename)
//-------------------------------------------------------------------------
{
  // Progressbar removed to improve vector data loading speed
  // reader->SetProgressText("Loading data...");
  // albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,reader));
  if (m_IOMode != MEMORY)
  {
    if (GetCrypting())
    {
#ifdef ALBA_USE_CRYPTO
      reader->ReadFromInputStringOn();
      reader->SetInputString(m_DecryptedFileString.c_str(),m_DecryptedFileString.size());
#else
      albaErrorMacro("Encrypted data not supported: ALBA not linked to Crypto library.");
      reader->Delete();
      return ALBA_ERROR;
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
    if (res != ALBA_OK)
    {
      albaErrorMacro("Error extracting item from the archive!");
      return ALBA_ERROR;
    }

    //BES: 14.5.2008 - SetInputString creates a copy of m_InputMemory    
    reader->ReadFromInputStringOn();
    //reader->SetInputString(m_InputMemory, m_InputMemorySize);
    vtkALBASmartPointer<vtkCharArray> pCharArray;
    pCharArray->SetArray(const_cast<char*>(m_InputMemory), m_InputMemorySize, 0);
    reader->SetInputArray(pCharArray);
    reader->Update();
    //delete m_InputMemory;
    //reader->SetInputArray(NULL);  //Paolo 19.05.2008: commented because otherwise the data is lost before
                                    //it is passed to the item.
    m_InputMemory = NULL;
    m_InputMemorySize = 0;
  }
  return ALBA_OK;
}
//-------------------------------------------------------------------------
int albaVMEItemVTK::InternalStoreData(const char *url)
//-------------------------------------------------------------------------
{
  if (GetData())
  {
    bool found = false;
    albaString filename;
          
    albaEventIO e(this,NODE_GET_STORAGE);
    albaEventMacro(e);
    albaStorage *storage=e.GetStorage();
    assert(storage);

    switch (m_IOMode)
    {
      case MEMORY:
        found = false;
      break;
      case TMP_FILE:
        found = false;
        filename = url; // use directly the url as a tmp file
        albaErrorMacro("Unsupported I/O Mode");
      return ALBA_ERROR;
      case DEFAULT:
        if (albaString::IsEmpty(url))
        {
          albaWarningMacro("No filename specified: cannot write data to disk");
          return ALBA_ERROR;
        }

        found=storage->IsFileInDirectory(url);
        storage->GetTmpFile(filename);
      break;
      default:
        albaErrorMacro("Unsupported I/O Mode");
      return ALBA_ERROR;
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
    int ret = ALBA_OK; // value returned by StoreToURL() function at the end of saving to file
    int vtk_err = VTK_OK;
    int chk_res = ALBA_OK;
    if ((IsDataPresent()&&(!found||(m_URL!=url)))||((IsDataPresent()==found)&&(found==IsDataModified())))
    {       
      //if (!item->IsDataModified()&&found)
      //  return;

      vtkDataSet *data = GetData();

      // problems retrieving data... (e.g. when a file has been erroneously deleted or corrupted...)
      if (data == NULL)
      {
        albaLogMessage(_("problems retrieving data... (e.g. when a file has been erroneously deleted or corrupted...)"));
        return ALBA_ERROR;
      }

      // force release old writer if present
      ReleaseOutputMemory();

      vtkALBASmartPointer<vtkDataSetWriter> writer;
      writer->SetInput(data);
      writer->SetFileTypeToBinary();
      writer->SetHeader("# ALBA data file - albaVMEItemVTK output\n");

      if (m_IOMode == MEMORY || GetCrypting())
      {
        writer->WriteToOutputStringOn();
        writer->Write();
        vtk_err = writer->GetErrorCode();
        if (vtk_err == VTK_ERROR)
        {
          return ALBA_ERROR;
        }
      }

      if (m_IOMode == MEMORY)
      {
        if (GetCrypting())
        {
#ifdef ALBA_USE_CRYPTO
          std::string encrypted_output;
//          albaDefaultEncryptFromMemory(writer->GetOutputString(), writer->GetOutputStringLength(), encrypted_output);
          albaDefaultEncryptFromMemory(writer->GetOutputString(), encrypted_output);
          m_OutputMemory = encrypted_output.c_str();
          m_OutputMemorySize = encrypted_output.size();
#else
          albaErrorMacro("Encrypted data is not supported: Crypto library not linked to ALBA!");
          return ALBA_ERROR;
#endif
        }
        else
        {
          m_OutputMemory = writer->GetOutputString();
          m_OutputMemorySize = writer->GetOutputStringLength();
        }
        m_DataWriter = writer;
        m_DataWriter->Register(NULL);
        
        chk_res = CheckFile(m_OutputMemory, m_OutputMemorySize);
        return chk_res != ALBA_OK ? ALBA_ERROR : ALBA_OK;
      }
      else
      {
        if (GetCrypting())
        {
#ifdef ALBA_USE_CRYPTO
          albaDefaultEncryptFileFromMemory(writer->GetOutputString(), writer->GetOutputStringLength(),filename);
#else
          albaErrorMacro("Encrypted data is not supported: Crypto library not linked to ALBA!");
          return ALBA_ERROR;
#endif
        }
        else
        {
          writer->SetFileName(filename);
          writer->Write();
          vtk_err = writer->GetErrorCode();
          if (vtk_err == VTK_ERROR)
          {
            return ALBA_ERROR;
          }
#ifndef _DEBUG
		  //file check is really slow in debug so is disabled
          int chk_res = CheckFile(filename);
          if (chk_res != ALBA_OK)
          {
            return ALBA_ERROR;
          }
#endif
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

    if (m_IOStatus != ALBA_OK)
      return ALBA_ERROR;

    // reset modified data flag
    SetDataModified(false);
    return ret;
  }
  return ALBA_NO_IO;
}
//-------------------------------------------------------------------------
int albaVMEItemVTK::CheckFile(const char *filename)
//-------------------------------------------------------------------------
{
  // Check stored file correctness
  vtkALBASmartPointer<vtkDataReader> file_chk;
  file_chk->SetFileName(filename);
  int res = file_chk->IsFileValid(m_DataType.GetCStr());
  if (res == VTK_ERROR)
  {
    albaMessage(_("File %s corrupted!!"), filename);
    return ALBA_ERROR;
  }
  std::string chk_result;
  albaCalculateteChecksum(filename, chk_result);
  m_ChecksumMD5 = chk_result.c_str();
  if(!GetTagArray()->IsTagPresent("MD5Checksum"))
  {
    albaTagItem ti;
    ti.SetName("MD5Checksum");
    GetTagArray()->SetTag(ti);
  }
  albaTagItem *md5tag = GetTagArray()->GetTag("MD5Checksum");
  md5tag->SetValue(m_ChecksumMD5);
  return ALBA_OK;
}
//-------------------------------------------------------------------------
int albaVMEItemVTK::CheckFile(const char *input_string, int input_len)
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkDataReader> file_chk;
  file_chk->SetInputString(input_string, input_len);
  file_chk->ReadFromInputStringOn();
  int res = file_chk->IsFileValid(m_DataType.GetCStr());
  if (res != 0)
  {
    albaMessage(_("String corrupted!!"));
    return ALBA_ERROR;
  }
  std::string chk_result;
  albaCalculateteChecksum(input_string, input_len, chk_result);
  m_ChecksumMD5 = chk_result.c_str();
  if(!GetTagArray()->IsTagPresent("MD5Checksum"))
  {
    albaTagItem ti;
    ti.SetName("MD5Checksum");
    GetTagArray()->SetTag(ti);
  }
  albaTagItem *md5tag = GetTagArray()->GetTag("MD5Checksum");
  md5tag->SetValue(m_ChecksumMD5);
  return ALBA_OK;
}
//-------------------------------------------------------------------------
void albaVMEItemVTK::ReleaseData()
//-------------------------------------------------------------------------
{
  vtkDEL(m_DataReader); // destroy reader
  m_Data = NULL;        // unregister dataset
}

/*
//-------------------------------------------------------------------------
void albaVMEItemVTK::ErrorHandler(void *ptr)
//-------------------------------------------------------------------------
{
  albaVMEItemVTK *self=(albaVMEItemVTK *)ptr;
  self->m_IOStatus = ALBA_ERROR;
}
*/

//-------------------------------------------------------------------------
void albaVMEItemVTK::GetOutputMemory(const char *&out_str, int &size)
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
bool albaVMEItemVTK::StoreToArchive(wxZipOutputStream &zip)
//-------------------------------------------------------------------------
{
  if (!zip.PutNextEntry(m_URL.GetCStr(), wxDateTime::Now(), m_OutputMemorySize) || !zip.Write(m_OutputMemory, m_OutputMemorySize))
    return false;
  bool write_res = zip.LastWrite() == m_OutputMemorySize;
  return write_res;
}
//-------------------------------------------------------------------------
void albaVMEItemVTK::ReleaseOutputMemory()
//-------------------------------------------------------------------------
{
  vtkDEL(m_DataWriter);
  vtkDEL(m_DataReader);

  m_OutputMemory = NULL;
  m_OutputMemorySize = 0;
}
//-------------------------------------------------------------------------
void albaVMEItemVTK::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);

  // to do: implement DUMP of internally stored data
  strstream ostr;
}
