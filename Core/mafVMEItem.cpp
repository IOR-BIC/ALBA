/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItem.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:36:38 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafVMEItem.h"
#include "mmaTagArray.h"
#include "mafIndent.h"
#include "mafOBB.h"
#include "mafStorageElement.h"
#include <assert.h>

bool mafVMEItem::m_GlobalCompareDataFlag=0;

//-------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafVMEItem);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEItem::mafVMEItem()
//-------------------------------------------------------------------------
{
  m_ModifiedData=false;
  m_VME=NULL;
  mafNEW(m_TagArray);
  cppNEW(m_Bounds);
  
  m_Id        = -1;
  m_TimeStamp = 0;
  m_Crypting  = false;
}

//-------------------------------------------------------------------------
mafVMEItem::~mafVMEItem()
//-------------------------------------------------------------------------
{
  mafDEL(m_TagArray);
  cppDEL(m_Bounds);
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
void mafVMEItem::GetBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  UpdateBounds();
  m_Bounds->CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEItem::GetBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{
  UpdateBounds();
  bounds=*m_Bounds;
}

//-------------------------------------------------------------------------
double *mafVMEItem::GetBounds()
//-------------------------------------------------------------------------
{
  UpdateBounds();
  return m_Bounds->m_Bounds;
}


//-------------------------------------------------------------------------
mmaTagArray *mafVMEItem::GetTagArray()
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
  SetVME(a->GetVME());

  // subclasses should reimplement DeepCopy to copy data

  Modified();
  //m_UpdateTime.Modified();
  SetModifiedData(true);
}

//-------------------------------------------------------------------------
void mafVMEItem::ShallowCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  assert(a);

  // make a copy of tags
  m_TagArray->DeepCopy(a->GetTagArray());

  this->SetTimeStamp(a->GetTimeStamp());
  this->m_DataType=a->GetDataType();
  this->SetVME(a->GetVME());

  // subclass should copy also data pointer

  Modified();
  SetModifiedData(true);
}

//-------------------------------------------------------------------------
bool mafVMEItem::Equals(mafVMEItem *o)
//-------------------------------------------------------------------------
{
  if (o==NULL || !(m_DataType.Equals(o->m_DataType)) || \
    m_TimeStamp!=o->m_TimeStamp)
  {
    return false;
  }

  UpdateBounds(); o->UpdateBounds();
  if (!m_Bounds->Equals(o->m_Bounds))
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
void mafVMEItem::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << GetTypeName() <<" Contents:\n";

  Superclass::Print(os,indent);

  os << indent << "DataType: " << "\""<<m_DataType<<"\""<<std::endl;

  os << indent << "TimeStamp: "<< this->GetTimeStamp()<<std::endl;

  os << indent << "TagArray:\n";
  m_TagArray->Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
int mafVMEItem::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (parent->StoreText(m_URL,"URL")==MAF_OK \
    &&parent->StoreInteger(m_Id,"Id")==MAF_OK \
    &&parent->StoreText(m_DataType,"DataType")==MAF_OK \
    &&parent->StoreDouble(m_TimeStamp,"TimeStamp")==MAF_OK \
    &&parent->StoreText("Crypting",m_Crypting?"true":"false")==MAF_OK \
    &&parent->StoreVectorN(m_Bounds->m_Bounds,6,"Bounds")==MAF_OK \
    &&parent->StoreObject(m_TagArray,"TagArray")!=NULL)
  {
    return InternalStoreData();
  }
  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEItem::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  mafString crypting;
  mafObject *obj;
  if (node->RestoreText(m_URL,"URL")==MAF_OK \
    &&node->RestoreInteger(m_Id,"Id")==MAF_OK \
    &&node->RestoreText(m_DataType,"DataType")==MAF_OK \
    &&node->RestoreDouble(m_TimeStamp,"TimeStamp")==MAF_OK \
    &&node->RestoreText(crypting,"Crypting")==MAF_OK \
    &&node->RestoreVectorN(m_Bounds->m_Bounds,6,"Bounds")==MAF_OK \
    &&node->RestoreObject(obj,"TagArray")==MAF_OK)
  {
    m_Crypting = (crypting=="true"||crypting=="True"||crypting=="TRUE")?true:false;
    mmaTagArray *tarray=mmaTagArray::SafeDownCast(obj);

    assert(tarray);
    mafDEL(m_TagArray);
    m_TagArray=mmaTagArray::SafeDownCast(tarray);

    assert(m_TagArray);
    m_TagArray->Register(this);

    // DATA is restored only on demand

    return MAF_OK;
  }

  return MAF_ERROR;
}