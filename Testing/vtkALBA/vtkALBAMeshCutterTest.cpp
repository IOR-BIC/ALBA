/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMeshCutterTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
#include <cppunit/config/SourcePrefix.h>
#include "vtkALBAMeshCutterTest.h"
#include "vtkALBAMeshCutter.h"



//------------------------------------------------------------------------------ 
void vtkALBAMeshCutterTest::TestFixture()
{
	vtkALBAMeshCutter *meshCutter = vtkALBAMeshCutter::New();

	vtkDEL(meshCutter);
}

