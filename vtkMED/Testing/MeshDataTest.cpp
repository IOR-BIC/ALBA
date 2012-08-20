/*=========================================================================

 Program: MAF2Medical
 Module: MeshDataTest
 Authors: Roberto Mucci
 
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
#include "MeshDataTest.h"

#include "vtkMEDExtrudeToCircle.h"



//-------------------------------------------------------------------------
void MeshDataTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMEDExtrudeToCircle::MeshData *mesh = new vtkMEDExtrudeToCircle::MeshData();
}

//-------------------------------------------------------------------------
void MeshDataTest::TestAllocateVertices()
//-------------------------------------------------------------------------
{
  vtkMEDExtrudeToCircle::MeshData *mesh = new vtkMEDExtrudeToCircle::MeshData();
  mesh->AllocateRings(1);
  mesh->Ring->AllocateVertices(5);
  delete mesh;
}
