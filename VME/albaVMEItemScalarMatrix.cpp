/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItemScalarMatrix
 Authors: Paolo Quadrani
 
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


#include "albaVMEItemScalarMatrix.h"

#include "albaEventIO.h"
#include "albaIndent.h"
#include "albaStorage.h"
#include "albaStorageElement.h"
#include "albaCrypt.h"
#include "albaTagItem.h"
#include "albaTagArray.h"

#include <assert.h>

#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEItemScalarMatrix)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEItemScalarMatrix::albaVMEItemScalarMatrix()
//-------------------------------------------------------------------------
{
  m_IOStatus   = ALBA_OK;
  m_DataString = "";
}

//-------------------------------------------------------------------------
albaVMEItemScalarMatrix::~albaVMEItemScalarMatrix()
//-------------------------------------------------------------------------
{
  m_Data.clear();
}

//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::DeepCopy(albaVMEItem *a)
//-------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *scalar_item = albaVMEItemScalarMatrix::SafeDownCast(a);
  assert(scalar_item);
  Superclass::DeepCopy(scalar_item);
  if (!scalar_item->GetData().empty())
  {
    m_Data = scalar_item->GetData();
  }
  else
  {
    m_Data.clear();
  }
}

//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::ShallowCopy(albaVMEItem *a)
//-------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *scaler_item = albaVMEItemScalarMatrix::SafeDownCast(a);
  assert(scaler_item);
  m_Data = scaler_item->GetData();
}

//-------------------------------------------------------------------------
bool albaVMEItemScalarMatrix::Equals(albaVMEItem *a)
//-------------------------------------------------------------------------
{
  if (!Superclass::Equals(a))
    return false;

  if ((*GetGlobalCompareDataFlag()))
  {
    albaVMEItemScalarMatrix *item = albaVMEItemScalarMatrix::SafeDownCast(a);

    vnl_matrix<double> data1 = GetData();
    vnl_matrix<double> data2 = item->GetData();

    if (!data1.empty() && !data2.empty())
    {
      // We test for equivalence of data types.
      unsigned elem1 = data1.size();
      unsigned elem2 = data2.size();

      if (elem1 != elem2)
        return false;
      
      unsigned col1 = data1.cols();
      unsigned col2 = data2.cols();
      unsigned row1 = data1.rows();
      unsigned row2 = data2.rows();

      if (col1 != col2 || row1 != row2)
        return false;

      double range1[2],range2[2];
      range1[0] = data1.min_value();
      range1[1] = data1.max_value();
      range2[0] = data2.min_value();
      range2[1] = data2.max_value();

      if (range1[0] != range2[0] || range1[1] != range2[1])
        return false;
    }
    else
    {
      if (data1 != data2)
      {
        return false;
      }
    }
  }

  return true;
}

//-------------------------------------------------------------------------
vnl_matrix<double> &albaVMEItemScalarMatrix::GetData()
//-------------------------------------------------------------------------
{
  UpdateData();
  return m_Data;
}

//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::SetData(vnl_matrix<double> &data)
//-------------------------------------------------------------------------
{
  if (m_Data != data)
  {
    if (!data.empty())
    {
      this->SetDataType("vnl_matrix");

      double bounds[6];
      bounds[0] = bounds[1] = bounds[2] = bounds[3] = bounds[4] = bounds[5] = 0.0;
      m_Bounds.DeepCopy(bounds);
    }
    else
    {
      this->SetDataType("");
      m_Bounds.Reset();
    }

    m_Data = data;
    m_ScalarBouns[0] = m_Data.min_value();
    m_ScalarBouns[1] = m_Data.max_value();
    m_UpdateTime.Modified();

    Modified();

    if (!m_IsLoadingData)
      SetDataModified(true);
  }
}

//-------------------------------------------------------------------------
// Update the internally stored dataset.
void albaVMEItemScalarMatrix::UpdateData()
//-------------------------------------------------------------------------
{
  if (IsDataModified() && !m_Data.empty())
  {
    return;
  }
   
  // At present... if data is already present, simply return
  // otherwise make it be read from disk. Notice that when read
  // from this SetData() is called: Bounds are updated but we need 
  // to reset the DataModified flag.
  if (m_Data.empty())
  {
    if (RestoreData() == ALBA_OK)
    {
      // Data has been generated internally
      //SetDataModified(false);
    }
  }
}

//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::UpdateBounds()
//-------------------------------------------------------------------------
{
  if (!m_Data.empty())
  {
    if (GetMTime()>m_Bounds.GetMTime() || !m_Bounds.IsValid())
    {
      double bounds[6];
      bounds[0] = bounds[1] = bounds[2] = bounds[3] = bounds[4] = bounds[5] = 0.0;
      m_Bounds.DeepCopy(bounds);
      m_ScalarBouns[0] = m_Data.min_value();
      m_ScalarBouns[1] = m_Data.max_value();
    }
  }
  else
  {
    if (!m_Bounds.IsValid())
    {
      // If no data is present and bounds are not valid call UpdateData()
      // to force reading the data. 
      UpdateData();
      
      if (!m_Data.empty())
        this->UpdateBounds(); // if new data loaded try to update bounds
    }
  }
}

//-------------------------------------------------------------------------
const char * albaVMEItemScalarMatrix::GetDataFileExtension()
//-------------------------------------------------------------------------
{
  return "sca";
}

//-------------------------------------------------------------------------
int albaVMEItemScalarMatrix::InternalRestoreData()
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
    assert(storage);
    
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
int albaVMEItemScalarMatrix::ReadData(albaString &filename, int resolvedURL)
//-------------------------------------------------------------------------
{
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
    
    return UpdateReader(filename);
  }
  else if (resolvedURL == ALBA_ERROR)
  {
    return ALBA_NO_IO;
  }
  return ALBA_OK;
}
//-------------------------------------------------------------------------
int albaVMEItemScalarMatrix::UpdateReader(albaString &filename)
//-------------------------------------------------------------------------
{
  vnl_matrix<double> data;

  albaTagItem *item = m_TagArray->GetTag("SCALAR_MATRIX_DIMENSIONS");
  if (item) 
  {
    int r,c;
    r = (int)item->GetComponentAsDouble(0);
    c = (int)item->GetComponentAsDouble(1);
    data.set_size(r,c);
  }

  if (m_IOMode != MEMORY)
  {
    if (GetCrypting())
    {
#ifdef ALBA_USE_CRYPTO
      vcl_stringstream decrypted_raw_matrix;
      decrypted_raw_matrix << m_DecryptedFileString.c_str();
      data.read_ascii(decrypted_raw_matrix);
#else
      albaErrorMacro("Encrypted data not supported: ALBA not linked to Crypto library.");
      return ALBA_ERROR;
#endif
    }
    else
    {
      vcl_ifstream v_raw_matrix(filename, std::ios::in);
      if(v_raw_matrix.is_open() != 0)
      {
        data.read_ascii(v_raw_matrix);
      }
      else
      {
        albaErrorMacro("Error accessing scalar data file.");
        return ALBA_ERROR;
      }
    }
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
    vcl_stringstream raw_matrix_string;
    raw_matrix_string << m_InputMemory << std::endl;
    data.read_ascii(raw_matrix_string);
    delete m_InputMemory;
    m_InputMemory = NULL;
    m_InputMemorySize = 0;
  }

  if (data.empty())
  {
    albaErrorMacro("Cannot read data file " << filename);
    return ALBA_ERROR;
  }
  else
  {
    SetData(data);
  }
  return ALBA_OK;
}
//-------------------------------------------------------------------------
int albaVMEItemScalarMatrix::InternalStoreData(const char *url)
//-------------------------------------------------------------------------
{
  if (!GetData().empty())
  {
    bool found = false;
    albaString filename;
          
    albaEventIO e(this,NODE_GET_STORAGE);
    albaEventMacro(e);
    albaStorage *storage = e.GetStorage();
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

        found = storage->IsFileInDirectory(url);
        storage->GetTmpFile(filename);
      break;
      default:
        albaErrorMacro("Unsupported I/O Mode");
      return ALBA_ERROR;
    }

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

    m_IOStatus = 0;
    int ret = ALBA_OK; // value returned by StoreToURL() function at the end of saving to file
    if ((IsDataPresent() && (!found || (m_URL != url))) || ((IsDataPresent() == found) && (found == IsDataModified())))
    {       
      vnl_matrix<double> data = GetData();

      // problems retrieving data... (e.g. when a file has been erroneously deleted or corrupted...)
      if (data.empty())
      {
        return 0;
      }
      
      // force release old writer if present
      ReleaseOutputMemory();

      int r,c;
      r = data.rows();
      c = data.columns();
      albaTagItem item;
      item.SetName("SCALAR_MATRIX_DIMENSIONS");
      item.SetNumberOfComponents(2);
      item.SetComponent(r,0);
      item.SetComponent(c,1);
      m_TagArray->SetTag(item);

      //unsigned data_size = data.size();
      //double *s = new double[data_size];
      //data.copy_out(s);
      vcl_stringstream data_stream;
      data.print(data_stream);
      m_DataString = data_stream.str().c_str();
      /*m_DataString << s[0];
      for (int i = 1; i < data_size; i++)
      {
        m_DataString << " ";
        m_DataString << s[i];
      }*/
      //delete s;

      if ( m_IOMode == MEMORY)
      {
        if (m_Crypting)
        {
#ifdef ALBA_USE_CRYPTO
          std::string encrypted_output;
//          albaDefaultEncryptFromMemory(m_DataString.GetCStr(), m_DataString.Length(), encrypted_output);
          albaDefaultEncryptFromMemory(m_DataString.GetCStr(), encrypted_output);
          m_OutputMemory = encrypted_output.c_str();
          m_OutputMemorySize = encrypted_output.size();
#else
          albaErrorMacro("Encrypted data is not supported: Crypto library not linked to ALBA!");
          return ALBA_ERROR;
#endif
        }
        else
        {
          m_OutputMemory = m_DataString.GetCStr();
          m_OutputMemorySize = m_DataString.Length();
        }
      }
      else
      {
        vcl_ofstream writer(filename);
        if (!writer.bad())
        {
          writer << m_DataString.GetCStr() << std::endl;
          writer.close();
        }
        else
        {
          albaErrorMacro("Error on writing data!");
          return ALBA_ERROR;
        }
      }
      if (m_IOMode == DEFAULT)
      {
        SetURL(url);
        ret = storage->StoreToURL(filename,m_URL);
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

/*
//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::ErrorHandler(void *ptr)
//-------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *self=(albaVMEItemScalarMatrix *)ptr;
  self->m_IOStatus = ALBA_ERROR;
}
*/
//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::ReleaseData()
//-------------------------------------------------------------------------
{
  m_Data.clear();
}

//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::GetOutputMemory(const char *&out_str, int &size)
//-------------------------------------------------------------------------
{
  if (!m_DataString.IsEmpty())
  {
    out_str = m_DataString.GetCStr();
    size = m_Data.size();
  }
  else
  {
    out_str = NULL;
    size = 0;
  }
}
//-------------------------------------------------------------------------
bool albaVMEItemScalarMatrix::StoreToArchive(wxZipOutputStream &zip)
//-------------------------------------------------------------------------
{
  wxStringInputStream data_stream(m_OutputMemory);
  if (!zip.PutNextEntry(m_URL.GetCStr(), wxDateTime::Now(), m_OutputMemorySize) || !zip.Write(data_stream))
    return false;
  return true;
}
//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::ReleaseOutputMemory()
//-------------------------------------------------------------------------
{
  m_OutputMemory = NULL;
  m_OutputMemorySize = 0;
}
//-------------------------------------------------------------------------
void albaVMEItemScalarMatrix::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);

  Superclass::Print(os,indent);

  // to do: implement DUMP of internally stored data
  os << indent << "Scalar Range: " << m_ScalarBouns[0] << m_ScalarBouns[1] << std::endl;
  os << indent << "Num rows: " << m_Data.rows() << std::endl;
  os << indent << "Num columns: " << m_Data.columns() << std::endl;
}
