/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeVectorField.cpp,v $ 
  Language: C++ 
  Date: $Date: 2011-06-16 09:18:40 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaPipeVectorField.h"

#include "albaSceneNode.h"
#include "albaVME.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#include "albaDbg.h"

//----------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaPipeVectorField);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeVectorField::albaPipeVectorField() : albaPipe()
//----------------------------------------------------------------------------
{  
  m_VectorFieldIndex = 0;  
  m_ScalarFieldIndex = 0;  
  m_BCreateVTKPipeAlways = false;
}

//----------------------------------------------------------------------------
void albaPipeVectorField::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); 
  _VERIFY_RET(m_Vme != NULL);    

  ComputeDefaultParameters();
  
  if ((m_ScalarFieldIndex = GetScalarFieldIndex()) < 0)          
    m_ScalarFieldIndex = 0; //no default vector field, use the first one  

  int nVectors = GetNumberOfVectors() > 0;
  if (nVectors > 0)
  {
    //detect the default one vector field
    if ((m_VectorFieldIndex = GetVectorFieldIndex("velocity")) < 0)          
        m_VectorFieldIndex = 0; //no default vector field, use the first one         
  }

  if (nVectors > 0 || m_BCreateVTKPipeAlways)
  {
    //now let us create VTK pipe
    CreateVTKPipe();  
    UpdateVTKPipe();  //and set visual properties as needed  
  }
}

//------------------------------------------------------------------------
/*virtual*/ void albaPipeVectorField::ComputeDefaultParameters()
//------------------------------------------------------------------------
{
  //Default radius, etc. should be calculated in this method, 
  //i.e., inherited classes should always override this method. 
  //The default implementation is to update VME
  
  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
}

//------------------------------------------------------------------------
int albaPipeVectorField::GetFieldIndex(const char* szName, bool bVectors)
//------------------------------------------------------------------------
{
  //Returns the index of specified field (scalar or vectors depending on
  //bVectors parameter). If szName is NULL, the index of currently active
  //(scalar or vector) field is returned.
  //The routine returns -1, if it cannot find appropriate field.
  
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return -1;

  int nQueryComps;
  vtkDataArray* pQueryDA;

  if (!bVectors)
  {
    nQueryComps = 1;
    pQueryDA = pd->GetScalars(szName);
  }
  else
  {
    nQueryComps = 3;
    pQueryDA = pd->GetVectors(szName);
  }
   
  if (pQueryDA == NULL)
    return -1;

  int nIndex = 0;
  int nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    if (da == pQueryDA)
      break;  //we found it
    
    if (da->GetNumberOfComponents() == nQueryComps)
      nIndex++; //this is correct array
  }
  
  return nIndex;
}

//------------------------------------------------------------------------
int albaPipeVectorField::GetNumberOfFields(bool bVectors)
//------------------------------------------------------------------------
{
  //Returns the number of available scalars/vectors.
  
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return -1;

  int nQueryComps = bVectors ? 3 : 1;
  int nRet = 0, nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    if (da->GetNumberOfComponents() == nQueryComps)
      nRet++; //this is correct array
  }

  return nRet;
}

//------------------------------------------------------------------------
const char* albaPipeVectorField::GetFieldName(int nIndex, bool bVectors)
//------------------------------------------------------------------------
{
  //Returns the name of field (scalar or vectors depending on
  //bVectors parameter) at the specified index. 
  //The routine returns NULL, if it cannot find appropriate field.
  
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return NULL;

  int nQueryComps = bVectors ? 3 : 1;
  int nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    if (da->GetNumberOfComponents() == nQueryComps)
    {
      if (nIndex == 0)
        return da->GetName();
      
      nIndex--;
    }
  }

  return NULL;
}

//------------------------------------------------------------------------
void albaPipeVectorField::PopulateCombo(wxComboBox* combo, bool bVectors)
//------------------------------------------------------------------------
{
  //Populates the combo box by names of scalar/vector fields
  
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd != NULL)
  {
    int nQueryComps = bVectors ? 3 : 1;
    int nCount = pd->GetNumberOfArrays();
    for (int i = 0; i < nCount; i++)
    {
      vtkDataArray* da = pd->GetArray(i);
      if (da->GetNumberOfComponents() == nQueryComps){
        combo->Append(da->GetName());
      }
    }
  }
}