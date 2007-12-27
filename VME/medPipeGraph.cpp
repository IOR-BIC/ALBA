/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-27 13:03:45 $
  Version:   $Revision: 1.15 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-timeArray error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medPipeGraph.h"
#include "mafDecl.h"
#include "mafVMEScalar.h"
#include "mafVMEOutputScalar.h"

#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeGraph);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeGraph::medPipeGraph()
:mafPipe()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
medPipeGraph::~medPipeGraph()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
vtkRectilinearGrid* medPipeGraph::GetData(mafNode *vme)
//----------------------------------------------------------------------------
{

  std::vector<mafTimeStamp> timeVector;  
  mafSmartPointer<mafVMEScalar> emgPlot = mafVMEScalar::SafeDownCast(vme);

  emgPlot->Update();
  emgPlot->GetTimeStamps(timeVector);

  vtkMAFSmartPointer<vtkDoubleArray> scalar;
  vtkMAFSmartPointer<vtkDoubleArray> timeArray;

  mafTimeStamp t;
  for (t = 0; t < timeVector.size(); t++)
  {
    timeArray->InsertNextValue(timeVector[t]);
  }  

  int x_dim = timeVector.size(); 
  int counter_array = 0;
  int counter = 0;

  for (t = 0; t < timeVector.size(); t++)
  { 
    emgPlot->SetTimeStamp(timeVector[t]);
    double scalar_data = emgPlot->GetScalarOutput()->GetScalarData();
    scalar->InsertValue(counter,scalar_data);
    counter++;
  }
  
  vtkRectilinearGrid *rectGrid = vtkRectilinearGrid::New();
  rectGrid->SetDimensions(x_dim, 1, 1);
  rectGrid->SetXCoordinates(timeArray); 
  rectGrid->GetPointData()->SetScalars(scalar);
  timeVector.clear();

  return rectGrid;
}

