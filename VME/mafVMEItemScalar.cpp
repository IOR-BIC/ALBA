/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2006-07-24 08:53:23 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
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


#include "mafVMEItemScalar.h"
#include "mafEventIO.h"
#include "mafIndent.h"
#include "mafStorage.h"
#include "mafStorageElement.h"
#include "mafCrypt.h"
#include "mafTagItem.h"
#include "mafTagArray.h"

#include <assert.h>

#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEItemScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEItemScalar::mafVMEItemScalar()
//-------------------------------------------------------------------------
{
  m_IOStatus    = MAF_OK;
  m_DataString  = "";
}

//-------------------------------------------------------------------------
mafVMEItemScalar::~mafVMEItemScalar()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void mafVMEItemScalar::DeepCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  mafVMEItemScalar *scalar_item = mafVMEItemScalar::SafeDownCast(a);
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
void mafVMEItemScalar::ShallowCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  mafVMEItemScalar *scaler_item = mafVMEItemScalar::SafeDownCast(a);
  assert(scaler_item);
  m_Data = scaler_item->GetData();
}

//-------------------------------------------------------------------------
bool mafVMEItemScalar::Equals(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  if (!Superclass::Equals(a))
    return false;

  if (m_GlobalCompareDataFlag)
  {
    mafVMEItemScalar *item = mafVMEItemScalar::SafeDownCast(a);

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
vnl_matrix<double> &mafVMEItemScalar::GetData()
//-------------------------------------------------------------------------
{
  UpdateData();
  return m_Data;
}

//-------------------------------------------------------------------------
void mafVMEItemScalar::SetData(vnl_matrix<double> &data)
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
void mafVMEItemScalar::UpdateData()
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
    if (RestoreData() == MAF_OK)
    {
      // Data has been generated internally
      //SetDataModified(false);
    }
  }
}

//-------------------------------------------------------------------------
void mafVMEItemScalar::UpdateBounds()
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
const char * mafVMEItemScalar::GetDataFileExtension()
//-------------------------------------------------------------------------
{
  return "sca";
}

//-------------------------------------------------------------------------
int mafVMEItemScalar::InternalRestoreData()
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
    
    mafString filename;
    storage->ResolveInputURL(m_URL,filename);
    
    vnl_matrix<double> data;

    mafTagItem *item = m_TagArray->GetTag("SCALAR_MATRIX_DIMENSIONS");
    if (item) 
    {
      int r,c;
      r = (int)item->GetComponentAsDouble(0);
      c = (int)item->GetComponentAsDouble(1);
      data.set_size(r,c);
    }
    
    if (GetCrypting())
    {
#ifdef MAF_USE_CRYPTO
      std::string file_string;
      mafDefaultDecryptInMemory(filename, file_string);
      //data.read_ascii(file_string); //------------------------------------------------------------------- <--
#else
      mafErrorMacro("Crypted data not supported: MAF not linked to Crypto library.");
      return MAF_ERROR;
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
        mafErrorMacro("Error accessing scalar data file.");
        return MAF_ERROR;
      }
    }

    if (data.empty())
    {
      mafErrorMacro("Cannot read data file " << filename);
      return MAF_ERROR;
    }
    else
    {
      SetData(data);
    }
    return MAF_OK;
  } 
  return MAF_NO_IO;
}

//-------------------------------------------------------------------------
int mafVMEItemScalar::InternalStoreData(const char *url)
//-------------------------------------------------------------------------
{
  if (!GetData().empty())
  {
    bool found = false;
    mafString filename;
          
    mafEventIO e(this,NODE_GET_STORAGE);
    mafEventMacro(e);
    mafStorage *storage = e.GetStorage();
    assert(storage);

    switch (m_IOMode)
    {
      case MEMORY:
        found = false;
        mafErrorMacro("Unsupported I/O Mode");
      return MAF_ERROR;
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

        found = storage->IsFileInDirectory(url);
        storage->GetTmpFile(filename);
      break;
      default:
        mafErrorMacro("Unsupported I/O Mode");
      return MAF_ERROR;
    }

    assert(!(m_IOMode!=MEMORY&&filename.IsEmpty()));

    if (m_IOMode != MEMORY && filename.IsEmpty())
    {
      mafErrorMacro("Unsupported I/O Mode");
      return MAF_ERROR;
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
    int ret = MAF_OK; // value returned by StoreToURL() function at the end of saving to file
    if ((IsDataPresent()&&(!found||(m_URL!=url)))||((IsDataPresent()==found)&&(found==IsDataModified())))
    {       
      vnl_matrix<double> data = GetData();

      // problems retrieving data... (e.g. when a file has been erroneously deleted or corrupted...)
      if (data.empty())
      {
        return 0;
      }
      
      int r,c;
      r = data.rows();
      c = data.columns();
      mafTagItem item;
      item.SetName("SCALAR_MATRIX_DIMENSIONS");
      item.SetNumberOfComponents(2);
      item.SetComponent(r,0);
      item.SetComponent(c,1);
      m_TagArray->SetTag(item);

      // force release old writer if present
      ReleaseOutputMemory();
      unsigned data_size = data.size();
      double *s = new double[data_size];
      data.copy_out(s);
      m_DataString << s[0];
      for (int i=1; i<data_size;i++)
      {
        m_DataString << " ";
        m_DataString << s[i];
      }
      delete s;

      if (m_Crypting)
      {
#ifdef MAF_USE_CRYPTO
        mafDefaultEncryptFromMemory(m_DataString.GetCStr(), data_size, filename);
#else
        mafErrorMacro("Crypted data is not supported: Crypto library not linked to MAF!");
        return MAF_ERROR;
#endif
      }
      else if ( m_IOMode == MEMORY)
      {
        // not yet supported
        m_OutputMemory = m_DataString.GetCStr();
        m_OutputMemorySize = data_size;
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
          mafErrorMacro("Error on writing data!");
          return MAF_ERROR;
        }
      }
      if (m_IOMode==DEFAULT)
        SetURL(url);

      ret = storage->StoreToURL(filename,m_URL);
    }
    else
    {
      // if data has been set to NULL reset the filename
      if ((!IsDataPresent()&&!found)||(!IsDataPresent()&&found&&IsDataModified()))
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

/*
//-------------------------------------------------------------------------
void mafVMEItemScalar::ErrorHandler(void *ptr)
//-------------------------------------------------------------------------
{
  mafVMEItemScalar *self=(mafVMEItemScalar *)ptr;
  self->m_IOStatus = MAF_ERROR;
}
*/
//-------------------------------------------------------------------------
void mafVMEItemScalar::ReleaseData()
//-------------------------------------------------------------------------
{
  m_Data.clear();
}

//-------------------------------------------------------------------------
void mafVMEItemScalar::GetOutputMemory(const char *&out_str, int &size)
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
void mafVMEItemScalar::ReleaseOutputMemory()
//-------------------------------------------------------------------------
{
  m_OutputMemory = NULL;
}
//-------------------------------------------------------------------------
void mafVMEItemScalar::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  Superclass::Print(os,indent);

  // to do: implement DUMP of internally stored data
  os << indent << "Scalar Range: " << m_ScalarBouns[0] << m_ScalarBouns[1] << std::endl;
  os << indent << "Num rows: " << m_Data.rows() << std::endl;
  os << indent << "Num columns: " << m_Data.columns() << std::endl;
}
