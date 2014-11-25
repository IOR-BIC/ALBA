/*=========================================================================

 Program: MAF2Medical
 Module: BinaryNodeTest
 Authors: Daniele Giunchi
 
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
#include "BinaryNodeTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void BinaryNodeTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  BinaryNode<Real> *binaryNode = new BinaryNode<Real>();

  delete binaryNode;
}
//-------------------------------------------------------------------------
void BinaryNodeTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  BinaryNode<Real> binaryNode;
}

//-------------------------------------------------------------------------
void BinaryNodeTest:: TestCenterCount()
//-------------------------------------------------------------------------
{
    int centerCount = BinaryNode<Real>::CenterCount(3);
    CPPUNIT_ASSERT(centerCount == 8);
}
//-------------------------------------------------------------------------
void BinaryNodeTest:: TestCumulativeCenterCount()
//-------------------------------------------------------------------------
{
    int centerCount = BinaryNode<Real>::CumulativeCenterCount(5);
    CPPUNIT_ASSERT(centerCount == 63);
}
//-------------------------------------------------------------------------
void BinaryNodeTest:: TestIndex()
//-------------------------------------------------------------------------
{
    int index = BinaryNode<Real>::Index(2, 1);
    CPPUNIT_ASSERT(index == 4);
}
//-------------------------------------------------------------------------
void BinaryNodeTest:: TestCornerIndex()
//-------------------------------------------------------------------------
{
    int cornerIndex = BinaryNode<Real>::CornerIndex(5,2,1,1);
    CPPUNIT_ASSERT(cornerIndex == 16);
}
//-------------------------------------------------------------------------
void BinaryNodeTest:: TestCornerIndexPosition()
//-------------------------------------------------------------------------
{
    Real cip = BinaryNode<Real>::CornerIndexPosition(0, 2);
    CPPUNIT_ASSERT(cip == 0);
}
//-------------------------------------------------------------------------
void BinaryNodeTest:: TestWidth()
//-------------------------------------------------------------------------
{
    Real width = BinaryNode<Real>::Width(3);
    CPPUNIT_ASSERT(width == 0.125);
}
//-------------------------------------------------------------------------
void BinaryNodeTest:: TestCenterAndWidth()
//-------------------------------------------------------------------------
{
    Real center, width;
    BinaryNode<Real>::CenterAndWidth(2, 1, center, width);
    
    CPPUNIT_ASSERT(center == 0.375 && width == 0.25);
    
    BinaryNode<Real>::CenterAndWidth(2, center, width);
    
    CPPUNIT_ASSERT(center == 0.75 && width == 0.5);
}
//-------------------------------------------------------------------------
void BinaryNodeTest:: TestDepthAndOffset()
//-------------------------------------------------------------------------
{
    int depth, offset;
    BinaryNode<Real>::DepthAndOffset(3, depth, offset);
    
    CPPUNIT_ASSERT(depth == 2 && offset == 0);
}
