/*=========================================================================

 Program: MAF2
 Module: CubeTest
 Authors: Roberto Mucci
 
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
#include "CubeTest.h"

#include "vtkMAFPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void CubeTest::TestCornerIndex()
//-------------------------------------------------------------------------
{
  int counter = 0;
  for(int i=0;i<2;i++){
    for(int j=0;j<2;j++){
      for(int k=0;k<2;k++){
        int idx=Cube::CornerIndex(i,j,k);
        switch(counter)
        {
        case 0:
          CPPUNIT_ASSERT(i == 0, idx == 0, j == 0, k == 0);
          break;
        case 1:
          CPPUNIT_ASSERT(i == 0, idx == 4, j == 0, k == 1);
          break;
        case 2:
          CPPUNIT_ASSERT(i == 0, idx == 2, j == 1, k == 0);
          break;
        case 3:
          CPPUNIT_ASSERT(i == 0, idx == 6, j == 1, k == 1);
          break;
        case 4:
          CPPUNIT_ASSERT(i == 1, idx == 1, j == 0, k ==0);
          break;
        case 5:
          CPPUNIT_ASSERT(i == 1, idx == 5, j == 0, k == 1);
          break;
        case 6:
          CPPUNIT_ASSERT(i == 1, idx == 3, j == 1, k == 0);
          break;
        case 7:
          CPPUNIT_ASSERT(i == 1, idx == 7, j == 1, k == 1);
          break;
        }
        counter++;
      }
    }
  }
}
  
//-------------------------------------------------------------------------
void CubeTest::FactorCornerIndexTest()
//-------------------------------------------------------------------------
{
  int x,y,z;
  int idx = 5;
  Cube::FactorCornerIndex(idx,x,y,z);
    
  CPPUNIT_ASSERT(x == 1);
  CPPUNIT_ASSERT(y == 0);
  CPPUNIT_ASSERT(z == 1);
}
//-------------------------------------------------------------------------
void CubeTest::EdgeIndexTest()
//-------------------------------------------------------------------------
{
  int dir = 1;
  int output = 0;
  output = Cube::EdgeIndex(dir,0,0);
  CPPUNIT_ASSERT(output == 4);
}
//-------------------------------------------------------------------------
void CubeTest::FactorEdgeIndexTest()
//-------------------------------------------------------------------------
{
  int orientation,a,b;
  int idx = 2;
  Cube::FactorEdgeIndex(idx,orientation,a,b);
  CPPUNIT_ASSERT(orientation == 0);
  CPPUNIT_ASSERT(a == 0);
  CPPUNIT_ASSERT(b == 1);
}
//-------------------------------------------------------------------------
void CubeTest::FaceIndexTest()
//-------------------------------------------------------------------------
{
  int face = 0;
  int dir = 1;
  face = Cube::FaceIndex(dir,0);
  CPPUNIT_ASSERT(face == 2);
}
//-------------------------------------------------------------------------
void CubeTest::FactorFaceIndexTest()
//-------------------------------------------------------------------------
{
  int faceIndex = 2;
  int dir = 0;
  int offset = 0;
  Cube::FactorFaceIndex(faceIndex,dir,offset);
  CPPUNIT_ASSERT(dir == 1);
  CPPUNIT_ASSERT(offset == 0);

}
//-------------------------------------------------------------------------
void CubeTest::FaceAdjacentToEdgesTest()
//-------------------------------------------------------------------------
{
  int indexA = 1;
  int indexB = 5;
  int index = 0;
  index =Cube::FaceAdjacentToEdges(indexA,indexB);
  CPPUNIT_ASSERT(index == 4);

}
//-------------------------------------------------------------------------
void CubeTest::EdgeCornersTest()
//-------------------------------------------------------------------------
{
  int idx = 3;
  int c1, c2;
  Cube::EdgeCorners(idx,c1,c2);
  CPPUNIT_ASSERT(c1 == 6);
  CPPUNIT_ASSERT(c2 == 7);

}
//-------------------------------------------------------------------------
void CubeTest::FaceCornersTest()
//-------------------------------------------------------------------------
{
  int faceIndex = 2;
  int corners[Cube::CORNERS/2];
  Cube::FaceCorners(faceIndex,corners[0],corners[1],corners[2],corners[3]);
  CPPUNIT_ASSERT(corners[0] == 0);
  CPPUNIT_ASSERT(corners[1] == 1);
  CPPUNIT_ASSERT(corners[2] == 4);
  CPPUNIT_ASSERT(corners[3] == 5);

}
//-------------------------------------------------------------------------
void CubeTest::FaceReflectEdgeIndexTest()
//-------------------------------------------------------------------------
{
  int idx = 2;
  int face = 3;
  int outputIndex;
  outputIndex = Cube::FaceReflectEdgeIndex(idx,face);
  CPPUNIT_ASSERT(outputIndex == 3);


}
//-------------------------------------------------------------------------
void CubeTest::FaceReflectCornerIndexTest()
//-------------------------------------------------------------------------
{
  int idx = 3;
  int face = 4;
  int faceOutput = Cube::FaceReflectCornerIndex(idx,face);
  CPPUNIT_ASSERT(faceOutput == 7);

}

