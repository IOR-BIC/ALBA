/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFreezeVMETest
 Authors: Daniele Giunchi
 
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
#include "albaOpFreezeVMETest.h"
#include "albaOpFreezeVME.h"

#include "albaString.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"
#include "albaVMEPolyline.h"
#include "albaVMEWrappedMEter.h"
#include "albaVMEMeter.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEVolumeGray.h"
#include "albaOpImporterVTK.h"
#include "albaVMEPolylineSpline.h"
#include "albaVMERefSys.h"

#include "albaOBB.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkCellArray.h"

//-----------------------------------------------------------
void albaOpFreezeVMETest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	albaOpFreezeVME *freeze = new albaOpFreezeVME();
	albaDEL(freeze);
}
//-----------------------------------------------------------
void albaOpFreezeVMETest::TestStaticAllocation() 
//-----------------------------------------------------------
{
	albaOpFreezeVME freeze; 
}
//-----------------------------------------------------------
void albaOpFreezeVMETest::TestFreezeVMESurfaceParametric() 
//-----------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();

  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->ReparentTo(root);
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  /* test surface parametric *//////////////////////////////////////////////////////////////////////////
  albaOpFreezeVME *freezeOp=new albaOpFreezeVME("freeze");
  freezeOp->TestModeOn();
  freezeOp->SetInput(vmeParametricSurfaceSTART);
  freezeOp->OpRun();

  albaVMESurface *surface=(albaVMESurface *)(freezeOp->GetOutput());
  surface->ReparentTo(root);
  surface->Update();

  CPPUNIT_ASSERT(surface && surface->GetOutput()->GetVTKData()->GetNumberOfPoints() == vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->GetNumberOfPoints());

  vmeParametricSurfaceSTART->ReparentTo(NULL);
  
  surface->ReparentTo(NULL);
  albaDEL(vmeParametricSurfaceSTART);
  cppDEL(freezeOp);
  albaDEL(storage);
}
//-----------------------------------------------------------
void albaOpFreezeVMETest::TestFreezeVMEMeter() 
//-----------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	//create a parametric surface
	albaVMESurfaceParametric *vmeParametricSurfaceSTART;
	albaNEW(vmeParametricSurfaceSTART);
	vmeParametricSurfaceSTART->ReparentTo(root);
	vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceSTART->Update();

	albaVMESurfaceParametric *vmeParametricSurfaceEND1;
	albaNEW(vmeParametricSurfaceEND1);	
	vmeParametricSurfaceEND1->ReparentTo(root);
	vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceEND1->Update();

	albaMatrix *matrix = vmeParametricSurfaceEND1->GetOutput()->GetAbsMatrix();
	enum {X,Y,Z};
	matrix->SetElement(X,3,4); //set a translation value on x axis of 4.0
	matrix->SetElement(Y,3,3); //set a translation value on x axis of 3.0

	
	albaOpFreezeVME *freezeOp=new albaOpFreezeVME("freeze");
	freezeOp->TestModeOn();
	
	
	albaVMEMeter *meter;
	albaNEW(meter);
	meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
	meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
	meter->ReparentTo(storage->GetRoot());
	//meter->GetOutput()->GetVTKData()->Update();
	meter->Modified();
	meter->Update();


	freezeOp->SetInput(meter);
	freezeOp->OpRun();

	albaVMEPolyline *polyline2=(albaVMEPolyline *)(freezeOp->GetOutput());
	polyline2->ReparentTo(root);
	polyline2->Update();
	CPPUNIT_ASSERT(polyline2 && polyline2->GetOutput()->GetVTKData()->GetNumberOfPoints() == meter->GetOutput()->GetVTKData()->GetNumberOfPoints());

	meter->ReparentTo(NULL);
	
	vmeParametricSurfaceSTART->ReparentTo(NULL);
	vmeParametricSurfaceEND1->ReparentTo(NULL);
	polyline2->ReparentTo(NULL);

  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);
	albaDEL(meter);

	cppDEL(freezeOp);
	albaDEL(storage);
}
//-----------------------------------------------------------
void albaOpFreezeVMETest::TestFreezeVMEWrappedMeter() 
//-----------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	//create a parametric surface
	albaVMESurfaceParametric *vmeParametricSurfaceSTART;
	albaNEW(vmeParametricSurfaceSTART);
	vmeParametricSurfaceSTART->ReparentTo(root);
	vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceSTART->Update();

	albaVMESurfaceParametric *vmeParametricSurfaceEND1;
	albaNEW(vmeParametricSurfaceEND1);	
	vmeParametricSurfaceEND1->ReparentTo(root);
	vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceEND1->Update();

	albaMatrix *matrix = vmeParametricSurfaceEND1->GetOutput()->GetAbsMatrix();
	enum {X,Y,Z};
	matrix->SetElement(X,3,4); //set a translation value on x axis of 4.0
	matrix->SetElement(Y,3,3); //set a translation value on x axis of 3.0
	
	albaOpFreezeVME *freezeOp=new albaOpFreezeVME("freeze");
	freezeOp->TestModeOn();
	
	albaVMEWrappedMeter *wrappedMeter;
	albaNEW(wrappedMeter);

	wrappedMeter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
	wrappedMeter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
	wrappedMeter->ReparentTo(storage->GetRoot());
	wrappedMeter->Modified();
	wrappedMeter->Update();


	freezeOp->SetInput(wrappedMeter);
	freezeOp->OpRun();

	albaVMEPolyline *polyline1=(albaVMEPolyline *)(freezeOp->GetOutput());
	polyline1->ReparentTo(root);
	polyline1->Update();
	CPPUNIT_ASSERT(polyline1 && polyline1->GetOutput()->GetVTKData()->GetNumberOfPoints() == wrappedMeter->GetOutput()->GetVTKData()->GetNumberOfPoints());
	
	wrappedMeter->ReparentTo(NULL);
	
	vmeParametricSurfaceSTART->ReparentTo(NULL);
	vmeParametricSurfaceEND1->ReparentTo(NULL);
	
	polyline1->ReparentTo(NULL);
	albaDEL(wrappedMeter);
	albaDEL(vmeParametricSurfaceSTART);
	albaDEL(vmeParametricSurfaceEND1);
	cppDEL(freezeOp);
	albaDEL(storage);
}

//-----------------------------------------------------------
void albaOpFreezeVMETest::TestFreezeVMEProfileSpline() 
//-----------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();


  albaVMEPolyline *vmePolyline;
  albaNEW(vmePolyline);	
  
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
  vtkIdType pointId[2];
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
  vmePolyline->SetData(polydata, 0.0);
  vmePolyline->ReparentTo(root);
  vmePolyline->Update();

  albaOpFreezeVME *freezeOp=new albaOpFreezeVME("freeze");
  freezeOp->TestModeOn();

  albaVMEPolylineSpline *spline;
  albaNEW(spline);
  spline->SetPolylineLink(vmePolyline);
  spline->ReparentTo(root);
  spline->Modified();
  spline->Update();

  freezeOp->SetInput(spline);
  freezeOp->OpRun();

  albaVMEPolyline *polyline2=(albaVMEPolyline *)(freezeOp->GetOutput());
  polyline2->ReparentTo(root);
  polyline2->Update();
  int value1 = spline->GetOutput()->GetVTKData()->GetNumberOfPoints();
  int value2 = polyline2->GetOutput()->GetVTKData()->GetNumberOfPoints();
  CPPUNIT_ASSERT(polyline2 && value1 == value2);

  spline->ReparentTo(NULL);
  vmePolyline->ReparentTo(NULL);
  polyline2->ReparentTo(NULL);

  vtkDEL(polydata);
  vtkDEL(points);
  vtkDEL(cells);
  
  albaDEL(vmePolyline);
  albaDEL(spline);
  
  cppDEL(freezeOp);
  albaDEL(storage);
}
//-----------------------------------------------------------
void albaOpFreezeVMETest::TestFreezeVMERefSys()
//-----------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();

  //create a parametric surface
  albaVMERefSys *vmeRefSys;
  albaNEW(vmeRefSys);
  vmeRefSys->ReparentTo(root);
  vmeRefSys->ReparentTo(storage->GetRoot());
  vmeRefSys->Update();

  // test surface parametric //////////////////////////////////////////////////////////////////////////
  albaOpFreezeVME *freezeOp=new albaOpFreezeVME("freeze");
  freezeOp->TestModeOn();
  freezeOp->SetInput(vmeRefSys);
  freezeOp->OpRun();

  albaVMESurface *surface=(albaVMESurface *)(freezeOp->GetOutput());
  surface->ReparentTo(root);
  surface->Update();

  int num1 = surface->GetOutput()->GetVTKData()->GetNumberOfPoints();
  int num2 = vmeRefSys->GetOutput()->GetVTKData()->GetNumberOfPoints();
  CPPUNIT_ASSERT(surface &&  num1 == num2);

  vmeRefSys->ReparentTo(NULL);

  surface->ReparentTo(NULL); 
  albaDEL(vmeRefSys);
  cppDEL(freezeOp);
  albaDEL(storage);
}