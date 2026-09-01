/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILandmarkTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaGUILandmarkTest.h"
#include "albaGUILandmark.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMESurface.h"
#include "vtkDataSet.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//---------------------------------------------------------
void albaGUILandmarkTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  //Create surface to be  clipped
  albaVMESurfaceParametric *surfaceParametric;
  albaNEW(surfaceParametric);
  surfaceParametric->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_SPHERE);
  surfaceParametric->SetSphereRadius(5.);
  surfaceParametric->Update();

  albaVMESurface *surface;
  albaNEW(surface);
  surface->SetData((vtkPolyData*)surfaceParametric->GetOutput()->GetVTKData(),0.0);


  albaGUILandmark *gui = new albaGUILandmark(surface, NULL, true);
  CPPUNIT_ASSERT(gui != NULL);

  cppDEL(gui);
  albaDEL(surface);
  albaDEL(surfaceParametric);
}

//---------------------------------------------------------
void albaGUILandmarkTest::TestVmeAccept()
//---------------------------------------------------------
{
  //Create surface to be  clipped
  albaVMESurfaceParametric *surfaceParametric;
  albaNEW(surfaceParametric);
  surfaceParametric->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_SPHERE);
  surfaceParametric->SetSphereRadius(5.);
  surfaceParametric->Update();

  albaVMESurface *surface;
  albaNEW(surface);
  surface->SetData((vtkPolyData*)surfaceParametric->GetOutput()->GetVTKData(),0.0);


  albaGUILandmark *gui = new albaGUILandmark(surface, NULL, true);
  CPPUNIT_ASSERT(!gui->VmeAccept(NULL));

  cppDEL(gui);
  albaDEL(surface);
  albaDEL(surfaceParametric);
}