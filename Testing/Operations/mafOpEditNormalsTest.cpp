/*=========================================================================

 Program: MAF2
 Module: mafOpEditNormalsTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpEditNormalsTest.h"
#include "mafOpEditNormals.h"

#include "mafString.h"
#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkSTLReader.h"

#define EPSILON 0.00001

//-----------------------------------------------------------
void mafOpEditNormalsTest::Test() 
//-----------------------------------------------------------
{
	mafString filename=MAF_DATA_ROOT;
	filename<<"/STL/normals.stl";

	vtkMAFSmartPointer<vtkSTLReader> reader;
	reader->SetFileName(filename.GetCStr());
	reader->Update();
  mafSmartPointer<mafVMESurface> surface;
	surface->SetData(reader->GetOutput(),0.0);
	surface->GetOutput()->GetVTKData()->Update();
	surface->Update();

	vtkMAFSmartPointer<vtkPolyData>originalPolydata;
	originalPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
	originalPolydata->Update();

	mafOpEditNormals *editNormals1 = new mafOpEditNormals();
	editNormals1->TestModeOn();
	editNormals1->SetInput(surface);
	editNormals1->OpRun();
	editNormals1->OnGenerateNormals();
	editNormals1->OpDo();

	surface->GetOutput()->GetVTKData()->Update();
	
	vtkMAFSmartPointer<vtkPolyData> resultPolydata1;
	resultPolydata1->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData())); 
	resultPolydata1->Update();

	vtkFloatArray *pointNormals1=vtkFloatArray::SafeDownCast(resultPolydata1->GetPointData()->GetNormals());

	CPPUNIT_ASSERT(pointNormals1);

	mafOpEditNormals *editNormals2 = new mafOpEditNormals();
	editNormals2->TestModeOn();
	editNormals2->SetInput(surface);
	editNormals2->SetFlipNormalsOn();
	editNormals2->OpRun();
	editNormals2->OnGenerateNormals();
	editNormals2->OpDo();

	surface->GetOutput()->GetVTKData()->Update();

	vtkMAFSmartPointer<vtkPolyData> resultPolydata2;
	resultPolydata2->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
	resultPolydata2->Update();

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
			normalBetweenEpsilon==false;
			break;
		}
	}
	CPPUNIT_ASSERT( normalBetweenEpsilon);

	mafDEL(editNormals1);
	mafDEL(editNormals2);
}
//-----------------------------------------------------------
void mafOpEditNormalsTest::TestUndo1() 
//-----------------------------------------------------------
{
	mafString filename=MAF_DATA_ROOT;
	filename<<"/STL/normals.stl";

	vtkMAFSmartPointer<vtkSTLReader> reader;
	reader->SetFileName(filename.GetCStr());
	reader->Update();
	mafSmartPointer<mafVMESurface> surface;
	surface->SetData(reader->GetOutput(),0.0);
	surface->GetOutput()->GetVTKData()->Update();
	surface->Update();

	vtkMAFSmartPointer<vtkPolyData>originalPolydata;
	originalPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
	originalPolydata->Update();

	mafOpEditNormals *editNormals = new mafOpEditNormals();
	editNormals->TestModeOn();
	editNormals->SetFlipNormalsOn();
	editNormals->SetInput(surface);
	editNormals->OpRun();
	editNormals->OnGenerateNormals();
	editNormals->OpDo();

	surface->GetOutput()->GetVTKData()->Update();

	editNormals->OpUndo();

	surface->GetOutput()->GetVTKData()->Update();

	vtkMAFSmartPointer<vtkPolyData>resultPolydata1;
	resultPolydata1->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData())); 
	resultPolydata1->Update();

	vtkFloatArray *pointNormalsResult1=vtkFloatArray::SafeDownCast(resultPolydata1->GetPointData()->GetNormals());
	vtkFloatArray *pointNormalsOriginal1=vtkFloatArray::SafeDownCast(originalPolydata->GetPointData()->GetNormals());

	CPPUNIT_ASSERT(!pointNormalsResult1 && !pointNormalsOriginal1);

	mafDEL(editNormals);
}
//-----------------------------------------------------------
void mafOpEditNormalsTest::TestUndo2() 
//-----------------------------------------------------------
{
	mafString filename=MAF_DATA_ROOT;
	filename<<"/STL/normals.stl";

	vtkMAFSmartPointer<vtkSTLReader> reader;
	reader->SetFileName(filename.GetCStr());
	reader->Update();
	mafSmartPointer<mafVMESurface> surface;
	surface->SetData(reader->GetOutput(),0.0);
	surface->GetOutput()->GetVTKData()->Update();
	surface->Update();

	mafOpEditNormals *editNormals1 = new mafOpEditNormals();
	editNormals1->TestModeOn();
	editNormals1->SetFlipNormalsOn();
	editNormals1->SetInput(surface);
	editNormals1->OpRun();
	editNormals1->OnGenerateNormals();
	editNormals1->OpDo();

	surface->GetOutput()->GetVTKData()->Update();

	vtkMAFSmartPointer<vtkPolyData>originalPolydata;
	originalPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
	originalPolydata->Update();

	mafOpEditNormals *editNormals2 = new mafOpEditNormals();
	editNormals2->TestModeOn();
	editNormals2->SetInput(surface);
	editNormals2->SetFlipNormalsOn();
	editNormals2->OpRun();
	editNormals2->OnGenerateNormals();
	editNormals2->OpDo();

	surface->GetOutput()->GetVTKData()->Update();

	editNormals2->OpUndo();

	surface->GetOutput()->GetVTKData()->Update();

	vtkMAFSmartPointer<vtkPolyData>resultPolydata2;
	resultPolydata2->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
	resultPolydata2->Update();

	resultPolydata2->GetPointData()->Update();
	originalPolydata->GetPointData()->Update();

	vtkFloatArray *pointNormalsResult2=vtkFloatArray::SafeDownCast(resultPolydata2->GetPointData()->GetNormals());
	vtkFloatArray *pointNormalsOriginal2=vtkFloatArray::SafeDownCast(originalPolydata->GetPointData()->GetNormals());
	
	float *v1,*v2;
	bool vectorEquals=true;
	for(int i=0; i< pointNormalsResult2->GetSize()-1; i++)
	{
		v1=pointNormalsResult2->GetPointer(i);
		v2=pointNormalsOriginal2->GetPointer(i);
		if(v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
		{
			mafLogMessage("Different vector  num:%d/%d  v1[%f,%f,%f]  v2[%f,%f,%f]",i,pointNormalsResult2->GetSize(), v1[0],v1[1],v1[2], v2[0],v2[1],v2[2]);
			vectorEquals=false;
			break;
		}
	}
	CPPUNIT_ASSERT(vectorEquals);

	mafDEL(editNormals1);
	mafDEL(editNormals2);
}
