/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrixVector.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:16:32 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMatrixVector_cpp
#define __mafMatrixVector_cpp

#include "mafMatrixVector.h"
#include "mafIndent.h"

//-----------------------------------------------------------------------
mafCxxTypeMacro(mafMatrixVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
mafMatrixVector::mafMatrixVector()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
mafMatrixVector::~mafMatrixVector()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
void mafMatrixVector::GetKeyMatrixVector(std::vector<mafMatrix *> &mvector)
//-----------------------------------------------------------------------
{
  mvector.clear();
  mvector.resize(GetNumberOfMatrices());
  int i=0;
  for (Iterator it=Begin();it!=End();it++,i++)
  {
    mvector[i]=it->second;
  }
}
//-----------------------------------------------------------------------
void mafMatrixVector::SetMatrix(const mafMatrix &mat)
//-----------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> tmp;
  *tmp=mat;
  SetMatrix(tmp);
}
//-----------------------------------------------------------------------
void mafMatrixVector::AppendKeyMatrix(const mafMatrix &m)
//-----------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> tmp;
  *tmp=m;
  AppendKeyMatrix(tmp);
}

//-----------------------------------------------------------------------
int mafMatrixVector::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
  parent->SetAttribute("NumberOfItems",mafString(GetNumberOfItems()));
  for (Iterator it=Begin();it!=End();it++)
  {
    mafMatrix *mat=it->second;
    if (parent->StoreMatrix("Matrix",mat)!=MAF_OK)
      return MAF_ERROR;
  }
  return MAF_OK;
}
//-----------------------------------------------------------------------
int mafMatrixVector::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  mafID num_items;
  if (node->GetAttributeAsInteger("NumberOfItems",num_items))
  {
    mafStorageElement::ChildrenVector vector_elements;
    node->GetNestedElementsByName("Matrix",vector_elements);

    assert(vector_elements.size()!=num_items);

    if (vector_elements.size()!=num_items)
      mafWarningMacro("Restore I/O error: found wrong number of matrices in restored MatrixVector.");

    for (int i=0;i<vector_elements.size();i++)
    {
      mafSmartPointer<mafMatrix> mat;
      if (vector_elements[i]->RestoreMatrix(mat)!=MAF_OK)
        return MAF_ERROR;
    }
    
    return MAF_OK;
  }

  return MAF_ERROR;
}
#endif
