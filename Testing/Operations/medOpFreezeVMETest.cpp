/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpFreezeVMETest.cpp,v $
Language:  C++
Date:      $Date: 2007-09-10 14:52:02 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
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

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medOpFreezeVMETest.h"
#include "medOpFreezeVME.h"

#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "medVMEWrappedMEter.h"
#include "mafVMEMeter.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEVolumeGray.h"
#include "mmoVTKImporter.h"
#include "mafVMEProber.h"
#include "mafVMESlicer.h"

#include "mafOBB.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkImageData.h"

//-----------------------------------------------------------
void medOpFreezeVMETest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	medOpFreezeVME *freeze = new medOpFreezeVME();
	mafDEL(freeze);
}
//-----------------------------------------------------------
void medOpFreezeVMETest::TestStaticAllocation() 
//-----------------------------------------------------------
{
	medOpFreezeVME freeze; 
}
//-----------------------------------------------------------
void medOpFreezeVMETest::TestFreezeVME() 
//-----------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();

  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->SetParent(root);
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  

  //test surface parametric

  medOpFreezeVME *freezeOp=new medOpFreezeVME("freeze");
  freezeOp->TestModeOn();
  freezeOp->SetInput(vmeParametricSurfaceSTART);
  freezeOp->OpRun();

  mafVMESurface *surface=(mafVMESurface *)(freezeOp->GetOutput());
  surface->SetParent(root);
  surface->GetOutput()->GetVTKData()->Update();
  surface->Update();

  CPPUNIT_ASSERT(surface && surface->GetOutput()->GetVTKData()->GetNumberOfPoints() == vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->GetNumberOfPoints());

  //test wrappedmeter

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->SetParent(root);
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  mafMatrix *matrix = vmeParametricSurfaceEND1->GetOutput()->GetAbsMatrix();
  enum {X,Y,Z};
  matrix->SetElement(X,3,4); //set a translation value on x axis of 4.0
  matrix->SetElement(Y,3,3); //set a translation value on x axis of 3.0

  medVMEWrappedMeter *wrappedMeter;
  mafNEW(wrappedMeter);

  wrappedMeter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  wrappedMeter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  wrappedMeter->SetParent(storage->GetRoot());
  wrappedMeter->GetOutput()->GetVTKData()->Update();
  wrappedMeter->Modified();
  wrappedMeter->Update();


  freezeOp->SetInput(wrappedMeter);
  freezeOp->OpRun();

  mafVMEPolyline *polyline1=(mafVMEPolyline *)(freezeOp->GetOutput());
  polyline1->SetParent(root);
  polyline1->GetOutput()->GetVTKData()->Update();
  polyline1->Update();
  CPPUNIT_ASSERT(polyline1 && polyline1->GetOutput()->GetVTKData()->GetNumberOfPoints() == wrappedMeter->GetOutput()->GetVTKData()->GetNumberOfPoints());

  //test meter

  mafVMEMeter *meter;
  mafNEW(meter);

  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetParent(storage->GetRoot());
  meter->GetOutput()->GetVTKData()->Update();
  meter->Modified();
  meter->Update();


  freezeOp->SetInput(meter);
  freezeOp->OpRun();

  mafVMEPolyline *polyline2=(mafVMEPolyline *)(freezeOp->GetOutput());
  polyline2->SetParent(root);
  polyline2->GetOutput()->GetVTKData()->Update();
  polyline2->Update();
  CPPUNIT_ASSERT(polyline2 && polyline2->GetOutput()->GetVTKData()->GetNumberOfPoints() == meter->GetOutput()->GetVTKData()->GetNumberOfPoints());

  // test prober
  mmoVTKImporter *importer=new mmoVTKImporter("importer");
  importer->TestModeOn();
  importer->SetInput(root);
  mafString filename=MED_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->OpRun();

  mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
  volume->GetOutput()->GetVTKData()->Update();
  volume->SetParent(root);
  volume->Update();

  
  mafVMEProber *prober;
  mafNEW(prober);

  double centerV[3], rotV[3];
  mafOBB b;
  rotV[0] = rotV[1] = rotV[2] = 0;
  volume->GetOutput()->GetVMELocalBounds(b);
  b.GetCenter(centerV);

  prober->SetPose(centerV,rotV,0);
  prober->SetVolumeLink(volume);
  prober->SetSurfaceLink(surface);
  prober->SetParent(storage->GetRoot());
  prober->GetOutput()->GetVTKData()->Update();
  prober->Modified();
  prober->Update();

  freezeOp->SetInput(prober);
  freezeOp->OpRun();

  mafVMESurface *probSurface=(mafVMESurface *)(freezeOp->GetOutput());
  probSurface->SetParent(root);
  probSurface->GetOutput()->GetVTKData()->Update();
  probSurface->Update();
  int numSrc = probSurface->GetOutput()->GetVTKData()->GetNumberOfPoints();
  int numDst = prober->GetOutput()->GetVTKData()->GetNumberOfPoints();

  int numCellSrc = probSurface->GetOutput()->GetVTKData()->GetNumberOfCells();
  int numCellDst = prober->GetOutput()->GetVTKData()->GetNumberOfCells();

  CPPUNIT_ASSERT(probSurface &&  numSrc == numDst && numCellSrc == numCellDst);

  double range1[2];
  vtkPolyData *polyProber;
  polyProber = (vtkPolyData *)prober->GetOutput()->GetVTKData();
  polyProber->GetPointData()->GetScalars()->GetRange(range1);
  double range2[2];
  probSurface->GetOutput()->GetVTKData()->GetPointData()->GetScalars()->GetRange(range2);
  CPPUNIT_ASSERT(fabs(range1[0] - range2[0])<0.1 && fabs(range1[1] - range2[1])<0.1);


  // test slicer
  mafVMESlicer *slicer;
  mafNEW(slicer);
  slicer->ReparentTo(volume);

  
  slicer->SetPose(centerV,rotV,0);
  slicer->Update();

  freezeOp->SetInput(slicer);
  freezeOp->OpRun();

  mafVMESurface *sliceSurface=(mafVMESurface *)(freezeOp->GetOutput());
  sliceSurface->SetParent(volume);
  sliceSurface->GetOutput()->GetVTKData()->Update();
  sliceSurface->Update();
  numSrc = sliceSurface->GetOutput()->GetVTKData()->GetNumberOfPoints();
  numDst = slicer->GetOutput()->GetVTKData()->GetNumberOfPoints();

  numCellSrc = sliceSurface->GetOutput()->GetVTKData()->GetNumberOfCells();
  numCellDst = slicer->GetOutput()->GetVTKData()->GetNumberOfCells();

  CPPUNIT_ASSERT(sliceSurface &&  numSrc == numDst && numCellSrc == numCellDst);

  /*for(int i=0; i< sliceSurface->GetSurfaceOutput()->GetTexture()->GetDimensions()[0]; i++)
  {
    for(int j=0; j< sliceSurface->GetSurfaceOutput()->GetTexture()->GetDimensions()[1]; j++)
    {
      int value1 = sliceSurface->GetSurfaceOutput()->GetTexture()->GetPointData()->GetScalars()->GetTuple1(i+i*j);
      int value2 = slicer->GetSurfaceOutput()->GetTexture()->GetPointData()->GetScalars()->GetTuple1(i+i*j);
      CPPUNIT_ASSERT(value1 == value2);
    }
  }*/

  /////////////////////////////////////////////////////////////////

  delete wxLog::SetActiveTarget(NULL);

  wrappedMeter->SetParent(NULL);
  meter->SetParent(NULL);
  volume->SetParent(NULL);
  probSurface->SetParent(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  vmeParametricSurfaceEND1->SetParent(NULL);
  surface->SetParent(NULL);
  
  cppDEL(importer);
  mafDEL(wrappedMeter);
  mafDEL(meter);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);
  cppDEL(freezeOp);
  mafDEL(storage);
}