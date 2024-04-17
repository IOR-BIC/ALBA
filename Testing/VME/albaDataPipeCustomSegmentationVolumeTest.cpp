/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustomSegmentationVolumeTest
 Authors: Alberto Losi, Gianluigi Crimi
 
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
#include "albaVMEVolumeGray.h"
#include "albaSmartPointer.h"
#include "albaDataPipeCustomSegmentationVolumeTest.h"
#include "albaDataPipeCustomSegmentationVolume.h"
#include "vtkStructuredPointsReader.h"
#include "vtkRectilinearGridReader.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "albaVMESegmentationVolume.h"

//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkStructuredPointsReader *readerSP;
  vtkRectilinearGridReader *readerRG;
  
  vtkNEW(readerSP);
  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_DataPipeCustomSegmentationVolume/VolumeSP.vtk";
  readerSP->SetFileName(filename.GetCStr());
  readerSP->Update();
  // Volume with 11 slices and scalar range between 0 and 9
  albaNEW(m_VolumeSP);
  m_VolumeSP->SetData((vtkImageData*)readerSP->GetOutput(),0.0);
  m_VolumeSP->GetOutput()->Update();
  m_VolumeSP->Update();
  vtkDEL(readerSP);

  vtkNEW(readerRG);
  filename = ALBA_DATA_ROOT;
  filename << "/Test_DataPipeCustomSegmentationVolume/VolumeRG.vtk";
  readerRG->SetFileName(filename.GetCStr());
  readerRG->Update();
  // Rectilinear Grid  10x10x10 values between 1 and 10
  albaNEW(m_VolumeRG);
  m_VolumeRG->SetData((vtkRectilinearGrid*)readerRG->GetOutput(),0.0);
  m_VolumeRG->GetOutput()->Update();
  m_VolumeRG->Update();
  vtkDEL(readerRG);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_VolumeSP);
  albaDEL(m_VolumeRG);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);
  CPPUNIT_ASSERT(pipe != NULL);
  albaDEL(pipe);

//   pipe = new albaDataPipeCustomSegmentationVolume();
//   CPPUNIT_ASSERT(pipe != NULL);
//   cppDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
//   albaDataPipeCustomSegmentationVolume *pipe;
//   albaNEW(pipe);
// 
//   albaDataPipeCustomSegmentationVolume *pipe_copy;
//   albaNEW(pipe_copy);
// 
//   pipe_copy->DeepCopy(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::SetGetVolumeTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipeCustomSegmentationVolume>pipe;

  pipe->SetVolume(m_VolumeSP);

  CPPUNIT_ASSERT(pipe->GetVolume() == m_VolumeSP);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::SetGetAutomaticSegmentationThresholdModalityTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);
  
  int modality = albaVMESegmentationVolume::RANGE;
  pipe->SetAutomaticSegmentationThresholdModality(modality);

  CPPUNIT_ASSERT(pipe->GetAutomaticSegmentationThresholdModality() == modality);

  modality = albaVMESegmentationVolume::GLOBAL;
  pipe->SetAutomaticSegmentationThresholdModality(modality);

  CPPUNIT_ASSERT(pipe->GetAutomaticSegmentationThresholdModality() == modality);

  albaDEL(pipe);
}

//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::AddGetRangeTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  CPPUNIT_ASSERT(ALBA_OK == pipe->AddRange(0,1,5));
  CPPUNIT_ASSERT(ALBA_OK == pipe->AddRange(2,3,4));
  CPPUNIT_ASSERT(2 == pipe->GetNumberOfRanges());
  CPPUNIT_ASSERT(ALBA_ERROR == pipe->AddRange(0,1,6));
  
  int startSlice;
  int endSlice;
  double threshold;

  CPPUNIT_ASSERT(ALBA_OK == pipe->GetRange(0,startSlice,endSlice,threshold));
  CPPUNIT_ASSERT(0 == startSlice && 1 == endSlice && 5 == threshold);

  CPPUNIT_ASSERT(ALBA_OK == pipe->GetRange(1,startSlice,endSlice,threshold));
  CPPUNIT_ASSERT(2 == startSlice && 3 == endSlice && 4 == threshold);

  CPPUNIT_ASSERT(ALBA_ERROR == pipe->GetRange(2,startSlice,endSlice,threshold));

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::UpdateRangeTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  pipe->AddRange(0,1,5);
  pipe->AddRange(2,3,4);

  int startSlice = 2;
  int endSlice = 3;
  double threshold = 5;

  CPPUNIT_ASSERT(ALBA_OK == pipe->UpdateRange(1,startSlice,endSlice,threshold));
  CPPUNIT_ASSERT(ALBA_ERROR == pipe->UpdateRange(2,startSlice,endSlice,threshold));

  startSlice = 1;
  CPPUNIT_ASSERT(ALBA_ERROR == pipe->UpdateRange(1,startSlice,endSlice,threshold));

  pipe->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(2 == startSlice && 3 == endSlice && 5 == threshold);

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::DeleteRangeTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  pipe->AddRange(0,1,5);
  pipe->AddRange(2,3,4);

  int startSlice = 2;
  int endSlice = 3;
  double threshold = 5;

  CPPUNIT_ASSERT(ALBA_OK == pipe->DeleteRange(1));
  CPPUNIT_ASSERT(1 == pipe->GetNumberOfRanges());

  CPPUNIT_ASSERT(ALBA_ERROR == pipe->DeleteRange(2));

  CPPUNIT_ASSERT(ALBA_ERROR == pipe->GetRange(1,startSlice,endSlice,threshold));

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::RemoveAllRangesTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  pipe->AddRange(0,1,5);
  pipe->AddRange(2,3,4);

  CPPUNIT_ASSERT(ALBA_OK == pipe->RemoveAllRanges());

  CPPUNIT_ASSERT(0 == pipe->GetNumberOfRanges());

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::GetNumberOfRangesTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  //pipe->SetVolume(m_Volume);

  pipe->AddRange(0,1,5);
  pipe->AddRange(2,3,4);
  CPPUNIT_ASSERT(2 == pipe->GetNumberOfRanges());

  pipe->DeleteRange(1);
  CPPUNIT_ASSERT(1 == pipe->GetNumberOfRanges());
  
  pipe->RemoveAllRanges();

  CPPUNIT_ASSERT(0 == pipe->GetNumberOfRanges());

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::SetGetAutomaticSegmentationGlobalThresholdTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  pipe->SetAutomaticSegmentationGlobalThreshold(5.);
  CPPUNIT_ASSERT(5 == pipe->GetAutomaticSegmentationGlobalThreshold());

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::CheckNumberOfThresholdsTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  pipe->SetVME(m_VolumeSP);
  pipe->SetVolume(m_VolumeSP);

  pipe->SetAutomaticSegmentationThresholdModality(albaVMESegmentationVolume::GLOBAL);
  CPPUNIT_ASSERT(true == pipe->CheckNumberOfThresholds());

  pipe->SetAutomaticSegmentationThresholdModality(albaVMESegmentationVolume::RANGE);
  CPPUNIT_ASSERT(false == pipe->CheckNumberOfThresholds());

  pipe->AddRange(0,9,5);
  CPPUNIT_ASSERT(false == pipe->CheckNumberOfThresholds());

  pipe->AddRange(10,11,6);
  CPPUNIT_ASSERT(true == pipe->CheckNumberOfThresholds());

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::SetGetManualVolumeMaskTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);
  
  pipe->SetManualVolumeMask(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetManualVolumeMask() == m_VolumeSP);

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
// GIGI
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::GetAutomaticOutputTest()
//----------------------------------------------------------------------------
{
  
  albaDataPipeCustomSegmentationVolume *pipe;

  //Check  without a volume
  albaNEW(pipe);
  CPPUNIT_ASSERT(pipe->GetAutomaticOutput()==NULL);
  albaDEL(pipe);

  //Check  the structured points case
  albaNEW(pipe);
  pipe->SetVolume(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetAutomaticOutput()!=NULL);
  albaDEL(pipe);

  //Check  the rectilinear grid case
  albaNEW(pipe);
  pipe->SetVolume(m_VolumeRG);
  CPPUNIT_ASSERT(pipe->GetAutomaticOutput()!=NULL);
  albaDEL(pipe);

}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::GetManualOutputTest()
//----------------------------------------------------------------------------
{

  albaDataPipeCustomSegmentationVolume *pipe;

  //Check  without a volume
  albaNEW(pipe);
  CPPUNIT_ASSERT(pipe->GetManualOutput()==NULL);
  albaDEL(pipe);

  //Check  the structured points case
  albaNEW(pipe);
  pipe->SetVolume(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetManualOutput()!=NULL);
  albaDEL(pipe);

  //Check  the rectilinear grid case
  albaNEW(pipe);
  pipe->SetVolume(m_VolumeRG);
  CPPUNIT_ASSERT(pipe->GetManualOutput()!=NULL);
  albaDEL(pipe);

}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::GetRefinementOutputTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;

  //Check  without a volume
  albaNEW(pipe);
  CPPUNIT_ASSERT(pipe->GetRefinementOutput()==NULL);
  albaDEL(pipe);

  //Check  the structured points case
  albaNEW(pipe);
  pipe->SetVolume(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetRefinementOutput()!=NULL);
  albaDEL(pipe);

  //Check  the rectilinear grid case
  albaNEW(pipe);
  pipe->SetVolume(m_VolumeRG);
  CPPUNIT_ASSERT(pipe->GetRefinementOutput()!=NULL);
  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::GetRegionGrowingOutputTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;

  //Check  without a volume
  albaNEW(pipe);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()==NULL);
  albaDEL(pipe);

  //Check  the structured points case
  albaNEW(pipe);
  pipe->SetVolume(m_VolumeSP);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()!=NULL);
  pipe->SetRegionGrowingLowerThreshold(1);
  pipe->SetRegionGrowingUpperThreshold(7);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()!=NULL);
  albaDEL(pipe);

  //Check  the rectilinear grid case
  albaNEW(pipe);
  pipe->SetVolume(m_VolumeRG);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()!=NULL);
  pipe->SetRegionGrowingLowerThreshold(1);
  pipe->SetRegionGrowingUpperThreshold(7);
  CPPUNIT_ASSERT(pipe->GetRegionGrowingOutput()!=NULL);
  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::SetGetRegionGrowingUpperThresholdTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  pipe->SetRegionGrowingUpperThreshold(7);
  CPPUNIT_ASSERT(7 == pipe->GetRegionGrowingUpperThreshold());

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::SetGetRegionGrowingLowerThresholdTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);
  
  pipe->SetRegionGrowingLowerThreshold(7);
  CPPUNIT_ASSERT(7 == pipe->GetRegionGrowingLowerThreshold());

  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::SetGetRegionGrowingSliceRangeTest()
//----------------------------------------------------------------------------
{
  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  int start,end;

  pipe->SetRegionGrowingSliceRange(2,7);

  pipe->GetRegionGrowingSliceRange(start,end);

  CPPUNIT_ASSERT(2==start && 7==end);
  
  albaDEL(pipe);

}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::AddGetSeedTest()
//----------------------------------------------------------------------------
{
  int s1[]={1,2,3};
  int s2[]={4,5,6};
  int s3[]={7,8,9};

  int r2[3];

  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  CPPUNIT_ASSERT(pipe->AddSeed(s1)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s2)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s3)==ALBA_OK);
  
  //out of bound error check
  CPPUNIT_ASSERT(pipe->GetSeed(-1,r2)==ALBA_ERROR);
  CPPUNIT_ASSERT(pipe->GetSeed(3,r2)==ALBA_ERROR);

  CPPUNIT_ASSERT(pipe->GetSeed(1,r2)==ALBA_OK);

  CPPUNIT_ASSERT(s2[0]==r2[0] && s2[1]==r2[1] && s2[2]==r2[2]);
  
  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::DeleteSeedTest()
//----------------------------------------------------------------------------
{
int s1[]={1,2,3};
  int s2[]={4,5,6};
  int s3[]={7,8,9};

  int r3[3];

  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  CPPUNIT_ASSERT(pipe->AddSeed(s1)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s2)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s3)==ALBA_OK);
  

  //out of bound error check
  CPPUNIT_ASSERT(pipe->DeleteSeed(-1)==ALBA_ERROR);
  CPPUNIT_ASSERT(pipe->DeleteSeed(3)==ALBA_ERROR);

  
  CPPUNIT_ASSERT(pipe->DeleteSeed(1)==ALBA_OK);
  //now s3 will be in 2-nd pos
  CPPUNIT_ASSERT(pipe->GetSeed(1,r3)==ALBA_OK);
  CPPUNIT_ASSERT(s3[0]==r3[0] && s3[1]==r3[1] && s3[2]==r3[2]);
  
  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::RemoveAllSeedsTest()
//----------------------------------------------------------------------------
{
  int s1[]={1,2,3};
  int s2[]={4,5,6};
  int s3[]={7,8,9};

  int r2[3];

  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  CPPUNIT_ASSERT(pipe->AddSeed(s1)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s2)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s3)==ALBA_OK);

  CPPUNIT_ASSERT(pipe->RemoveAllSeeds()==ALBA_OK);

  CPPUNIT_ASSERT(pipe->GetSeed(0,r2)==ALBA_ERROR);

  
  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomSegmentationVolumeTest::GetNumberOfSeedsTest()
//----------------------------------------------------------------------------
{
  int s1[]={1,2,3};

  albaDataPipeCustomSegmentationVolume *pipe;
  albaNEW(pipe);

  CPPUNIT_ASSERT(pipe->GetNumberOfSeeds()==0);

  CPPUNIT_ASSERT(pipe->AddSeed(s1)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s1)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->AddSeed(s1)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->GetNumberOfSeeds()==3);

  CPPUNIT_ASSERT(pipe->DeleteSeed(1)==ALBA_OK);
  CPPUNIT_ASSERT(pipe->GetNumberOfSeeds()==2);
  
  CPPUNIT_ASSERT(pipe->RemoveAllSeeds()==ALBA_OK);
  CPPUNIT_ASSERT(pipe->GetNumberOfSeeds()==0);

  albaDEL(pipe);
}