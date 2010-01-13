/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpScaleDatasetTest.cpp,v $
Language:  C++
Date:      $Date: 2010-01-13 14:47:25 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
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
#include "medOpScaleDatasetTest.h"
#include "medOpScaleDataset.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMERoot.h"
#include "mafMatrix.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCubeSource.h"
#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

//-----------------------------------------------------------
void medOpScaleDatasetTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpScaleDataset *op = new medOpScaleDataset();
  mafDEL(op);
}
//-----------------------------------------------------------
void medOpScaleDatasetTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  medOpScaleDataset extrusion; 
}
//-----------------------------------------------------------
void medOpScaleDatasetTest::TestOpDoVolume() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkStructuredPoints> sp;
  sp->SetOrigin(0.0,0.0,0.0);
  sp->SetSpacing(1.0,1.0,1.0);
  sp->SetDimensions(5,10,6);
  sp->Update();

  vtkMAFSmartPointer<vtkDoubleArray> scalars;
  scalars->SetNumberOfTuples(300);
  for (int i=0;i<300;i++)
  {
    scalars->SetTuple1(i,0.0);
  }
  sp->GetPointData()->SetScalars(scalars);
  sp->Update();

  double startPoint[3];
  sp->GetPoint(0,startPoint);

  mafSmartPointer<mafVMEVolumeGray> volumeInput;

  volumeInput->SetData(sp,0.0);
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  medOpScaleDataset *op = new medOpScaleDataset();
  op->TestModeOn();
  op->SetInput(volumeInput);

  mafMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  mafTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->OpDo();

  mafVMEVolumeGray *volumeOutput = mafVMEVolumeGray::SafeDownCast(op->GetInput());
  volumeOutput->GetOutput()->Update();
  volumeOutput->Update();

  vtkStructuredPoints *spOutput = vtkStructuredPoints::SafeDownCast(volumeOutput->GetOutput()->GetVTKData());
  spOutput->Update();

  double endPoint[3];
  spOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0]*scaleFactorX );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1]*scaleFactorY );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2]*scaleFactorZ );  

  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpScaleDatasetTest::TestOpDoSurface() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  double startPoint[3];
  cube->GetOutput()->GetPoint(0,startPoint);

  mafSmartPointer<mafVMESurface> surfaceInput;

  surfaceInput->SetData(cube->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  medOpScaleDataset *op = new medOpScaleDataset();
  op->TestModeOn();
  op->SetInput(surfaceInput);

  mafMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  mafTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->OpDo();

  mafVMESurface *surfaceOutput = mafVMESurface::SafeDownCast(op->GetInput());
  surfaceOutput->GetOutput()->Update();
  surfaceOutput->Update();

  vtkPolyData *cubeOutput = vtkPolyData::SafeDownCast(surfaceOutput->GetOutput()->GetVTKData());
  cubeOutput->Update();
  
  double endPoint[3];
  cubeOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0]*scaleFactorX );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1]*scaleFactorY );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2]*scaleFactorZ );  

  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpScaleDatasetTest::TestReset() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  double startPoint[3];
  cube->GetOutput()->GetPoint(0,startPoint);

  mafSmartPointer<mafVMESurface> surfaceInput;

  surfaceInput->SetData(cube->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  medOpScaleDataset *op = new medOpScaleDataset();
  op->TestModeOn();
  op->SetInput(surfaceInput);

  mafMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  mafTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->Reset();

  mafVMESurface *surfaceOutput = mafVMESurface::SafeDownCast(op->GetInput());
  surfaceOutput->GetOutput()->Update();
  surfaceOutput->Update();

  vtkPolyData *cubeOutput = vtkPolyData::SafeDownCast(surfaceOutput->GetOutput()->GetVTKData());
  cubeOutput->Update();

  double endPoint[3];
  cubeOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0] );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1] );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2] );  

  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpScaleDatasetTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMERoot> root;
  medOpScaleDataset *op = new medOpScaleDataset();
  op->TestModeOn();

  CPPUNIT_ASSERT( op->Accept(surface) );
  CPPUNIT_ASSERT( op->Accept(volume) );
  CPPUNIT_ASSERT( !op->Accept(root) );

  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
