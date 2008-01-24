/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafScalarVector.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-24 12:20:27 $
  Version:   $Revision: 1.1 $
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


#include "mafScalarVector.h"
#include "mafIndent.h"

//-----------------------------------------------------------------------
mafCxxTypeMacro(mafScalarVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
mafScalarVector::mafScalarVector()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
mafScalarVector::~mafScalarVector()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
void mafScalarVector::GetScalarVector(std::vector<double> &svector)
//-----------------------------------------------------------------------
{
  svector.clear();
  svector.resize(GetNumberOfScalars());
  int i = 0;
  for (Iterator it = BeginScalarVector(); it != EndScalarVector(); it++, i++)
  {
    svector[i] = it->second;
  }
}
//-----------------------------------------------------------------------
int mafScalarVector::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
  parent->SetAttribute("NumberOfItems",mafString(GetNumberOfItems()));
  for (Iterator it = BeginScalarVector(); it != EndScalarVector(); it++)
  {
    double sca[2];
    sca[0] = it->first;
    sca[1] = it->second;
    if (parent->StoreVectorN("Scalar", sca , 2) != MAF_OK)
      return MAF_ERROR;
  }
  return MAF_OK;
}
//-----------------------------------------------------------------------
int mafScalarVector::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  mafID num_items;
  if (node->GetAttributeAsInteger("NumberOfItems",num_items))
  {
    mafStorageElement::ChildrenVector vector_elements;
    node->GetNestedElementsByName("Scalar", vector_elements);

    assert(vector_elements.size() == num_items);

    if (vector_elements.size() != num_items)
      mafWarningMacro("Restore I/O error: found wrong number of scalar value in restored ScalarVector.");

    for (int i = 0; i < vector_elements.size(); i++)
    {
      double sca[2];
      if (vector_elements[i]->RestoreVectorN(sca, 2) != MAF_OK)
        return MAF_ERROR;
      AppendItem(sca[0], sca[1]);
    }
    
    return MAF_OK;
  }

  return MAF_ERROR;
}
