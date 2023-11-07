/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterRAWTest
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
#include "albaOpExporterRAWTest.h"

#include "albaOpExporterRAW.h"
#include "albaOpImporterRAWVolume.h"
#include "albaOpImporterVTK.h"

#include "albaString.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"

#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkShortArray.h"

#include "wx/filename.h"

//-----------------------------------------------------------
void albaOpExporterRAWTest::Test() 
//-----------------------------------------------------------
{
	//Initialize storage
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	//Import a RAW Volume
	albaOpImporterRAWVolume *importer=new albaOpImporterRAWVolume("importer");
	importer->TestModeOn();
  importer->SetInput(storage->GetRoot());
	albaString FilenameImporter=ALBA_DATA_ROOT;
  FilenameImporter<<"/RAW_Volume/Volume.raw";
	importer->SetFileName(FilenameImporter.GetCStr());
	int Dimensions[3]={512,512,5};
	importer->SetDataDimensions(Dimensions);
	double Spacing[3]={0.3,0.5,1};
	importer->SetDataSpacing(Spacing);
	importer->SetScalarType(importer->SHORT_SCALAR);
	importer->ScalarSignedOn();
	importer->SetScalarDataToLittleEndian();
	int VOI[2]={0,5};
	importer->SetDataVOI(VOI);
	bool ok=importer->Import();
	CPPUNIT_ASSERT(ok);

	albaVMEVolumeGray *vmeVolumeGrayImported = albaVMEVolumeGray::SafeDownCast(importer->GetOutput());
	vmeVolumeGrayImported->Update();
	((vtkImageData *)(vmeVolumeGrayImported->GetOutput()->GetVTKData()))->UpdateData();

	//Initialize exporter and export the volume imported
	albaOpExporterRAW *exporter=new albaOpExporterRAW("exporter raw");
	exporter->TestModeOn();
	exporter->SetInput(vmeVolumeGrayImported);
	
	albaString FilenameExporter = GET_TEST_DATA_DIR();
  FilenameExporter<<"/ExportTest1.raw";
	exporter->SetFileName(FilenameExporter);
	
	albaString filenameZ = GET_TEST_DATA_DIR();
  filenameZ<<"/ExportZTest1.txt";
	exporter->SetFileZ(filenameZ);
	exporter->SaveVolume();

	//Import the volume exported

  albaOpImporterRAWVolume *importer2=new albaOpImporterRAWVolume("importer");
  importer2->TestModeOn();
	importer2->SetInput(storage->GetRoot());
	wxString path,name,ext;
	wxFileName::SplitPath(FilenameExporter.GetCStr(),&path,&name,&ext);
	path+= "/";
	wxString FilenameCHECK = albaString::Format("%s%s_%dx%dx%d.raw",path,name,Dimensions[0],Dimensions[1],Dimensions[2]);
	importer2->SetFileName(FilenameCHECK);
	importer2->SetDataDimensions(Dimensions);
	importer2->SetDataSpacing(Spacing);
	importer2->SetScalarType(importer->SHORT_SCALAR);
	importer2->ScalarSignedOn();
	importer2->SetScalarDataToLittleEndian();
	importer2->SetDataVOI(VOI);
	ok=importer2->Import();
	CPPUNIT_ASSERT(ok);

	albaVMEVolumeGray *vmeVolumeGrayExported = albaVMEVolumeGray::SafeDownCast(importer2->GetOutput());
	vmeVolumeGrayExported->Update();
	((vtkRectilinearGrid *)(vmeVolumeGrayExported->GetOutput()->GetVTKData()))->UpdateData();

	vmeVolumeGrayImported->SetName("");
	vmeVolumeGrayExported->SetName("");

	//Check if the volume imported and the volume exported are equal
	vtkImageData *DataExported=vtkImageData::SafeDownCast(vmeVolumeGrayExported->GetOutput()->GetVTKData());
	DataExported->UpdateData();
	DataExported->ComputeBounds();
	CPPUNIT_ASSERT(DataExported);

	vtkImageData *DataImported=vtkImageData::SafeDownCast(vmeVolumeGrayImported->GetOutput()->GetVTKData());
	DataImported->UpdateData();
	DataImported->ComputeBounds();
	CPPUNIT_ASSERT(DataImported);

	CPPUNIT_ASSERT(vmeVolumeGrayImported->Equals(vmeVolumeGrayExported));
	CPPUNIT_ASSERT(DataImported->GetNumberOfCells() == DataExported->GetNumberOfCells());
	CPPUNIT_ASSERT(DataImported->GetNumberOfPoints() == DataExported->GetNumberOfPoints());
	double bI[6],bE[6];
	vmeVolumeGrayImported->GetOutput()->GetVTKData()->GetBounds(bI);
	vmeVolumeGrayExported->GetOutput()->GetVTKData()->GetBounds(bE);
	CPPUNIT_ASSERT( bI[0]==bE[0] && bI[1]==bE[1] && bI[2]==bE[2] && bI[3]==bE[3] && bI[4]==bE[4] && bI[5]==bE[5] );

	//Check Point to Point
	vtkShortArray *ScalarsImported;
	vtkShortArray *ScalarsExported;
	CPPUNIT_ASSERT(DataImported->GetPointData()->GetScalars()->GetDataType()==VTK_SHORT);
	ScalarsImported=(vtkShortArray*)(DataImported->GetPointData()->GetScalars());
	ScalarsExported=(vtkShortArray*)(DataExported->GetPointData()->GetScalars());

	bool sameScalars=true;
	for(long i=0;i<DataImported->GetNumberOfPoints();i++)
	{
		if(ScalarsImported->GetValue(i)!=ScalarsExported->GetValue(i))
			sameScalars=false;
	}

	CPPUNIT_ASSERT(sameScalars);


  DataExported = NULL;
  DataImported = NULL;
	vmeVolumeGrayImported = NULL;
	vmeVolumeGrayExported = NULL;
	albaDEL(exporter);
  albaDEL(importer2);
	albaDEL(importer);
	albaDEL(storage);
}
//-----------------------------------------------------------
void albaOpExporterRAWTest::TestRG() 
//-----------------------------------------------------------
{
	//Initialize storage
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	//Import a volume RG
	albaOpImporterVTK *importerVTK=new albaOpImporterVTK("importer VTK");
	importerVTK->TestModeOn();
	importerVTK->SetInput(storage->GetRoot());
	albaString Filename=ALBA_DATA_ROOT;
  Filename<<"/VTK_Volumes/CropTestVolumeRG.vtk";
	importerVTK->SetFileName(Filename.GetCStr());
	importerVTK->ImportVTK();

	albaVMEVolumeGray *vmeVolumeGrayImported=albaVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	vmeVolumeGrayImported->Update();
	((vtkRectilinearGrid *)(vmeVolumeGrayImported->GetOutput()->GetVTKData()))->UpdateData();
	CPPUNIT_ASSERT(vmeVolumeGrayImported);

	double bounds[6];
	vmeVolumeGrayImported->GetOutput()->GetBounds(bounds);

	double xmin = bounds[0];
	double xmax = bounds[1];
	double ymin = bounds[2];
	double ymax = bounds[3];
	double zmin = bounds[4];
	double zmax = bounds[5];	

	int Dimensions[3];
	((vtkRectilinearGrid *)(vmeVolumeGrayImported->GetOutput()->GetVTKData()))->GetDimensions(Dimensions);

	double Spacing[3];
	Spacing[0] = (xmax-xmin)/Dimensions[0];
	Spacing[1] = (ymax-ymin)/Dimensions[1];
	Spacing[2] = 1;

	//Initialize exporter and export the volume imported
	albaOpExporterRAW *exporter=new albaOpExporterRAW("exporter raw");
	exporter->TestModeOn();
	exporter->SetInput(vmeVolumeGrayImported);
	
	albaString FilenameExporter = GET_TEST_DATA_DIR();
	FilenameExporter << "/ExportTest2.raw";
	exporter->SetFileName(FilenameExporter);
	
	albaString filenameZ = GET_TEST_DATA_DIR();
  filenameZ<<"/ExportZTest2.txt";
	exporter->SetFileZ(filenameZ);
	exporter->SaveVolume();

	//Import the volume exported
	albaOpImporterRAWVolume *importer=new albaOpImporterRAWVolume("importer");
	importer->TestModeOn();
  importer->SetInput(storage->GetRoot());
	wxString path,name,ext;
	wxFileName::SplitPath(FilenameExporter.GetCStr(),&path,&name,&ext);
	path+= "/";
	wxString FilenameCHECK = albaString::Format("%s%s_%dx%dx%d.raw",path,name,Dimensions[0],Dimensions[1],Dimensions[2]);
	importer->SetFileName(FilenameCHECK);
	importer->SetDataDimensions(Dimensions);
	importer->SetDataSpacing(Spacing);
	importer->SetScalarType(importer->SHORT_SCALAR);
	importer->ScalarSignedOn();
	importer->SetScalarDataToLittleEndian();
	int VOI[2]={0,Dimensions[2]};
	importer->SetDataVOI(VOI);
	bool ok=importer->Import();
	CPPUNIT_ASSERT(ok);

	albaVMEVolumeGray *vmeVolumeGrayExported = albaVMEVolumeGray::SafeDownCast(importer->GetOutput());
	vmeVolumeGrayExported->Update();
	((vtkImageData *)(vmeVolumeGrayExported->GetOutput()->GetVTKData()))->UpdateData();

	//Check if the volume imported and the volume exported are equal
	vtkImageData *DataExported=vtkImageData::SafeDownCast(vmeVolumeGrayExported->GetOutput()->GetVTKData());
	DataExported->UpdateData();
	DataExported->ComputeBounds();
	CPPUNIT_ASSERT(DataExported);

	vtkRectilinearGrid *DataImported=vtkRectilinearGrid::SafeDownCast(vmeVolumeGrayImported->GetOutput()->GetVTKData());
	DataImported->UpdateData();
	DataImported->ComputeBounds();
	CPPUNIT_ASSERT(DataImported);

	//Check spacing
	double NewSpacing[3];
	DataExported->GetSpacing(NewSpacing);
	CPPUNIT_ASSERT(NewSpacing[0]==Spacing[0]);
	CPPUNIT_ASSERT(NewSpacing[1]==Spacing[1]);
	CPPUNIT_ASSERT(NewSpacing[2]==Spacing[2]);

	//Check number of points
	int NewDimensions[3];
	DataExported->GetDimensions(NewDimensions);
	CPPUNIT_ASSERT(NewDimensions[0]==Dimensions[0]);
	CPPUNIT_ASSERT(NewDimensions[1]==Dimensions[1]);
	CPPUNIT_ASSERT(NewDimensions[2]==Dimensions[2]);

	CPPUNIT_ASSERT(DataImported->GetNumberOfCells() == DataExported->GetNumberOfCells());
	CPPUNIT_ASSERT(DataImported->GetNumberOfPoints() == DataExported->GetNumberOfPoints());

	//Check Point to Point
	vtkShortArray *ScalarsImported;
	vtkShortArray *ScalarsExported;
	CPPUNIT_ASSERT(DataImported->GetPointData()->GetScalars()->GetDataType()==VTK_SHORT);
	ScalarsImported=(vtkShortArray*)(DataImported->GetPointData()->GetScalars());
	ScalarsExported=(vtkShortArray*)(DataExported->GetPointData()->GetScalars());
	
	bool sameScalars=true;
	for(long i=0;i<DataImported->GetNumberOfPoints();i++)
	{
		if(ScalarsImported->GetValue(i)!=ScalarsExported->GetValue(i))
			sameScalars=false;
	}

	CPPUNIT_ASSERT(sameScalars);

  DataExported = NULL;
  DataImported = NULL;
	vmeVolumeGrayImported = NULL;
	vmeVolumeGrayExported = NULL;
	albaDEL(exporter);
	albaDEL(importerVTK);
	albaDEL(importer);
	albaDEL(storage);
}
