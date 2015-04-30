/*=========================================================================

 Program: MAF2
 Module: mafVMELabeledVolumeTest
 Authors: Roberto Mucci
 
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

#include "mafVMELabeledVolumeTest.h"
#include "mafVMELabeledVolume.h"

#include "mafVMEVolumeGray.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkRectilinearGridReader.h"

#define OUTRANGE_SCALAR -1000

//----------------------------------------------------------------------------
void mafVMELabeledVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMELabeledVolumeTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMELabeledVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------------
void mafVMELabeledVolumeTest::TestDynamicAllocation()
//---------------------------------------------------------------
{
  mafVMELabeledVolume *labeledVolume;
  mafNEW(labeledVolume);
  mafDEL(labeledVolume);
}

//---------------------------------------------------------------
void mafVMELabeledVolumeTest::TestVolumeCopy()
//---------------------------------------------------------------
{
  mafSmartPointer<mafVMERoot> root;

  mafString filename_volume=MAF_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkMAFSmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->ReparentTo(root);
  volume->Update();

  mafSmartPointer<mafVMELabeledVolume> labeled; 
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  //Get the scalar data of the labeled volume 
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  datasetLabel->Update();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the mafVMELabeledVolume without labels has all the scalars set to 0.
  int not = labelScalars->GetNumberOfTuples();
  for ( int i = 0; i < not; i++ )
  {
    double labelValue = labelScalars->GetComponent( i, 0 );
    CPPUNIT_ASSERT(labelValue == OUTRANGE_SCALAR);
  }
}

//---------------------------------------------------------------
void mafVMELabeledVolumeTest::TestGenerateLabeledVolume()
//---------------------------------------------------------------
{
  mafSmartPointer<mafVMERoot> root;

  mafString filename_volume=MAF_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkMAFSmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->ReparentTo(root);
  volume->Update();

  mafSmartPointer<mafVMELabeledVolume> labeled; 
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
  datasetLabel->Update();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the mafVMELabeledVolume with one label has the correct scalars
  int not = volumeScalars->GetNumberOfTuples();
  double labelValue, volumeValue;
  for ( int i = 0; i < not; i++ )
  {
    volumeValue = volumeScalars->GetComponent( i, 0 );
    labelValue = labelScalars->GetComponent( i, 0 );
    if ( volumeValue >= min && volumeValue <= max )
    { 
      CPPUNIT_ASSERT(labelValue == newValue);
    }
    else
    {
      CPPUNIT_ASSERT(labelValue == OUTRANGE_SCALAR);
    }
  }
}

//---------------------------------------------------------------
void mafVMELabeledVolumeTest::TestRemoveLabelTag()
//---------------------------------------------------------------
{
  mafSmartPointer<mafVMERoot> root;

  mafString filename_volume=MAF_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkMAFSmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->ReparentTo(root);
  volume->Update();

  mafSmartPointer<mafVMELabeledVolume> labeled; 
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  wxString label = "testLabel 1000 0 1500";
  labeled->FillLabelVector(label);

  //Remove tha label
  labeled->RemoveItemLabelVector(0);
  labeled->GenerateLabeledVolume();

  //Get the scalar data of the labeled volume 
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  datasetLabel->Update();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the mafVMELabeledVolume without labels has all the scalars set to 0.
  int not = labelScalars->GetNumberOfTuples();
  for ( int i = 0; i < not; i++ )
  {
    double labelValue = labelScalars->GetComponent( i, 0 );
    CPPUNIT_ASSERT(labelValue == OUTRANGE_SCALAR);
  }
}

//---------------------------------------------------------------
void mafVMELabeledVolumeTest::TestSetLabelTag()
//---------------------------------------------------------------
{
  mafSmartPointer<mafVMERoot> root;

  mafString filename_volume=MAF_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkMAFSmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->ReparentTo(root);
  volume->Update();

  //Get the scalar data of the copied original volume 
  vtkMAFSmartPointer<vtkDataArray> volumeScalars;
  volumeScalars = volumeReader->GetOutput()->GetPointData()->GetScalars();
  volumeScalars->Modified();

  mafSmartPointer<mafVMELabeledVolume> labeled; 
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  wxString label = "testLabel 1000 0 1500";
  labeled->FillLabelVector(label);

  //Modify the label 
  int min = 0;
  int max = 1000;
  int newValue = 500;

  wxString labelNew = "testLabelNew 500 0 1000";
  labeled->ModifyLabelVector(0, labelNew, TRUE);
  labeled->GenerateLabeledVolume();


  //Get the scalar data of the labeled volume 
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  datasetLabel->Update();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the mafVMELabeledVolume with one label has the correct scalars
  int not = volumeScalars->GetNumberOfTuples();
  double labelValue, volumeValue;
  for ( int i = 0; i < not; i++ )
  {
    volumeValue = volumeScalars->GetComponent( i, 0 );
    labelValue = labelScalars->GetComponent( i, 0 );
    if ( volumeValue >= min && volumeValue <= max )
    { 
      CPPUNIT_ASSERT(labelValue == newValue);
    }
    else
    {
      CPPUNIT_ASSERT(labelValue == OUTRANGE_SCALAR);
    }
  }
}
//---------------------------------------------------------------
void mafVMELabeledVolumeTest::TestDeepCopy()
//---------------------------------------------------------------
{
  mafSmartPointer<mafVMERoot> root;

  mafString filename_volume=MAF_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkMAFSmartPointer<vtkRectilinearGridReader>volumeReader;
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->ReparentTo(root);
  volume->Update();

  mafSmartPointer<mafVMELabeledVolume> labeled; 
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
  datasetLabel->Update();
  vtkRectilinearGrid *recGrid = (vtkRectilinearGrid*) datasetLabel;
  vtkDataArray *labelScalars;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  mafSmartPointer<mafVMELabeledVolume> labeledCopied; 
  labeledCopied->DeepCopy(labeled);
  labeledCopied->Update();

  //Check if the tag has been copied and retreive it
  mafTagItem *tagLabel = new mafTagItem;
  tagLabel = labeledCopied->GetTagArray()->GetTag( "LABELS" );
  wxString tagString = tagLabel->GetValue(0);
  labeledCopied->FillLabelVector(tagString);
  labeledCopied->GenerateLabeledVolume();

  //Get the scalar data of the copied labeled volume 
  vtkDataArray *labelCopiedScalars;
  vtkDataSet *datasetLabelCopied = labeledCopied->GetOutput()->GetVTKData();
  datasetLabelCopied->Update();
  vtkRectilinearGrid *recGridCopied = (vtkRectilinearGrid*) datasetLabelCopied;
  labelCopiedScalars = recGridCopied->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the labeled volume and its copy have the same scalars
  int not = labelScalars->GetNumberOfTuples();
  double labelValue, labelCopiedValue;
  for ( int i = 0; i < not; i++ )
  {
    labelCopiedValue = labelCopiedScalars->GetComponent( i, 0 );
    labelValue = labelScalars->GetComponent( i, 0 );
    
    CPPUNIT_ASSERT(labelValue == labelCopiedValue);
  }
}
