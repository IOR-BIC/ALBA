/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAErythrocyteSourceTest
 Authors: Roberto Mucci
 
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

#include "vtkALBAErythrocyteSourceTest.h"
#include "vtkALBAErythrocyteSource.h"

#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkALBASmartPointer.h"

//-----------------------------------------------------------
void vtkALBAErythrocyteSourceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{ 
  vtkALBAErythrocyteSource *source = vtkALBAErythrocyteSource::New();
  vtkDEL(source);
}
//-----------------------------------------------------------
void vtkALBAErythrocyteSourceTest::TestExecuteData() 
//-----------------------------------------------------------
{
  vtkALBAErythrocyteSource *source = vtkALBAErythrocyteSource::New();

  source->Update();
  vtkALBASmartPointer<vtkPolyData> poly = source->GetOutput();
  CPPUNIT_ASSERT(poly != NULL);

  double bounds[6];
  poly->GetCellsBounds(bounds);
	CPPUNIT_ASSERT((bounds[0] - -0.19) < 0.001 && (bounds[1] - 0.19) < 0.001 && (bounds[2] - -0.19) < 0.001 && (bounds[3] - 0.19) < 0.001 \
    && (bounds[4] - 0.054) < 0.001 && (bounds[5] - 0.054) < 0.001);

  vtkDEL(source);
}