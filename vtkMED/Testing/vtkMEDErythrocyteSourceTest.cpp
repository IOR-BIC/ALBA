/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDErythrocyteSourceTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-22 13:11:20 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkMEDErythrocyteSourceTest.h"
#include "vtkMEDErythrocyteSource.h"

#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkMAFSmartPointer.h"

//-----------------------------------------------------------
void vtkMEDErythrocyteSourceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{ 
  vtkMEDErythrocyteSource *source = vtkMEDErythrocyteSource::New();
  vtkDEL(source);
}
//-----------------------------------------------------------
void vtkMEDErythrocyteSourceTest::TestExecuteData() 
//-----------------------------------------------------------
{
  vtkMEDErythrocyteSource *source = vtkMEDErythrocyteSource::New();

  //source->ExecuteData(cube);
  source->Update();
  vtkMAFSmartPointer<vtkPolyData> poly = source->GetOutput();
  CPPUNIT_ASSERT(poly != NULL);

  double bounds[6];
  poly->GetBounds(bounds);
  CPPUNIT_ASSERT((bounds[0] - -0.19) < 0.001 && (bounds[1] - 0.19) < 0.001 && (bounds[2] - -0.19) < 0.001 && (bounds[3] - 0.19) < 0.001 \
    && (bounds[4] - 0.054) < 0.001 && (bounds[5] - 0.054) < 0.001);

  vtkDEL(source);
}