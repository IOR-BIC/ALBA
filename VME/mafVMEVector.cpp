/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVector.cpp,v $
  Language:  C++
  Date:      $Date: 2007-04-04 13:52:47 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
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

#include "mafVMEVector.h"
#include "mmaMaterial.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include <sstream>
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEVector)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEVector::mafVMEVector()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEVector::~mafVMEVector()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}


//-------------------------------------------------------------------------
mafVMEOutput *mafVMEVector::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputPolyline::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
int mafVMEVector::InternalInitialize()
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
int mafVMEVector::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int mafVMEVector::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);
  
  if (polydata)
    polydata->Update();

  // check this is a ploydata containing only lines
  if (polydata && polydata->GetPolys()->GetNumberOfCells()==0 && \
      polydata->GetStrips()->GetNumberOfCells()==0 && \
      polydata->GetVerts()->GetNumberOfCells()==0)
  {
    return Superclass::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of fata inside a VME Polyline :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEVector::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputPolyline *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
