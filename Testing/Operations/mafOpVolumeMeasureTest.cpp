/*=========================================================================

 Program: MAF2Medical
 Module: mafOpSmoothSurfaceCells
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

#include "mafOpVolumeMeasureTest.h"
#include "mafOpVolumeMeasure.h"

#include "mafString.h"
#include "mafVMESurface.h"
#include "vtkCubeSource.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpSmoothSurfaceCells::Test() 
//-----------------------------------------------------------
{
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->SetXLength(10.0);
  cube->SetYLength(20.0);
  cube->SetZLength(30.0);
  cube->Update();
  	
  mafSmartPointer<mafVMESurface> vmeSurface;
  //mafNEW(vmeSurface);
  vmeSurface->SetData(cube->GetOutput(), 0);
  vmeSurface->Update();
  	
	mafOpVolumeMeasure *volumeMeasureOp=new mafOpVolumeMeasure("Volume Measure");
	volumeMeasureOp->TestModeOn();
  mafString result;
  volumeMeasureOp->VolumeCompute(vmeSurface);
  result = volumeMeasureOp->GetVolume();
  volumeMeasureOp->OpStop(MAF_OK);
	
  //printf("%s", result);
  CPPUNIT_ASSERT(result == mafString("6000"));

  
  vtkDEL(cube);
  mafDEL(volumeMeasureOp);
  
}
