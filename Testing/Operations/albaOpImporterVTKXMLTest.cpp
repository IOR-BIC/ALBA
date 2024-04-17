/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVTKXMLTest
 Authors: Matteo Giacomoni
 
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
#include "albaOpImporterVTKXMLTest.h"

#include "albaOpImporterVTKXML.h"
#include "albaVMEGroup.h"
#include "albaVMEPolylineGraph.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMEMesh.h"
#include "albaVMEPointSet.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"

//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::BeforeTest() 
//-----------------------------------------------------------
{
	//required by the importer
	vtkObject::GlobalWarningDisplayOn();
}

//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestAccept() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
  albaVMEGroup *group;
  albaNEW(group);
  
  CPPUNIT_ASSERT(importer->Accept(group));
  
  albaDEL(group);
  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestSetFileName() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
  importer->TestModeOn();
  char *fileName={"file name"};
  importer->SetFileName(fileName);

  CPPUNIT_ASSERT(strcmp(importer->GetFileName(),fileName)==0);
  
  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestImportVTKPolylineGraph() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
	
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/VTKXML/PolylineGraph.vtp";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  albaVMEPolylineGraph *polylineGraph=albaVMEPolylineGraph::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(polylineGraph!=NULL);

  polylineGraph->Modified();
  polylineGraph->Update();

  vtkPolyData *polyData=vtkPolyData::SafeDownCast(polylineGraph->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(polyData!=NULL);
  CPPUNIT_ASSERT(polyData->GetNumberOfPoints()==5);
  CPPUNIT_ASSERT(polyData->GetNumberOfLines()==4);

  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestImportVTKVolumeSP() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/VTKXML/volumeSP.vti";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  albaVMEVolumeGray *volume=albaVMEVolumeGray::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(volume!=NULL);
  volume->Modified();
  volume->Update();

  vtkImageData *sp=vtkImageData::SafeDownCast(volume->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(sp!=NULL);
  CPPUNIT_ASSERT(sp->GetNumberOfPoints()==1000);
  CPPUNIT_ASSERT(sp->GetNumberOfCells()==729);

  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestImportVTKVolumeRG() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/VTKXML/volumeRG.vtr";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  albaVMEVolumeGray *volume=albaVMEVolumeGray::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(volume!=NULL);
  volume->Modified();
  volume->Update();

  vtkRectilinearGrid *rg=vtkRectilinearGrid::SafeDownCast(volume->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(rg!=NULL);
  CPPUNIT_ASSERT(rg->GetNumberOfPoints()==1000);
  CPPUNIT_ASSERT(rg->GetNumberOfCells()==729);

  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestImportVTKSurface() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/VTKXML/sphere.vtp";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  albaVMESurface *surface=albaVMESurface::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(surface!=NULL);
  surface->Modified();
  surface->Update();

  vtkPolyData *polydata=vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(polydata!=NULL);
  CPPUNIT_ASSERT(polydata->GetNumberOfPoints()==50);
  CPPUNIT_ASSERT(polydata->GetNumberOfPolys()==96);

  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestImportVTKMesh() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/VTKXML/mesh.vtu";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  albaVMEMesh *surface=albaVMEMesh::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(surface!=NULL);
  surface->Modified();
  surface->Update();

  vtkUnstructuredGrid *ug=vtkUnstructuredGrid::SafeDownCast(surface->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(ug!=NULL);
  CPPUNIT_ASSERT(ug->GetNumberOfPoints()==5);
  CPPUNIT_ASSERT(ug->GetNumberOfCells()==2);

  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterVTKXMLTest::TestImportVTKPointSet() 
//-----------------------------------------------------------
{
  albaOpImporterVTKXML *importer=new albaOpImporterVTKXML();
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/VTKXML/pointset.vtp";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  albaVMEPointSet *pointset=albaVMEPointSet::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(pointset!=NULL);
  pointset->Modified();
  pointset->Update();

  vtkPolyData *pts=vtkPolyData::SafeDownCast(pointset->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(pts!=NULL);
  CPPUNIT_ASSERT(pts->GetNumberOfPoints()==2);
  CPPUNIT_ASSERT(pts->GetNumberOfCells()==2);

  albaDEL(importer);
}