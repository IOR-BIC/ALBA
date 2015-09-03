/*=========================================================================

 Program: MAF2
 Module: mafGUILandmarkTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafGUILandmarkTest.h"
#include "mafGUILandmark.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMESurface.h"
#include "vtkDataSet.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//---------------------------------------------------------
void mafGUILandmarkTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  //Create surface to be  clipped
  mafVMESurfaceParametric *surfaceParametric;
  mafNEW(surfaceParametric);
  surfaceParametric->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_SPHERE);
  surfaceParametric->SetSphereRadius(5.);
  surfaceParametric->Update();
  surfaceParametric->GetOutput()->GetVTKData()->Update();

  mafVMESurface *surface;
  mafNEW(surface);
  surface->SetData((vtkPolyData*)surfaceParametric->GetOutput()->GetVTKData(),0.0);


  mafGUILandmark *gui = new mafGUILandmark(surface, NULL, true);
  CPPUNIT_ASSERT(gui != NULL);

  cppDEL(gui);
  mafDEL(surface);
  mafDEL(surfaceParametric);
}

//---------------------------------------------------------
void mafGUILandmarkTest::TestVmeAccept()
//---------------------------------------------------------
{
  //Create surface to be  clipped
  mafVMESurfaceParametric *surfaceParametric;
  mafNEW(surfaceParametric);
  surfaceParametric->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_SPHERE);
  surfaceParametric->SetSphereRadius(5.);
  surfaceParametric->Update();
  surfaceParametric->GetOutput()->GetVTKData()->Update();

  mafVMESurface *surface;
  mafNEW(surface);
  surface->SetData((vtkPolyData*)surfaceParametric->GetOutput()->GetVTKData(),0.0);


  mafGUILandmark *gui = new mafGUILandmark(surface, NULL, true);
  CPPUNIT_ASSERT(!gui->VmeAccept(NULL));

  cppDEL(gui);
  mafDEL(surface);
  mafDEL(surfaceParametric);
}