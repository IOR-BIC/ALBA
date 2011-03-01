/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: AllocatorTest.cpp,v $
Language:  C++
Date:      $Date: 2011-03-01 11:07:35 $
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
#include "AllocatorTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"

class P3D{
public:
    double x,y,z;
    double u,v;
};

class P2D{
public:
    int x,y;
};

//-------------------------------------------------------------------------
void AllocatorTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  Allocator<double> *ad1 = new Allocator<double>();
  delete ad1;
  Allocator<int> *ad2 = new Allocator<int>();
  delete ad2;
}
//-------------------------------------------------------------------------
void AllocatorTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  Allocator<double> ad1();
  Allocator<int> ad2();
}

//-------------------------------------------------------------------------
void AllocatorTest::TestSet()
//-------------------------------------------------------------------------
{

    Allocator<P3D> ad1;
    const int bSize = 5;

    AllocatorState asBeforeSet = ad1.getState();
    ad1.Set(bSize);

    AllocatorState asAfterSet = ad1.getState();
    CPPUNIT_ASSERT(asBeforeSet.index != asAfterSet.index);
}

//-------------------------------------------------------------------------
void AllocatorTest::TestReset()
//-------------------------------------------------------------------------
{
    Allocator<P2D> ad1;
    ad1.Set(5);
    P2D *p2d = ad1.NewElements(4);
    p2d[0].x = 5;
    p2d[0].y = 10;

    p2d[1].x = 5;
    p2d[1].y = 10;

    p2d[2].x = 5;
    p2d[2].y = 10;

    p2d[3].x = 5;
    p2d[3].y = 10;

    CPPUNIT_ASSERT(p2d[0].x == 5 && p2d[0].y == 10 &&
                   p2d[1].x == 5 && p2d[1].y == 10 &&
                   p2d[2].x == 5 && p2d[2].y == 10 &&
                   p2d[3].x == 5 && p2d[3].y == 10);

    AllocatorState stateB = ad1.getState();
    CPPUNIT_ASSERT(stateB.remains == 1 && stateB.index == 0);

    ad1.Reset();
    AllocatorState state = ad1.getState();
    CPPUNIT_ASSERT(state.remains == 0 && state.index == 0);
}

//-------------------------------------------------------------------------
void AllocatorTest::TestNewElements()
//-------------------------------------------------------------------------
{
    Allocator<P3D> ad1;
    ad1.Set(10);
    P3D *p3d = NULL;
    p3d= ad1.NewElements(15);
    CPPUNIT_ASSERT(p3d == NULL);
    p3d = ad1.NewElements(5);
    CPPUNIT_ASSERT(p3d != NULL);

    AllocatorState stateB = ad1.getState();
    CPPUNIT_ASSERT(stateB.remains == 5 && stateB.index == 0);

    p3d = ad1.NewElements(3);
    stateB = ad1.getState();
    CPPUNIT_ASSERT(stateB.remains == 2 && stateB.index == 0);

    ad1.Reset();
}

//-------------------------------------------------------------------------
void AllocatorTest::TestRollBack()
//-------------------------------------------------------------------------
{
    Allocator<P3D> ad1;
    ad1.Set(10);
    P3D *p3d = NULL;
    
    p3d = ad1.NewElements(5);
    CPPUNIT_ASSERT(p3d != NULL);

    AllocatorState stateA = ad1.getState();
    CPPUNIT_ASSERT(stateA.remains == 5 && stateA.index == 0);

    p3d = ad1.NewElements(3);
    AllocatorState stateB = ad1.getState();
    CPPUNIT_ASSERT(stateB.remains == 2 && stateB.index == 0);

    p3d = ad1.NewElements(2);
    AllocatorState stateC = ad1.getState();
    CPPUNIT_ASSERT(stateC.remains == 0 && stateC.index == 0);

    ad1.RollBack();
    AllocatorState stateF = ad1.getState();
    CPPUNIT_ASSERT(stateF.remains == 10 && stateF.index == 0);
}