/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: SortedTreeNodesTest
 Authors: Eleonora Mambrini
 
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
#include "albaString.h"
#include "vtkALBAPoissonSurfaceReconstruction.h"
#include "SortedTreeNodesTest.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"

//-------------------------------------------------------------------------
void SortedTreeNodesTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{

  SortedTreeNodes *data;
  data = new SortedTreeNodes();

  cppDEL(data);
}

//-------------------------------------------------------------------------
void SortedTreeNodesTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{

  SortedTreeNodes data;

}

//-------------------------------------------------------------------------
void SortedTreeNodesTest::TestSet()
//-------------------------------------------------------------------------
{

  SortedTreeNodes *treeNodes = new SortedTreeNodes();
  
  TreeNodeData data;
  data.centerWeightContribution = 1.0;
  data.mcIndex = 1;
  data.nodeIndex = 0;
  data.value = 5.0;

  TreeOctNode root; 
  root.nodeData = data;

  const int setIndex = 1;

  treeNodes->Set(root, setIndex);

  CPPUNIT_ASSERT(treeNodes->maxDepth == 1);
  CPPUNIT_ASSERT(*(treeNodes->nodeCount) == 0);

  TreeNodeData resNodeData = (*(treeNodes->treeNodes))->nodeData;

  CPPUNIT_ASSERT(resNodeData.centerWeightContribution == data.centerWeightContribution);
  CPPUNIT_ASSERT(resNodeData.mcIndex == data.mcIndex);
  CPPUNIT_ASSERT(resNodeData.nodeIndex == data.nodeIndex);
  CPPUNIT_ASSERT(resNodeData.value == data.value);

  cppDEL(treeNodes);


}