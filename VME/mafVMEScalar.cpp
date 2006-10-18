/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2006-10-18 13:09:02 $
  Version:   $Revision: 1.2 $
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


#include "mafVMEScalar.h"
#include "mmgGui.h"
#include "mafVME.h"
#include "mafVMEOutputScalar.h"

#include "mafScalarInterpolator.h"
#include "mafDataVector.h"
#include "mafVMEItemScalar.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEScalar::mafVMEScalar()
//-------------------------------------------------------------------------
{
  m_DataVector = mafDataVector::New();
  m_DataVector->SetItemTypeName(mafVMEItemScalar::GetStaticTypeName());  
  m_DataVector->SetListener(this);
  SetDataPipe(mafScalarInterpolator::New()); // interpolator data pipe
}

//-------------------------------------------------------------------------
mafVMEScalar::~mafVMEScalar()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEScalar::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputScalar::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEScalar::SetData(vnl_matrix<double> &data, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemScalar> item;
  item->SetData(data);
  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  return MAF_OK;
}

//-------------------------------------------------------------------------
void mafVMEScalar::SetScalarIdForXCoordinate(int id)
//-------------------------------------------------------------------------
{
  m_XID = id;
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetScalarIdForYCoordinate(int id)
//-------------------------------------------------------------------------
{
  m_YID = id;
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetScalarIdForZCoordinate(int id)
//-------------------------------------------------------------------------
{
  m_ZID = id;
}
//-------------------------------------------------------------------------
char** mafVMEScalar::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEScalar.xpm"
  return mafVMEScalar_xpm;
}
