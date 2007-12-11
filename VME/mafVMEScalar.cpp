/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:23:37 $
  Version:   $Revision: 1.8 $
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


#include "mafVMEScalar.h"
#include "mmgGui.h"

#include "mafStorageElement.h"
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
int mafVMEScalar::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a) == MAF_OK)
  {
    //mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(a);
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEScalar::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    //mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(vme);
    ret = true;
  }
  return ret;
}
//-------------------------------------------------------------------------
mafVMEOutput *mafVMEScalar::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output == NULL)
  {
    SetOutput(mafVMEOutputScalar::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEScalar::SetData(double data, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemScalar> item;
  item->SetData(data);
  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  return MAF_OK;
}
//-----------------------------------------------------------------------
int mafVMEScalar::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent) == MAF_OK)
  {
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEScalar::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    return MAF_OK;
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mafVMEScalar::InternalPreUpdate()
//-----------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
}
//-----------------------------------------------------------------------
void mafVMEScalar::SetTimeStamp(mafTimeStamp t)
//-----------------------------------------------------------------------
{
  t = t < 0 ? 0 : t;
  bool update_vtk_data = t != m_CurrentTime;
  if (update_vtk_data)
  {
    Superclass::SetTimeStamp(t);

#ifdef MAF_USE_VTK
    GetScalarOutput()->UpdateVTKRepresentation();
#endif
  }
}
//-----------------------------------------------------------------------
void mafVMEScalar::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}
//-------------------------------------------------------------------------
char** mafVMEScalar::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEScalar.xpm"
  return mafVMEScalar_xpm;
}
