/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMELabeledVolumeTest.cpp,v $
Language:  C++
Date:      $Date: 2007-10-30 15:42:11 $
Version:   $Revision: 1.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medVMELabeledVolumeTest.h"
#include "medVMELabeledVolume.h"

#include "mmoVTKImporter.h"
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

//----------------------------------------------------------------------------
void medVMELabeledVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medVMELabeledVolumeTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medVMELabeledVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------------
void medVMELabeledVolumeTest::TestDynamicAllocation()
//---------------------------------------------------------------
{
  medVMELabeledVolume *labeledVolume;
  mafNEW(labeledVolume);
  mafDEL(labeledVolume);
}

//---------------------------------------------------------------
void medVMELabeledVolumeTest::TestVolumeCopy()
//---------------------------------------------------------------
{
  mafVMERoot *root;
  mafNEW(root);

  mafString filename_volume=MED_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkRectilinearGridReader *volumeReader = vtkRectilinearGridReader::New();
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  mafVMEVolumeGray *volume;
  mafNEW(volume);
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->SetParent(root);
  volume->Update();

  medVMELabeledVolume *labeled; 
  mafNEW(labeled);
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  //Get the scalar data of the copied original volume 
  vtkMAFSmartPointer<vtkDataArray> volumeScalars;
  volumeScalars = volumeReader->GetOutput()->GetPointData()->GetScalars();
  volumeScalars->Modified();

  //Get the scalar data of the labeled volume 
  vtkMAFSmartPointer<vtkDataArray> labelScalars;
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  datasetLabel->Update();
  vtkMAFSmartPointer<vtkRectilinearGrid> recGrid = (vtkRectilinearGrid*) datasetLabel;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the medVMELabeledVolume without labels has the same scalars of the VME parent
  int not = volumeScalars->GetNumberOfTuples();
  for ( int i = 0; i < not; i++ )
  {
    double scalarValue = volumeScalars->GetComponent( i, 0 );
    double labelValue = labelScalars->GetComponent( i, 0 );
    CPPUNIT_ASSERT(scalarValue == labelValue);
  }
  
  mafDEL(labeled);
  mafDEL(volume);
  mafDEL(volumeReader);
  mafDEL(root);

}

//---------------------------------------------------------------
void medVMELabeledVolumeTest::TestGenerateLabeledVolume()
//---------------------------------------------------------------
{
  mafVMERoot *root;
  mafNEW(root);

  mafString filename_volume=MED_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkRectilinearGridReader *volumeReader = vtkRectilinearGridReader::New();
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  mafVMEVolumeGray *volume;
  mafNEW(volume);
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->SetParent(root);
  volume->Update();

  medVMELabeledVolume *labeled; 
  mafNEW(labeled);
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  //Get the scalar data of the copied original volume 
  vtkMAFSmartPointer<vtkDataArray> volumeScalars;
  volumeScalars = volumeReader->GetOutput()->GetPointData()->GetScalars();
  volumeScalars->Modified();

  int min = 0;
  int max = 1500;
  int newValue = 1000;

  labeled->FillLabelVector(0, "testLabel 1000 0 1500");
  labeled->GenerateLabeledVolume();

  //Get the scalar data of the labeled volume 
  vtkMAFSmartPointer<vtkDataArray> labelScalars;
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  datasetLabel->Update();
  vtkMAFSmartPointer<vtkRectilinearGrid> recGrid = (vtkRectilinearGrid*) datasetLabel;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  //Check if the medVMELabeledVolume with one label has the correct scalars
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
      CPPUNIT_ASSERT(labelValue == 0);
    }
  }

  mafDEL(volume);
  mafDEL(labeled);
  mafDEL(root);
}

//---------------------------------------------------------------
void medVMELabeledVolumeTest::TestDeepCopy()
//---------------------------------------------------------------
{
  mafVMERoot *root;
  mafNEW(root);

  mafString filename_volume=MED_DATA_ROOT;
  filename_volume<<"/VTK_Volumes/LabeledVolumeTest.vtk";

  vtkRectilinearGridReader *volumeReader = vtkRectilinearGridReader::New();
  volumeReader->SetFileName(filename_volume);
  volumeReader->Update();

  mafVMEVolumeGray *volume;
  mafNEW(volume);
  volume->SetData(volumeReader->GetOutput(), 0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->SetParent(root);
  volume->Update();

  medVMELabeledVolume *labeled; 
  mafNEW(labeled);
  labeled->SetName("Labeled Volume");
  labeled->SetVolumeLink(volume);

  //Get the scalar data of the copied original volume 
  vtkMAFSmartPointer<vtkDataArray> volumeScalars;
  volumeScalars = volumeReader->GetOutput()->GetPointData()->GetScalars();
  volumeScalars->Modified();

  int min = 0;
  int max = 1500;
  int newValue = 1000;
  wxString label = "testLabel 1000 0 1500";
  
  labeled->FillLabelVector(0, label);
  labeled->SetLabelTag(label, 0);
  labeled->GenerateLabeledVolume();

  //Get the scalar data of the labeled volume 
  vtkMAFSmartPointer<vtkDataArray> labelScalars;
  vtkDataSet *datasetLabel = labeled->GetOutput()->GetVTKData();
  datasetLabel->Update();
  vtkMAFSmartPointer<vtkRectilinearGrid> recGrid = (vtkRectilinearGrid*) datasetLabel;
  labelScalars = recGrid->GetPointData()->GetScalars();
  labelScalars->Modified();

  medVMELabeledVolume *labeledCopied; 
  mafNEW(labeledCopied);

  labeledCopied->DeepCopy(labeled);
  labeledCopied->Update();

  //Check if the tag has been copied and retreive it
  mafTagItem *tagLabel = new mafTagItem;
  tagLabel = labeledCopied->GetTagArray()->GetTag( "LABELS" );
  wxString tagString = tagLabel->GetValue(0);
  labeledCopied->FillLabelVector(0, tagString);
  labeledCopied->GenerateLabeledVolume();

  //Get the scalar data of the copied labeled volume 
  vtkMAFSmartPointer<vtkDataArray> labelCopiedScalars;
  vtkDataSet *datasetLabelCopied = labeledCopied->GetOutput()->GetVTKData();
  datasetLabelCopied->Update();
  vtkMAFSmartPointer<vtkRectilinearGrid> recGridCopied = (vtkRectilinearGrid*) datasetLabelCopied;
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

  mafDEL(volume);
  mafDEL(labeled);
  mafDEL(labeledCopied);
  mafDEL(root);
}
