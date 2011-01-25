/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: TreeNodeDataTest.cpp,v $
Language:  C++
Date:      $Date: 2011-01-25 14:04:51 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
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
#include "mafString.h"
#include "vtkMEDPoissonSurfaceReconstruction.h"
#include "TreeNodeDataTest.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"

//-------------------------------------------------------------------------
void TreeNodeDataTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{

  TreeNodeData *data;
  data = new TreeNodeData();

  cppDEL(data);
}

//-------------------------------------------------------------------------
void TreeNodeDataTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{

  TreeNodeData data;

}