/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpFlipNormalsTest.cpp,v $
Language:  C++
Date:      $Date: 2010-02-22 10:09:58 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medOpFlipNormalsTest.h"
#include "medOpFlipNormals.h"

#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "medVMEPolylineGraph.h"
#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

//-----------------------------------------------------------
void medOpFlipNormalsTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpFlipNormals *flip = new medOpFlipNormals();
  mafDEL(flip);
}
//-----------------------------------------------------------
void medOpFlipNormalsTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  medOpFlipNormals flip; 
}
//-----------------------------------------------------------
void medOpFlipNormalsTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<medVMEPolylineGraph> polyline;
  mafSmartPointer<mafVMESurface> surface;

  medOpFlipNormals *flip = new medOpFlipNormals();
  flip->TestModeOn();
  CPPUNIT_ASSERT( !flip->Accept(group) );
  CPPUNIT_ASSERT( !flip->Accept(volume) );
  CPPUNIT_ASSERT( !flip->Accept(polyline) );

  //Only a surface is accepted as input
  CPPUNIT_ASSERT( flip->Accept(surface) );

  mafDEL(flip);
}
//-----------------------------------------------------------
void medOpFlipNormalsTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  medOpFlipNormals *flip = new medOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  CPPUNIT_ASSERT( (vtkPolyData*)(surface->GetOutput()->GetVTKData())->GetCellData()->GetNormals() != NULL );

  mafDEL(flip);
}
//-----------------------------------------------------------
void medOpFlipNormalsTest::TestSetSeed() 
//-----------------------------------------------------------
{
  medOpFlipNormals *flip = new medOpFlipNormals();
  flip->TestModeOn();
  flip->SetSeed(0);  

  CPPUNIT_ASSERT( flip->GetSeed() == 0 );

  mafDEL(flip);
}
//-----------------------------------------------------------
void medOpFlipNormalsTest::TestSetDiameter() 
//-----------------------------------------------------------
{
  medOpFlipNormals *flip = new medOpFlipNormals();
  flip->TestModeOn();
  flip->SetDiameter(10.0);  

  CPPUNIT_ASSERT( flip->GetDiameter() == 10.0 );

  mafDEL(flip);
}
//-----------------------------------------------------------
void medOpFlipNormalsTest::TestFlipNormals() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  medOpFlipNormals *flip = new medOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  vtkMAFSmartPointer<vtkPolyData> polydataInput;
  polydataInput->DeepCopy(mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());
  polydataInput->Update();

  flip->SetSeed(0);
  flip->SetDiameter(0.01);
  flip->MarkCells();
  flip->FlipNormals();
  flip->OpDo();

  vtkMAFSmartPointer<vtkPolyData> polydataOutput;
  polydataOutput->DeepCopy(mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());
  polydataOutput->Update();
  
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
void medOpFlipNormalsTest::TestModifyAllNormal() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  CPPUNIT_ASSERT( sphere->GetOutput()->GetCellData()->GetNormals() == NULL );

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  medOpFlipNormals *flip = new medOpFlipNormals();
  flip->TestModeOn();
  flip->SetInput(surface);
  flip->OpRun();

  vtkMAFSmartPointer<vtkPolyData> polydataInput;
  polydataInput->DeepCopy(mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());
  polydataInput->Update();

  flip->SetSeed(0);
  flip->SetDiameter(1);
  flip->MarkCells();
  flip->FlipNormals();
  flip->ModifyAllNormal();
  flip->OpDo();

  vtkMAFSmartPointer<vtkPolyData> polydataOutput;
  polydataOutput->DeepCopy(mafVMESurface::SafeDownCast(flip->GetInput())->GetOutput()->GetVTKData());
  polydataOutput->Update();

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
