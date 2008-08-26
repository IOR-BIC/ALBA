/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFGlobalAxisCoordinateTest.cpp,v $
Language:  C++
Date:      $Date: 2008-08-26 15:05:41 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFGlobalAxisCoordinateTest.h"
#include "vtkMAFGlobalAxisCoordinate.h"

#include "vtkMAFSmartPointer.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"



void vtkMAFGlobalAxisCoordinateTest::setUp()
{
 
}

void vtkMAFGlobalAxisCoordinateTest::tearDown()
{

}

void vtkMAFGlobalAxisCoordinateTest::TestFixture()
{

}

void vtkMAFGlobalAxisCoordinateTest::TestDynamicAllocation()
{
  vtkMAFGlobalAxisCoordinate *coord = NULL;
  coord = vtkMAFGlobalAxisCoordinate::New();

  coord->Delete();
  coord = NULL;
}
/*void vtkMAFGlobalAxisCoordinateTest::TestStaticAllocation()
{
  vtkMAFGlobalAxisCoordinate coord;
}*/
void vtkMAFGlobalAxisCoordinateTest::TestGetComputedUserDefinedValue()
{
  vtkMAFSmartPointer<vtkMAFGlobalAxisCoordinate> coord;

  vtkMAFSmartPointer<vtkRenderWindow> renWin;
  vtkMAFSmartPointer<vtkRenderer> ren1;
  vtkMAFSmartPointer<vtkCamera> camera;

  double w[4] = {2.,7.,5.,1.};
  coord->SetValue(w[0],w[1],w[2]);

  double *value;

  ren1->SetActiveCamera (camera);
  renWin->AddRenderer (ren1);
  renWin->SetSize (40, 40);
  value = coord->GetComputedUserDefinedValue(ren1);

  //this computation is equal to a world to display transformation using a render window of h = 40 w = 40
  //and with a reset camera with |0.5| as bounds in every direction
  //control code
  double b = 0.5;
  ren1->ResetCamera(-b,b,-b,b,-b,b);

  double control[3];
  ren1->SetWorldPoint(w);
  ren1->WorldToDisplay();
  ren1->GetDisplayPoint(control);

  bool result = false;
  result = ((value[0] == control[0]) &&
           (value[1] == control[1]) &&
           (value[2] == control[2]));
  
  CPPUNIT_ASSERT(result);
}