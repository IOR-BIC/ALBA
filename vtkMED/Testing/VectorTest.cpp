/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: VectorTest.cpp,v $
Language:  C++
Date:      $Date: 2011-01-10 12:06:58 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
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
#include "VectorTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"
#include "vtkMEDPoissonSurfaceReconstruction.cxx"

//-------------------------------------------------------------------------
void VectorTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  Vector<double> *v = new Vector<double>(2);
  delete v;
}
//-------------------------------------------------------------------------
void VectorTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  Vector<double> v;
  v.Resize(5);
  Vector<double> v2 = Vector<double>(v);
  CPPUNIT_ASSERT(v.Dimensions() == v2.Dimensions());
  
  Vector<double> v3(5);
  CPPUNIT_ASSERT(v.Dimensions() == v3.Dimensions());

  v3.Resize(6);
  CPPUNIT_ASSERT(v.Dimensions() != v3.Dimensions());
  
  //this code crash in the constructor
  /*double *arr = new double[5];
  arr[0] = arr[1] = arr[2] = arr[3] = arr[4] = 5.5;
  Vector<double> *v4 = new Vector<double>(5, arr);
  delete v4;*/
}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorRoundBrackets()
//-------------------------------------------------------------------------
{
    Vector<double> v(5);
    v[0] = 4.;
    CPPUNIT_ASSERT(v(0) == 4.);

    const Vector<double> v2(v);
    CPPUNIT_ASSERT(v2(0) == 4.);
}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorSquareBrackets()
//-------------------------------------------------------------------------
{
    Vector<double> v(5);
    v[0] = 4.;
    CPPUNIT_ASSERT(v[0] == 4.);

    const Vector<double> v2(v);
    CPPUNIT_ASSERT(v2[0] == 4.);
}
//-------------------------------------------------------------------------
void VectorTest::TestSetZero()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    v[0] = v[1] =v[2] =4.;
    v.SetZero();
    CPPUNIT_ASSERT(v[0] == 0. && v[1] == 0. && v[2] == 0.);
}
//-------------------------------------------------------------------------
void VectorTest::TestDimensions()
//-------------------------------------------------------------------------
{
    Vector<double> v;
    CPPUNIT_ASSERT(v.Dimensions() == 0);

    v.Resize(3);
    CPPUNIT_ASSERT(v.Dimensions() == 3);
}
//-------------------------------------------------------------------------
void VectorTest::TestResize()
//-------------------------------------------------------------------------
{
    TestDimensions(); // the test can be considered the same
}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorAsterisk()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    Vector<double> res(3);
    v[0] = v[1] =v[2] =4.;
    res = v * 3.;
    CPPUNIT_ASSERT(res[0] == 12. && res[1] == 12. && res[2] == 12.);

}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorSlash()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    Vector<double> res(3);
    v[0] = v[1] =v[2] =4.;
    res = v / 2.;
    CPPUNIT_ASSERT(res[0] == 2. && res[1] == 2. && res[2] == 2.);

}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorMinus()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    Vector<double> vm(3);
    Vector<double> res(3);
    v[0] = v[1] =v[2] =4.;
    vm[0] = vm[1] =vm[2] =1.;
    res = v - vm;
    CPPUNIT_ASSERT(res[0] == 3. && res[1] == 3. && res[2] == 3.);

}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorPlus()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    Vector<double> vm(3);
    Vector<double> res(3);
    v[0] = v[1] =v[2] =4.;
    vm[0] = vm[1] =vm[2] =1.;
    res = v + vm;
    CPPUNIT_ASSERT(res[0] == 5. && res[1] == 5. && res[2] == 5.);

}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorAsteriskEqual()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    v[0] = v[1] =v[2] =4.;
    v *= 3.;
    CPPUNIT_ASSERT(v[0] == 12. && v[1] == 12. && v[2] == 12.);

}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorSlashEqual()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    v[0] = v[1] =v[2] =4.;
    v /= 2.;
    CPPUNIT_ASSERT(v[0] == 2. && v[1] == 2. && v[2] == 2.);

}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorPlusEqual()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    Vector<double> vm(3);
    v[0] = v[1] =v[2] =4.;
    vm[0] = vm[1] =vm[2] =1.;
    v += vm;
    CPPUNIT_ASSERT(v[0] == 5. && v[1] == 5. && v[2] == 5.);
}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorMinusEqual()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    Vector<double> vm(3);
    v[0] = v[1] =v[2] =4.;
    vm[0] = vm[1] =vm[2] =1.;
    v -= vm;
    CPPUNIT_ASSERT(v[0] == 3. && v[1] == 3. && v[2] == 3.);

}

//-------------------------------------------------------------------------
void VectorTest::TestAddScaled()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    Vector<double> vm(3);
    v[0] = v[1] =v[2] =4.;
    vm[0] = vm[1] =vm[2] =1.;
    v.AddScaled(vm, 3);
    CPPUNIT_ASSERT(v[0] == 7. && v[1] == 7. && v[2] == 7.);

}
//-------------------------------------------------------------------------
void VectorTest::TestSubtractScaled()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    Vector<double> vm(3);
    v[0] = v[1] =v[2] =4.;
    vm[0] = vm[1] =vm[2] =1.;
    v.SubtractScaled(vm, 3);
    CPPUNIT_ASSERT(v[0] == 1. && v[1] == 1. && v[2] == 1.);


}
//-------------------------------------------------------------------------
void VectorTest::TestAdd()
//-------------------------------------------------------------------------
{
    Vector<double> v1(3);
    Vector<double> v2(3);
    Vector<double> out(3);
    v1[0] = v1[1] =v1[2] =4.;
    v2[0] = v2[1] =v2[2] =1.;
    out.Add(v1,2,v2,3,out);
    CPPUNIT_ASSERT(out[0] == 11. && out[1] == 11. && out[2] == 11.);

    out.Add(v1,2,v2,out);
    CPPUNIT_ASSERT(out[0] == 9. && out[1] == 9. && out[2] == 9.);
}
//-------------------------------------------------------------------------
void VectorTest::TestOperatorUnaryMinus()
//-------------------------------------------------------------------------
{
    Vector<double> v(3);
    v[0] = v[1] =v[2] =4.;
    v = (-v);
    CPPUNIT_ASSERT(v[0] == -4. && v[1] == -4. && v[2] == -4.);

    Vector<double> v2(v);
    v = (-v2);
    CPPUNIT_ASSERT(v[0] == 4. && v[1] == 4. && v[2] == 4.);

}

//-------------------------------------------------------------------------
void VectorTest::TestOperatorEqual()
//-------------------------------------------------------------------------
{
    Vector<double> v1(3);
    Vector<double> v2(3);
    v1[0] = v1[1] =v1[2] =4.;
    v2[0] = v2[1] =v2[2] =1.;
    v1 = v2;
    CPPUNIT_ASSERT(v1[0] == 1. && v1[1] == 1. && v1[2] == 1.);


}
//-------------------------------------------------------------------------
void VectorTest::TestDot()
//-------------------------------------------------------------------------
{
    Vector<double> v1(3);
    Vector<double> v2(3);
    v1[0] = v1[1] =v1[2] =4.;
    v2[0] = v2[1] =v2[2] =1.;
    double dot = v1.Dot(v2);
    CPPUNIT_ASSERT(dot == 12.);

}
//-------------------------------------------------------------------------
void VectorTest::TestLength()
//-------------------------------------------------------------------------
{
    Vector<double> v1(3);
    v1[0] = 0; v1[1] = 0; v1[2] =4.;
    CPPUNIT_ASSERT(v1.Length() == 4.);
}
//-------------------------------------------------------------------------
void VectorTest::TestNorm()
//-------------------------------------------------------------------------
{
    Vector<double> v1(3);
    v1[0] = 0; v1[1] = 0; v1[2] =4.;
    double res = v1.Norm(2);
    CPPUNIT_ASSERT(res == 4.);

}
//-------------------------------------------------------------------------
void VectorTest::TestNormalize()
//-------------------------------------------------------------------------
{
    Vector<double> v1(3);
    v1[0] = 0; v1[1] = 0; v1[2] =4.;
    double res = v1.Norm(2);
    CPPUNIT_ASSERT(res == 4.);

    v1.Normalize();
    res = v1.Norm(2);
    CPPUNIT_ASSERT(res == 1.);

}