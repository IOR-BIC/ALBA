/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrixVector.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:29:50 $
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

#endif
