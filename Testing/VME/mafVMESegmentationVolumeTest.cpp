/*=========================================================================

 Program: MAF2
 Module: mafVMESegmentationVolumeTest
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
#include "mafVMESegmentationVolumeTest.h"
#include "mafVMESegmentationVolume.h"
#include "mafAttributeSegmentationVolume.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSetReader.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafVMESegmentationVolumeTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;

  m_Storage = mafVMEStorage::New();
  m_Storage->GetRoot()->SetName("root");
  m_Storage->GetRoot()->Initialize();

  m_Volume = NULL;
  mafNEW(m_Volume);
  m_Volume->ReparentTo(m_Storage->GetRoot());
  vtkMAFSmartPointer<vtkDataSetReader> r;
  mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/volumeSP.vtk";
  r->SetFileName(filename);
  r->Update();
  m_Volume->SetData(vtkStructuredPoints::SafeDownCast(r->GetOutput()),0.0);
  m_Volume->Update();

  m_VolumeRG = NULL;
  mafNEW(m_VolumeRG);
  m_VolumeRG->ReparentTo(m_Storage->GetRoot());
  filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/volumeRG.vtk";
  r->SetFileName(filename);
  r->Update();
  m_VolumeRG->SetData(vtkRectilinearGrid::SafeDownCast(r->GetOutput()),0.0);
  m_VolumeRG->Update();

  m_VolumeManualMask = NULL;
  mafNEW(m_VolumeManualMask);
  m_VolumeManualMask->ReparentTo(m_Volume);
  filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/manualMask.vtk";
  r->SetFileName(filename);
  r->Update();
  m_VolumeManualMask->SetData(vtkStructuredPoints::SafeDownCast(r->GetOutput()),0.0);
  m_VolumeManualMask->Update();

  m_VolumeRefinementMask = NULL;
  mafNEW(m_VolumeRefinementMask);
  m_VolumeRefinementMask->ReparentTo(m_Volume);
  filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/refinementMask.vtk";
  r->SetFileName(filename);
  r->Update();
  m_VolumeRefinementMask->SetData(vtkStructuredPoints::SafeDownCast(r->GetOutput()),0.0);
  m_VolumeRefinementMask->Update();
}
//----------------------------------------------------------------------------
void mafVMESegmentationVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_VolumeRefinementMask);
  mafDEL(m_VolumeManualMask);
  mafDEL(m_Volume);
  mafDEL(m_VolumeRG);

  delete m_Storage;
  delete wxLog::SetActiveTarget(NULL);
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafVMESegmentationVolume *vme1 = NULL;
  mafNEW(vme1);
  mafDEL(vme1);

  mafSmartPointer<mafVMESegmentationVolume> vme2;
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestSetAutomaticSegmentationThresholdModality()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::GLOBAL);

  CPPUNIT_ASSERT( vme->GetAutomaticSegmentationThresholdModality() == mafVMESegmentationVolume::GLOBAL );
  CPPUNIT_ASSERT( mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetAutomaticSegmentationThresholdModality() == mafVMESegmentationVolume::GLOBAL );
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::RANGE);

  CPPUNIT_ASSERT( vme->GetAutomaticSegmentationThresholdModality() == mafVMESegmentationVolume::RANGE );
  CPPUNIT_ASSERT( mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetAutomaticSegmentationThresholdModality() == mafVMESegmentationVolume::RANGE );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestAddRange()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  double threshold;
  int startSlice,endSlice;
  int result;
  vme->ReparentTo(m_Storage->GetRoot());
  vme->SetVolumeLink(m_Volume);
  //////////////////////////////////////////////////////////////////////////
  result = vme->AddRange(0,20,20.5);
  CPPUNIT_ASSERT( result == MAF_OK );
  CPPUNIT_ASSERT( vme->GetNumberOfRanges() == 1 );
  result = vme->AddRange(15,100,10.0);
  CPPUNIT_ASSERT( result == MAF_ERROR );//Overlaps of slices
  CPPUNIT_ASSERT( vme->GetNumberOfRanges() == 1 );
  result = vme->AddRange(21,100,15.0);
  CPPUNIT_ASSERT( result == MAF_OK );
  CPPUNIT_ASSERT( vme->GetNumberOfRanges() == 2 );
  //////////////////////////////////////////////////////////////////////////
  vme->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 0 && endSlice == 20 && threshold == 20.5);
  mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 0 && endSlice == 20 && threshold == 20.5);
  //////////////////////////////////////////////////////////////////////////
  vme->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 21 && endSlice == 100 && threshold == 15.0);
  mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 21 && endSlice == 100 && threshold == 15.0);
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestUpdateRange()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  double threshold;
  int startSlice,endSlice;
  vme->ReparentTo(m_Storage->GetRoot());
  vme->SetVolumeLink(m_Volume);
  vme->AddRange(0,20,20.5);
  vme->AddRange(21,100,15.0);
  //////////////////////////////////////////////////////////////////////////
  int result;
  result = vme->UpdateRange(3,50,100,2.0);
  CPPUNIT_ASSERT( result == MAF_ERROR );//Exceed of index
  result = vme->UpdateRange(0,50,100,2.0);
  CPPUNIT_ASSERT( result == MAF_ERROR );//Overlaps of slices
  result = vme->UpdateRange(1,101,200,12.5);
  CPPUNIT_ASSERT( result == MAF_OK );
  result = vme->UpdateRange(0,0,100,2.0);
  CPPUNIT_ASSERT( result == MAF_OK );
  //////////////////////////////////////////////////////////////////////////
  vme->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 0 && endSlice == 100 && threshold == 2.0);
  mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 0 && endSlice == 100 && threshold == 2.0);
  //////////////////////////////////////////////////////////////////////////
  vme->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 101 && endSlice == 200 && threshold == 12.5);
  mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 101 && endSlice == 200 && threshold == 12.5);
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestDeleteRange()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  double threshold;
  int startSlice,endSlice;
  vme->ReparentTo(m_Storage->GetRoot());
  vme->SetVolumeLink(m_Volume);
  vme->AddRange(0,20,20.5);
  vme->AddRange(21,100,15.0);
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT(  vme->GetNumberOfRanges() == 2 );
  //////////////////////////////////////////////////////////////////////////
  int result = vme->DeleteRange(5);
  CPPUNIT_ASSERT(  result ==  MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////
  result = vme->DeleteRange(0);
  CPPUNIT_ASSERT(  result == MAF_OK && vme->GetNumberOfRanges() ==  1 );
  //////////////////////////////////////////////////////////////////////////
  vme->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 21 && endSlice == 100 && threshold == 15.0);
  mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 21 && endSlice == 100 && threshold == 15.0);
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestSetAutomaticSegmentationGlobalThreshold()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationGlobalThreshold(1.0);

  CPPUNIT_ASSERT( vme->GetAutomaticSegmentationGlobalThreshold() == 1.0 );
  CPPUNIT_ASSERT( mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetAutomaticSegmentationGlobalThreshold() == 1.0 );
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationGlobalThreshold(5.2);

  CPPUNIT_ASSERT( vme->GetAutomaticSegmentationGlobalThreshold() == 5.2 );
  CPPUNIT_ASSERT( mafAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetAutomaticSegmentationGlobalThreshold() == 5.2 );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestSetVolumeLink()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  //////////////////////////////////////////////////////////////////////////
  int result = vme->SetVolumeLink((mafNode*)m_VolumeRG);
  CPPUNIT_ASSERT( result == MAF_OK && vme->GetVolumeLink() == (mafNode*)m_VolumeRG );
  //////////////////////////////////////////////////////////////////////////
  result = vme->SetVolumeLink((mafNode*)m_Volume);
  CPPUNIT_ASSERT( result == MAF_OK  && vme->GetVolumeLink() == (mafNode*)m_Volume );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestAutomaticSegmentation()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  //////////////////////////////////////////////////////////////////////////
  vme->SetVolumeLink(m_Volume);
  CPPUNIT_ASSERT( vme->GetVolumeLink() == (mafNode*)m_Volume );
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::RANGE);
  vme->AddRange(0,50,10.0);
  vme->AddRange(51,76,5.0);
  CPPUNIT_ASSERT( vme->CheckNumberOfThresholds() == true );
  //////////////////////////////////////////////////////////////////////////
  vme->GetOutput()->Update();
  vme->Update();
  CPPUNIT_ASSERT( vme->GetAutomaticOutput()->GetNumberOfPoints() == m_Volume->GetOutput()->GetVTKData()->GetNumberOfPoints() );
  vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(vme->GetOutput()->GetVTKData());
  sp->Update();
  double sr[2];
  sp->GetScalarRange(sr);
  CPPUNIT_ASSERT( sr[0] == 0.0 && sr[1] == 255.0 );
  CPPUNIT_ASSERT( sp->GetNumberOfPoints() == vme->GetAutomaticOutput()->GetNumberOfPoints() );
  for (int i=0;i<sp->GetNumberOfPoints();i++)
  {
    CPPUNIT_ASSERT( sp->GetPointData()->GetScalars()->GetTuple1(i) == vme->GetAutomaticOutput()->GetPointData()->GetScalars()->GetTuple1(i) );
  }
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestManualSegmentation()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  //////////////////////////////////////////////////////////////////////////
  vme->SetVolumeLink(m_Volume);
  CPPUNIT_ASSERT( vme->GetVolumeLink() == (mafNode*)m_Volume );
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::GLOBAL);
  vme->SetAutomaticSegmentationGlobalThreshold(5.0);
  CPPUNIT_ASSERT( vme->CheckNumberOfThresholds() == true );
  int seed[3] = {0,0,0};
  vme->AddSeed(seed);
  vme->SetRegionGrowingLowerThreshold(0);
  vme->SetRegionGrowingUpperThreshold(255);
  //////////////////////////////////////////////////////////////////////////
  vme->SetManualVolumeMask(m_VolumeManualMask);
  //////////////////////////////////////////////////////////////////////////
  vme->GetOutput()->Update();
  vme->Update();
  int n1 = vme->GetManualOutput()->GetNumberOfPoints();
  int n2 = m_Volume->GetOutput()->GetVTKData()->GetNumberOfPoints();

  CPPUNIT_ASSERT( vme->GetAutomaticOutput()->GetNumberOfPoints() == m_Volume->GetOutput()->GetVTKData()->GetNumberOfPoints() );
  CPPUNIT_ASSERT( vme->GetManualOutput()->GetNumberOfPoints() == m_Volume->GetOutput()->GetVTKData()->GetNumberOfPoints() );
  vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(vme->GetOutput()->GetVTKData());
  sp->Update();
  double sr[2];
  sp->GetScalarRange(sr);
  CPPUNIT_ASSERT( sr[0] == 0.0 && sr[1] == 255.0 );
  CPPUNIT_ASSERT( sp->GetNumberOfPoints() == vme->GetManualOutput()->GetNumberOfPoints() );
  for (int i=0;i<sp->GetNumberOfPoints();i++)
  {
    CPPUNIT_ASSERT( sp->GetPointData()->GetScalars()->GetTuple1(i) == vme->GetManualOutput()->GetPointData()->GetScalars()->GetTuple1(i) );
  }
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestRefinementSegmentation()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  //////////////////////////////////////////////////////////////////////////
  vme->SetVolumeLink(m_Volume);
  CPPUNIT_ASSERT( vme->GetVolumeLink() == (mafNode*)m_Volume );
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::GLOBAL);
  vme->SetAutomaticSegmentationGlobalThreshold(5.0);
  CPPUNIT_ASSERT( vme->CheckNumberOfThresholds() == true );
  //////////////////////////////////////////////////////////////////////////
  vme->SetManualVolumeMask(m_VolumeManualMask);
  //////////////////////////////////////////////////////////////////////////
  vme->SetRefinementVolumeMask(m_VolumeRefinementMask);
  //////////////////////////////////////////////////////////////////////////
  vme->GetOutput()->Update();
  vme->Update();
  CPPUNIT_ASSERT( vme->GetAutomaticOutput()->GetNumberOfPoints() == m_Volume->GetOutput()->GetVTKData()->GetNumberOfPoints() );
  CPPUNIT_ASSERT( vme->GetManualOutput()->GetNumberOfPoints() == m_Volume->GetOutput()->GetVTKData()->GetNumberOfPoints() );
  CPPUNIT_ASSERT( vme->GetRefinementOutput()->GetNumberOfPoints() == m_Volume->GetOutput()->GetVTKData()->GetNumberOfPoints() );
  vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(vme->GetOutput()->GetVTKData());
  sp->Update();
  double sr[2];
  sp->GetScalarRange(sr);
  CPPUNIT_ASSERT( sr[0] == 0.0 && sr[1] == 255.0 );
  CPPUNIT_ASSERT( sp->GetNumberOfPoints() == vme->GetManualOutput()->GetNumberOfPoints() );
  for (int i=0;i<sp->GetNumberOfPoints();i++)
  {
    CPPUNIT_ASSERT( sp->GetPointData()->GetScalars()->GetTuple1(i) == vme->GetRefinementOutput()->GetPointData()->GetScalars()->GetTuple1(i) );
  }
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestSetManualVolumeMask()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  vme->SetVolumeLink(m_Volume);
  //////////////////////////////////////////////////////////////////////////
  vme->SetManualVolumeMask(m_VolumeManualMask);
  CPPUNIT_ASSERT( vme->GetManualVolumeMask() == (mafNode*)m_VolumeManualMask );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestSetRefinementVolumeMask()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  vme->SetVolumeLink(m_Volume);
  //////////////////////////////////////////////////////////////////////////
  vme->SetRefinementVolumeMask(m_VolumeRefinementMask);
  CPPUNIT_ASSERT( vme->GetRefinementVolumeMask() == (mafNode*)m_VolumeRefinementMask );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestAddSeed()
//---------------------------------------------------------
{
  int seed[3];
  int result;

  seed[0] = 0;
  seed[1] = 1;
  seed[2] = 2;

  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  result = vme->AddSeed(seed);
  CPPUNIT_ASSERT( result == MAF_OK );

  seed[0] = 3;
  seed[1] = 4;
  seed[2] = 5;

  result = vme->AddSeed(seed);
  CPPUNIT_ASSERT( result == MAF_OK );
  vme->Update();
  
  result = vme->GetSeed(0,seed);
  CPPUNIT_ASSERT( result == MAF_OK && seed[0]==0 && seed[1]==1 && seed[2]==2 );
  result = vme->GetSeed(1,seed);
  CPPUNIT_ASSERT( result == MAF_OK && seed[0]==3 && seed[1]==4 && seed[2]==5 );
  result = vme->GetSeed(2,seed);
  CPPUNIT_ASSERT( result == MAF_ERROR );
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestSetRegionGrowingUpperThreshold()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  //////////////////////////////////////////////////////////////////////////
  vme->SetRegionGrowingUpperThreshold(5.0);
  CPPUNIT_ASSERT( vme->GetRegionGrowingUpperThreshold() == 5.0 );
  //////////////////////////////////////////////////////////////////////////
  vme->SetRegionGrowingUpperThreshold(20.5);
  CPPUNIT_ASSERT( vme->GetRegionGrowingUpperThreshold() == 20.5 );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestSetRegionGrowingLowerThreshold()
//---------------------------------------------------------
{
  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  //////////////////////////////////////////////////////////////////////////
  vme->SetRegionGrowingLowerThreshold(5.0);
  CPPUNIT_ASSERT( vme->GetRegionGrowingLowerThreshold() == 5.0 );
  //////////////////////////////////////////////////////////////////////////
  vme->SetRegionGrowingLowerThreshold(20.5);
  CPPUNIT_ASSERT( vme->GetRegionGrowingLowerThreshold() == 20.5 );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void mafVMESegmentationVolumeTest::TestRemoveAllSeeds()
//---------------------------------------------------------
{
  int seed[3];

  seed[0] = 0;
  seed[1] = 1;
  seed[2] = 2;

  mafSmartPointer<mafVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  vme->AddSeed(seed);

  seed[0] = 3;
  seed[1] = 4;
  seed[2] = 5;

  vme->AddSeed(seed);
  vme->Update();

  CPPUNIT_ASSERT( vme->GetNumberOfSeeds() == 2 );

  vme->RemoveAllSeeds();

  CPPUNIT_ASSERT( vme->GetNumberOfSeeds() == 0 );

}
