/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEMesh.cpp,v $
  Language:  C++
  Date:      $Date: 2007-04-04 16:38:14 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
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



#include "mafVMEMesh.h"
#include "mafVME.h"
#include "mafMatrixInterpolator.h"
#include "mafDataVector.h"
#include "mafVTKInterpolator.h"
#include "mafVMEItemVTK.h"
#include "mafAbsMatrixPipe.h"

#include "vtkDataSet.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEMesh)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEMesh::mafVMEMesh()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEMesh::~mafVMEMesh()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric

  //cppDEL(m_MaterialButton);
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEMesh::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputMesh::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEMesh::SetData(vtkUnstructuredGrid *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkUnstructuredGrid *unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(data);

  if(unstructuredGrid)
  {
    unstructuredGrid->Update();
    return Superclass::SetData(unstructuredGrid,t,mode);
  }
  else
  {
    mafErrorMacro("Trying to set the wrong type of data inside a VME Mesh :"<< (data?data->GetClassName():"NULL"));
    return MAF_ERROR;
  }
}
//-------------------------------------------------------------------------
int mafVMEMesh::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkUnstructuredGrid *unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(data);

  if (unstructuredGrid) 
  {
    unstructuredGrid->Update();
    return Superclass::SetData(data,t,mode);
  }
  else
  {
    mafErrorMacro("Trying to set the wrong type of data inside a VME Mesh :"<< (data?data->GetClassName():"NULL"));
    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
char** mafVMEMesh::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEFem.xpm"
  return mafVMEFem_xpm;
}
