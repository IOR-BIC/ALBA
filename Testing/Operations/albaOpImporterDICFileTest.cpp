/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterDICFileTest
 Authors: Gianluigi Crimi
 
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

#include "albaOpImporterDICFileTest.h"
#include "albaOpImporterDICFile.h"


#include "albaString.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEPointCloud.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"

#include <string>
#include <assert.h>
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

//-----------------------------------------------------------
void albaOpImporterDICFileTest::Test() 
//-----------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();


	albaOpImporterDicFile *importer=new albaOpImporterDicFile("importer");
	importer->TestModeOn();

	albaString filename=ALBA_DATA_ROOT;
  filename<<"/PointCloud/PointCloud.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	importer->ImportFile();
	albaVMEPointCloud *node=(albaVMEPointCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfPoints() == 30);

  double xyz[3];
	node->GetPoint(19, xyz);

  
  double x = -12.349327;
  double y = 8.346818;
  double z = 0.964494;

  CPPUNIT_ASSERT( fabs(xyz[0] - x) < 0.01 &&
                  fabs(xyz[1] - y) < 0.01 &&
                  fabs(xyz[2] - z) < 0.01 );

	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());

	double scalarValue = polyData->GetPointData()->GetScalars("DIC")->GetTuple1(19);

	CPPUNIT_ASSERT(fabs(scalarValue - 0.159694) < 0.01);

  cppDEL(importer);
	albaDEL(storage);
}
