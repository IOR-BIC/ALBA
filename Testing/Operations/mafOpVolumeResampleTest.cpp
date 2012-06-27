/*=========================================================================

 Program: MAF2
 Module: mafOpVolumeResampleTest
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
#include "mafOpVolumeResampleTest.h"
#include "mafOpVolumeResample.h"

#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "mafOpImporterVTK.h"

#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"

#include "vtkDataSet.h"
#include "vtkStructuredPoints.h"

enum BOUNDS
{
	ID_VME4DBOUNDS = 0,
	ID_VMELOCALBOUNDS,
	ID_VMEBOUNDS,
	ID_PERSONALBOUNDS,
};

//----------------------------------------------------------------------------
void mafOpVolumeResampleTest::TestBase()
//----------------------------------------------------------------------------
{
	//Inizialize Storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	//Import volume
	mafOpImporterVTK *importerVTK=new mafOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Test_VolumeResample/volume.vtk";
	importerVTK->SetFileName(filename);
	importerVTK->OpRun();
	mafVMEVolumeGray *Volume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	Volume->ReparentTo(storage->GetRoot());
	//Check if the input volume is a VMEVolumeGray
	CPPUNIT_ASSERT(Volume!=NULL);
	//Inizialize the operation
	mafOpVolumeResample *VolumeResample=new mafOpVolumeResample("Volume Resample");
	VolumeResample->SetInput(Volume);
	VolumeResample->TestModeOn();
	//VolumeResample->OpRun();
	double Spacing[3],NewSpacing[3];
	Spacing[0] = 1;
	Spacing[1] = 1.5;
	Spacing[2] = 2;
	double Bounds[6],NewBounds[6];
	Bounds[0]=0.0;
	Bounds[1]=4.0;
	Bounds[2]=1.0;
	Bounds[3]=7.0;
	Bounds[4]=3.5;
	Bounds[5]=7.5;
	VolumeResample->SetSpacing(Spacing);
	VolumeResample->SetBounds(Bounds,ID_PERSONALBOUNDS);
	VolumeResample->Resample();
	//Check output 
	mafVME *Output = mafVME::SafeDownCast(VolumeResample->GetOutput());
	vtkStructuredPoints *VTKData=vtkStructuredPoints::SafeDownCast(Output->GetOutput()->GetVTKData());
	VTKData->UpdateData();
	//Check if the output volume is a vtkStructuredPoints
	CPPUNIT_ASSERT(VTKData!=NULL);
	//Check spacing
	VTKData->GetSpacing(NewSpacing);
	CPPUNIT_ASSERT(NewSpacing[0]==Spacing[0] && NewSpacing[1]==Spacing[1] && NewSpacing[2]==Spacing[2]);
	//Check bounds
	VTKData->GetBounds(NewBounds);
	CPPUNIT_ASSERT(NewBounds[0]==Bounds[0] && NewBounds[1]==Bounds[1] && NewBounds[2]==Bounds[2]);
	CPPUNIT_ASSERT(NewBounds[3]==Bounds[3] && NewBounds[4]==Bounds[4] && NewBounds[5]==Bounds[5]);

	VTKData = NULL;
	Output = NULL;
	Volume = NULL;
	mafDEL(VolumeResample);
	mafDEL(importerVTK);
	mafDEL(storage); 
}
//----------------------------------------------------------------------------
void mafOpVolumeResampleTest::TestVMELocalBounds()
//----------------------------------------------------------------------------
{
	//Inizialize Storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	//Import volume
	mafOpImporterVTK *importerVTK=new mafOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Test_VolumeResample/volume.vtk";
	importerVTK->SetFileName(filename);
	importerVTK->OpRun();
	mafVMEVolumeGray *Volume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	//Check if the input volume is a VMEVolumeGray
	CPPUNIT_ASSERT(Volume!=NULL);
	Volume->ReparentTo(storage->GetRoot());
	//Inizialize the operation
	mafOpVolumeResample *VolumeResample=new mafOpVolumeResample("Volume Resample");
	VolumeResample->SetInput(Volume);
	VolumeResample->TestModeOn();
	VolumeResample->OpRun();
	double Spacing[3],NewSpacing[3];
	Spacing[0] = 1;
	Spacing[1] = 1.5;
	Spacing[2] = 2;
	double Bounds[6],NewBounds[6];
	Volume->GetOutput()->GetVMELocalBounds(Bounds);
	VolumeResample->SetSpacing(Spacing);
	VolumeResample->SetBounds(Bounds,ID_VMELOCALBOUNDS);
	VolumeResample->Resample();
	//Check output
	mafVME *Output = mafVME::SafeDownCast(VolumeResample->GetOutput());
	vtkStructuredPoints *VTKData=vtkStructuredPoints::SafeDownCast(Output->GetOutput()->GetVTKData());
	VTKData->UpdateData();
	//Check if the output volume is a vtkStructuredPoints
	CPPUNIT_ASSERT(VTKData!=NULL);
	//Check spacing
	VTKData->GetSpacing(NewSpacing);
	CPPUNIT_ASSERT(NewSpacing[0]==Spacing[0] && NewSpacing[1]==Spacing[1] && NewSpacing[2]==Spacing[2]);
	//Check bounds
	VTKData->GetBounds(NewBounds);
	CPPUNIT_ASSERT(NewBounds[0]-Spacing[0]<Bounds[0] && NewBounds[0]+Spacing[0]>Bounds[0]);
	CPPUNIT_ASSERT(NewBounds[1]-Spacing[0]<Bounds[1] && NewBounds[1]+Spacing[0]>Bounds[1]);
	CPPUNIT_ASSERT(NewBounds[2]-Spacing[1]<Bounds[2] && NewBounds[2]+Spacing[1]>Bounds[2]);
	CPPUNIT_ASSERT(NewBounds[3]-Spacing[1]<Bounds[3] && NewBounds[3]+Spacing[1]>Bounds[3]);
	CPPUNIT_ASSERT(NewBounds[4]-Spacing[2]<Bounds[4] && NewBounds[4]+Spacing[2]>Bounds[4]);
	CPPUNIT_ASSERT(NewBounds[5]-Spacing[2]<Bounds[5] && NewBounds[5]+Spacing[2]>Bounds[5]);

	VTKData = NULL;
	Output = NULL;
	Volume = NULL;
	cppDEL(importerVTK);
	cppDEL(VolumeResample);
	mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafOpVolumeResampleTest::TestVME4DBounds()
//----------------------------------------------------------------------------
{
	//Inizialize Storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	//Import volume
	mafOpImporterVTK *importerVTK=new mafOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Test_VolumeResample/volume.vtk";
	importerVTK->SetFileName(filename);
	importerVTK->OpRun();
	mafVMEVolumeGray *Volume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	//Check if the input volume is a VMEVolumeGray
	CPPUNIT_ASSERT(Volume!=NULL);
	Volume->ReparentTo(storage->GetRoot());
	//Inizialize the operation
	mafOpVolumeResample *VolumeResample=new mafOpVolumeResample("Volume Resample");
	VolumeResample->SetInput(Volume);
	VolumeResample->TestModeOn();
	VolumeResample->OpRun();
	double Spacing[3],NewSpacing[3];
	Spacing[0] = 1;
	Spacing[1] = 1.5;
	Spacing[2] = 2;
	double Bounds[6],NewBounds[6];
	Volume->GetOutput()->GetVME4DBounds(Bounds);
	double dims[3];
  dims[0] = Bounds[1] - Bounds[0];
  dims[1] = Bounds[3] - Bounds[2];
  dims[2] = Bounds[5] - Bounds[4];

  Bounds[0] = -dims[0] / 2.0;
  Bounds[1] =  dims[0] / 2.0;
  Bounds[2] = -dims[1] / 2.0;
  Bounds[3] =  dims[1] / 2.0;
  Bounds[4] = -dims[2] / 2.0;
  Bounds[5] =  dims[2] / 2.0;
	VolumeResample->SetSpacing(Spacing);
	VolumeResample->SetBounds(Bounds,ID_VME4DBOUNDS);
	VolumeResample->Resample();
	//Check output
	mafVME *Output = mafVME::SafeDownCast(VolumeResample->GetOutput());
	vtkStructuredPoints *VTKData=vtkStructuredPoints::SafeDownCast(Output->GetOutput()->GetVTKData());
	VTKData->UpdateData();
	//Check if the output volume is a vtkStructuredPoints
	CPPUNIT_ASSERT(VTKData!=NULL);
	//Check spacing
	VTKData->GetSpacing(NewSpacing);
	CPPUNIT_ASSERT(NewSpacing[0]==Spacing[0] && NewSpacing[1]==Spacing[1] && NewSpacing[2]==Spacing[2]);
	//Check bounds
	VTKData->GetBounds(NewBounds);
	CPPUNIT_ASSERT(NewBounds[0]-Spacing[0]<Bounds[0] && NewBounds[0]+Spacing[0]>Bounds[0]);
	CPPUNIT_ASSERT(NewBounds[1]-Spacing[0]<Bounds[1] && NewBounds[1]+Spacing[0]>Bounds[1]);
	CPPUNIT_ASSERT(NewBounds[2]-Spacing[1]<Bounds[2] && NewBounds[2]+Spacing[1]>Bounds[2]);
	CPPUNIT_ASSERT(NewBounds[3]-Spacing[1]<Bounds[3] && NewBounds[3]+Spacing[1]>Bounds[3]);
	CPPUNIT_ASSERT(NewBounds[4]-Spacing[2]<Bounds[4] && NewBounds[4]+Spacing[2]>Bounds[4]);
	CPPUNIT_ASSERT(NewBounds[5]-Spacing[2]<Bounds[5] && NewBounds[5]+Spacing[2]>Bounds[5]);

	VTKData = NULL;
	Output = NULL;
	Volume = NULL;
	cppDEL(importerVTK);
	cppDEL(VolumeResample);
	mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafOpVolumeResampleTest::TestVMEBounds()
//----------------------------------------------------------------------------
{
	//Inizialize Storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	//Import volume
	mafOpImporterVTK *importerVTK=new mafOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Test_VolumeResample/volume.vtk";
	importerVTK->SetFileName(filename);
	importerVTK->OpRun();
	mafVMEVolumeGray *Volume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	//Check if the input volume is a VMEVolumeGray
	CPPUNIT_ASSERT(Volume!=NULL);
	Volume->ReparentTo(storage->GetRoot());
	//Inizialize the operation
	mafOpVolumeResample *VolumeResample=new mafOpVolumeResample("Volume Resample");
	VolumeResample->SetInput(Volume);
	VolumeResample->TestModeOn();
	VolumeResample->OpRun();
	double Spacing[3],NewSpacing[3];
	Spacing[0] = 1;
	Spacing[1] = 1.5;
	Spacing[2] = 2;
	double Bounds[6],NewBounds[6];
	Volume->GetOutput()->GetVMEBounds(Bounds);
	double dims[3];
  dims[0] = Bounds[1] - Bounds[0];
  dims[1] = Bounds[3] - Bounds[2];
  dims[2] = Bounds[5] - Bounds[4];

  Bounds[0] = -dims[0] / 2.0;
  Bounds[1] =  dims[0] / 2.0;
  Bounds[2] = -dims[1] / 2.0;
  Bounds[3] =  dims[1] / 2.0;
  Bounds[4] = -dims[2] / 2.0;
  Bounds[5] =  dims[2] / 2.0;
	VolumeResample->SetSpacing(Spacing);
	VolumeResample->SetBounds(Bounds,ID_VMEBOUNDS);
	VolumeResample->Resample();
	//Check output
	mafVME *Output = mafVME::SafeDownCast(VolumeResample->GetOutput());
	vtkStructuredPoints *VTKData=vtkStructuredPoints::SafeDownCast(Output->GetOutput()->GetVTKData());
	VTKData->UpdateData();
	//Check if the output volume is a vtkStructuredPoints
	CPPUNIT_ASSERT(VTKData!=NULL);
	//Check spacing
	VTKData->GetSpacing(NewSpacing);
	CPPUNIT_ASSERT(NewSpacing[0]==Spacing[0] && NewSpacing[1]==Spacing[1] && NewSpacing[2]==Spacing[2]);
	//Check bounds
	VTKData->GetBounds(NewBounds);
	CPPUNIT_ASSERT(NewBounds[0]-Spacing[0]<Bounds[0] && NewBounds[0]+Spacing[0]>Bounds[0]);
	CPPUNIT_ASSERT(NewBounds[1]-Spacing[0]<Bounds[1] && NewBounds[1]+Spacing[0]>Bounds[1]);
	CPPUNIT_ASSERT(NewBounds[2]-Spacing[1]<Bounds[2] && NewBounds[2]+Spacing[1]>Bounds[2]);
	CPPUNIT_ASSERT(NewBounds[3]-Spacing[1]<Bounds[3] && NewBounds[3]+Spacing[1]>Bounds[3]);
	CPPUNIT_ASSERT(NewBounds[4]-Spacing[2]<Bounds[4] && NewBounds[4]+Spacing[2]>Bounds[4]);
	CPPUNIT_ASSERT(NewBounds[5]-Spacing[2]<Bounds[5] && NewBounds[5]+Spacing[2]>Bounds[5]);

	VTKData = NULL;
	Output = NULL;
	Volume = NULL;
	cppDEL(importerVTK);
	cppDEL(VolumeResample);
	mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafOpVolumeResampleTest::TestScalarRange()
//----------------------------------------------------------------------------
{
	//Inizialize Storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	//Import volume
	mafOpImporterVTK *importerVTK=new mafOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Test_VolumeResample/volumeRG.vtk";
	importerVTK->SetFileName(filename);
	importerVTK->OpRun();
	mafVMEVolumeGray *Volume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	//Check if the input volume is a VMEVolumeGray
	CPPUNIT_ASSERT(Volume!=NULL);
	Volume->ReparentTo(storage->GetRoot());
	//Inizialize the operation
	mafOpVolumeResample *VolumeResample=new mafOpVolumeResample("Volume Resample");
	VolumeResample->SetInput(Volume);
	VolumeResample->TestModeOn();
	VolumeResample->OpRun();
	double NewScalarRange[2],ScalarRange[2];
	Volume->Update();
	Volume->GetOutput()->GetVTKData()->GetScalarRange(ScalarRange);
	double Spacing[3];
	Spacing[0] = 1;
	Spacing[1] = 1;
	Spacing[2] = 1;
	double Bounds[6];
	Volume->GetOutput()->GetVTKData()->GetBounds(Bounds);
	VolumeResample->SetSpacing(Spacing);
	VolumeResample->SetBounds(Bounds,ID_PERSONALBOUNDS);
	VolumeResample->Resample();
	//Check output
	mafVME *Output = mafVME::SafeDownCast(VolumeResample->GetOutput());
	vtkStructuredPoints *VTKData=vtkStructuredPoints::SafeDownCast(Output->GetOutput()->GetVTKData());
	VTKData->UpdateData();
	//Check if the output volume is a vtkStructuredPoints
	CPPUNIT_ASSERT(VTKData!=NULL);
	//Check scalar range
	VTKData->GetScalarRange(NewScalarRange);
	CPPUNIT_ASSERT(NewScalarRange[0]==ScalarRange[0] && NewScalarRange[1]==ScalarRange[1]);

	VTKData = NULL;
	Output = NULL;
	Volume = NULL;
	cppDEL(importerVTK);
	cppDEL(VolumeResample);
	mafDEL(storage);
}
