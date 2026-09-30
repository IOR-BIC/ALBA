/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditNormalsTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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
#include "albaOpEditNormalsTest.h"
#include "albaOpEditNormals.h"

#include "albaString.h"
#include "albaVMESurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkSTLReader.h"

#define EPSILON 0.00001

//-----------------------------------------------------------
void albaOpEditNormalsTest::Test() 
//-----------------------------------------------------------
{
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/STL/normals.stl";

	vtkALBASmartPointer<vtkSTLReader> reader;
	reader->SetFileName(filename.GetCStr());
	reader->Update();
  albaSmartPointer<albaVMESurface> surface;
	surface->SetData(reader->GetOutput(),0.0);
	surface->Update();

	vtkALBASmartPointer<vtkPolyData>originalPolydata;
	originalPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));

	albaOpEditNormals *editNormals1 = new albaOpEditNormals();
	editNormals1->TestModeOn();
	editNormals1->SetInput(surface);
	editNormals1->OpRun();
	editNormals1->OnGenerateNormals();
	editNormals1->OpDo();

	
	vtkALBASmartPointer<vtkPolyData> resultPolydata1;
	resultPolydata1->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData())); 

	vtkFloatArray *pointNormals1=vtkFloatArray::SafeDownCast(resultPolydata1->GetPointData()->GetNormals());

	CPPUNIT_ASSERT(pointNormals1);

	albaOpEditNormals *editNormals2 = new albaOpEditNormals();
	editNormals2->TestModeOn();
	editNormals2->SetInput(surface);
	editNormals2->SetFlipNormalsOn();
	editNormals2->OpRun();
	editNormals2->OnGenerateNormals();
	editNormals2->OpDo();


	vtkALBASmartPointer<vtkPolyData> resultPolydata2;
	resultPolydata2->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));

	vtkFloatArray *pointNormals2=vtkFloatArray::SafeDownCast(resultPolydata2->GetPointData()->GetNormals());

	bool normalBetweenEpsilon=true;
	float *v1,*v2;
  for(int i=0; i< pointNormals1->GetSize(); i++)
	{
		v1=pointNormals1->GetPointer(i);
		v2=pointNormals2->GetPointer(i);
		float norm1[3],norm2[3];
		norm1[0] = v1[0];
		norm1[1] = v1[1];
		norm1[2] = v1[2];
		norm2[0] = v2[0];
		norm2[1] = v2[1];
		norm2[2] = v2[2];
		vtkMath::Normalize(norm1);
		vtkMath::Normalize(norm2);
		double result = vtkMath::Dot(norm1,norm2);
		if( !(result+EPSILON >= -1 || result-EPSILON <= -1) )
		{
			normalBetweenEpsilon=false;
			break;
		}
	}
	CPPUNIT_ASSERT( normalBetweenEpsilon);

	albaDEL(editNormals1);
	albaDEL(editNormals2);
}
//-----------------------------------------------------------
void albaOpEditNormalsTest::TestUndo1() 
//-----------------------------------------------------------
{
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/STL/normals.stl";

	vtkALBASmartPointer<vtkSTLReader> reader;
	reader->SetFileName(filename.GetCStr());
	reader->Update();
	albaSmartPointer<albaVMESurface> surface;
	surface->SetData(reader->GetOutput(),0.0);
	surface->Update();

	vtkALBASmartPointer<vtkPolyData>originalPolydata;
	originalPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));

	albaOpEditNormals *editNormals = new albaOpEditNormals();
	editNormals->TestModeOn();
	editNormals->SetFlipNormalsOn();
	editNormals->SetInput(surface);
	editNormals->OpRun();
	editNormals->OnGenerateNormals();
	editNormals->OpDo();


	editNormals->OpUndo();


	vtkALBASmartPointer<vtkPolyData>resultPolydata1;
	resultPolydata1->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData())); 

	vtkFloatArray *pointNormalsResult1=vtkFloatArray::SafeDownCast(resultPolydata1->GetPointData()->GetNormals());
	vtkFloatArray *pointNormalsOriginal1=vtkFloatArray::SafeDownCast(originalPolydata->GetPointData()->GetNormals());

	CPPUNIT_ASSERT(!pointNormalsResult1 && !pointNormalsOriginal1);

	albaDEL(editNormals);
}
//-----------------------------------------------------------
void albaOpEditNormalsTest::TestUndo2() 
//-----------------------------------------------------------
{
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/STL/normals.stl";

	vtkALBASmartPointer<vtkSTLReader> reader;
	reader->SetFileName(filename.GetCStr());
	reader->Update();
	albaSmartPointer<albaVMESurface> surface;
	surface->SetData(reader->GetOutput(),0.0);
	surface->Update();

	albaOpEditNormals *editNormals1 = new albaOpEditNormals();
	editNormals1->TestModeOn();
	editNormals1->SetFlipNormalsOn();
	editNormals1->SetInput(surface);
	editNormals1->OpRun();
	editNormals1->OnGenerateNormals();
	editNormals1->OpDo();


	vtkALBASmartPointer<vtkPolyData>originalPolydata;
	originalPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));

	albaOpEditNormals *editNormals2 = new albaOpEditNormals();
	editNormals2->TestModeOn();
	editNormals2->SetInput(surface);
	editNormals2->SetFlipNormalsOn();
	editNormals2->OpRun();
	editNormals2->OnGenerateNormals();
	editNormals2->OpDo();


	editNormals2->OpUndo();


	vtkALBASmartPointer<vtkPolyData>resultPolydata2;
	resultPolydata2->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));

	resultPolydata2->GetPointData()->Update();
	originalPolydata->GetPointData()->Update();

	vtkFloatArray *pointNormalsResult2=vtkFloatArray::SafeDownCast(resultPolydata2->GetPointData()->GetNormals());
	vtkFloatArray *pointNormalsOriginal2=vtkFloatArray::SafeDownCast(originalPolydata->GetPointData()->GetNormals());
	
	double *v1,*v2;
	bool vectorEquals=true;
	for(int i=0; i< pointNormalsResult2->GetNumberOfTuples()-1; i++)
	{
		v1=pointNormalsResult2->GetTuple(i);
		v2=pointNormalsOriginal2->GetTuple(i);
		if(v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
		{
			albaLogMessage("Different vector  num:%d/%d  v1[%f,%f,%f]  v2[%f,%f,%f]",i, pointNormalsResult2->GetNumberOfTuples(), v1[0],v1[1],v1[2], v2[0],v2[1],v2[2]);
			vectorEquals=false;
			break;
		}
	}
	CPPUNIT_ASSERT(vectorEquals);

	albaDEL(editNormals1);
	albaDEL(editNormals2);
}
