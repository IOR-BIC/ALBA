/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESlicerTest
 Authors: Stefano Perticoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaVMESlicerTest.h"

#include "albaVMERoot.h"
#include "albaVMESlicer.h"
#include "albaVMEVolumeGray.h"
#include "albaTransform.h"
#include "albaVMEOutputSurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkOutlineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkTexture.h"
#include "vtkDataSetReader.h"
#include "vtkImageActor.h"
#include "vtkDataSetWriter.h"
#include "vtkImageData.h"

#include <iostream>
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"

#define TEST_RESULT CPPUNIT_CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaVMESlicerTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaVMESlicerTest::TestBug2454And2524Regression()
//----------------------------------------------------------------------------
{
	// create a small tree with a root, a volume and a slicer 
	albaSmartPointer<albaVMERoot> root;
	root->SetName("ROOT");

	albaSmartPointer<albaVMESlicer> vmeSlicer;
	albaSmartPointer<albaVMEVolumeGray> volumeToSlice;

	vmeSlicer->SetName("slicer");
	volumeToSlice->SetName("volume");

	root->AddChild(volumeToSlice);
	volumeToSlice->AddChild(vmeSlicer);	
	vmeSlicer->SetSlicedVMELink(volumeToSlice);

	vtkALBASmartPointer<vtkDataSetReader> datasetReader;
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/VTK_Volumes/CropTestVolumeSP.vtk";
	datasetReader->SetFileName(filename);

	datasetReader->Update();

	int numPoints = datasetReader->GetOutput()->GetNumberOfPoints();

	volumeToSlice->SetData((vtkImageData *)datasetReader->GetStructuredPointsOutput(), -1);

	albaSmartPointer<albaTransform> trans;

	vtkALBASmartPointer<vtkRenderer> renderer;
	vtkALBASmartPointer<vtkRenderWindow> renWin;
	renWin->AddRenderer(renderer);

	renWin->SetSize(1000,1000);
	renderer->SetBackground( 0.1, 0.2, 0.4 );

	vmeSlicer->Update();

	vtkALBASmartPointer<vtkPolyDataMapper> slicerMapper;

	vtkDataSet *slicerData = vmeSlicer->GetSurfaceOutput()->GetVTKData();
	CPPUNIT_ASSERT(slicerData);

	vtkPolyData *surfaceData = vmeSlicer->GetSurfaceOutput()->GetSurfaceData();
	CPPUNIT_ASSERT(surfaceData);

	numPoints = surfaceData->GetNumberOfPoints();
	CPPUNIT_ASSERT(numPoints == 4);

	slicerMapper->SetInputData(surfaceData);

	vtkALBASmartPointer<vtkTexture> slicerTexure;
	vtkImageData *texture = vmeSlicer->GetSurfaceOutput()->GetTexture();
	CPPUNIT_ASSERT(texture);
	slicerTexure->SetInputData(texture);

	vtkALBASmartPointer<vtkActor> slicerActor;
	slicerActor->SetMapper(slicerMapper);
	slicerActor->SetTexture(slicerTexure);

	renderer->AddActor(slicerActor);

	renWin->Render();

	double pos[3] = {-1.47, -1.47, -1.47};
	trans->SetPosition(pos);

	double or[3] = {0,90,0};
	trans->SetOrientation(or);

	vmeSlicer->SetAbsMatrix(trans->GetMatrix());

	texture = vmeSlicer->GetSurfaceOutput()->GetTexture();
	CPPUNIT_ASSERT(texture);

	surfaceData = vmeSlicer->GetSurfaceOutput()->GetSurfaceData();
	CPPUNIT_ASSERT(surfaceData);

	double p[3];
	surfaceData->GetPoints()->GetPoint(0, p);
	surfaceData->GetPoints()->GetPoint(1, p);
	surfaceData->GetPoints()->GetPoint(2, p);
	surfaceData->GetPoints()->GetPoint(3, p);

	vtkPointData *pd = texture->GetPointData();

	vtkUnsignedCharArray *da = vtkUnsignedCharArray::SafeDownCast(pd->GetScalars());
	CPPUNIT_ASSERT(da);

	int numTuples = da->GetNumberOfTuples();

	unsigned char value = da->GetValue(100011);
	CPPUNIT_ASSERT(value == 43);

	value = da->GetValue(200087);
	CPPUNIT_ASSERT(value == 108);

	value = da->GetValue(240289);
	CPPUNIT_ASSERT(value == 48);

	renWin->Render();

	double bb[6] ;

	//expected results
	double bbRes[6] = { -123.52912139892578, 123.52999877929688, -123.52999877929688, 123.52912139892578, -2.8610257274408468e-08,-2.8610202207346447e-08 };

	slicerActor->GetBounds(bb);
	CPPUNIT_ASSERT(bb[0] == bbRes[0]);
	CPPUNIT_ASSERT(bb[1] == bbRes[1]);
	CPPUNIT_ASSERT(bb[2] == bbRes[2]);
	CPPUNIT_ASSERT(bb[3] == bbRes[3]);
	CPPUNIT_ASSERT(bb[4] == bbRes[4]);
	CPPUNIT_ASSERT(bb[5] == bbRes[5]);

}