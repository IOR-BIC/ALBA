/*=========================================================================

 Program: MAF2
 Module: mafVMEVolumeLargeUtilsTest
 Authors: Alberto Losi
 
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
#include "mafVMEVolumeLargeUtilsTest.h"
#include "mafVMEVolumeLargeUtils.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafVMEVolumeLargeUtilsTest::setUp()
//----------------------------------------------------------------------------
{
  vtkObject::SetGlobalWarningDisplay(false);

  m_TestFilePath = MAF_DATA_ROOT;
  m_TestFilePath << "/Test_VMEVolumeLargeUtils/";
  m_TestFilePath << "volume_1.vtk";
}
//----------------------------------------------------------------------------
void mafVMEVolumeLargeUtilsTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEVolumeLargeUtilsTest::GetFileLengthTest()
//----------------------------------------------------------------------------
{
  vtkIdType64 file_size = mafVMEVolumeLargeUtils::GetFileLength(m_TestFilePath);
  CPPUNIT_ASSERT(file_size == 12749330);
}
//----------------------------------------------------------------------------
void mafVMEVolumeLargeUtilsTest::IsFileLargeTest()
//----------------------------------------------------------------------------
{
  CPPUNIT_ASSERT(mafVMEVolumeLargeUtils::IsFileLarge(m_TestFilePath) == false);
}
//----------------------------------------------------------------------------
void mafVMEVolumeLargeUtilsTest::IsVolumeLargeTest()
//----------------------------------------------------------------------------
{
  int voi[6];
  int data_type = VTK_DOUBLE;
  int mem_limit = 200; // in MB

  voi[0] = 0;
  voi[1] = 255;
  voi[2] = 0;
  voi[3] = 255;
  voi[4] = 0;
  voi[5] = 255;
  CPPUNIT_ASSERT(mafVMEVolumeLargeUtils::IsVolumeLarge(voi,data_type,mem_limit,200) == true);

  voi[0] = 0;
  voi[1] = 1;
  voi[2] = 0;
  voi[3] = 1;
  voi[4] = 0;
  voi[5] = 1;
  CPPUNIT_ASSERT(mafVMEVolumeLargeUtils::IsVolumeLarge(voi,data_type,mem_limit,200) == false);

}