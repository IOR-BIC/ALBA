/*=========================================================================

 Program: MAF2
 Module: mafOpCropTest
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

#include "mafOpCrop.h"
#include "mafOpCropTest.h"

#include "mafVME.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "mafNode.h"
#include "mafOpImporterVTK.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"

#include "vtkRectilinearGrid.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkRectilinearGridReader.h"
#include "vtkStructuredPoints.h"

//-----------------------------------------------------------
void mafOpCropTest::TestCropRG() 
//-----------------------------------------------------------
{
	//Import VTK data
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	mafOpImporterVTK *Importer=new mafOpImporterVTK("importer");
	mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/CropTestVolumeRG.vtk";
	Importer->TestModeOn();
	Importer->SetFileName(filename);
	Importer->SetInput(storage->GetRoot());
	Importer->ImportVTK();
	mafVMEVolumeGray *Input=mafVMEVolumeGray::SafeDownCast(Importer->GetOutput());
	Input->Update();
	Input->GetOutput()->GetVTKData()->Update();

	vtkRectilinearGrid *rgrid = vtkRectilinearGrid::SafeDownCast(Input->GetOutput()->GetVTKData());

	// Compute Volume Spacing
	double volumeSpacing[3]={VTK_DOUBLE_MAX,VTK_DOUBLE_MAX,VTK_DOUBLE_MAX};
	double spcx = VTK_DOUBLE_MIN;
	double spcy = VTK_DOUBLE_MIN;
	double spcz = VTK_DOUBLE_MIN;
	for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples (); xi++)
	{
		double spcx = rgrid->GetXCoordinates()->GetTuple1(xi)-rgrid->GetXCoordinates()->GetTuple1(xi-1);
		if (volumeSpacing[0] > spcx)
			volumeSpacing[0] = spcx;
	}

	for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
	{
		double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
		if (volumeSpacing[1] > spcy)
			volumeSpacing[1] = spcy;
	}

	for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
	{
		double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
		if (volumeSpacing[2] > spcz)
			volumeSpacing[2] = spcz;
	}

	//Creating a cropping area
	double b_in[6],b_out[6];
	b_in[0] = 40;
	b_in[1] = 140.50;
	b_in[2] = 145.50;
	b_in[3] = 250.60;
	b_in[4] = -300;
	b_in[5] = -190;

	//Crop operation
	mafOpCrop *Crop=new mafOpCrop("Crop");
	Crop->TestModeOn();
	Crop->SetInput(Input);
	Crop->SetCroppingBoxBounds(b_in);
	Crop->OpRun();
	Crop->Crop();
	Crop->OpDo();

	Input->Update();
	Input->GetOutput()->Update();
	Input->GetOutput()->GetVTKData()->Update();
	Input->GetOutput()->GetVTKData()->ComputeBounds();
	Input->GetOutput()->GetVTKData()->GetBounds(b_out);
	
	//Compare Bounds
	for(int i=0;i<3;i++)
	{
		CPPUNIT_ASSERT((b_in[2*i]-volumeSpacing[i]<b_out[2*i])&&(b_in[2*i+1]+volumeSpacing[i]>b_out[2*i+1]));
	}

	mafDEL(Crop);
	mafDEL(Importer);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpCropTest::TestCropSP() 
//-----------------------------------------------------------
{
	//Import VTK data
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	mafOpImporterVTK *Importer=new mafOpImporterVTK("importer");
	mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/CropTestVolumeSP.vtk";
	Importer->TestModeOn();
	Importer->SetFileName(filename);
	Importer->SetInput(storage->GetRoot());
	Importer->ImportVTK();
	mafVMEVolumeGray *Input=mafVMEVolumeGray::SafeDownCast(Importer->GetOutput());
	Input->Update();
	Input->GetOutput()->GetVTKData()->Update();

	//Creating a cropping area
	double b_in[6],b_out[6],b_original[6];
	Input->GetOutput()->GetBounds(b_original);

	b_in[0] = -100.50;
	b_in[1] = 100.50;
	b_in[2] = -100.50;
	b_in[3] = 100.50;
	b_in[4] = -100.50;
	b_in[5] = 100.50;

	//Crop operation
	mafOpCrop *Crop=new mafOpCrop("Crop");
	Crop->TestModeOn();
	Crop->SetInput(Input);
	Crop->SetCroppingBoxBounds(b_in);
	Crop->OpRun();
	Crop->Crop();
	Crop->OpDo();

	Input->Update();
	Input->GetOutput()->GetBounds(b_out);
	
	double volumeSpacing[3];	
	((vtkStructuredPoints*)Input->GetOutput()->GetVTKData())->GetSpacing(volumeSpacing);
	((vtkStructuredPoints*)Input->GetOutput()->GetVTKData())->GetBounds(b_out);

	for(int i=0;i<3;i++)
	{
		CPPUNIT_ASSERT((b_in[2*i]-volumeSpacing[i]<b_out[2*i])&&(b_in[2*i+1]+volumeSpacing[i]>b_out[2*i+1]));
	}
	
	mafDEL(Crop);
	mafDEL(Importer);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpCropTest::DynamicAllocation() 
//-----------------------------------------------------------
{
	mafOpCrop *Crop=new mafOpCrop("Crop");
	mafDEL(Crop);
}
