/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItem
 Authors: Marco Petrone - Paolo Quadrani
 
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


#include "albaVMEItem.h"
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "albaVMERoot.h"
#include "albaTagArray.h"
#include "albaIndent.h"
#include "albaStorageElement.h"
#include "mmuIdFactory.h"
#include "albaEventIO.h"
#include "albaStorage.h"
#include <math.h>
#include <assert.h>

// bool albaVMEItem::m_GlobalCompareDataFlag=0;

ALBA_ID_IMP(albaVMEItem::VME_ITEM_DATA_MODIFIED);

//-------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaVMEItem);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEItem::albaVMEItem()
//-------------------------------------------------------------------------
{
  m_DataModified  = false;
  //m_VME=NULL;
  albaNEW(m_TagArray);
  
  m_Id            = -1;
  m_TimeStamp     = 0;
  m_Crypting      = false;
  m_IOMode        = DEFAULT;
  
  m_OutputMemory    = NULL;
  m_OutputMemorySize= 0;
  m_InputMemory     = NULL;
  m_InputMemorySize = 0;

  m_DataObserver= new albaVMEItemAsynchObserver();

  m_ReleaseOldFile  = true;
  m_IsLoadingData   = false;

  m_ArchiveFileName = "";
  m_TmpFileName     = "";
  m_ChecksumMD5     = "";
}

//-------------------------------------------------------------------------
albaVMEItem::~albaVMEItem()
//-------------------------------------------------------------------------
{
  cppDEL(m_DataObserver);
  SetURL(""); // this simply force to garbage collect the linked URL when the item is destroyed
  albaDEL(m_TagArray);
}

//-------------------------------------------------------------------------
bool albaVMEItem::GetCrypting()
//-------------------------------------------------------------------------
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
void albaVMEItem::SetCrypting(bool flag)
//-------------------------------------------------------------------------
{
  m_Crypting = flag;
  Modified();
}

//-------------------------------------------------------------------------
void albaVMEItem::SetDataModified(bool flag)
//-------------------------------------------------------------------------
{
  m_DataModified=flag;
  Modified();
  InvokeEvent(this, VME_ITEM_DATA_MODIFIED);
}

//-------------------------------------------------------------------------
void albaVMEItem::GetBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  UpdateBounds();
  m_Bounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void albaVMEItem::GetBounds(albaOBB &bounds)
//-------------------------------------------------------------------------
{
  UpdateBounds();
  bounds=m_Bounds;
}

//-------------------------------------------------------------------------
double *albaVMEItem::GetBounds()
//-------------------------------------------------------------------------
{
  UpdateBounds();
  return m_Bounds.m_Bounds;
}


//-------------------------------------------------------------------------
albaTagArray *albaVMEItem::GetTagArray()
//-------------------------------------------------------------------------
{
  return m_TagArray;
}

//-------------------------------------------------------------------------
void albaVMEItem::DeepCopy(albaVMEItem *a)
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
void albaVMEItem::ShallowCopy(albaVMEItem *a)
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
bool albaVMEItem::Equals(albaVMEItem *o)
//-------------------------------------------------------------------------
{
  if (o==NULL || !o->IsA(GetTypeId()) || \
    m_DataType!=o->m_DataType || \
    !albaEquals(m_TimeStamp,o->m_TimeStamp)) // consider only 15 digits to avoid the dirty bit
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
void albaVMEItem::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);

  os << indent << GetTypeName() <<" Contents:\n";

  Superclass::Print(os,indent);

  os << indent << "DataType: " << "\""<<m_DataType<<"\""<<std::endl;

  os << indent << "TimeStamp: "<< GetTimeStamp()<<std::endl;

  os << indent << "TagArray:\n";
  m_TagArray->Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
int albaVMEItem::InternalStore(albaStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (parent->StoreText("URL",m_URL)==ALBA_OK \
    &&parent->StoreInteger("Id",m_Id)==ALBA_OK \
    &&parent->StoreText("DataType",m_DataType)==ALBA_OK \
    &&parent->StoreDouble("TimeStamp",m_TimeStamp)==ALBA_OK \
    &&parent->StoreText("Crypting",m_Crypting?"true":"false")==ALBA_OK \
    &&parent->StoreVectorN("Bounds",m_Bounds.m_Bounds,6)==ALBA_OK \
    &&parent->StoreObject("TagArray",m_TagArray)!=NULL)
  {
    return ALBA_OK;
  }
  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
void albaVMEItem::SetURL(const char *name)
//-------------------------------------------------------------------------
{
  if (!m_URL.IsEmpty()&&m_URL!=name)
  {
    albaEventIO e(this,NODE_GET_STORAGE);
    albaEventMacro(e);
    albaStorage *storage=e.GetStorage();
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
int albaVMEItem::InternalRestore(albaStorageElement *node)
//-------------------------------------------------------------------------
{
  albaString crypting;
  if (node->RestoreText("URL",m_URL)==ALBA_OK \
    &&node->RestoreInteger("Id",m_Id)==ALBA_OK \
    &&node->RestoreText("DataType",m_DataType)==ALBA_OK \
    &&node->RestoreDouble("TimeStamp",m_TimeStamp)==ALBA_OK \
    &&node->RestoreText("Crypting",crypting)==ALBA_OK \
    &&node->RestoreVectorN("Bounds",m_Bounds.m_Bounds,6)==ALBA_OK \
    &&node->RestoreObject("TagArray",m_TagArray)==ALBA_OK)
  {
    m_Crypting = (crypting=="true"||crypting=="True"||crypting=="true")?true:false;

    // DATA is restored only on demand when is on Default mode
    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
int albaVMEItem::StoreData(const char *url)
//-------------------------------------------------------------------------
{
  return InternalStoreData(url);
}
//-------------------------------------------------------------------------
int albaVMEItem::RestoreData()
//-------------------------------------------------------------------------
{
  m_IsLoadingData = true;

  // To prevent errors when saving in different way of loaded data
  // archive file name have to be tested. It is not set before item is written
  // into the archive, so we have to switch into the DEFAULT mode to read correctly
  // the item and then restore the IO Mode.
  int old_mode = m_IOMode;
  if (m_ArchiveFileName.IsEmpty())
  {
    SetIOMode(DEFAULT);
  }
  int ret = InternalRestoreData();
  SetIOMode(old_mode);
  //m_IsLoadingData = false;
  m_IsLoadingData = ret != ALBA_OK;
  return ret;
}

//-------------------------------------------------------------------------
int albaVMEItem::ExtractFileFromArchive(albaString &archive_fullname, albaString &item_file)
//-------------------------------------------------------------------------
{
  wxFileInputStream in(archive_fullname.GetCStr());
  wxZipInputStream zip(in);
  if (!in || !zip)
    return ALBA_ERROR;
  wxZipEntry *entry = NULL;
  // convert the local name we are looking for into the internal format
  wxString name = wxZipEntry::GetInternalName(item_file.GetCStr());

  // call GetNextEntry() until the required internal name is found
  // to be re-factored for efficiency reasons.
  do 
  {
    if (entry)
    {
      delete entry;
      entry = NULL;
    }
    entry = zip.GetNextEntry();
  } while(entry != NULL && entry->GetInternalName() != name);

  if (entry != NULL) 
  {
    // read the entry's data...
    m_InputMemorySize = entry->GetSize();
    m_InputMemory = new char[m_InputMemorySize];
    zip.Read((char *)m_InputMemory, m_InputMemorySize);
    delete entry;
    entry = NULL;
  }
  
  return ALBA_OK;
}

//-------------------------------------------------------------------------
void albaVMEItem::SetInputMemory(const char *int_str,unsigned long size)
//-------------------------------------------------------------------------
{
  m_InputMemory = int_str;
  m_InputMemorySize = size;
}

//-------------------------------------------------------------------------
void albaVMEItem::UpdateItemId()
//-------------------------------------------------------------------------
{
  // retrieve the tree root
  albaEventIO e(this,NODE_GET_ROOT);
  InvokeEvent(e);

  albaVMERoot *root = albaVMERoot::SafeDownCast(e.GetRoot());
  albaID itemId = root ? root->GetNextItemId():-1;

  SetId(itemId);
}
//----------------------------------------------------------------------------
bool* albaVMEItem::GetGlobalCompareDataFlag()
//----------------------------------------------------------------------------
{
  static bool globalCompareDataFlag = false;
  return &globalCompareDataFlag;
}
//----------------------------------------------------------------------------
void albaVMEItem::SetGlobalCompareDataFlag(bool f)
//----------------------------------------------------------------------------
{
  (*GetGlobalCompareDataFlag()) = f;
}

//----------------------------------------------------------------------------
//     ****************  albaVMEItemAsynchObserver  ****************
//----------------------------------------------------------------------------
ALBA_ID_IMP(albaVMEItemAsynchObserver::VME_ITEM_DATA_DOWNLOADED);

//----------------------------------------------------------------------------
albaVMEItemAsynchObserver::albaVMEItemAsynchObserver()
//----------------------------------------------------------------------------
{
  m_Item = NULL;
}
//----------------------------------------------------------------------------
albaVMEItemAsynchObserver::~albaVMEItemAsynchObserver() 
//----------------------------------------------------------------------------
{
  m_Item = NULL;
}
//----------------------------------------------------------------------------
void albaVMEItemAsynchObserver::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (alba_event->GetId() == VME_ITEM_DATA_DOWNLOADED)
  {
    m_Item->ReadData(m_Filename);
  }
}
