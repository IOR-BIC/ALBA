/*=========================================================================

 Program: MAF2
 Module: mafOpFlipNormalsTest
 Authors: Matteo Giacomoni
 
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
#include "mafOpFlipNormalsTest.h"
#include "mafOpFlipNormals.h"

#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEPolylineGraph.h"
#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

//-----------------------------------------------------------
void mafOpFlipNormalsTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpFlipNormals *flip = new mafOpFlipNormals();
  mafDEL(flip);
}
//-----------------------------------------------------------
void mafOpFlipNormalsTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpFlipNormals flip; 
}
//-----------------------------------------------------------
void mafOpFlipNormalsTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMEPolylineGraph> polyline;
  mafSmartPointer<mafVMESurface> surface;

  mafOpFlipNormals *flip = new mafOpFlipNormals();
  flip->TestModeOn();
  CPPUNIT_ASSERT( !flip->Accept(group) );
  CPPUNIT_ASSERT( !flip->Accept(volume) );
  CPPUNIT_ASSERT( !flip->Accept(polyline) );

  //Only a surface is accepted as input
  CPPUNIT_ASSERT( flip->Accept(surface) );

  mafDEL(flip);
}
//-----------------------------------------------------------
void mafOpFlipNormalsTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  mafOpFlipNormals *flip = new mafOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  CPPUNIT_ASSERT( (vtkPolyData*)(surface->GetOutput()->GetVTKData())->GetCellData()->GetNormals() != NULL );

  mafDEL(flip);
}
//-----------------------------------------------------------
void mafOpFlipNormalsTest::TestSetSeed() 
//-----------------------------------------------------------
{
  mafOpFlipNormals *flip = new mafOpFlipNormals();
  flip->TestModeOn();
  flip->SetSeed(0);  

  CPPUNIT_ASSERT( flip->GetSeed() == 0 );

  mafDEL(flip);
}
//-----------------------------------------------------------
void mafOpFlipNormalsTest::TestSetDiameter() 
//-----------------------------------------------------------
{
  mafOpFlipNormals *flip = new mafOpFlipNormals();
  flip->TestModeOn();
  flip->SetDiameter(10.0);  

  CPPUNIT_ASSERT( flip->GetDiameter() == 10.0 );

  mafDEL(flip);
}
//-----------------------------------------------------------
void mafOpFlipNormalsTest::TestFlipNormals() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  mafOpFlipNormals *flip = new mafOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  vtkMAFSmartPointer<vtkPolyData> polydataInput;
  polydataInput->DeepCopy(mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());

  flip->SetSeed(0);
  flip->SetDiameter(0.01);
  flip->MarkCells();
  flip->FlipNormals();
  flip->OpDo();

  vtkMAFSmartPointer<vtkPolyData> polydataOutput;
  polydataOutput->DeepCopy(mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());
  
  for (int i=0;i<mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData()->GetNumberOfCells();i++)
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

  mafDEL(flip);

}
//-----------------------------------------------------------
void mafOpFlipNormalsTest::TestModifyAllNormal() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  mafOpFlipNormals *flip = new mafOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  vtkMAFSmartPointer<vtkPolyData> polydataInput;
  polydataInput->DeepCopy(mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());

  flip->SetSeed(0);
  flip->SetDiameter(1);
  flip->MarkCells();
  flip->FlipNormals();
  flip->ModifyAllNormal();
  flip->OpDo();

  vtkMAFSmartPointer<vtkPolyData> polydataOutput;
  polydataOutput->DeepCopy(mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());

  for (int i=0;i<mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData()->GetNumberOfCells();i++)
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

  mafDEL(flip);
}
