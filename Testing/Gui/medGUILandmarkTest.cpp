/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUILandmarkTest.cpp,v $
Language:  C++
Date:      $Date: 2011-02-14 14:25:29 $
Version:   $Revision: 1.1.2.2 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medGUILandmarkTest.h"
#include "medGUILandmark.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMESurface.h"
#include "vtkDataSet.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void medGUILandmarkTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void medGUILandmarkTest::tearDown()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------
void medGUILandmarkTest::TestDynamicAllocation()
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


  medGUILandmark *gui = new medGUILandmark(surface, NULL, true);
  CPPUNIT_ASSERT(gui != NULL);

  cppDEL(gui);
  mafDEL(surface);
  mafDEL(surfaceParametric);
}

//---------------------------------------------------------
void medGUILandmarkTest::TestVmeAccept()
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


  medGUILandmark *gui = new medGUILandmark(surface, NULL, true);
  CPPUNIT_ASSERT(!gui->VmeAccept(NULL));

  cppDEL(gui);
  mafDEL(surface);
  mafDEL(surfaceParametric);
}