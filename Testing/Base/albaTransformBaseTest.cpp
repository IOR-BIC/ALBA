/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformBaseTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaTransformBaseTest.h"

#include "albaTransformBase.h"
#include "albaSmartPointer.h"

#include "vtkALBASmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkLinearTransform.h"
#include "vtkTransform.h"


#include <iostream>

//support object
class albaTransformBaseDerivedClass : public albaTransformBase
{
	albaTypeMacro(albaTransformBaseDerivedClass,albaTransformBase);
	albaTransformBaseDerivedClass(){;};
	
	virtual void SetMatrix(const albaMatrix &input) {*m_Matrix=input;SetTimeStamp(input.GetTimeStamp());Modified();}

	~albaTransformBaseDerivedClass(){;};
  protected:
		virtual void InternalUpdate(){;};
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaTransformBaseDerivedClass)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaTransformBaseTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaTransformBaseTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaTransformBaseDerivedClass t;
}
//----------------------------------------------------------------------------
void albaTransformBaseTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaTransformBaseDerivedClass *t = new albaTransformBaseDerivedClass();
  cppDEL(t);
}
//----------------------------------------------------------------------------
void albaTransformBaseTest::TestCopyConstructor()
//----------------------------------------------------------------------------
{
  albaTransformBaseDerivedClass t =  albaTransformBaseDerivedClass();
	t.SetTimeStamp(100);

	albaMatrix mat;
	mat.Identity();
	mat.SetElement(2,2,10.0);
	t.SetMatrix(mat);

  albaTransformBaseDerivedClass *t1 = new albaTransformBaseDerivedClass(t);

  CPPUNIT_ASSERT(t1 != NULL);
	CPPUNIT_ASSERT(albaEquals(t1->GetTimeStamp(), t.GetTimeStamp()));
	CPPUNIT_ASSERT(albaEquals(t1->GetMatrix().GetElement(2,2), t.GetMatrix().GetElement(2,2)));
	cppDEL(t1); //there was a delete problem , correct the copy constructor
}
//----------------------------------------------------------------------------
void albaTransformBaseTest::TestGetMatrix()
//----------------------------------------------------------------------------
{
	albaTransformBaseDerivedClass t =  albaTransformBaseDerivedClass();
	albaMatrix mat;
	mat.Identity();
	mat.SetElement(2,2,10.0);
	t.SetMatrix(mat);

	//GetMatrix
	CPPUNIT_ASSERT(t.GetMatrix().GetElement(0,1) == 0 && 
		             t.GetMatrix().GetElement(1,1) == 1 && 
								 t.GetMatrix().GetElement(2,2) == 10.0);

	//GetMatrixPointer
	CPPUNIT_ASSERT(t.GetMatrixPointer()->GetElement(0,1) == 0 && 
		             t.GetMatrixPointer()->GetElement(1,1) == 1 && 
		             t.GetMatrixPointer()->GetElement(2,2) == 10.0);
}
//----------------------------------------------------------------------------
void albaTransformBaseTest::TestTimeStamp()
//----------------------------------------------------------------------------
{
	albaTransformBaseDerivedClass t =  albaTransformBaseDerivedClass();
	t.SetTimeStamp(50);
	CPPUNIT_ASSERT(t.GetTimeStamp() == 50);
}
//----------------------------------------------------------------------------
void albaTransformBaseTest::TestModifiedTime()
//----------------------------------------------------------------------------
{
  albaTransformBaseDerivedClass t =  albaTransformBaseDerivedClass();
	unsigned long modifiedTime1, modifiedTime2;
	modifiedTime1 = t.GetMTime();
	
  t.Update();
  //must not change
	modifiedTime2 = t.GetMTime();
	CPPUNIT_ASSERT(modifiedTime1 == modifiedTime2);

	t.Modified();
	//must change
  modifiedTime2 = t.GetMTime();
	CPPUNIT_ASSERT(modifiedTime1 < modifiedTime2);
}
//----------------------------------------------------------------------------
void albaTransformBaseTest::TestUpdateTime()
//----------------------------------------------------------------------------
{
	albaTransformBaseDerivedClass t =  albaTransformBaseDerivedClass();
	unsigned long updateTime1, updateTime2;
	updateTime1 = t.GetUpdateTime();

	t.Modified();
  //must not change
	updateTime2 = t.GetUpdateTime();
	CPPUNIT_ASSERT(updateTime1 == updateTime2);

	t.Update();
	//must change
	updateTime2 = t.GetUpdateTime();
	CPPUNIT_ASSERT(updateTime1 < updateTime2);
}

//----------------------------------------------------------------------------
void albaTransformBaseTest::TestGetVTKTransform()
//----------------------------------------------------------------------------
{
	albaTransformBaseDerivedClass t =  albaTransformBaseDerivedClass();
	CPPUNIT_ASSERT(t.GetVTKTransform() != NULL);

	//casting operator
	vtkLinearTransform *linearTransform;
	CPPUNIT_ASSERT(vtkLinearTransform::SafeDownCast(t.GetVTKTransform()));
  CPPUNIT_ASSERT(linearTransform = ((vtkLinearTransform *)(t.GetVTKTransform())));
}
//----------------------------------------------------------------------------
void albaTransformBaseTest::TestInternalTransformPoint()
//----------------------------------------------------------------------------
{
	double pointToTransform[3], *pointTransformed, controlTransformPoint[3];
	pointToTransform[0] = 1.0;
	pointToTransform[1] = 1.0;
	pointToTransform[2] = 1.0;

	albaTransformBaseDerivedClass t =  albaTransformBaseDerivedClass();

	albaMatrix mat;
	mat.Identity();
	mat.SetElement(2,2,10.0);
	t.SetMatrix(mat);

	vtkTransform *transform = vtkTransform::New();
  transform->SetMatrix(mat.GetVTKMatrix());
	pointTransformed = transform->TransformDoublePoint(pointToTransform);


	t.InternalTransformPoint(pointToTransform, controlTransformPoint);

	CPPUNIT_ASSERT(pointTransformed[0] == controlTransformPoint[0] &&
		             pointTransformed[1] == controlTransformPoint[1] &&
								 pointTransformed[2] == controlTransformPoint[2]
								 );

	vtkDEL(transform);
}
