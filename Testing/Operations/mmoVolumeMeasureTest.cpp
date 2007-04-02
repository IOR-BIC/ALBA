/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoVolumeMeasureTest.cpp,v $
Language:  C++
Date:      $Date: 2007-04-02 13:15:33 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoVolumeMeasureTest.h"
#include "mmoVolumeMeasure.h"

#include "mafString.h"
#include "mafVMESurface.h"
#include "vtkCubeSource.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mmoVolumeMeasureTest::Test() 
//-----------------------------------------------------------
{
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->SetXLength(10.0);
  cube->SetYLength(20.0);
  cube->SetZLength(30.0);
  cube->Update();
  	
  mafVMESurface *vmeSurface;
  mafNEW(vmeSurface);
  vmeSurface->SetData(cube->GetOutput(), 0);
  vmeSurface->Update();
  	
	mmoVolumeMeasure *volumeMeasureOp=new mmoVolumeMeasure("Volume Measure");
	volumeMeasureOp->TestModeOn();
  mafString result;
 volumeMeasureOp->VolumeCompute(vmeSurface);
 result = volumeMeasureOp->GetVolume();
	
  //printf("%s", result);
  CPPUNIT_ASSERT(result == mafString("6000"));

  
  mafDEL(vmeSurface);
  vtkDEL(cube);
  
}
