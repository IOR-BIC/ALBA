/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrixVector.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-18 19:55:57 $
  Version:   $Revision: 1.3 $
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

    assert(vector_elements.size()==num_items);

    if (vector_elements.size()!=num_items)
      mafWarningMacro("Restore I/O error: found wrong number of matrices in restored MatrixVector.");

    for (int i=0;i<vector_elements.size();i++)
    {
      mafSmartPointer<mafMatrix> mat;
      if (vector_elements[i]->RestoreMatrix(mat)!=MAF_OK)
        return MAF_ERROR;
      AppendItem(mat);
    }
    
    return MAF_OK;
  }

  return MAF_ERROR;
}
