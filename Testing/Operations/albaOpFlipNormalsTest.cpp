/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFlipNormalsTest
 Authors: Matteo Giacomoni
 
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
#include "albaOpFlipNormalsTest.h"
#include "albaOpFlipNormals.h"

#include "albaVMEGroup.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEPolylineGraph.h"
#include "albaVMESurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

//-----------------------------------------------------------
void albaOpFlipNormalsTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpFlipNormals *flip = new albaOpFlipNormals();
  albaDEL(flip);
}
//-----------------------------------------------------------
void albaOpFlipNormalsTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpFlipNormals flip; 
}
//-----------------------------------------------------------
void albaOpFlipNormalsTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMEPolylineGraph> polyline;
  albaSmartPointer<albaVMESurface> surface;

  albaOpFlipNormals *flip = new albaOpFlipNormals();
  flip->TestModeOn();
  CPPUNIT_ASSERT( !flip->Accept(group) );
  CPPUNIT_ASSERT( !flip->Accept(volume) );
  CPPUNIT_ASSERT( !flip->Accept(polyline) );

  //Only a surface is accepted as input
  CPPUNIT_ASSERT( flip->Accept(surface) );

  albaDEL(flip);
}
//-----------------------------------------------------------
void albaOpFlipNormalsTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  albaOpFlipNormals *flip = new albaOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  CPPUNIT_ASSERT( (vtkPolyData*)(surface->GetOutput()->GetVTKData())->GetCellData()->GetNormals() != NULL );

  albaDEL(flip);
}
//-----------------------------------------------------------
void albaOpFlipNormalsTest::TestSetSeed() 
//-----------------------------------------------------------
{
  albaOpFlipNormals *flip = new albaOpFlipNormals();
  flip->TestModeOn();
  flip->SetSeed(0);  

  CPPUNIT_ASSERT( flip->GetSeed() == 0 );

  albaDEL(flip);
}
//-----------------------------------------------------------
void albaOpFlipNormalsTest::TestSetDiameter() 
//-----------------------------------------------------------
{
  albaOpFlipNormals *flip = new albaOpFlipNormals();
  flip->TestModeOn();
  flip->SetDiameter(10.0);  

  CPPUNIT_ASSERT( flip->GetDiameter() == 10.0 );

  albaDEL(flip);
}
//-----------------------------------------------------------
void albaOpFlipNormalsTest::TestFlipNormals() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  albaOpFlipNormals *flip = new albaOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  vtkALBASmartPointer<vtkPolyData> polydataInput;
  polydataInput->DeepCopy(albaVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());

  flip->SetSeed(0);
  flip->SetDiameter(0.01);
  flip->MarkCells();
  flip->FlipNormals();
  flip->OpDo();

  vtkALBASmartPointer<vtkPolyData> polydataOutput;
  polydataOutput->DeepCopy(albaVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());
  
  for (int i=0;i<albaVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData()->GetNumberOfCells();i++)
  {

    double normalInput[3];
    normalInput[0] = polydataInput->GetCellData()->GetNormals()->GetTuple3(i)[0];
    normalInput[1] = polydataInput->GetCellData()->GetNormals()->GetTuple3(i)[1];
    normalInput[2] = polydataInput->GetCellData()->GetNormals()->GetTuple3(i)[2];


    double normalOutput[3];
    normalOutput[0] = polydataOutput->GetCellData()->GetNormals()->GetTuple3(i)[0];
    normalOutput[1] = polydataOutput->GetCellData()->GetNormals()->GetTuple3(i)[1];
    normalOutput[2] = polydataOutput->GetCellData()->GetNormals()->GetTuple3(i)[2];

	  if (i==0)
	  {
	  	CPPUNIT_ASSERT( normalInput[0] == -normalOutput[0] && normalInput[1] == -normalOutput[1] && normalInput[2] == -normalOutput[2] );
	  }
    else
    {
      CPPUNIT_ASSERT( normalInput[0] == normalOutput[0] && normalInput[1] == normalOutput[1] && normalInput[2] == normalOutput[2] );
    }
  }

  albaDEL(flip);

}
//-----------------------------------------------------------
void albaOpFlipNormalsTest::TestModifyAllNormal() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  albaOpFlipNormals *flip = new albaOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  vtkALBASmartPointer<vtkPolyData> polydataInput;
  polydataInput->DeepCopy(albaVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());

  flip->SetSeed(0);
  flip->SetDiameter(1);
  flip->MarkCells();
  flip->FlipNormals();
  flip->ModifyAllNormal();
  flip->OpDo();

  vtkALBASmartPointer<vtkPolyData> polydataOutput;
  polydataOutput->DeepCopy(albaVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());

  for (int i=0;i<albaVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData()->GetNumberOfCells();i++)
  {

    double normalInput[3];
    normalInput[0] = polydataInput->GetCellData()->GetNormals()->GetTuple3(i)[0];
    normalInput[1] = polydataInput->GetCellData()->GetNormals()->GetTuple3(i)[1];
    normalInput[2] = polydataInput->GetCellData()->GetNormals()->GetTuple3(i)[2];


    double normalOutput[3];
    normalOutput[0] = polydataOutput->GetCellData()->GetNormals()->GetTuple3(i)[0];
    normalOutput[1] = polydataOutput->GetCellData()->GetNormals()->GetTuple3(i)[1];
    normalOutput[2] = polydataOutput->GetCellData()->GetNormals()->GetTuple3(i)[2];

    CPPUNIT_ASSERT( normalInput[0] == -normalOutput[0] && normalInput[1] == -normalOutput[1] && normalInput[2] == -normalOutput[2] );
  }

  albaDEL(flip);
}
