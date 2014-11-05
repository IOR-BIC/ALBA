/*=========================================================================

 Program: MAF2Medical
 Module: NVectorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "NVectorTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"
#include "vtkMEDPoissonSurfaceReconstruction.cxx"

//-------------------------------------------------------------------------
void NVectorTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  NVector<double, 3> *v = new NVector<double, 3>(2);
  delete v;
}
//-------------------------------------------------------------------------
void NVectorTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  NVector<double, 3> v;
  v.Resize(5);
  NVector<double, 3> v2 = NVector<double, 3>(v);
  CPPUNIT_ASSERT(v.Dimensions() == v2.Dimensions());
  
  NVector<double, 3> v3(5);
  CPPUNIT_ASSERT(v.Dimensions() == v3.Dimensions());

  v3.Resize(6);
  CPPUNIT_ASSERT(v.Dimensions() != v3.Dimensions());
  
  //this code crash in the constructor
  /*double *arr = new double[5];
  arr[0] = arr[1] = arr[2] = arr[3] = arr[4] = 5.5;
  NVector<double, 3> *v4 = new NVector<double, 3>(5, arr);
  delete v4;*/
}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorRoundBrackets()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(5);
    v[0][0] = 4.;
    CPPUNIT_ASSERT(v(0)[0] == 4.);

    const NVector<double, 3> v2(v);
    CPPUNIT_ASSERT(v2(0)[0] == 4.);
}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorSquareBrackets()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(5);
    v[0][0] = 4.;
    CPPUNIT_ASSERT(v[0][0] == 4.);

    const NVector<double, 3> v2(v);
    CPPUNIT_ASSERT(v2[0][0] == 4.);
}
//-------------------------------------------------------------------------
void NVectorTest::TestSetZero()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    v.SetZero();
    CPPUNIT_ASSERT(v[0][0] == 0. && v[0][1] == 0. && v[0][2] == 0.);
}
//-------------------------------------------------------------------------
void NVectorTest::TestDimensions()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v;
    CPPUNIT_ASSERT(v.Dimensions() == 0);

    v.Resize(3);
    CPPUNIT_ASSERT(v.Dimensions() == 3);
}
//-------------------------------------------------------------------------
void NVectorTest::TestResize()
//-------------------------------------------------------------------------
{
    TestDimensions(); // the test can be considered the same
}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorAsterisk()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    NVector<double, 3> res(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    res = v * 3.;
    CPPUNIT_ASSERT(res[0][0] == 12. && res[0][1] == 12. && res[0][2] == 12.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorSlash()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    NVector<double, 3> res(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    res = v / 2.;
    CPPUNIT_ASSERT(res[0][0] == 2. && res[0][1] == 2. && res[0][2] == 2.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorMinus()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    NVector<double, 3> vm(3);
    NVector<double, 3> res(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    vm[0][0] = vm[0][1] =vm[0][2] =1.;
    res = v - vm;
    CPPUNIT_ASSERT(res[0][0] == 3. && res[0][1] == 3. && res[0][2] == 3.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorPlus()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    NVector<double, 3> vm(3);
    NVector<double, 3> res(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    vm[0][0] = vm[0][1] =vm[0][2] =1.;
    res = v + vm;
    CPPUNIT_ASSERT(res[0][0] == 5. && res[0][1] == 5. && res[0][2] == 5.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorAsteriskEqual()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    v *= 3.;
    CPPUNIT_ASSERT(v[0][0] == 12. && v[0][1] == 12. && v[0][2] == 12.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorSlashEqual()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    v /= 2.;
    CPPUNIT_ASSERT(v[0][0] == 2. && v[0][1] == 2. && v[0][2] == 2.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorPlusEqual()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    NVector<double, 3> vm(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    vm[0][0] = vm[0][1] =vm[0][2] =1.;
    v += vm;
    CPPUNIT_ASSERT(v[0][0] == 5. && v[0][1] == 5. && v[0][2] == 5.);
}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorMinusEqual()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    NVector<double, 3> vm(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    vm[0][0] = vm[0][1] =vm[0][2] =1.;
    v -= vm;
    CPPUNIT_ASSERT(v[0][0] == 3. && v[0][1] == 3. && v[0][2] == 3.);

}

//-------------------------------------------------------------------------
void NVectorTest::TestAddScaled()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    NVector<double, 3> vm(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    vm[0][0] = vm[0][1] =vm[0][2] =1.;
    v.AddScaled(vm, 3);
    CPPUNIT_ASSERT(v[0][0] == 7. && v[0][1] == 7. && v[0][2] == 7.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestSubtractScaled()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    NVector<double, 3> vm(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    vm[0][0] = vm[0][1] =vm[0][2] =1.;
    v.SubtractScaled(vm, 3);
    CPPUNIT_ASSERT(v[0][0] == 1. && v[0][1] == 1. && v[0][2] == 1.);


}
//-------------------------------------------------------------------------
void NVectorTest::TestAdd()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v1(3);
    NVector<double, 3> v2(3);
    NVector<double, 3> out(3);
    v1[0][0] = v1[0][1] =v1[0][2] =4.;
    v2[0][0] = v2[0][1] =v2[0][2] =1.;
    out.Add(v1,2,v2,3,out);
    CPPUNIT_ASSERT(out[0][0] == 11. && out[0][1] == 11. && out[0][2] == 11.);

    out.Add(v1,2,v2,out);
    CPPUNIT_ASSERT(out[0][0] == 9. && out[0][1] == 9. && out[0][2] == 9.);
}
//-------------------------------------------------------------------------
void NVectorTest::TestOperatorUnaryMinus()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v(3);
    v[0][0] = v[0][1] =v[0][2] =4.;
    v = (-v);
    CPPUNIT_ASSERT(v[0][0] == -4. && v[0][1] == -4. && v[0][2] == -4.);

    NVector<double, 3> v2(v);
    v = (-v2);
    CPPUNIT_ASSERT(v[0][0] == 4. && v[0][1] == 4. && v[0][2] == 4.);

}

//-------------------------------------------------------------------------
void NVectorTest::TestOperatorEqual()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v1(3);
    NVector<double, 3> v2(3);
    v1[0][0] = v1[0][1] =v1[0][2] =4.;
    v2[0][0] = v2[0][1] =v2[0][2] =1.;
    v1 = v2;
    CPPUNIT_ASSERT(v1[0][0] == 1. && v1[0][1] == 1. && v1[0][2] == 1.);


}
//-------------------------------------------------------------------------
void NVectorTest::TestDot()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v1(3);
    NVector<double, 3> v2(3);
    v1[0][0] = v1[0][1] =v1[0][2] =4.;
    v2[0][0] = v2[0][1] =v2[0][2] =1.;
    double dot = v1.Dot(v2);
    CPPUNIT_ASSERT(dot == 12.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestLength()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v1(3);
    v1[0][0] = 0; v1[0][1] = 0; v1[0][2] =4.;
    CPPUNIT_ASSERT(v1.Length() == 4.);
}
//-------------------------------------------------------------------------
void NVectorTest::TestNorm()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v1(3);
    v1[0][0] = 0; v1[0][1] = 0; v1[0][2] =4.;
    double res = v1.Norm(2);
    CPPUNIT_ASSERT(res == 4.);

}
//-------------------------------------------------------------------------
void NVectorTest::TestNormalize()
//-------------------------------------------------------------------------
{
    NVector<double, 3> v1(3);
    v1[0][0] = 0; v1[0][1] = 0; v1[0][2] =4.;
    double res = v1.Norm(2);
    CPPUNIT_ASSERT(res == 4.);

    v1.Normalize();
    res = v1.Norm(2);
    CPPUNIT_ASSERT(res == 1.);

}