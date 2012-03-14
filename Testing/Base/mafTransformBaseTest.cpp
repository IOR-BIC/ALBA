/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafTransformBaseTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.4.1 $
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

#include <cppunit/config/SourcePrefix.h>
#include "mafTransformBaseTest.h"

#include "mafTransformBase.h"
#include "mafSmartPointer.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkLinearTransform.h"
#include "vtkTransform.h"


#include <iostream>

//support object
class mafTransformBaseDerivedClass : public mafTransformBase
{
	mafTypeMacro(mafTransformBaseDerivedClass,mafTransformBase);
	mafTransformBaseDerivedClass(){;};
	
	virtual void SetMatrix(const mafMatrix &input) {*m_Matrix=input;SetTimeStamp(input.GetTimeStamp());Modified();}

	~mafTransformBaseDerivedClass(){;};
  protected:
		virtual void InternalUpdate(){;};
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafTransformBaseDerivedClass)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafTransformBaseTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafTransformBaseDerivedClass t;
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafTransformBaseDerivedClass *t = new mafTransformBaseDerivedClass();
  cppDEL(t);
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::TestCopyConstructor()
//----------------------------------------------------------------------------
{
  mafTransformBaseDerivedClass t =  mafTransformBaseDerivedClass();
	t.SetTimeStamp(100);

	mafMatrix mat;
	mat.Identity();
	mat.SetElement(2,2,10.0);
	t.SetMatrix(mat);

  mafTransformBaseDerivedClass *t1 = new mafTransformBaseDerivedClass(t);

  CPPUNIT_ASSERT(t1 != NULL);
	CPPUNIT_ASSERT(mafEquals(t1->GetTimeStamp(), t.GetTimeStamp()));
	CPPUNIT_ASSERT(mafEquals(t1->GetMatrix().GetElement(2,2), t.GetMatrix().GetElement(2,2)));
	cppDEL(t1); //there was a delete problem , correct the copy constructor
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::TestGetMatrix()
//----------------------------------------------------------------------------
{
	mafTransformBaseDerivedClass t =  mafTransformBaseDerivedClass();
	mafMatrix mat;
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
void mafTransformBaseTest::TestTimeStamp()
//----------------------------------------------------------------------------
{
	mafTransformBaseDerivedClass t =  mafTransformBaseDerivedClass();
	t.SetTimeStamp(50);
	CPPUNIT_ASSERT(t.GetTimeStamp() == 50);
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::TestModifiedTime()
//----------------------------------------------------------------------------
{
  mafTransformBaseDerivedClass t =  mafTransformBaseDerivedClass();
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
void mafTransformBaseTest::TestUpdateTime()
//----------------------------------------------------------------------------
{
	mafTransformBaseDerivedClass t =  mafTransformBaseDerivedClass();
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
void mafTransformBaseTest::TestGetEventSource()
//----------------------------------------------------------------------------
{
  mafTransformBaseDerivedClass t =  mafTransformBaseDerivedClass();
	CPPUNIT_ASSERT(t.GetEventSource() != NULL);
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::TestGetVTKTransform()
//----------------------------------------------------------------------------
{
	mafTransformBaseDerivedClass t =  mafTransformBaseDerivedClass();
	CPPUNIT_ASSERT(t.GetVTKTransform() != NULL);

	//casting operator
	vtkLinearTransform *linearTransform;
	CPPUNIT_ASSERT(vtkLinearTransform::SafeDownCast(t.GetVTKTransform()));
  CPPUNIT_ASSERT(linearTransform = ((vtkLinearTransform *)(t.GetVTKTransform())));
}
//----------------------------------------------------------------------------
void mafTransformBaseTest::TestInternalTransformPoint()
//----------------------------------------------------------------------------
{
	double pointToTransform[3], *pointTransformed, controlTransformPoint[3];
	pointToTransform[0] = 1.0;
	pointToTransform[1] = 1.0;
	pointToTransform[2] = 1.0;

	mafTransformBaseDerivedClass t =  mafTransformBaseDerivedClass();

	mafMatrix mat;
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
