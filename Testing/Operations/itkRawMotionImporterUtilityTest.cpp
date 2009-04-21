/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: itkRawMotionImporterUtilityTest.cpp,v $
Language:  C++
Date:      $Date: 2009-04-21 07:23:44 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2007
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "itkRawMotionImporterUtilityTest.h"
#include "itkRawMotionImporterUtility.h"
#include "mafString.h"

#include "vnl/vnl_matrix.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//matrix test 
//0.0 0.1 0.2 0.3
//1.0 1.1 1.2 1.3
//2.0 2.1 2.2 2.3
//3.0 3.1 3.2 3.3

//----------------------------------------------------------------------------
void itkRawMotionImporterUtilityTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void itkRawMotionImporterUtilityTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void itkRawMotionImporterUtilityTest::tearDown()
//----------------------------------------------------------------------------
{
}
//-----------------------------------------------------------
void itkRawMotionImporterUtilityTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  itkRawMotionImporterUtility *rMIU=new itkRawMotionImporterUtility();
  delete rMIU;
}
//-----------------------------------------------------------
void itkRawMotionImporterUtilityTest::TestReadMatrix() 
//-----------------------------------------------------------
{
  enum ID_RESULT
  {
    RES_OK = 0,
    RES_ERROR = 1,
  };

  mafString matrixFile = MED_DATA_ROOT;
  matrixFile << "/Matrix/TestMatrix001.txt";
  vnl_matrix<double> mat;
  itkRawMotionImporterUtility *rMIU=new itkRawMotionImporterUtility();
  int res = rMIU->ReadMatrix(mat,matrixFile);

  m_Result = res == RES_OK;
  TEST_RESULT;

  m_Result = mat.columns() == 4 && mat.rows() == 4;
  TEST_RESULT;

  m_Result = mat.get(0,0) == 0.0 && 
             mat.get(0,1) == 0.1 &&
             mat.get(0,2) == 0.2 &&
             mat.get(0,3) == 0.3 &&

             mat.get(1,0) == 1.0 &&
             mat.get(1,1) == 1.1 &&
             mat.get(1,2) == 1.2 &&
             mat.get(1,3) == 1.3 &&

             mat.get(2,0) == 2.0 &&
             mat.get(2,1) == 2.1 &&
             mat.get(2,2) == 2.2 &&
             mat.get(2,3) == 2.3 &&

             mat.get(3,0) == 3.0 &&
             mat.get(3,1) == 3.1 &&
             mat.get(3,2) == 3.2 &&
             mat.get(3,3) == 3.3;
  TEST_RESULT;

  delete rMIU;
}
