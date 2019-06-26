/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkTriangleQualityRatioTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkTriangleQualityRatioTEST_H__
#define __CPP_UNIT_vtkTriangleQualityRatioTEST_H__

#include "albaTest.h"

class vtkTriangleQualityRatioTest : public albaTest
{
  CPPUNIT_TEST_SUITE( vtkTriangleQualityRatioTest );
  CPPUNIT_TEST( QualityTestValues );
	CPPUNIT_TEST( QualityTestRender );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void QualityTestValues();
    void QualityTestRender();
		void RenderData(vtkPolyData *data);
};

#endif