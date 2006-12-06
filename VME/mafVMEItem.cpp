/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItem.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-06 09:37:06 $
  Version:   $Revision: 1.10 $
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


#include "mafVMEItem.h"
#include "mafTagArray.h"
#include "mafIndent.h"
#include "mafStorageElement.h"
#include "mmuIdFactory.h"
#include "mafEventIO.h"
#include "mafStorage.h"
#include <math.h>
#include <assert.h>

bool mafVMEItem::m_GlobalCompareDataFlag=0;

MAF_ID_IMP(mafVMEItem::VME_ITEM_DATA_MODIFIED);

//-------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafVMEItem);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEItem::mafVMEItem()
//-------------------------------------------------------------------------
{
  m_DataModified  = false;
  //m_VME=NULL;
  mafNEW(m_TagArray);
  
  m_Id            = -1;
  m_TimeStamp     = 0;
  m_Crypting      = false;
  m_IOMode        = DEFAULT;
  
  m_OutputMemory    = NULL;
  m_OutputMemorySize= 0;
  m_InputMemory     = NULL;
  m_InputMemorySize = 0;

  m_ReleaseOldFile  = true;
  m_IsLoadingData   = false;
}

//-------------------------------------------------------------------------
mafVMEItem::~mafVMEItem()
//-------------------------------------------------------------------------
{
  SetURL(""); // this simply force to garbage collect the linked URL when the item is destroyed
  mafDEL(m_TagArray);
}

//-------------------------------------------------------------------------
bool mafVMEItem::GetCrypting()
//-------------------------------------------------------------------------
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
void mafVMEItem::SetCrypting(bool flag)
//-------------------------------------------------------------------------
{
  m_Crypting = flag;
  Modified();
}

//-------------------------------------------------------------------------
void mafVMEItem::SetDataModified(bool flag)
//-------------------------------------------------------------------------
{
  m_DataModified=flag;
  Modified();
  InvokeEvent(VME_ITEM_DATA_MODIFIED);
}

//-------------------------------------------------------------------------
void mafVMEItem::GetBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  UpdateBounds();
  m_Bounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEItem::GetBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{
  UpdateBounds();
  bounds=m_Bounds;
}

//-------------------------------------------------------------------------
double *mafVMEItem::GetBounds()
//-------------------------------------------------------------------------
{
  UpdateBounds();
  return m_Bounds.m_Bounds;
}


//-------------------------------------------------------------------------
mafTagArray *mafVMEItem::GetTagArray()
//-------------------------------------------------------------------------
{
  return m_TagArray;
}

//-------------------------------------------------------------------------
void mafVMEItem::DeepCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  assert(a);
  
  // copy member variables
  m_TagArray->DeepCopy(a->GetTagArray());
  SetTimeStamp(a->GetTimeStamp());
  m_DataType=a->GetDataType();
  //SetVME(a->GetVME());

  // subclasses should reimplement DeepCopy to copy data

  Modified();
  //m_UpdateTime.Modified();
  SetDataModified(true);
}

//-------------------------------------------------------------------------
void mafVMEItem::ShallowCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  assert(a);

  // make a copy of tags
  m_TagArray->DeepCopy(a->GetTagArray());

  SetTimeStamp(a->GetTimeStamp());
  m_DataType=a->GetDataType();
  //SetVME(a->GetVME());

  // subclass should copy also data pointer

  Modified();
  SetDataModified(true);
}

//-------------------------------------------------------------------------
bool mafVMEItem::Equals(mafVMEItem *o)
//-------------------------------------------------------------------------
{
  if (o==NULL || !o->IsA(GetTypeId()) || \
    m_DataType!=o->m_DataType || \
    !mafEquals(m_TimeStamp,o->m_TimeStamp)) // consider only 15 digits to avoid the dirty bit
  {
    return false;
  }

  UpdateBounds(); o->UpdateBounds();
  if (!m_Bounds.Equals(o->m_Bounds))
  {
    return false;
  }

  // must check the tags after updating the bounds, since UpdateBounds() could 
  // generate a new TAG for storing bounds.
  if (!GetTagArray()->Equals(o->GetTagArray()))
  {
    return false;
  }

  return true;
}

//-------------------------------------------------------------------------
void mafVMEItem::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << GetTypeName() <<" Contents:\n";

  Superclass::Print(os,indent);

  os << indent << "DataType: " << "\""<<m_DataType<<"\""<<std::endl;

  os << indent << "TimeStamp: "<< GetTimeStamp()<<std::endl;

  os << indent << "TagArray:\n";
  m_TagArray->Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
int mafVMEItem::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (parent->StoreText("URL",m_URL)==MAF_OK \
    &&parent->StoreInteger("Id",m_Id)==MAF_OK \
    &&parent->StoreText("DataType",m_DataType)==MAF_OK \
    &&parent->StoreDouble("TimeStamp",m_TimeStamp)==MAF_OK \
    &&parent->StoreText("Crypting",m_Crypting?"true":"false")==MAF_OK \
    &&parent->StoreVectorN("Bounds",m_Bounds.m_Bounds,6)==MAF_OK \
    &&parent->StoreObject("TagArray",m_TagArray)!=NULL)
  {
    return MAF_OK;
  }
  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
void mafVMEItem::SetURL(const char *name)
//-------------------------------------------------------------------------
{
  if (!m_URL.IsEmpty()&&m_URL!=name)
  {
    mafEventIO e(this,NODE_GET_STORAGE);
    mafEventMacro(e);
    mafStorage *storage=e.GetStorage();
    if (storage)
    {
      if (m_ReleaseOldFile)
        storage->ReleaseURL(m_URL); // remove old file
      SetDataModified(true); // force rewriting data 
    }
  }
  m_URL=name;
}

//-------------------------------------------------------------------------
int mafVMEItem::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  mafString crypting;
  if (node->RestoreText("URL",m_URL)==MAF_OK \
    &&node->RestoreInteger("Id",m_Id)==MAF_OK \
    &&node->RestoreText("DataType",m_DataType)==MAF_OK \
    &&node->RestoreDouble("TimeStamp",m_TimeStamp)==MAF_OK \
    &&node->RestoreText("Crypting",crypting)==MAF_OK \
    &&node->RestoreVectorN("Bounds",m_Bounds.m_Bounds,6)==MAF_OK \
    &&node->RestoreObject("TagArray",m_TagArray)==MAF_OK)
  {
    m_Crypting = (crypting=="true"||crypting=="True"||crypting=="TRUE")?true:false;

    // DATA is restored only on demand

    return MAF_OK;
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEItem::StoreData(const char *url)
//-------------------------------------------------------------------------
{
  return InternalStoreData(url);
}
//-------------------------------------------------------------------------
int mafVMEItem::RestoreData()
//-------------------------------------------------------------------------
{
  m_IsLoadingData = true;
  int ret = InternalRestoreData();
  m_IsLoadingData = false;
  return ret;
}

//-------------------------------------------------------------------------
void mafVMEItem::SetInputMemory(const char *int_str,unsigned long size)
//-------------------------------------------------------------------------
{
  m_InputMemory = int_str;
  m_InputMemorySize = size;
}
