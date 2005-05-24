/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-24 14:37:33 $
  Version:   $Revision: 1.7 $
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



#include "mafVMESurface.h"
#include "mafMatrixInterpolator.h"
#include "mafDataVector.h"
#include "mafVTKInterpolator.h"
#include "mafVMEItemVTK.h"
#include "mafAbsMatrixPipe.h"
#include "mmaMaterial.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"

#include "mafPipeSurface.h" //SIL. 21-4-2005: 
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESurface)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMESurface::mafVMESurface()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMESurface::~mafVMESurface()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}


//-------------------------------------------------------------------------
mafVMEOutput *mafVMESurface::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputSurface::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMESurface::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int mafVMESurface::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
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
  
  mafErrorMacro("Trying to set the wrong type of fata inside a VME Image :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMESurface::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
  }
  return material;
}
//-------------------------------------------------------------------------
char** mafVMESurface::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}