/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVector
 Authors: Roberto Mucci
 
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

#include "albaVMEVector.h"
#include "mmaMaterial.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include <sstream>
//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEVector)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEVector::albaVMEVector()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
albaVMEVector::~albaVMEVector()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGeneric
}
//-------------------------------------------------------------------------
double albaVMEVector::GetModule()
//-------------------------------------------------------------------------
{
  this->Update();
  return GetPolylineOutput()->CalculateLength();
}
//-------------------------------------------------------------------------
int albaVMEVector::SetData(vtkPolyData *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return this->SetData((vtkDataSet*)data,t,mode);
}
//-------------------------------------------------------------------------
int albaVMEVector::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  // check this is a polydata containing only lines with 2 points (vector)
  if (polydata && polydata->GetPolys()->GetNumberOfCells()==0 && \
      polydata->GetStrips()->GetNumberOfCells()==0 && \
      polydata->GetVerts()->GetNumberOfCells()==0 && \
      polydata->GetPoints()->GetNumberOfPoints() == 2)
  {
    return albaVMEGeneric::SetData(data,t,mode);
  }
  
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
char** albaVMEVector::GetIcon() 
//-------------------------------------------------------------------------
{
#include "albaVMEVector.xpm"
  return albaVMEVector_xpm;
}
