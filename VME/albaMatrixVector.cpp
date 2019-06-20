/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixVector
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


#include "albaMatrixVector.h"
#include "albaIndent.h"

//-----------------------------------------------------------------------
albaCxxTypeMacro(albaMatrixVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
albaMatrixVector::albaMatrixVector()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
albaMatrixVector::~albaMatrixVector()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
void albaMatrixVector::GetKeyMatrixVector(std::vector<albaMatrix *> &mvector)
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
void albaMatrixVector::SetMatrix(const albaMatrix &mat)
//-----------------------------------------------------------------------
{
  albaSmartPointer<albaMatrix> tmp;
  *tmp=mat;
  SetMatrix(tmp);
}
//-----------------------------------------------------------------------
void albaMatrixVector::AppendKeyMatrix(const albaMatrix &m)
//-----------------------------------------------------------------------
{
  albaSmartPointer<albaMatrix> tmp;
  *tmp=m;
  AppendKeyMatrix(tmp);
}

//-----------------------------------------------------------------------
int albaMatrixVector::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{
  parent->SetAttribute("NumberOfItems",albaString(GetNumberOfItems()));
  for (Iterator it=Begin();it!=End();it++)
  {
    albaMatrix *mat=it->second;
    if (parent->StoreMatrix("Matrix",mat)!=ALBA_OK)
      return ALBA_ERROR;
  }
  return ALBA_OK;
}
//-----------------------------------------------------------------------
int albaMatrixVector::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  albaID num_items;
  if (node->GetAttributeAsInteger("NumberOfItems",num_items))
  {
    albaStorageElement::ChildrenVector vector_elements;
    node->GetNestedElementsByName("Matrix",vector_elements);

    assert(vector_elements.size()==num_items);

    if (vector_elements.size()!=num_items)
      albaWarningMacro("Restore I/O error: found wrong number of matrices in restored MatrixVector.");

    for (int i=0;i<vector_elements.size();i++)
    {
      albaSmartPointer<albaMatrix> mat;
      if (vector_elements[i]->RestoreMatrix(mat)!=ALBA_OK)
        return ALBA_ERROR;
      AppendItem(mat);
    }
    
    return ALBA_OK;
  }

  return ALBA_ERROR;
}
