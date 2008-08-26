/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFLocalAxisCoordinateTest.cpp,v $
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

#include "vtkMAFLocalAxisCoordinateTest.h"
#include "vtkMAFLocalAxisCoordinate.h"

#include "vtkMAFSmartPointer.h"

#include "vtkCubeSource.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkDataSet.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"



void vtkMAFLocalAxisCoordinateTest::setUp()
{
 
}

void vtkMAFLocalAxisCoordinateTest::tearDown()
{

}

void vtkMAFLocalAxisCoordinateTest::TestFixture()
{

}

void vtkMAFLocalAxisCoordinateTest::TestDynamicAllocation()
{
  vtkMAFLocalAxisCoordinate *coord = NULL;
  coord = vtkMAFLocalAxisCoordinate::New();

  coord->Delete();
  coord = NULL;
}
/*void vtkMAFLocalAxisCoordinateTest::TestStaticAllocation()
{
  vtkMAFLocalAxisCoordinate coord;
}*/

void vtkMAFLocalAxisCoordinateTest::TestMatrixAccessors()
{
  vtkMAFSmartPointer<vtkMAFLocalAxisCoordinate> coord;
  vtkMAFSmartPointer<vtkMatrix4x4> matrix;
  coord->SetMatrix(matrix);

  bool result = true;
  for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
    {
      if(matrix->GetElement(i,j) != coord->GetMatrix()->GetElement(i,j))
      {
        result = false;
      }
    }
  
  CPPUNIT_ASSERT(result);

}
void vtkMAFLocalAxisCoordinateTest::TestDatasetAccessors()
{
  vtkMAFSmartPointer<vtkMAFLocalAxisCoordinate> coord;
  vtkMAFSmartPointer<vtkCubeSource> cube;

  coord->SetDataSet((vtkDataSet*)cube->GetOutput());

  CPPUNIT_ASSERT(((vtkDataSet*)cube->GetOutput()) == coord->GetDataSet());
}

void vtkMAFLocalAxisCoordinateTest::TestGetComputedUserDefinedValue()
{
  vtkMAFSmartPointer<vtkMAFLocalAxisCoordinate> coord;

  vtkMAFSmartPointer<vtkRenderWindow> renWin;
  vtkMAFSmartPointer<vtkRenderer> ren1;
  vtkMAFSmartPointer<vtkCamera> camera;

  vtkMAFSmartPointer<vtkCubeSource> cube;
  vtkMAFSmartPointer<vtkMatrix4x4> matrix;

  double w[4] = {2.,7.,5.,1.};
  coord->SetValue(w[0],w[1],w[2]);
  coord->SetDataSet((vtkDataSet*)cube->GetOutput());
  coord->SetMatrix(matrix);

  double *value;

  ren1->SetActiveCamera (camera);
  renWin->AddRenderer (ren1);
  value = coord->GetComputedUserDefinedValue(ren1);

  //this computation is equal to a world to display transformation using matrix and dataset derived parameters
  //control code
  double size;
  size = cube->GetOutput()->GetLength()/8.0;
  double v[4];
  for(int i=0; i<3; i++) v[i] = w[i] * size;
  v[3] = 1.;
  w[0] = v[0]*matrix->Element[0][0] + v[1]*matrix->Element[0][1] +  v[2]*matrix->Element[0][2] + v[3]*matrix->Element[0][3];
  w[1] = v[0]*matrix->Element[1][0] + v[1]*matrix->Element[1][1] +  v[2]*matrix->Element[1][2] + v[3]*matrix->Element[1][3];
  w[2] = v[0]*matrix->Element[2][0] + v[1]*matrix->Element[2][1] +  v[2]*matrix->Element[2][2] + v[3]*matrix->Element[2][3];
  
  double control[3];
  ren1->SetWorldPoint(w);
  ren1->WorldToDisplay();
  ren1->GetDisplayPoint(control);

  bool result = false;
  result = ((value[0] == control[0]) &&
           (value[1] == control[1]) &&
           (value[2] == control[2]));

  //printf("\n%f %f %f\n", value[0], value[1], value[2]);
  //printf("\n%f %f %f\n", control[0], control[1], control[2]);
  
  CPPUNIT_ASSERT(result);
}