/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-24 12:23:06 $
  Version:   $Revision: 1.9 $
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

#include "mmuTimeSet.h"
#include "mafStorageElement.h"
#include "mafVME.h"
#include "mafVMEOutputScalar.h"

//#include "mafDataPipeCustom.h"
#include "mafScalarVector.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEScalar::mafVMEScalar()
//-------------------------------------------------------------------------
{
  m_ScalarVector = new mafScalarVector();
  SetDataPipe(NULL);
}

//-------------------------------------------------------------------------
mafVMEScalar::~mafVMEScalar()
//-------------------------------------------------------------------------
{
  cppDEL(m_ScalarVector);
}
//-------------------------------------------------------------------------
int mafVMEScalar::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a) == MAF_OK)
  {
    mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(a);
    if (scalar->GetScalarVector())
    {
      if(m_ScalarVector == NULL)
        m_ScalarVector = scalar->GetScalarVector()->NewInstance(); // create a new instance of the same type
      m_ScalarVector->DeepCopy(scalar->GetScalarVector()); // copy data
    }
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
    mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(vme);
    if (m_ScalarVector)
    {
      ret = m_ScalarVector->Equals(scalar->GetScalarVector());
    }
    else
      ret = true;
  }
  return ret;
}

//-------------------------------------------------------------------------
bool mafVMEScalar::IsAnimated()
//-------------------------------------------------------------------------
{
  bool anim = Superclass::IsAnimated();
  return anim || (m_ScalarVector->GetNumberOfItems() > 1);
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
  m_ScalarVector->SetScalar(t, data);

  return MAF_OK;
}

//-------------------------------------------------------------------------
void mafVMEScalar::GetDataTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  if (m_ScalarVector)
  {
    m_ScalarVector->GetTimeStamps(kframes);
  }
}

//-------------------------------------------------------------------------
void mafVMEScalar::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();

  std::vector<mafTimeStamp> datatimestamps;
  std::vector<mafTimeStamp> matrixtimestamps;

  if (m_ScalarVector)
  {
    m_ScalarVector->GetTimeStamps(datatimestamps);
  }
  Superclass::GetLocalTimeStamps(matrixtimestamps);

  mmuTimeSet::Merge(datatimestamps,matrixtimestamps,kframes);
}

//-------------------------------------------------------------------------
void mafVMEScalar::GetLocalTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  Superclass::GetLocalTimeBounds(tbounds);

  mafTimeStamp tmp[2];
  m_ScalarVector->GetTimeBounds(tmp);

  tmp[0] = tmp[0] < 0 ? 0 : tmp[0];
  tmp[1] = tmp[1] < 0 ? 0 : tmp[1];

  if (tmp[0]<tbounds[0]||tbounds[0]<0)
    tbounds[0]=tmp[0];

  if (tmp[1]>tbounds[1]||tbounds[1]<0)
    tbounds[1]=tmp[1];
}

//-----------------------------------------------------------------------
int mafVMEScalar::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent) == MAF_OK)
  {
    if (m_ScalarVector)
    {
      mafStorageElement *data_vector = parent->AppendChild("ScalarVector");
      if(m_ScalarVector->Store(data_vector) == MAF_ERROR)
        return MAF_ERROR;
    }
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
    // restore Data Vector
    if (m_ScalarVector)
    {
      mafStorageElement *data_vector = node->FindNestedElement("ScalarVector");
      if (data_vector)
      {
        return m_ScalarVector->Restore(data_vector);
      }
    }
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
