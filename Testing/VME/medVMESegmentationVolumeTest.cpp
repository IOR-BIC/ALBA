/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMESegmentationVolumeTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-19 15:26:12 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medVMESegmentationVolumeTest.h"
#include "medVMESegmentationVolume.h"
#include "medAttributeSegmentationVolume.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSetReader.h"
#include "vtkStructuredPoints.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void medVMESegmentationVolumeTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
  mafNEW(m_VME);

  m_Storage = mafVMEStorage::New();
  m_Storage->GetRoot()->SetName("root");
  m_Storage->GetRoot()->Initialize();

  mafNEW(m_Volume);
  m_Volume->ReparentTo(m_Storage->GetRoot());
  vtkMAFSmartPointer<vtkDataSetReader> r;
  mafString filename=MED_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  r->SetFileName(filename);
  r->Update();
  m_Volume->SetData(vtkStructuredPoints::SafeDownCast(r->GetOutput()),0.0);
  m_Volume->Update();
}
//----------------------------------------------------------------------------
void medVMESegmentationVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_VME);
  mafDEL(m_Volume);
  delete m_Storage;
  delete wxLog::SetActiveTarget(NULL);
}
//---------------------------------------------------------
void medVMESegmentationVolumeTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  medVMESegmentationVolume *vme1 = NULL;
  mafNEW(vme1);
  mafDEL(vme1);

  mafSmartPointer<medVMESegmentationVolume> vme2;
}
//---------------------------------------------------------
void medVMESegmentationVolumeTest::TestSetAutomaticSegmentationThresholdModality()
//---------------------------------------------------------
{
  mafSmartPointer<medVMESegmentationVolume> vme;
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::GLOBAL);

  CPPUNIT_ASSERT( vme->GetAutomaticSegmentationThresholdModality() == medVMESegmentationVolume::GLOBAL );
  CPPUNIT_ASSERT( medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetAutomaticSegmentationThresholdModality() == medVMESegmentationVolume::GLOBAL );
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::RANGE);

  CPPUNIT_ASSERT( vme->GetAutomaticSegmentationThresholdModality() == medVMESegmentationVolume::RANGE );
  CPPUNIT_ASSERT( medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetAutomaticSegmentationThresholdModality() == medVMESegmentationVolume::RANGE );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void medVMESegmentationVolumeTest::TestAddRange()
//---------------------------------------------------------
{
  mafSmartPointer<medVMESegmentationVolume> vme;
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
  medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 0 && endSlice == 20 && threshold == 20.5);
  //////////////////////////////////////////////////////////////////////////
  vme->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 21 && endSlice == 100 && threshold == 15.0);
  medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 21 && endSlice == 100 && threshold == 15.0);
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void medVMESegmentationVolumeTest::TestUpdateRange()
//---------------------------------------------------------
{
  mafSmartPointer<medVMESegmentationVolume> vme;
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
  medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 0 && endSlice == 100 && threshold == 2.0);
  //////////////////////////////////////////////////////////////////////////
  vme->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 101 && endSlice == 200 && threshold == 12.5);
  medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 101 && endSlice == 200 && threshold == 12.5);
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void medVMESegmentationVolumeTest::TestDeleteRange()
//---------------------------------------------------------
{
  mafSmartPointer<medVMESegmentationVolume> vme;
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
  CPPUNIT_ASSERT(  startSlice == 0 && endSlice == 20 && threshold == 20.5);
  medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT(  startSlice == 0 && endSlice == 20 && threshold == 20.5);
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void medVMESegmentationVolumeTest::TestSetAutomaticSegmentationGlobalThreshold()
//---------------------------------------------------------
{
  mafSmartPointer<medVMESegmentationVolume> vme;
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationGlobalThreshold(1.0);

  CPPUNIT_ASSERT( vme->GetAutomaticSegmentationGlobalThreshold() == 1.0 );
  CPPUNIT_ASSERT( medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetAutomaticSegmentationGlobalThreshold() == 1.0 );
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationGlobalThreshold(5.2);

  CPPUNIT_ASSERT( vme->GetAutomaticSegmentationGlobalThreshold() == 5.2 );
  CPPUNIT_ASSERT( medAttributeSegmentationVolume::SafeDownCast(vme->GetAttribute("SegmentationVolumeData"))->GetAutomaticSegmentationGlobalThreshold() == 5.2 );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void medVMESegmentationVolumeTest::TestSetVolumeLink()
//---------------------------------------------------------
{
  mafSmartPointer<medVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  //////////////////////////////////////////////////////////////////////////
  vme->SetVolumeLink((mafNode*)m_Volume);
  CPPUNIT_ASSERT( vme->GetVolumeLink() == (mafNode*)m_Volume );
  //////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------
void medVMESegmentationVolumeTest::TestAutomaticSegmentation()
//---------------------------------------------------------
{
  mafSmartPointer<medVMESegmentationVolume> vme;
  vme->ReparentTo(m_Storage->GetRoot());
  //////////////////////////////////////////////////////////////////////////
  vme->SetVolumeLink(m_Volume);
  CPPUNIT_ASSERT( vme->GetVolumeLink() == (mafNode*)m_Volume );
  //////////////////////////////////////////////////////////////////////////
  vme->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::RANGE);
  vme->AddRange(0,50,10.0);
  vme->AddRange(51,76,5.0);
  CPPUNIT_ASSERT( vme->CheckNumberOfThresholds() == true );
  //////////////////////////////////////////////////////////////////////////
  vme->GetOutput()->Update();
  vme->Update();
  vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(vme->GetOutput()->GetVTKData());
  sp->Update();
  double sr[2];
  sp->GetScalarRange(sr);
  CPPUNIT_ASSERT( sr[0] == 0.0 && sr[1] == 255.0 );
  //////////////////////////////////////////////////////////////////////////
}
