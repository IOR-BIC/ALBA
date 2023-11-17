/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBALocalAxisCoordinateTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkALBALocalAxisCoordinateTest.h"
#include "vtkALBALocalAxisCoordinate.h"

#include "vtkALBASmartPointer.h"

#include "vtkCubeSource.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkDataSet.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


void vtkALBALocalAxisCoordinateTest::TestFixture()
{

}

void vtkALBALocalAxisCoordinateTest::TestDynamicAllocation()
{
  vtkALBALocalAxisCoordinate *coord = NULL;
  coord = vtkALBALocalAxisCoordinate::New();

  coord->Delete();
  coord = NULL;
}
/*void vtkALBALocalAxisCoordinateTest::TestStaticAllocation()
{
  vtkALBALocalAxisCoordinate coord;
}*/

void vtkALBALocalAxisCoordinateTest::TestMatrixAccessors()
{
  vtkALBASmartPointer<vtkALBALocalAxisCoordinate> coord;
  vtkALBASmartPointer<vtkMatrix4x4> matrix;
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
void vtkALBALocalAxisCoordinateTest::TestDatasetAccessors()
{
  vtkALBASmartPointer<vtkALBALocalAxisCoordinate> coord;
  vtkALBASmartPointer<vtkCubeSource> cube;

  coord->SetDataSet((vtkDataSet*)cube->GetOutput());

  CPPUNIT_ASSERT(((vtkDataSet*)cube->GetOutput()) == coord->GetDataSet());
}

void vtkALBALocalAxisCoordinateTest::TestGetComputedUserDefinedValue()
{
  vtkALBASmartPointer<vtkALBALocalAxisCoordinate> coord;

  vtkALBASmartPointer<vtkRenderWindow> renWin;
  vtkALBASmartPointer<vtkRenderer> ren1;
  vtkALBASmartPointer<vtkCamera> camera;

  vtkALBASmartPointer<vtkCubeSource> cube;
  vtkALBASmartPointer<vtkMatrix4x4> matrix;

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