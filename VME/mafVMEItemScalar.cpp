/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:23:37 $
  Version:   $Revision: 1.10 $
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

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEItemScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEItemScalar::mafVMEItemScalar()
//-------------------------------------------------------------------------
{
  m_IOStatus    = MAF_OK;
  m_DataString  = "";
  m_Data        = MAF_SCALAR_MIN;
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
  m_Data = scalar_item->GetData();
}

//-------------------------------------------------------------------------
void mafVMEItemScalar::ShallowCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  mafVMEItemScalar *scaler_item = mafVMEItemScalar::SafeDownCast(a);
  assert(scaler_item);
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
    double data = item->GetData();
    if (IsDataPresent() && item->IsDataPresent())
    {
      return mafEquals(m_Data, data);
    }
    else
    {
      if (!mafEquals(m_Data, data))
      {
        return false;
      }
    }
  }

  return true;
}

//-------------------------------------------------------------------------
double mafVMEItemScalar::GetData()
//-------------------------------------------------------------------------
{
  UpdateData();
  return m_Data;
}

//-------------------------------------------------------------------------
void mafVMEItemScalar::SetData(double data)
//-------------------------------------------------------------------------
{
  if (!mafEquals(m_Data, data))
  {
    if (!mafEquals(MAF_SCALAR_MIN, data))
    {
      this->SetDataType("double");

      double bounds[6];
      bounds[0] = bounds[1] = bounds[2] = bounds[3] = bounds[4] = bounds[5] = data;
      m_Bounds.DeepCopy(bounds);
    }
    else
    {
      this->SetDataType("");
      m_Bounds.Reset();
    }

    m_Data = data;
    m_DataString = "";
    m_DataString << m_Data;
    m_ScalarBouns[0] = m_ScalarBouns[1] = m_Data;
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
  if (IsDataModified() && IsDataPresent())
  {
    return;
  }
   
  // At present... if data is already present, simply return
  // otherwise make it be read from disk. Notice that when read
  // from this SetData() is called: Bounds are updated but we need 
  // to reset the DataModified flag.
  if (mafEquals(m_Data, MAF_SCALAR_MIN))
  {
    // m_Data should be always different from MAF_SCALAR_MIN
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
  if (IsDataPresent())
  {
    if (GetMTime()>m_Bounds.GetMTime() || !m_Bounds.IsValid())
    {
      double bounds[6];
      bounds[0] = bounds[1] = bounds[2] = bounds[3] = bounds[4] = bounds[5] = m_Data;
      m_Bounds.DeepCopy(bounds);
      m_ScalarBouns[0] = m_Data;
      m_ScalarBouns[1] = m_Data;
    }
  }
  else
  {
    if (!m_Bounds.IsValid())
    {
      // If no data is present and bounds are not valid call UpdateData()
      // to force reading the data. 
      UpdateData();
      
      if (IsDataPresent())
        this->UpdateBounds(); // if new data loaded try to update bounds
    }
  }
}

//-------------------------------------------------------------------------
const char *mafVMEItemScalar::GetDataFileExtension()
//-------------------------------------------------------------------------
{
  return "sca";
}
//-------------------------------------------------------------------------
int mafVMEItemScalar::ReadData(mafString &filename, int resolvedURL)
//-------------------------------------------------------------------------
{
  return MAF_OK;
}
//-------------------------------------------------------------------------
int mafVMEItemScalar::InternalRestoreData()
//-------------------------------------------------------------------------
{
  return MAF_OK;
}
//-------------------------------------------------------------------------
int mafVMEItemScalar::InternalStoreData(const char *url)
//-------------------------------------------------------------------------
{
  return MAF_OK;
}
//-------------------------------------------------------------------------
int mafVMEItemScalar::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (mafVMEItem::InternalStore(parent) == MAF_OK)
  {
    if (parent->StoreDouble("ScalarData", m_Data) == MAF_OK)
    {
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
int mafVMEItemScalar::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  if (mafVMEItem::InternalRestore(node) == MAF_OK)
  {
    if (node->RestoreDouble("ScalarData", m_Data) == MAF_OK)
    {
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMEItemScalar::ReleaseData()
//-------------------------------------------------------------------------
{
  m_Data = MAF_SCALAR_MIN;
  m_DataString = "";
}

//-------------------------------------------------------------------------
void mafVMEItemScalar::GetOutputMemory(const char *&out_str, int &size)
//-------------------------------------------------------------------------
{
  if (!m_DataString.IsEmpty())
  {
    out_str = m_DataString.GetCStr();
    size = m_DataString.Length();
  }
  else
  {
    out_str = NULL;
    size = 0;
  }
}
//-------------------------------------------------------------------------
bool mafVMEItemScalar::StoreToArchive(wxZipOutputStream &zip)
//-------------------------------------------------------------------------
{
  return true;
}
//-------------------------------------------------------------------------
void mafVMEItemScalar::ReleaseOutputMemory()
//-------------------------------------------------------------------------
{
  m_OutputMemory = NULL;
  m_OutputMemorySize = 0;
}
//-------------------------------------------------------------------------
void mafVMEItemScalar::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  Superclass::Print(os,indent);

  // to do: implement DUMP of internally stored data
  os << indent << "Scalar Range: " << m_ScalarBouns[0] << m_ScalarBouns[1] << std::endl;
  os << indent << "data: " << m_Data << std::endl;
}
