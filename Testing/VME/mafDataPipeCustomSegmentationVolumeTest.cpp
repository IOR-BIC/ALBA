/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustomSegmentationVolumeTest
 Authors: Alberto Losi, Gianluigi Crimi
 
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
#include "mafVMEVolumeGray.h"
#include "mafSmartPointer.h"
#include "mafDataPipeCustomSegmentationVolumeTest.h"
#include "mafDataPipeCustomSegmentationVolume.h"
#include "vtkStructuredPointsReader.h"
#include "vtkRectilinearGridReader.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "mafVMESegmentationVolume.h"

//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::setUp()
//----------------------------------------------------------------------------
{
  vtkStructuredPointsReader *readerSP;
  vtkRectilinearGridReader *readerRG;
  
  vtkNEW(readerSP);
  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_DataPipeCustomSegmentationVolume/VolumeSP.vtk";
  readerSP->SetFileName(filename.GetCStr());
  readerSP->Update();
  // Volume with 11 slices and scalar range between 0 and 9
  mafNEW(m_VolumeSP);
  m_VolumeSP->SetData((vtkImageData*)readerSP->GetOutput(),0.0);
  m_VolumeSP->GetOutput()->GetVTKData()->Update();
  m_VolumeSP->GetOutput()->Update();
  m_VolumeSP->Update();
  vtkDEL(readerSP);

  vtkNEW(readerRG);
  filename = MAF_DATA_ROOT;
  filename << "/Test_DataPipeCustomSegmentationVolume/VolumeRG.vtk";
  readerRG->SetFileName(filename.GetCStr());
  readerRG->Update();
  // Rectilinear Grid  10x10x10 values between 1 and 10
  mafNEW(m_VolumeRG);
  m_VolumeRG->SetData((vtkRectilinearGrid*)readerRG->GetOutput(),0.0);
  m_VolumeRG->GetOutput()->GetVTKData()->Update();
  m_VolumeRG->GetOutput()->Update();
  m_VolumeRG->Update();
  vtkDEL(readerRG);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_VolumeSP);
  mafDEL(m_VolumeRG);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);
  CPPUNIT_ASSERT(pipe != NULL);
  mafDEL(pipe);

//   pipe = new mafDataPipeCustomSegmentationVolume();
//   CPPUNIT_ASSERT(pipe != NULL);
//   cppDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
//   mafDataPipeCustomSegmentationVolume *pipe;
//   mafNEW(pipe);
// 
//   mafDataPipeCustomSegmentationVolume *pipe_copy;
//   mafNEW(pipe_copy);
// 
//   pipe_copy->DeepCopy(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::SetGetVolumeTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeCustomSegmentationVolume>pipe;

  pipe->SetVolume(m_VolumeSP);

  CPPUNIT_ASSERT(pipe->GetVolume() == m_VolumeSP);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::SetGetAutomaticSegmentationThresholdModalityTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);
  
  int modality = mafVMESegmentationVolume::RANGE;
  pipe->SetAutomaticSegmentationThresholdModality(modality);

  CPPUNIT_ASSERT(pipe->GetAutomaticSegmentationThresholdModality() == modality);

  modality = mafVMESegmentationVolume::GLOBAL;
  pipe->SetAutomaticSegmentationThresholdModality(modality);

  CPPUNIT_ASSERT(pipe->GetAutomaticSegmentationThresholdModality() == modality);

  mafDEL(pipe);
}

//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::AddGetRangeTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  CPPUNIT_ASSERT(MAF_OK == pipe->AddRange(0,1,5));
  CPPUNIT_ASSERT(MAF_OK == pipe->AddRange(2,3,4));
  CPPUNIT_ASSERT(2 == pipe->GetNumberOfRanges());
  CPPUNIT_ASSERT(MAF_ERROR == pipe->AddRange(0,1,6));
  
  int startSlice;
  int endSlice;
  double threshold;

  CPPUNIT_ASSERT(MAF_OK == pipe->GetRange(0,startSlice,endSlice,threshold));
  CPPUNIT_ASSERT(0 == startSlice && 1 == endSlice && 5 == threshold);

  CPPUNIT_ASSERT(MAF_OK == pipe->GetRange(1,startSlice,endSlice,threshold));
  CPPUNIT_ASSERT(2 == startSlice && 3 == endSlice && 4 == threshold);

  CPPUNIT_ASSERT(MAF_ERROR == pipe->GetRange(2,startSlice,endSlice,threshold));

  mafDEL(pipe);
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::UpdateRangeTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  pipe->AddRange(0,1,5);
  pipe->AddRange(2,3,4);

  int startSlice = 2;
  int endSlice = 3;
  double threshold = 5;

  CPPUNIT_ASSERT(MAF_OK == pipe->UpdateRange(1,startSlice,endSlice,threshold));
  CPPUNIT_ASSERT(MAF_ERROR == pipe->UpdateRange(2,startSlice,endSlice,threshold));

  startSlice = 1;
  CPPUNIT_ASSERT(MAF_ERROR == pipe->UpdateRange(1,startSlice,endSlice,threshold));

  pipe->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(2 == startSlice && 3 == endSlice && 5 == threshold);

  mafDEL(pipe);
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::DeleteRangeTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  pipe->AddRange(0,1,5);
  pipe->AddRange(2,3,4);

  int startSlice = 2;
  int endSlice = 3;
  double threshold = 5;

  CPPUNIT_ASSERT(MAF_OK == pipe->DeleteRange(1));
  CPPUNIT_ASSERT(1 == pipe->GetNumberOfRanges());

  CPPUNIT_ASSERT(MAF_ERROR == pipe->DeleteRange(2));

  CPPUNIT_ASSERT(MAF_ERROR == pipe->GetRange(1,startSlice,endSlice,threshold));

  mafDEL(pipe);
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::RemoveAllRangesTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  pipe->AddRange(0,1,5);
  pipe->AddRange(2,3,4);

  CPPUNIT_ASSERT(MAF_OK == pipe->RemoveAllRanges());

  CPPUNIT_ASSERT(0 == pipe->GetNumberOfRanges());

  mafDEL(pipe);
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::GetNumberOfRangesTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  pipe->AddRange(0,1,5);
  pipe->AddRange(2,3,4);
  CPPUNIT_ASSERT(2 == pipe->GetNumberOfRanges());

  pipe->DeleteRange(1);
  CPPUNIT_ASSERT(1 == pipe->GetNumberOfRanges());
  
  pipe->RemoveAllRanges();

  CPPUNIT_ASSERT(0 == pipe->GetNumberOfRanges());

  mafDEL(pipe);
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::SetGetAutomaticSegmentationGlobalThresholdTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  pipe->SetAutomaticSegmentationGlobalThreshold(5.);
  CPPUNIT_ASSERT(5 == pipe->GetAutomaticSegmentationGlobalThreshold());

  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::CheckNumberOfThresholdsTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  pipe->SetVolume(m_VolumeSP);

  pipe->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::GLOBAL);
  CPPUNIT_ASSERT(true == pipe->CheckNumberOfThresholds());

  pipe->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::RANGE);
  CPPUNIT_ASSERT(false == pipe->CheckNumberOfThresholds());

  pipe->AddRange(0,9,5);
  CPPUNIT_ASSERT(false == pipe->CheckNumberOfThresholds());

  pipe->AddRange(10,11,6);
  CPPUNIT_ASSERT(true == pipe->CheckNumberOfThresholds());

  mafDEL(pipe);
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::SetGetManualVolumeMaskTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);
  
  pipe->SetManualVolumeMask(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetManualVolumeMask() == m_VolumeSP);

  mafDEL(pipe);
}
//----------------------------------------------------------------------------
// GIGI
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::GetAutomaticOutputTest()
//----------------------------------------------------------------------------
{
  
  mafDataPipeCustomSegmentationVolume *pipe;

  //Check  without a volume
  mafNEW(pipe);
  CPPUNIT_ASSERT(pipe->GetAutomaticOutput()==NULL);
  mafDEL(pipe);

  //Check  the structured points case
  mafNEW(pipe);
  pipe->SetVolume(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetAutomaticOutput()!=NULL);
  mafDEL(pipe);

  //Check  the rectilinear grid case
  mafNEW(pipe);
  pipe->SetVolume(m_VolumeRG);
  CPPUNIT_ASSERT(pipe->GetAutomaticOutput()!=NULL);
  mafDEL(pipe);

}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::GetManualOutputTest()
//----------------------------------------------------------------------------
{

  mafDataPipeCustomSegmentationVolume *pipe;

  //Check  without a volume
  mafNEW(pipe);
  CPPUNIT_ASSERT(pipe->GetManualOutput()==NULL);
  mafDEL(pipe);

  //Check  the structured points case
  mafNEW(pipe);
  pipe->SetVolume(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetManualOutput()!=NULL);
  mafDEL(pipe);

  //Check  the rectilinear grid case
  mafNEW(pipe);
  pipe->SetVolume(m_VolumeRG);
  CPPUNIT_ASSERT(pipe->GetManualOutput()!=NULL);
  mafDEL(pipe);

}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::GetRefinementOutputTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;

  //Check  without a volume
  mafNEW(pipe);
  CPPUNIT_ASSERT(pipe->GetRefinementOutput()==NULL);
  mafDEL(pipe);

  //Check  the structured points case
  mafNEW(pipe);
  pipe->SetVolume(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetRefinementOutput()!=NULL);
  mafDEL(pipe);

  //Check  the rectilinear grid case
  mafNEW(pipe);
  pipe->SetVolume(m_VolumeRG);
  CPPUNIT_ASSERT(pipe->GetRefinementOutput()!=NULL);
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::GetRegionGrowingOutputTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;

  //Check  without a volume
  mafNEW(pipe);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()==NULL);
  mafDEL(pipe);

  //Check  the structured points case
  mafNEW(pipe);
  pipe->SetVolume(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()!=NULL);
  pipe->SetRegionGrowingLowerThreshold(1);
  pipe->SetRegionGrowingUpperThreshold(7);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()!=NULL);
  mafDEL(pipe);

  //Check  the rectilinear grid case
  mafNEW(pipe);
  pipe->SetVolume(m_VolumeRG);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()!=NULL);
  pipe->SetRegionGrowingLowerThreshold(1);
  pipe->SetRegionGrowingUpperThreshold(7);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()!=NULL);
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::SetGetRegionGrowingUpperThresholdTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  pipe->SetRegionGrowingUpperThreshold(7);
  CPPUNIT_ASSERT(7 == pipe->GetRegionGrowingUpperThreshold());

  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::SetGetRegionGrowingLowerThresholdTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);
  
  pipe->SetRegionGrowingLowerThreshold(7);
  CPPUNIT_ASSERT(7 == pipe->GetRegionGrowingLowerThreshold());

  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::SetGetRegionGrowingSliceRangeTest()
//----------------------------------------------------------------------------
{
  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  int start,end;

  pipe->SetRegionGrowingSliceRange(2,7);

  pipe->GetRegionGrowingSliceRange(start,end);

  CPPUNIT_ASSERT(2==start && 7==end);
  
  mafDEL(pipe);

}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::AddGetSeedTest()
//----------------------------------------------------------------------------
{
  int s1[]={1,2,3};
  int s2[]={4,5,6};
  int s3[]={7,8,9};

  int r2[3];

  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  CPPUNIT_ASSERT(pipe->AddSeed(s1)==MAF_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s2)==MAF_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s3)==MAF_OK);
  
  //out of bound error check
  CPPUNIT_ASSERT(pipe->GetSeed(-1,r2)==MAF_ERROR);
  CPPUNIT_ASSERT(pipe->GetSeed(3,r2)==MAF_ERROR);

  CPPUNIT_ASSERT(pipe->GetSeed(1,r2)==MAF_OK);

  CPPUNIT_ASSERT(s2[0]==r2[0] && s2[1]==r2[1] && s2[2]==r2[2]);
  
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::DeleteSeedTest()
//----------------------------------------------------------------------------
{
int s1[]={1,2,3};
  int s2[]={4,5,6};
  int s3[]={7,8,9};

  int r3[3];

  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  CPPUNIT_ASSERT(pipe->AddSeed(s1)==MAF_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s2)==MAF_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s3)==MAF_OK);
  

  //out of bound error check
  CPPUNIT_ASSERT(pipe->DeleteSeed(-1)==MAF_ERROR);
  CPPUNIT_ASSERT(pipe->DeleteSeed(3)==MAF_ERROR);

  
  CPPUNIT_ASSERT(pipe->DeleteSeed(1)==MAF_OK);
  //now s3 will be in 2-nd pos
  CPPUNIT_ASSERT(pipe->GetSeed(1,r3)==MAF_OK);
  CPPUNIT_ASSERT(s3[0]==r3[0] && s3[1]==r3[1] && s3[2]==r3[2]);
  
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::RemoveAllSeedsTest()
//----------------------------------------------------------------------------
{
  int s1[]={1,2,3};
  int s2[]={4,5,6};
  int s3[]={7,8,9};

  int r2[3];

  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  CPPUNIT_ASSERT(pipe->AddSeed(s1)==MAF_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s2)==MAF_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s3)==MAF_OK);

  CPPUNIT_ASSERT(pipe->RemoveAllSeeds()==MAF_OK);

  CPPUNIT_ASSERT(pipe->GetSeed(0,r2)==MAF_ERROR);

  
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolumeTest::GetNumberOfSeedsTest()
//----------------------------------------------------------------------------
{
  int s1[]={1,2,3};

  mafDataPipeCustomSegmentationVolume *pipe;
  mafNEW(pipe);

  CPPUNIT_ASSERT(pipe->GetNumberOfSeeds()==0);

  CPPUNIT_ASSERT(pipe->AddSeed(s1)==MAF_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s1)==MAF_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s1)==MAF_OK);
  CPPUNIT_ASSERT(pipe->GetNumberOfSeeds()==3);

  CPPUNIT_ASSERT(pipe->DeleteSeed(1)==MAF_OK);
  CPPUNIT_ASSERT(pipe->GetNumberOfSeeds()==2);
  
  CPPUNIT_ASSERT(pipe->RemoveAllSeeds()==MAF_OK);
  CPPUNIT_ASSERT(pipe->GetNumberOfSeeds()==0);

  mafDEL(pipe);
}