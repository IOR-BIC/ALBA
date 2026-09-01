/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELabeledVolumeTest
 Authors: Roberto Mucci
 
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

#include "albaVMELabeledVolumeTest.h"
#include "albaVMELabeledVolume.h"

#include "albaVMEVolumeGray.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkALBASmartPointer.h"
#include "vtkPointData.h"
#include "vtkRectilinearGridReader.h"
#include "vtkFloatArray.h"

#define OUTRANGE_SCALAR -1000

//----------------------------------------------------------------------------
void albaVMELabeledVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------------
void albaVMELabeledVolumeTest::TestDynamicAllocation()
//---------------------------------------------------------------
{
  albaVMELabeledVolume *labeledVolume;
  albaNEW(labeledVolume);
  albaDEL(labeledVolume);
}

//---------------------------------------------------------------
void albaVMELabeledVolumeTest::TestVolumeCopy()
//---------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;

  albaString filename_volume=ALBA_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkALBASmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  albaSmartPointer<albaVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->ReparentTo(root);
  volume->Update();

  albaSmartPointer<albaVMELabeledVolume> labeled; 
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  //Get the scalar data of the labeled volume 
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the albaVMELabeledVolume without labels has all the scalars set to 0.
  int not = labelScalars->GetNumberOfTuples();
	bool sameScalars=true;
  for ( int i = 0; i < not; i++ )
  {
    double labelValue = labelScalars->GetComponent( i, 0 );
    if (labelValue != OUTRANGE_SCALAR)
		{
			sameScalars=false;
			break;
		}
  }
	CPPUNIT_ASSERT(sameScalars);
}

//---------------------------------------------------------------
void albaVMELabeledVolumeTest::TestGenerateLabeledVolume()
//---------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;

  albaString filename_volume=ALBA_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkALBASmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  albaSmartPointer<albaVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->ReparentTo(root);
  volume->Update();

  albaSmartPointer<albaVMELabeledVolume> labeled; 
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  //Get the scalar data of the copied original volume 
  vtkDataArray *volumeScalars;
  volumeScalars = volumeReader->GetOutput()->GetPointData()->GetScalars();
  volumeScalars->Modified();

  int min = 0;
  int max = 1500;
  int newValue = 1000;

  labeled->FillLabelVector("testLabel 1000 0 1500");
  labeled->GenerateLabeledVolume();

  //Get the scalar data of the labeled volume 
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the albaVMELabeledVolume with one label has the correct scalars
  int not = volumeScalars->GetNumberOfTuples();
  double labelValue, volumeValue;
	bool sameScalars=true;
  for ( int i = 0; i < not; i++ )
  {
    volumeValue = volumeScalars->GetComponent( i, 0 );
    labelValue = labelScalars->GetComponent( i, 0 );
    if ( volumeValue >= min && volumeValue <= max )
    { 
      if(labelValue != newValue)
			{
				sameScalars=false;
				break;
			}
    }
    else
    {
      if(labelValue != OUTRANGE_SCALAR)
			{
				sameScalars=false;
				break;
			}
    }
  }
	CPPUNIT_ASSERT(sameScalars);
}

//---------------------------------------------------------------
void albaVMELabeledVolumeTest::TestRemoveLabelTag()
//---------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;

  albaString filename_volume=ALBA_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkALBASmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  albaSmartPointer<albaVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->ReparentTo(root);
  volume->Update();

  albaSmartPointer<albaVMELabeledVolume> labeled; 
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  wxString label = "testLabel 1000 0 1500";
  labeled->FillLabelVector(label);

  //Remove tha label
  labeled->RemoveItemLabelVector(0);
  labeled->GenerateLabeledVolume();

  //Get the scalar data of the labeled volume 
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the albaVMELabeledVolume without labels has all the scalars set to 0.
  int not = labelScalars->GetNumberOfTuples();
	bool sameScalars=true;
  for ( int i = 0; i < not; i++ )
  {
    double labelValue = labelScalars->GetComponent( i, 0 );
		if (labelValue != OUTRANGE_SCALAR)
		{
			sameScalars=false;
			break;
		}
	}
	CPPUNIT_ASSERT(sameScalars);

}

//---------------------------------------------------------------
void albaVMELabeledVolumeTest::TestSetLabelTag()
//---------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;

  albaString filename_volume=ALBA_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkALBASmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  albaSmartPointer<albaVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->ReparentTo(root);
  volume->Update();

  //Get the scalar data of the copied original volume 
  vtkALBASmartPointer<vtkDataArray> volumeScalars;
  volumeScalars = volumeReader->GetOutput()->GetPointData()->GetScalars();
  volumeScalars->Modified();

  albaSmartPointer<albaVMELabeledVolume> labeled; 
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  wxString label = "testLabel 1000 0 1500";
  labeled->FillLabelVector(label);

  //Modify the label 
  int min = 0;
  int max = 1000;
  int newValue = 500;

  wxString labelNew = "testLabelNew 500 0 1000";
  labeled->ModifyLabelVector(0, labelNew, true);
  labeled->GenerateLabeledVolume();


  //Get the scalar data of the labeled volume 
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the albaVMELabeledVolume with one label has the correct scalars
  int not = volumeScalars->GetNumberOfTuples();
  double labelValue, volumeValue;
	bool sameValues=true;
  for ( int i = 0; i < not; i++ )
  {
    volumeValue = volumeScalars->GetComponent( i, 0 );
    labelValue = labelScalars->GetComponent( i, 0 );
    if ( volumeValue >= min && volumeValue <= max )
    { 
      if(labelValue != newValue)
			{
				sameValues=false;
				break;
			}
    }
    else
    {
      if (labelValue != OUTRANGE_SCALAR)
			{
				sameValues=false;
				break;
			}
    }
  }
	CPPUNIT_ASSERT(sameValues);
}
//---------------------------------------------------------------
void albaVMELabeledVolumeTest::TestDeepCopy()
//---------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;

  albaString filename_volume=ALBA_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkALBASmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  albaSmartPointer<albaVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->ReparentTo(root);
  volume->Update();

  albaSmartPointer<albaVMELabeledVolume> labeled; 
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  //Get the scalar data of the copied original volume 
  vtkDataArray *volumeScalars;
  volumeScalars = volumeReader->GetOutput()->GetPointData()->GetScalars();
  volumeScalars->Modified();

  int min = 0;
  int max = 1500;
  int newValue = 1000;
  wxString label = "testLabel 1000 0 1500";
  
  labeled->FillLabelVector(label);
  labeled->SetLabelTag(label, 0);
  labeled->GenerateLabeledVolume();

  //Get the scalar data of the labeled volume 
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  albaSmartPointer<albaVMELabeledVolume> labeledCopied; 
  labeledCopied->DeepCopy(labeled);
  labeledCopied->Update();

  //Check if the tag has been copied and retreive it
  albaTagItem *tagLabel = new albaTagItem;
  tagLabel = labeledCopied->GetTagArray()->GetTag( "LABELS" );
  wxString tagString = tagLabel->GetValue(0);
  labeledCopied->FillLabelVector(tagString);
  labeledCopied->GenerateLabeledVolume();

  //Get the scalar data of the copied labeled volume 
  vtkDataArray *labelCopiedScalars;
  vtkDataSet *datasetLabelCopied = labeledCopied->GetOutput()->GetVTKData();
  vtkRectilinearGrid *recGridCopied = (vtkRectilinearGrid*) datasetLabelCopied;
  labelCopiedScalars = recGridCopied->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the labeled volume and its copy have the same scalars
  int not = labelScalars->GetNumberOfTuples();
  double labelValue, labelCopiedValue;
	bool sameValues=true;
  for ( int i = 0; i < not; i++ )
  {
    labelCopiedValue = labelCopiedScalars->GetComponent( i, 0 );
    labelValue = labelScalars->GetComponent( i, 0 );
    
    if(labelValue != labelCopiedValue)
		{
			sameValues=false;
			break;
		}
  }
	CPPUNIT_ASSERT(sameValues);
}
