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
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	albaOpImporterDicFile *importer=new albaOpImporterDicFile("importer");
	importer->TestModeOn();

	albaString filename=ALBA_DATA_ROOT;
  filename<<"/DIC/DicData.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	importer->ImportFile();
	albaVMEPointCloud *node=(albaVMEPointCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfPoints() == 20);

  double xyz[3];
	node->GetPoint(19, xyz);

  
  double x = 4.256419;
  double y = 3.118922;
	double z = 3.852819;

  CPPUNIT_ASSERT( fabs(xyz[0] - x) < 0.01 &&
                  fabs(xyz[1] - y) < 0.01 &&
                  fabs(xyz[2] - z) < 0.01 );

	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());

	double scalarValue = polyData->GetPointData()->GetScalars("Displ Magnitudo")->GetTuple1(19);

	CPPUNIT_ASSERT(fabs(scalarValue - 0.05344) < 0.01);

	albaDEL(storage);
	delete importer;
}

//-----------------------------------------------------------
void albaOpImporterDICFileTest::TestScalarsPreservation()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	// Import the DIC file
	albaOpImporterDicFile *importer = new albaOpImporterDicFile("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/DIC/DicData.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	importer->ImportFile();
	albaVMEPointCloud *node = (albaVMEPointCloud *)importer->GetOutput();

	CPPUNIT_ASSERT(node != NULL);
	CPPUNIT_ASSERT(node->GetNumberOfPoints() > 0);

	// Get the polydata and update
	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());
	polyData->Update();

	albaString scalarNames[] = { "Deformed Coordinates", "Displacements", "Displ Magnitudo", "Index X", "Index Y" };
	
	double targetValues[] = { 4.335968,3.194908,4.357641,4.350800,3.241916,4.047214,4.353354,3.248729,3.986236,4.357296,3.261731,3.924155,4.365315,3.289216,3.861467,4.374266,3.322722,3.796649,
															4.242736,3.094534,4.667981,4.252263,3.125966,4.544213,4.251788,3.122994,4.481005,4.249956,3.111884,4.420639,4.244765,3.087779,4.359333,4.243119,3.078506,4.297593,
															4.243430,3.076845,4.237264,4.246660,3.087963,4.173547,4.253085,3.112522,4.112544,4.255856,3.119669,4.048685,4.258237,3.126065,3.987345,4.259158,3.125789,3.925261,
															4.260650,3.127656,3.863140,4.260135,3.123123,3.799684,0.003822,0.003296,-0.053618,0.005615,0.013340,-0.053751,0.003898,0.006116,-0.053255,0.002504,0.000848,
															-0.052870,0.003588,0.003684,-0.052721,0.007883,0.021662,-0.052912,0.003734,0.003017,-0.053982,0.004537,0.005620,-0.053715,0.004472,0.007308,-0.053924,0.004793,
															0.007800,-0.053755,0.004011,0.004315,-0.053705,0.003828,0.003893,-0.053821,0.005515,0.010724,-0.053196,0.004506,0.007935,-0.053853,0.005386,0.013346,-0.053194,
															0.004706,0.009738,-0.053153,0.004197,0.007598,-0.052968,0.004207,0.006689,-0.053135,0.003368,0.002261,-0.052805,0.003716,0.004200,-0.053145,0.053855,0.055665,
															0.053746,0.052936,0.052972,0.057715,0.054195,0.054198,0.054600,0.054529,0.054027,0.054097,0.054546,0.054621,0.055106,0.054242,0.053675,0.053720,0.052960,0.053440,
															92.000000,97.000000,98.000000,99.000000,100.000000,101.000000,87.000000,89.000000,90.000000,91.000000,92.000000,93.000000,94.000000,95.000000,96.000000,97.000000,
															98.000000,99.000000,100.000000,101.000000,6.000000,6.000000,6.000000,6.000000,6.000000,6.000000,7.000000,7.000000,7.000000,7.000000,7.000000,7.000000,7.000000,
															7.000000,7.000000,7.000000,7.000000,7.000000,7.000000,7.000000 };
	int curTarget = 0;

	// Verify all scalars are present and have correct values
	for (int scalarIdx = 0; scalarIdx < 5; scalarIdx++)
	{
		vtkDataArray *scalars = polyData->GetPointData()->GetScalars(scalarNames[scalarIdx].GetCStr());
		
		CPPUNIT_ASSERT(scalars != NULL);

		int numComponents = scalars->GetNumberOfComponents();
		int numTuples = scalars->GetNumberOfTuples();

		// Verify each tuple and component
		for (int i = 0; i < numTuples; i++)
		{
			// Verify each component value for the specified point
			for (int comp = 0; comp < numComponents; comp++)
			{
				double scalarValue = scalars->GetComponent(i, comp);
				CPPUNIT_ASSERT(fabs(scalarValue - targetValues[curTarget]) < 0.01);
				curTarget++;
			}
		}
	}

	albaDEL(storage);
	delete importer;
}
