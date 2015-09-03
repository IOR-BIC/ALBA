/*=========================================================================

 Program: MAF2
 Module: mafVMEMeshAnsysTextExporterTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEMeshAnsysTextExporterTest_H__
#define __CPP_UNIT_mafVMEMeshAnsysTextExporterTest_H__

#include "mafTest.h"
#include "mafString.h"
#include "mafVMEMeshAnsysTextExporter.h"
#include "mafVMEMeshAnsysTextImporter.h"

class vtkFileOutputWindow;
class vtkUnstructuredGrid;

class mafVMEMeshAnsysTextExporterTest : public mafTest
{

public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafVMEMeshAnsysTextExporterTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestTetra10TrivialMeshExport );
  CPPUNIT_TEST( TestTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExport );
  CPPUNIT_TEST( TestBonemattedTetra10ElementsIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExport);
  CPPUNIT_TEST( TestBonemattedTetra10ElementsIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExportWithAppliedPose );
  CPPUNIT_TEST( TestBonemattedTetra10ElementsIdJumpingNoMaterialsNoTimeVarExport);
  CPPUNIT_TEST(TestExportTetra10VtkWithoutAnsysInformation);
  CPPUNIT_TEST(TestExportTetra4VtkWithoutAnsysInformation);
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestConstructorDestructor();
  void TestTetra10TrivialMeshExport();
  void TestTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExport();
  void TestBonemattedTetra10ElementsIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExport();
  void TestBonemattedTetra10ElementsIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExportWithAppliedPose();
  void TestBonemattedTetra10ElementsIdJumpingNoMaterialsNoTimeVarExport();
  void TestExportTetra10VtkWithoutAnsysInformation();
  void TestExportTetra4VtkWithoutAnsysInformation();

  // dataType
  enum
  {
    POINT_DATA = 0,
    CELL_DATA,
  };

private:
  
  void ReadAndDisplay(mafString &dirPrefix, int dataType = 0, bool readMaterials = false, bool writeToDisk = false);
  void SaveUnstructuredGridToFile(mafString &dirPrefix, mafString &fileName, vtkUnstructuredGrid *data);

  void RenderData( vtkUnstructuredGrid *data, int dataType = 0);
  void Read( mafVMEMeshAnsysTextImporter *reader, mafString &dirPrefix, \
bool readMaterials /*= false*/, bool writeToDisk /*= false*/, mafString &outputFileName , \
mafString inputNLISTFileName = "NLIST.lis", mafString inputELISTFileName = "ELIST.lis", \
mafString mafinputMPLISTFileName = "MPLIST.LIS");

  void WriteNodesFile(vtkUnstructuredGrid *ugridconst, const char *outputDirPrefix, const char *outputFileName);
  int  GetRowsNumber(vtkUnstructuredGrid *inputUGrid);
	void WriteElementsFile(vtkUnstructuredGrid *inputUGrid, const char *outputDirPrefix, const char *outputFileName);
	void WriteMaterialsFile(vtkUnstructuredGrid *inputUGrid, const char *outputDirPrefix, const char *outputFileName);
  
  vtkFileOutputWindow               *m_Log;

};

#endif
