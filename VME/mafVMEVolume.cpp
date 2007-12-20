/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVolume.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-20 14:00:15 $
  Version:   $Revision: 1.4 $
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

#include "mafVMEVolume.h"
#include "mafMatrixInterpolator.h"
#include "mafDataVector.h"
#include "mafVTKInterpolator.h"
#include "mafVMEItemVTK.h"
#include "mafAbsMatrixPipe.h"
#include "mmaVolumeMaterial.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkLookupTable.h"
//-------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafVMEVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEVolume::mafVMEVolume()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEVolume::~mafVMEVolume()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}

//-------------------------------------------------------------------------
int mafVMEVolume::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEVolume::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
mmaVolumeMaterial *mafVMEVolume::GetMaterial()
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
      ((mafVMEOutputVolume *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}

//-------------------------------------------------------------------------
int mafVMEVolume::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkImageData")||data->IsA("vtkRectilinearGrid")||data->IsA("vtkUnstructuredGrid"))
  {
    return Superclass::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of data inside a "<<GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
char** mafVMEVolume::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEVolume.xpm"
  return mafVMEVolume_xpm;
}
