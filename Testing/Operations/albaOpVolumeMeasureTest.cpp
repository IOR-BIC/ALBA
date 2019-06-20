/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeMeasureTest
 Authors: Daniele Giunchi
 
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

#include "albaOpVolumeMeasureTest.h"
#include "albaOpVolumeMeasure.h"

#include "albaString.h"
#include "albaVMESurface.h"
#include "vtkCubeSource.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpVolumeMeasureTest::Test() 
//-----------------------------------------------------------
{
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->SetXLength(10.0);
  cube->SetYLength(20.0);
  cube->SetZLength(30.0);
  cube->Update();
  	
  albaSmartPointer<albaVMESurface> vmeSurface;
  //albaNEW(vmeSurface);
  vmeSurface->SetData(cube->GetOutput(), 0);
  vmeSurface->Update();
  	
	albaOpVolumeMeasure *volumeMeasureOp=new albaOpVolumeMeasure("Volume Measure");
	volumeMeasureOp->TestModeOn();
  albaString result;
  volumeMeasureOp->VolumeCompute(vmeSurface);
  result = volumeMeasureOp->GetVolume();
  volumeMeasureOp->OpStop(ALBA_OK);
	
  //printf("%s", result);
  CPPUNIT_ASSERT(result == albaString("6000"));

  
  vtkDEL(cube);
  albaDEL(volumeMeasureOp);
  
}
