/*=========================================================================

 Program: MAF2
 Module: mafRefSysTest
 Authors: Stefano Perticoni
 
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

#include "mafRefSysTest.h"
#include <cppunit/config/SourcePrefix.h>
#include "mafRefSys.h"
#include "mafVMESurface.h"
#include "mafTransform.h"
#include "vtkRenderer.h"

void mafRefSysTest::TestFixture()
{

}

void mafRefSysTest::TestConstructorDestructor()
{
  mafRefSys refSys;
}

void mafRefSysTest::TestCopyConstructor()
{
  mafRefSys refSysSource;
  refSysSource.SetTypeToView();
  mafRefSys refSysTarget;
  refSysTarget = refSysSource;
  int targetType = refSysTarget.GetType();
  CPPUNIT_ASSERT(targetType == mafRefSys::VIEW);
}

void mafRefSysTest::TestSetTypeToCustom()
{
  mafRefSys refSys;
  mafMatrix *matrix = mafMatrix::New();
  refSys.SetTypeToCustom(matrix);
  CPPUNIT_ASSERT(refSys.GetType() == mafRefSys::CUSTOM);
  CPPUNIT_ASSERT(refSys.GetMatrix() == matrix);
}

void mafRefSysTest::TestSetTypeToLocal()
{
  mafRefSys refSys;
  refSys.SetTypeToLocal();
  CPPUNIT_ASSERT(refSys.GetType() == mafRefSys::LOCAL);
}

void mafRefSysTest::TestSetTypeToView()
{
  mafRefSys refSys;
  refSys.SetTypeToView();
  CPPUNIT_ASSERT(refSys.GetType() == mafRefSys::VIEW);
}

void mafRefSysTest::TestSetTypeToParent()
{
  mafRefSys refSys;
  mafVMESurface *vme = mafVMESurface::New();
  refSys.SetVME(vme);
  refSys.SetTypeToParent(vme);
  CPPUNIT_ASSERT(refSys.GetType() == mafRefSys::PARENT);
}

void mafRefSysTest::TestSetTypeToGlobal()
{
  mafRefSys refSys;
  refSys.SetTypeToGlobal();
  CPPUNIT_ASSERT(refSys.GetType() == mafRefSys::GLOBAL);
}

void mafRefSysTest::TestSetGetType()
{
  mafRefSys refSys;
  int type = mafRefSys::LOCAL;
  refSys.SetType(type);
  int retType = refSys.GetType();
  CPPUNIT_ASSERT(retType == type);
}

void mafRefSysTest::TestSetGetTransform()
{
  mafRefSys refSys;
  mafTransform *transform = mafTransform::New();
  refSys.SetTransform(transform);  
}

void mafRefSysTest::TestSetGetMatrix()
{
  mafRefSys refSys;
  mafMatrix *matrix = mafMatrix::New();
  refSys.SetMatrix(matrix);
  refSys.GetMatrix();
}

void mafRefSysTest::TestSetGetRenderer()
{
  mafRefSys refSys;
  vtkRenderer *renderer = vtkRenderer::New();
  refSys.SetRenderer(renderer);
  CPPUNIT_ASSERT(renderer == refSys.GetRenderer());
  vtkDEL(renderer);
}

void mafRefSysTest::TestSetGetVME()
{
  mafRefSys refSys;
  mafVMESurface *vme = mafVMESurface::New();
  refSys.SetVME(vme);
  CPPUNIT_ASSERT(vme == refSys.GetVME());
}

void mafRefSysTest::TestDeepCopy()
{
  mafRefSys refSysSource;
  mafRefSys refSysTarget;
  refSysTarget.DeepCopy(&refSysSource);
}

void mafRefSysTest::TestReset()
{
  mafRefSys refSys;
  refSys.SetTypeToView();
  refSys.Reset();
  CPPUNIT_ASSERT(refSys.GetType() == mafRefSys::GLOBAL);
}

