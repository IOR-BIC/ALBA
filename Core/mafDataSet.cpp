/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-02 00:32:45 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafDataSet.h"
#include "mmaTagArray.h"
#include "mafIndent.h"
#include "mafOBB.h"
#include "mafStorageElement.h"
#include <assert.h>

bool mafDataSet::m_GlobalCompareDataFlag=0;

//-------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafDataSet);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafDataSet::mafDataSet()
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
mafDataSet::~mafDataSet()
//-------------------------------------------------------------------------
{
  mafDEL(m_TagArray);
  cppDEL(m_Bounds);
}

//-------------------------------------------------------------------------
bool mafDataSet::GetCrypting()
//-------------------------------------------------------------------------
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
void mafDataSet::SetCrypting(bool flag)
//-------------------------------------------------------------------------
{
  m_Crypting = flag;
  Modified();
}

//-------------------------------------------------------------------------
void mafDataSet::GetBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  UpdateBounds();
  m_Bounds->CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafDataSet::GetBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{
  UpdateBounds();
  bounds=*m_Bounds;
}

//-------------------------------------------------------------------------
double *mafDataSet::GetBounds()
//-------------------------------------------------------------------------
{
  UpdateBounds();
  return m_Bounds->m_Bounds;
}


//-------------------------------------------------------------------------
mmaTagArray *mafDataSet::GetTagArray()
//-------------------------------------------------------------------------
{
  return m_TagArray;
}


//-------------------------------------------------------------------------
void mafDataSet::DeepCopy(mafDataSet *a)
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
void mafDataSet::ShallowCopy(mafDataSet *a)
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
bool mafDataSet::Equals(mafDataSet *o)
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
void mafDataSet::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "mafDataSet Contents:\n";

  Superclass::Print(os,indent);

  os << indent << "DataType: " << "\""<<m_DataType<<"\""<<std::endl;

  os << indent << "TimeStamp: "<< this->GetTimeStamp()<<std::endl;

  os << indent << "TagArray:\n";
  m_TagArray->Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
int mafDataSet::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->StoreText(m_URL,"URL");
  parent->StoreInteger(m_Id,"Id");
  parent->StoreText(m_DataType,"DataType");
  parent->StoreText("Crypting",m_Crypting?"true":"false");
  parent->StoreVectorN(m_Bounds->m_Bounds,6,"Bounds");
  parent->StoreObject(m_TagArray,"TagArray");

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafDataSet::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  mafString crypting;
  mafObject *obj;
  if (node->RestoreText(m_URL,"URL")==MAF_OK &&
      node->RestoreInteger(m_Id,"Id")==MAF_OK &&
      node->RestoreText(m_DataType,"DataType")==MAF_OK &&
      node->RestoreText(crypting,"Crypting")==MAF_OK &&
      node->RestoreVectorN(m_Bounds->m_Bounds,6,"Bounds")==MAF_OK &&
      node->RestoreObject(obj,"TagArray")==MAF_OK)
  {
    m_Crypting = (crypting=="true"||crypting=="True"||crypting=="TRUE")?true:false;
    mmaTagArray *tarray=mmaTagArray::SafeDownCast(obj);

    assert(tarray);
    mafDEL(m_TagArray);
    m_TagArray=mmaTagArray::SafeDownCast(tarray);

    assert(m_TagArray);
    m_TagArray->Register(this);

    return MAF_OK;
  }

  return MAF_ERROR;
}