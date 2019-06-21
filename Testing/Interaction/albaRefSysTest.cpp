/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRefSysTest
 Authors: Stefano Perticoni
 
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

#include "albaRefSysTest.h"
#include <cppunit/config/SourcePrefix.h>
#include "albaRefSys.h"
#include "albaVMESurface.h"
#include "albaTransform.h"
#include "vtkRenderer.h"

void albaRefSysTest::TestFixture()
{

}

void albaRefSysTest::TestConstructorDestructor()
{
  albaRefSys refSys;
}

void albaRefSysTest::TestCopyConstructor()
{
  albaRefSys refSysSource;
  refSysSource.SetTypeToView();
  albaRefSys refSysTarget;
  refSysTarget = refSysSource;
  int targetType = refSysTarget.GetType();
  CPPUNIT_ASSERT(targetType == albaRefSys::VIEW);
}

void albaRefSysTest::TestSetTypeToCustom()
{
  albaRefSys refSys;
  albaMatrix *matrix = albaMatrix::New();
  refSys.SetTypeToCustom(matrix);
  CPPUNIT_ASSERT(refSys.GetType() == albaRefSys::CUSTOM);
  CPPUNIT_ASSERT(refSys.GetMatrix() == matrix);
}

void albaRefSysTest::TestSetTypeToLocal()
{
  albaRefSys refSys;
  refSys.SetTypeToLocal();
  CPPUNIT_ASSERT(refSys.GetType() == albaRefSys::LOCAL);
}

void albaRefSysTest::TestSetTypeToView()
{
  albaRefSys refSys;
  refSys.SetTypeToView();
  CPPUNIT_ASSERT(refSys.GetType() == albaRefSys::VIEW);
}

void albaRefSysTest::TestSetTypeToParent()
{
  albaRefSys refSys;
  albaVMESurface *vme = albaVMESurface::New();
  refSys.SetVME(vme);
  refSys.SetTypeToParent(vme);
  CPPUNIT_ASSERT(refSys.GetType() == albaRefSys::PARENT);
}

void albaRefSysTest::TestSetTypeToGlobal()
{
  albaRefSys refSys;
  refSys.SetTypeToGlobal();
  CPPUNIT_ASSERT(refSys.GetType() == albaRefSys::GLOBAL);
}

void albaRefSysTest::TestSetGetType()
{
  albaRefSys refSys;
  int type = albaRefSys::LOCAL;
  refSys.SetType(type);
  int retType = refSys.GetType();
  CPPUNIT_ASSERT(retType == type);
}

void albaRefSysTest::TestSetGetTransform()
{
  albaRefSys refSys;
  albaTransform *transform = albaTransform::New();
  refSys.SetTransform(transform);  
}

void albaRefSysTest::TestSetGetMatrix()
{
  albaRefSys refSys;
  albaMatrix *matrix = albaMatrix::New();
  refSys.SetMatrix(matrix);
  refSys.GetMatrix();
}

void albaRefSysTest::TestSetGetRenderer()
{
  albaRefSys refSys;
  vtkRenderer *renderer = vtkRenderer::New();
  refSys.SetRenderer(renderer);
  CPPUNIT_ASSERT(renderer == refSys.GetRenderer());
  vtkDEL(renderer);
}

void albaRefSysTest::TestSetGetVME()
{
  albaRefSys refSys;
  albaVMESurface *vme = albaVMESurface::New();
  refSys.SetVME(vme);
  CPPUNIT_ASSERT(vme == refSys.GetVME());
}

void albaRefSysTest::TestDeepCopy()
{
  albaRefSys refSysSource;
  albaRefSys refSysTarget;
  refSysTarget.DeepCopy(&refSysSource);
}

void albaRefSysTest::TestReset()
{
  albaRefSys refSys;
  refSys.SetTypeToView();
  refSys.Reset();
  CPPUNIT_ASSERT(refSys.GetType() == albaRefSys::GLOBAL);
}

