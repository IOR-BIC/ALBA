/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterVTKXMLTest
 Authors: Matteo Giacomoni
 
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
#include "medOpImporterVTKXMLTest.h"

#include "medOpImporterVTKXML.h"
#include "mafVMEGroup.h"
#include "medVMEPolylineGraph.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMEMesh.h"
#include "mafVMEPointSet.h"

#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"

//-----------------------------------------------------------
void medOpImporterVTKXMLTest::setUp() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::tearDown() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  mafDEL(importer);
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestAccept() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  mafVMEGroup *group;
  mafNEW(group);
  
  CPPUNIT_ASSERT(importer->Accept(group));
  
  mafDEL(group);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestSetFileName() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  importer->TestModeOn();
  char *fileName={"file name"};
  importer->SetFileName(fileName);

  CPPUNIT_ASSERT(strcmp(importer->GetFileName(),fileName)==0);
  
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestImportVTKPolylineGraph() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  importer->TestModeOn();
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/VTKXML/PolylineGraph.vtp";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  medVMEPolylineGraph *polylineGraph=medVMEPolylineGraph::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(polylineGraph!=NULL);

  polylineGraph->Modified();
  polylineGraph->Update();

  vtkPolyData *polyData=vtkPolyData::SafeDownCast(polylineGraph->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(polyData!=NULL);

  polyData->Modified();
  polyData->Update();

  CPPUNIT_ASSERT(polyData->GetNumberOfPoints()==5);
  CPPUNIT_ASSERT(polyData->GetNumberOfLines()==4);

  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestImportVTKVolumeSP() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  importer->TestModeOn();
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/VTKXML/volumeSP.vti";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(volume!=NULL);
  volume->Modified();
  volume->Update();

  vtkImageData *sp=vtkImageData::SafeDownCast(volume->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(sp!=NULL);

  sp->Modified();
  sp->Update();

  CPPUNIT_ASSERT(sp->GetNumberOfPoints()==1000);
  CPPUNIT_ASSERT(sp->GetNumberOfCells()==729);

  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestImportVTKVolumeRG() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  importer->TestModeOn();
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/VTKXML/volumeRG.vtr";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(volume!=NULL);
  volume->Modified();
  volume->Update();

  vtkRectilinearGrid *rg=vtkRectilinearGrid::SafeDownCast(volume->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(rg!=NULL);

  rg->Modified();
  rg->Update();

  CPPUNIT_ASSERT(rg->GetNumberOfPoints()==1000);
  CPPUNIT_ASSERT(rg->GetNumberOfCells()==729);

  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestImportVTKSurface() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  importer->TestModeOn();
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/VTKXML/sphere.vtp";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(surface!=NULL);
  surface->Modified();
  surface->Update();

  vtkPolyData *polydata=vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(polydata!=NULL);

  polydata->Modified();
  polydata->Update();

  CPPUNIT_ASSERT(polydata->GetNumberOfPoints()==50);
  CPPUNIT_ASSERT(polydata->GetNumberOfPolys()==96);

  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestImportVTKMesh() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  importer->TestModeOn();
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/VTKXML/mesh.vtu";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  mafVMEMesh *surface=mafVMEMesh::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(surface!=NULL);
  surface->Modified();
  surface->Update();

  vtkUnstructuredGrid *ug=vtkUnstructuredGrid::SafeDownCast(surface->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(ug!=NULL);

  ug->Modified();
  ug->Update();

  CPPUNIT_ASSERT(ug->GetNumberOfPoints()==5);
  CPPUNIT_ASSERT(ug->GetNumberOfCells()==2);

  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterVTKXMLTest::TestImportVTKPointSet() 
//-----------------------------------------------------------
{
  medOpImporterVTKXML *importer=new medOpImporterVTKXML();
  importer->TestModeOn();
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/VTKXML/pointset.vtp";
  importer->SetFileName(fileName);
  importer->ImportVTKXML();
  mafVMEPointSet *pointset=mafVMEPointSet::SafeDownCast(importer->GetOutput());

  CPPUNIT_ASSERT(pointset!=NULL);
  pointset->Modified();
  pointset->Update();

  vtkPolyData *pts=vtkPolyData::SafeDownCast(pointset->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(pts!=NULL);

  pts->Modified();
  pts->Update();

  CPPUNIT_ASSERT(pts->GetNumberOfPoints()==2);
  CPPUNIT_ASSERT(pts->GetNumberOfCells()==2);

  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}