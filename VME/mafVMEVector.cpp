/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVector.cpp,v $
  Language:  C++
  Date:      $Date: 2009-02-13 12:44:29 $
  Version:   $Revision: 1.3.4.1 $
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
double mafVMEVector::GetModule()
//-------------------------------------------------------------------------
{
  return GetPolylineOutput()->CalculateLength();
}
//-------------------------------------------------------------------------
int mafVMEVector::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return this->SetData((vtkDataSet*)data,t,mode);
}
//-------------------------------------------------------------------------
int mafVMEVector::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);
  
  if (polydata)
    polydata->Update();

  // check this is a polydata containing only lines with 2 points (vector)
  if (polydata && polydata->GetPolys()->GetNumberOfCells()==0 && \
      polydata->GetStrips()->GetNumberOfCells()==0 && \
      polydata->GetVerts()->GetNumberOfCells()==0 && \
      polydata->GetPoints()->GetNumberOfPoints() == 2)
  {
    return mafVMEGeneric::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of fata inside a VME Vector :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
char** mafVMEVector::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMEVector.xpm"
  return mafVMEVector_xpm;
}
