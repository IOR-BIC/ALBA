/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolume
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

#include "albaVMEVolume.h"
#include "albaMatrixInterpolator.h"
#include "albaDataVector.h"
#include "albaDataPipeInterpolatorVTK.h"
#include "albaVMEItemVTK.h"
#include "albaAbsMatrixPipe.h"
#include "mmaVolumeMaterial.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkLookupTable.h"
//-------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaVMEVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEVolume::albaVMEVolume()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaVMEVolume::~albaVMEVolume()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGeneric
}

//-------------------------------------------------------------------------
int albaVMEVolume::InternalInitialize()
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
albaVMEOutput *albaVMEVolume::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
mmaVolumeMaterial *albaVMEVolume::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = (mmaVolumeMaterial *)GetAttribute("VolumeMaterialAttributes");
  if (material == NULL)
  {
    material = mmaVolumeMaterial::New();

    // Paolo Q.: commented lines below to avoid loading data during the msf loading on startup
    /*if(GetOutput() && GetOutput()->GetVTKData())
    {
      GetOutput()->GetVTKData()->Update();
      double sr[2];
      GetOutput()->GetVTKData()->GetScalarRange(sr);
      material->m_ColorLut->SetTableRange(sr);
      material->m_ColorLut->SetRange(sr);
      material->UpdateFromTables();
    }*/
    
    SetAttribute("VolumeMaterialAttributes", material);
    if (m_Output)
    {
      ((albaVMEOutputVolume *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}

//-------------------------------------------------------------------------
int albaVMEVolume::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkImageData")||data->IsA("vtkRectilinearGrid")||data->IsA("vtkUnstructuredGrid"))
  {
    return Superclass::SetData(data,t,mode);
  }
  
  albaErrorMacro("Trying to set the wrong type of data inside a "<<GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
char** albaVMEVolume::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEVolume.xpm"
  return albaVMEVolume_xpm;
}
