/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkWeightedLandmarkTransformTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-03 16:33:05 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "vtkWeightedLandmarkTransform.h"
#include "vtkWeightedLandmarkTransformTest.h"
#include "vtkPoints.h"
#include "vtkMatrix4x4.h"

//------------------------------------------------------------
void vtkWeightedLandmarkTransformTest::setUp()
//------------------------------------------------------------
{
}

//------------------------------------------------------------
void vtkWeightedLandmarkTransformTest::tearDown()
//------------------------------------------------------------
{
}


//------------------------------------------------------------
void vtkWeightedLandmarkTransformTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkWeightedLandmarkTransform *wlt = vtkWeightedLandmarkTransform::New();
  CPPUNIT_ASSERT(wlt->IsA("vtkWeightedLandmarkTransform"));
  vtkDEL(wlt);
}

//------------------------------------------------------------
void vtkWeightedLandmarkTransformTest::TestUpdate()
//------------------------------------------------------------
{
  vtkPoints *pts;
  vtkNEW(pts);

  pts->SetNumberOfPoints(3);
  pts->SetPoint(0,0,0,0);
  pts->SetPoint(1,1,1,1);
  pts->SetPoint(2,2,2,2);

  vtkWeightedLandmarkTransform *wlt = vtkWeightedLandmarkTransform::New();
  wlt->SetSourceLandmarks(pts);
  wlt->SetTargetLandmarks(pts);
  double weights[3];
  weights[0] = 1;
  weights[1] = 2;
  weights[2] = 3;
  wlt->SetWeights(weights,3);
  wlt->Update();

  vtkMatrix4x4 *mat = wlt->GetMatrix();
  vtkMatrix4x4 *ident;
  vtkNEW(ident);
  ident->Identity();

  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      CPPUNIT_ASSERT(ident->GetElement(i,j) == mat->GetElement(i,j));
    }
  }
  vtkDEL(ident);
  vtkDEL(pts);
  vtkDEL(wlt); 
}

