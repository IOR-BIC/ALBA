/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeTensorField.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-07-02 08:58:02 $ 
  Version: $Revision: 1.1.2.2 $ 
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
#include "albaPipeTensorField.h"

#include "albaSceneNode.h"
#include "albaVME.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#include "albaDbg.h"

//----------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaPipeTensorField);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeTensorField::albaPipeTensorField() : albaPipe()
//----------------------------------------------------------------------------
{  
  m_TensorFieldIndex = 0;  
  m_ScalarFieldIndex = 0;  
  m_BCreateVTKPipeAlways = false;
}

//----------------------------------------------------------------------------
void albaPipeTensorField::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); 
  _VERIFY_RET(m_Vme != NULL);    

  ComputeDefaultParameters();
  
  if ((m_ScalarFieldIndex = GetScalarFieldIndex()) < 0)          
    m_ScalarFieldIndex = 0; //no default tensor field, use the first one  

  int nTensors = GetNumberOfTensors() > 0;
  if (nTensors > 0)
  {
    //detect the default one tensor field
    if ((m_TensorFieldIndex = GetTensorFieldIndex("velocity")) < 0)          
        m_TensorFieldIndex = 0; //no default tensor field, use the first one         
  }

  if (nTensors > 0 || m_BCreateVTKPipeAlways)
  {
    //now let us create VTK pipe
    CreateVTKPipe();  
    UpdateVTKPipe();  //and set visual properties as needed  
  }
}

//------------------------------------------------------------------------
//Default radius, etc. should be calculated in this method, 
//i.e., inherited classes should always override this method. 
//The default implementation is to update VME
/*virtual*/ void albaPipeTensorField::ComputeDefaultParameters()
//------------------------------------------------------------------------
{
  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
}

//------------------------------------------------------------------------
//Returns the index of specified field (scalar or tensors depending on
//bTensors parameter). If szName is NULL, the index of currently active
//(scalar or tensor) field is returned.
//The routine returns -1, if it cannot find appropriate field.
int albaPipeTensorField::GetFieldIndex(const char* szName, bool bTensors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return -1;

  vtkDataArray* pQueryDA = bTensors ? pd->GetTensors(szName) : pd->GetScalars(szName);     
  if (pQueryDA == NULL)
    return -1;

  int nIndex = 0;
  int nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    if (da == pQueryDA)
      break;  //we found it
    
    int nComps = da->GetNumberOfComponents();
    if ((!bTensors && nComps == 1) || (bTensors && nComps > 3))
      nIndex++; //this is correct array
  }
  
  return nIndex;
}

//------------------------------------------------------------------------
//Returns the number of available scalars/tensors.
int albaPipeTensorField::GetNumberOfFields(bool bTensors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return -1;
  
  int nRet = 0, nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    int nComps = da->GetNumberOfComponents();
    if ((!bTensors && nComps == 1) || (bTensors && nComps > 3))
      nRet++; //this is correct array
  }

  return nRet;
}

//------------------------------------------------------------------------
//Returns the name of field (scalar or tensors depending on
//bTensors parameter) at the specified index. 
//The routine returns NULL, if it cannot find appropriate field.
const char* albaPipeTensorField::GetFieldName(int nIndex, bool bTensors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return NULL;
  
  int nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    int nComps = da->GetNumberOfComponents();
    if ((!bTensors && nComps == 1) || (bTensors && nComps > 3))      
    {
      if (nIndex == 0)
        return da->GetName();
      
      nIndex--;
    }
  }

  return NULL;
}

//------------------------------------------------------------------------
//Populates the combo box by names of scalar/tensor fields
void albaPipeTensorField::PopulateCombo(wxComboBox* combo, bool bTensors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd != NULL)
  {    
    int nCount = pd->GetNumberOfArrays();
    for (int i = 0; i < nCount; i++)
    {
      vtkDataArray* da = pd->GetArray(i);
      int nComps = da->GetNumberOfComponents();
      if ((!bTensors && nComps == 1) || (bTensors && nComps > 3)){
        combo->Append(da->GetName());
      }
    }
  }
}