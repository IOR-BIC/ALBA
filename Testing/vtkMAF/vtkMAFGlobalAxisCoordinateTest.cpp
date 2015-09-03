/*=========================================================================

 Program: MAF2
 Module: vtkMAFGlobalAxisCoordinateTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFGlobalAxisCoordinateTest.h"
#include "vtkMAFGlobalAxisCoordinate.h"

#include "vtkMAFSmartPointer.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


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