/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurface
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



#include "albaVMESurface.h"
#include "albaVME.h"
#include "albaMatrixInterpolator.h"
#include "albaDataVector.h"
#include "albaDataPipeInterpolatorVTK.h"
#include "albaVMEItemVTK.h"
#include "albaAbsMatrixPipe.h"
#include "mmaMaterial.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMESurface)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMESurface::albaVMESurface()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaVMESurface::~albaVMESurface()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGeneric

  //cppDEL(m_MaterialButton);
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMESurface::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputSurface::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int albaVMESurface::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==ALBA_OK)
  {
    // force material allocation
    ((albaVMEOutputSurface *)GetOutput())->SetMaterial(GetMaterial());
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
int albaVMESurface::SetData(vtkPolyData *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  if(polydata)
  {
    if (polydata->GetPoints() && (polydata->GetVerts()==NULL || polydata->GetVerts()->GetNumberOfCells()==0) && \
      ((polydata->GetPolys()!=NULL && polydata->GetPolys()->GetNumberOfCells() > 0) || (polydata->GetStrips()!=NULL && polydata->GetStrips()->GetNumberOfCells() > 0)) && \
      (polydata->GetLines()==NULL || polydata->GetLines()->GetNumberOfCells() == 0))
    {
      int res = Superclass::SetData(polydata,t,mode);
      GetOutput()->Update();
      return res;
    }
  }
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
int albaVMESurface::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  if (polydata && polydata->GetPoints() && polydata->GetVerts()->GetNumberOfCells()==0 && \
     (polydata->GetPolys()->GetNumberOfCells() > 0 || polydata->GetStrips()->GetNumberOfCells() > 0) && \
      polydata->GetLines()->GetNumberOfCells() == 0)
  {
    return Superclass::SetData(data,t,mode);
  }
  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMESurface::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((albaVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//-------------------------------------------------------------------------
char** albaVMESurface::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMESurface.xpm"
  return albaVMESurface_xpm;
}
