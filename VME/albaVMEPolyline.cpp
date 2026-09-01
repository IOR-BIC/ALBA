/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolyline
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

#include "albaVMEPolyline.h"
#include "mmaMaterial.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include <sstream>
//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEPolyline)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEPolyline::albaVMEPolyline()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaVMEPolyline::~albaVMEPolyline()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGeneric
}


//-------------------------------------------------------------------------
albaVMEOutput *albaVMEPolyline::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputPolyline::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
char** albaVMEPolyline::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEPolyline.xpm"
	return albaVMEPolyline_xpm;
}
//-------------------------------------------------------------------------
int albaVMEPolyline::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==ALBA_OK)
  {
    // force material allocation
    GetMaterial();
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
int albaVMEPolyline::SetData(vtkPolyData *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int albaVMEPolyline::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);
  
  // check this is a polydata containing only lines
  if (polydata && polydata->GetPolys()->GetNumberOfCells()==0 && \
      polydata->GetStrips()->GetNumberOfCells()==0 && \
      polydata->GetVerts()->GetNumberOfCells()==0)
  {
    return Superclass::SetData(data,t,mode);
  }
  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMEPolyline::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((albaVMEOutputPolyline *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
