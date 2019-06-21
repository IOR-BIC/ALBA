/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEScalar
 Authors: Paolo Quadrani
 
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


#include "albaVMEScalar.h"
#include "albaGUI.h"

#include "mmuTimeSet.h"
#include "albaStorageElement.h"
#include "albaVME.h"
#include "albaVMEOutputScalar.h"

//#include "albaDataPipeCustom.h"
#include "albaScalarVector.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEScalar::albaVMEScalar()
//-------------------------------------------------------------------------
{
  m_ScalarVector = new albaScalarVector();
  SetDataPipe(NULL);
}

//-------------------------------------------------------------------------
albaVMEScalar::~albaVMEScalar()
//-------------------------------------------------------------------------
{
  cppDEL(m_ScalarVector);
}
//-------------------------------------------------------------------------
int albaVMEScalar::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a) == ALBA_OK)
  {
    albaVMEScalar *scalar = albaVMEScalar::SafeDownCast(a);
    if (scalar->GetScalarVector())
    {
      if(m_ScalarVector == NULL)
        m_ScalarVector = scalar->GetScalarVector()->NewInstance(); // create a new instance of the same type
      m_ScalarVector->DeepCopy(scalar->GetScalarVector()); // copy data
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
bool albaVMEScalar::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    albaVMEScalar *scalar = albaVMEScalar::SafeDownCast(vme);
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
bool albaVMEScalar::IsAnimated()
//-------------------------------------------------------------------------
{
  bool anim = Superclass::IsAnimated();
  return anim || (m_ScalarVector->GetNumberOfItems() > 1);
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMEScalar::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output == NULL)
  {
    SetOutput(albaVMEOutputScalar::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int albaVMEScalar::SetData(double data, albaTimeStamp t)
//-------------------------------------------------------------------------
{
  m_ScalarVector->SetScalar(t, data);

  return ALBA_OK;
}

//-------------------------------------------------------------------------
void albaVMEScalar::GetDataTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  if (m_ScalarVector)
  {
    m_ScalarVector->GetTimeStamps(kframes);
  }
}

//-------------------------------------------------------------------------
void albaVMEScalar::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();

  std::vector<albaTimeStamp> datatimestamps;
  std::vector<albaTimeStamp> matrixtimestamps;

  if (m_ScalarVector)
  {
    m_ScalarVector->GetTimeStamps(datatimestamps);
  }
  Superclass::GetLocalTimeStamps(matrixtimestamps);

  mmuTimeSet::Merge(datatimestamps,matrixtimestamps,kframes);
}

//-------------------------------------------------------------------------
void albaVMEScalar::GetLocalTimeBounds(albaTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  Superclass::GetLocalTimeBounds(tbounds);

  albaTimeStamp tmp[2];
  m_ScalarVector->GetTimeBounds(tmp);

  tmp[0] = tmp[0] < 0 ? 0 : tmp[0];
  tmp[1] = tmp[1] < 0 ? 0 : tmp[1];

  if (tmp[0]<tbounds[0]||tbounds[0]<0)
    tbounds[0]=tmp[0];

  if (tmp[1]>tbounds[1]||tbounds[1]<0)
    tbounds[1]=tmp[1];
}

//-----------------------------------------------------------------------
int albaVMEScalar::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent) == ALBA_OK)
  {
    if (m_ScalarVector)
    {
      albaStorageElement *data_vector = parent->AppendChild("ScalarVector");
      if(m_ScalarVector->Store(data_vector) == ALBA_ERROR)
        return ALBA_ERROR;
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMEScalar::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == ALBA_OK)
  {
    // restore Data Vector
    if (m_ScalarVector)
    {
      albaStorageElement *data_vector = node->FindNestedElement("ScalarVector");
      if (data_vector)
      {
        return m_ScalarVector->Restore(data_vector);
      }
    }
    return ALBA_OK;
  }

  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
void albaVMEScalar::InternalPreUpdate()
//-----------------------------------------------------------------------
{
#ifdef ALBA_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
}
//-----------------------------------------------------------------------
void albaVMEScalar::SetTimeStamp(albaTimeStamp t)
//-----------------------------------------------------------------------
{
  t = t < 0 ? 0 : t;
  bool update_vtk_data = t != m_CurrentTime;
  if (update_vtk_data)
  {
    Superclass::SetTimeStamp(t);

#ifdef ALBA_USE_VTK
    GetScalarOutput()->UpdateVTKRepresentation();
#endif
  }
}
//-----------------------------------------------------------------------
void albaVMEScalar::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);
}
//-------------------------------------------------------------------------
char** albaVMEScalar::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEScalar.xpm"
  return albaVMEScalar_xpm;
}
