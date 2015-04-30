/*=========================================================================

 Program: MAF2
 Module: mafOpFreezeVMETest
 Authors: Daniele Giunchi
 
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
#include "mafOpFreezeVMETest.h"
#include "mafOpFreezeVME.h"

#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafVMEWrappedMEter.h"
#include "mafVMEMeter.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEVolumeGray.h"
#include "mafOpImporterVTK.h"
#include "mafVMEProber.h"
#include "mafVMESlicer.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMERefSys.h"

#include "mafOBB.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkCellArray.h"

//-----------------------------------------------------------
void mafOpFreezeVMETest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	mafOpFreezeVME *freeze = new mafOpFreezeVME();
	mafDEL(freeze);
}
//-----------------------------------------------------------
void mafOpFreezeVMETest::TestStaticAllocation() 
//-----------------------------------------------------------
{
	mafOpFreezeVME freeze; 
}
//-----------------------------------------------------------
void mafOpFreezeVMETest::TestFreezeVMESurfaceParametric() 
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

  /* test surface parametric *//////////////////////////////////////////////////////////////////////////
  mafOpFreezeVME *freezeOp=new mafOpFreezeVME("freeze");
  freezeOp->TestModeOn();
  freezeOp->SetInput(vmeParametricSurfaceSTART);
  freezeOp->OpRun();

  mafVMESurface *surface=(mafVMESurface *)(freezeOp->GetOutput());
  surface->SetParent(root);
  surface->GetOutput()->GetVTKData()->Update();
  surface->Update();

  CPPUNIT_ASSERT(surface && surface->GetOutput()->GetVTKData()->GetNumberOfPoints() == vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->GetNumberOfPoints());

  delete wxLog::SetActiveTarget(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  
  surface->SetParent(NULL); 
  mafDEL(vmeParametricSurfaceSTART);
  cppDEL(freezeOp);
  mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpFreezeVMETest::TestFreezeVMEMeter() 
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

	
	mafOpFreezeVME *freezeOp=new mafOpFreezeVME("freeze");
	freezeOp->TestModeOn();
	
	
	mafVMEMeter *meter;
	mafNEW(meter);
	meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
	meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
	meter->SetParent(storage->GetRoot());
	//meter->GetOutput()->GetVTKData()->Update();
	meter->Modified();
	meter->Update();

  meter->GetOutput()->GetVTKData()->Update();

	freezeOp->SetInput(meter);
	freezeOp->OpRun();

	mafVMEPolyline *polyline2=(mafVMEPolyline *)(freezeOp->GetOutput());
	polyline2->SetParent(root);
	polyline2->GetOutput()->GetVTKData()->Update();
	polyline2->Update();
	CPPUNIT_ASSERT(polyline2 && polyline2->GetOutput()->GetVTKData()->GetNumberOfPoints() == meter->GetOutput()->GetVTKData()->GetNumberOfPoints());


	delete wxLog::SetActiveTarget(NULL);
	meter->SetParent(NULL);
	
	vmeParametricSurfaceSTART->SetParent(NULL);
	vmeParametricSurfaceEND1->SetParent(NULL);
	polyline2->SetParent(NULL);

  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);
	mafDEL(meter);

	cppDEL(freezeOp);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpFreezeVMETest::TestFreezeVMEWrappedMeter() 
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
	
	mafOpFreezeVME *freezeOp=new mafOpFreezeVME("freeze");
	freezeOp->TestModeOn();
	
	mafVMEWrappedMeter *wrappedMeter;
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
	
	delete wxLog::SetActiveTarget(NULL);

	wrappedMeter->SetParent(NULL);
	
	vmeParametricSurfaceSTART->SetParent(NULL);
	vmeParametricSurfaceEND1->SetParent(NULL);
	
	polyline1->SetParent(NULL);
	mafDEL(wrappedMeter);
	mafDEL(vmeParametricSurfaceSTART);
	mafDEL(vmeParametricSurfaceEND1);
	cppDEL(freezeOp);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpFreezeVMETest::TestFreezeVMESlicer() 
//-----------------------------------------------------------
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *root=storage->GetRoot();

	mafOpFreezeVME *freezeOp=new mafOpFreezeVME("freeze");
	freezeOp->TestModeOn();

	mafOpImporterVTK *importer=new mafOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	mafString filename=MAF_DATA_ROOT;
	filename<<"/VTK_Volumes/volume.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

	mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->GetOutput()->GetVTKData()->Update();
	volume->SetParent(root);
	volume->Update();

	double centerV[3], rotV[3];
	mafOBB b;
	rotV[0] = rotV[1] = rotV[2] = 0;
	volume->GetOutput()->GetVMELocalBounds(b);
	b.GetCenter(centerV);

	mafVMESlicer *slicer;
	mafNEW(slicer);
	slicer->ReparentTo(volume);
  slicer->SetSlicedVMELink(volume);

	slicer->SetPose(centerV,rotV,0);
	slicer->Update();

	freezeOp->SetInput(slicer);
	freezeOp->OpRun();

	mafVMESurface *sliceSurface=(mafVMESurface *)(freezeOp->GetOutput());
	sliceSurface->SetParent(volume);
	sliceSurface->GetOutput()->GetVTKData()->Update();
	sliceSurface->Update();
	int numSrc = sliceSurface->GetOutput()->GetVTKData()->GetNumberOfPoints();
	int numDst = slicer->GetOutput()->GetVTKData()->GetNumberOfPoints();

	int numCellSrc = sliceSurface->GetOutput()->GetVTKData()->GetNumberOfCells();
	int numCellDst = slicer->GetOutput()->GetVTKData()->GetNumberOfCells();

	CPPUNIT_ASSERT(sliceSurface &&  numSrc == numDst && numCellSrc == numCellDst);

	bool sameValues=true;

	for(int i=0; i< sliceSurface->GetSurfaceOutput()->GetTexture()->GetDimensions()[0]; i++)
	{
		for(int j=0; j< sliceSurface->GetSurfaceOutput()->GetTexture()->GetDimensions()[1]; j++)
		{
			int value1 = sliceSurface->GetSurfaceOutput()->GetTexture()->GetPointData()->GetScalars()->GetTuple1(i+i*j);
			int value2 = slicer->GetSurfaceOutput()->GetTexture()->GetPointData()->GetScalars()->GetTuple1(i+i*j);
			if(value1 != value2)
				sameValues=false;
		}
	}

	CPPUNIT_ASSERT(sameValues);

	delete wxLog::SetActiveTarget(NULL);

	
	volume->SetParent(NULL);

	slicer->SetParent(NULL);
	sliceSurface->SetParent(NULL);
	

	cppDEL(importer);	
	mafDEL(slicer);
	cppDEL(freezeOp);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpFreezeVMETest::TestFreezeVMEProber() 
//-----------------------------------------------------------
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *root=storage->GetRoot();

	mafOpFreezeVME *freezeOp=new mafOpFreezeVME("freeze");
	freezeOp->TestModeOn();
	
	mafOpImporterVTK *importer=new mafOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	mafString filename=MAF_DATA_ROOT;
	filename<<"/VTK_Volumes/volume.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

	//create a parametric surface
	mafVMESurfaceParametric *vmeParametricSurfaceSTART;
	mafNEW(vmeParametricSurfaceSTART);
	vmeParametricSurfaceSTART->SetParent(root);
	vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
	vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
	vmeParametricSurfaceSTART->Update();

	mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->GetOutput()->GetVTKData()->Update();
	volume->SetParent(root);
	volume->Update();

	double centerV[3], rotV[3];
	mafOBB b;
	rotV[0] = rotV[1] = rotV[2] = 0;
	volume->GetOutput()->GetVMELocalBounds(b);
	b.GetCenter(centerV);

	mafVMEProber *prober;
	mafNEW(prober);

	prober->SetPose(centerV,rotV,0);
	prober->SetVolumeLink(volume);
	prober->SetSurfaceLink(vmeParametricSurfaceSTART);
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

	
	delete wxLog::SetActiveTarget(NULL);
	volume->SetParent(NULL);	
	prober->SetParent(NULL);
	probSurface->SetParent(NULL);

  vmeParametricSurfaceSTART->SetParent(NULL);
  mafDEL(vmeParametricSurfaceSTART);
	cppDEL(importer);	
	mafDEL(prober);
	cppDEL(freezeOp);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpFreezeVMETest::TestFreezeVMEProfileSpline() 
//-----------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();


  mafVMEPolyline *vmePolyline;
  mafNEW(vmePolyline);	
  
  vtkPolyData *polydata;
  vtkPoints   *points;
  vtkCellArray *cells;
  vtkNEW(points);
  vtkNEW(cells);
  vtkNEW(polydata);

  points->InsertNextPoint(0.0,0.0,0.0);
  points->InsertNextPoint(1.0,1.0,0.0);
  points->InsertNextPoint(2.0,0.0,0.0);

  //create cells
  int pointId[2];
  for(int i = 0; i< points->GetNumberOfPoints();i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      cells->InsertNextCell(2 , pointId);  
    }
  }

  polydata->SetPoints(points);
  polydata->SetLines(cells);
  polydata->Update();
  vmePolyline->SetData(polydata, 0.0);
  vmePolyline->SetParent(root);
  vmePolyline->Update();

  mafOpFreezeVME *freezeOp=new mafOpFreezeVME("freeze");
  freezeOp->TestModeOn();

  mafVMEPolylineSpline *spline;
  mafNEW(spline);
  spline->SetPolylineLink(vmePolyline);
  spline->SetParent(root);
  spline->GetOutput()->GetVTKData()->Update();
  spline->Modified();
  spline->Update();

  freezeOp->SetInput(spline);
  freezeOp->OpRun();

  mafVMEPolyline *polyline2=(mafVMEPolyline *)(freezeOp->GetOutput());
  polyline2->SetParent(root);
  polyline2->GetOutput()->GetVTKData()->Update();
  polyline2->Update();
  int value1 = spline->GetOutput()->GetVTKData()->GetNumberOfPoints();
  int value2 = polyline2->GetOutput()->GetVTKData()->GetNumberOfPoints();
  CPPUNIT_ASSERT(polyline2 && value1 == value2);


  delete wxLog::SetActiveTarget(NULL);
  spline->SetParent(NULL);
  vmePolyline->SetParent(NULL);
  polyline2->SetParent(NULL);

  vtkDEL(polydata);
  vtkDEL(points);
  vtkDEL(cells);
  
  mafDEL(vmePolyline);
  mafDEL(spline);
  
  cppDEL(freezeOp);
  mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpFreezeVMETest::TestFreezeVMERefSys()
//-----------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();

  //create a parametric surface
  mafVMERefSys *vmeRefSys;
  mafNEW(vmeRefSys);
  vmeRefSys->SetParent(root);
  vmeRefSys->GetOutput()->GetVTKData()->Update();
  vmeRefSys->SetParent(storage->GetRoot());
  vmeRefSys->Update();

  // test surface parametric //////////////////////////////////////////////////////////////////////////
  mafOpFreezeVME *freezeOp=new mafOpFreezeVME("freeze");
  freezeOp->TestModeOn();
  freezeOp->SetInput(vmeRefSys);
  freezeOp->OpRun();

  mafVMESurface *surface=(mafVMESurface *)(freezeOp->GetOutput());
  surface->SetParent(root);
  surface->GetOutput()->GetVTKData()->Update();
  surface->Update();

  int num1 = surface->GetOutput()->GetVTKData()->GetNumberOfPoints();
  int num2 = vmeRefSys->GetOutput()->GetVTKData()->GetNumberOfPoints();
  CPPUNIT_ASSERT(surface &&  num1 == num2);

  delete wxLog::SetActiveTarget(NULL);
  vmeRefSys->SetParent(NULL);

  surface->SetParent(NULL); 
  mafDEL(vmeRefSys);
  cppDEL(freezeOp);
  mafDEL(storage);
}