/*=========================================================================

 Program: MAF2Medical
 Module: vtkMAFDataArrayDescriptorTest
 Authors: Matteo Giacomoni
 
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

#include "vtkMAFDataArrayDescriptorTest.h"

#include "../BES_Beta/vtkMAF/vtkMAFDataArrayDescriptor.h"

#include "vtkMAFSmartPointer.h"


//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::setUp()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::tearDown()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMAFDataArrayDescriptor *array1 = vtkMAFDataArrayDescriptor::New();
  array1->Delete();

  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array2;

  vtkMAFDataArrayDescriptor *array3;
  vtkNEW(array3);
  vtkDEL(array3);
}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestSetName()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;

  array->SetName("TEST1");

  CPPUNIT_ASSERT( strcmp(array->GetName(),"TEST1")==0 );

  array->SetName("TEST2");

  CPPUNIT_ASSERT( strcmp(array->GetName(),"TEST2")==0 );

}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestSetDataType()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_BIT);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_BIT );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_CHAR);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_CHAR );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_CHAR);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_UNSIGNED_CHAR );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_SHORT);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_SHORT );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_SHORT);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_UNSIGNED_SHORT );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_INT);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_INT );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_INT);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_UNSIGNED_INT );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_LONG);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_LONG );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_LONG);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_UNSIGNED_LONG );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_FLOAT);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_FLOAT );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_DOUBLE);
  CPPUNIT_ASSERT( array->GetDataType() == VTK_DOUBLE );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( array->GetDataTypeMaxValue() == VTK_ID_TYPE );
  CPPUNIT_ASSERT( array->GetDataTypeMinValue() == VTK_BIT );
  //////////////////////////////////////////////////////////////////////////
}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestGetDataTypeSize()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_BIT);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == 0 );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_CHAR);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(char) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_CHAR);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(unsigned char) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_SHORT);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(short) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_SHORT);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(unsigned short) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_INT);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(int) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_INT);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(unsigned int) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_LONG);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(long) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_LONG);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(unsigned long) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_FLOAT);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(float) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_DOUBLE);
  CPPUNIT_ASSERT( array->GetDataTypeSize() == sizeof(double) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_BIT) == 0 );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_CHAR) == sizeof(char) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_UNSIGNED_CHAR) == sizeof(unsigned char) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_SHORT) == sizeof(short) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_UNSIGNED_SHORT) == sizeof(unsigned short) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_INT) == sizeof(int) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_UNSIGNED_INT) == sizeof(unsigned int) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_LONG) == sizeof(long) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_UNSIGNED_LONG) == sizeof(unsigned long) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_FLOAT) == sizeof(float) );
  //////////////////////////////////////////////////////////////////////////
  CPPUNIT_ASSERT( vtkMAFDataArrayDescriptor::GetDataTypeSize(VTK_DOUBLE) == sizeof(double) );
  //////////////////////////////////////////////////////////////////////////
}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestSetNumberOfComponents()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  array->SetNumberOfComponents(1);

  CPPUNIT_ASSERT( array->GetNumberOfComponents() == 1 );

  array->SetNumberOfComponents(100);

  CPPUNIT_ASSERT( array->GetNumberOfComponents() == 100 );
}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestSetNumberOfTuples()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  array->SetNumberOfTuples(1);

  CPPUNIT_ASSERT( array->GetNumberOfTuples() == 1 );

  array->SetNumberOfTuples(100);

  CPPUNIT_ASSERT( array->GetNumberOfTuples() == 100 );
}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestGetSize()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);

  CPPUNIT_ASSERT( array->GetSize() == 15 );

  array->SetNumberOfComponents(1);
  array->SetNumberOfTuples(100);

  CPPUNIT_ASSERT( array->GetSize() == 100 );
}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestGetActualMemorySize()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_BIT);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 2 );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_CHAR);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(char) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_CHAR);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(unsigned char) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_SHORT);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(short) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_SHORT);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(unsigned short) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_INT);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(int) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_INT);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(unsigned int) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_LONG);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(long) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_LONG);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(unsigned long) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_FLOAT);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(float) );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_DOUBLE);
  array->SetNumberOfComponents(3);
  array->SetNumberOfTuples(5);
  CPPUNIT_ASSERT( array->GetActualMemorySize() == 15*sizeof(double) );
  //////////////////////////////////////////////////////////////////////////

}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestGetDataTypeMin()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_BIT);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_BIT_MIN );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_CHAR);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_CHAR_MIN );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_CHAR);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_UNSIGNED_CHAR_MIN );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_SHORT);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_SHORT_MIN );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_SHORT);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_UNSIGNED_SHORT_MIN);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_INT);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_INT_MIN);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_INT);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_UNSIGNED_INT_MIN);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_LONG);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_LONG_MIN);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_LONG);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_UNSIGNED_LONG_MIN);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_FLOAT);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_FLOAT_MIN);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_DOUBLE);
  CPPUNIT_ASSERT( array->GetDataTypeMin() == VTK_DOUBLE_MIN);
  //////////////////////////////////////////////////////////////////////////

}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestGetDataTypeMax()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_BIT);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_BIT_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_CHAR);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_CHAR_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_CHAR);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_UNSIGNED_CHAR_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_SHORT);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_SHORT_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_SHORT);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_UNSIGNED_SHORT_MAX);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_INT);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_INT_MAX);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_INT);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_UNSIGNED_INT_MAX);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_LONG);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_LONG_MAX);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_LONG);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_UNSIGNED_LONG_MAX);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_FLOAT);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_FLOAT_MAX);
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_DOUBLE);
  CPPUNIT_ASSERT( array->GetDataTypeMax() == VTK_DOUBLE_MAX);
  //////////////////////////////////////////////////////////////////////////

}
//-------------------------------------------------------------------------
void vtkMAFDataArrayDescriptorTest::TestGetDataTypeRange()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataArrayDescriptor> array;
  double range[2];
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_BIT);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_BIT_MIN && range[1] == VTK_BIT_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_CHAR);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_CHAR_MIN && range[1] == VTK_CHAR_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_CHAR);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_UNSIGNED_CHAR_MIN && range[1] == VTK_UNSIGNED_CHAR_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_SHORT);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_SHORT_MIN && range[1] == VTK_SHORT_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_SHORT);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_UNSIGNED_SHORT_MIN && range[1] == VTK_UNSIGNED_SHORT_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_INT);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_INT_MIN && range[1] == VTK_INT_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_INT);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_UNSIGNED_INT_MIN && range[1] == VTK_UNSIGNED_INT_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_LONG);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_LONG_MIN && range[1] == VTK_LONG_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_UNSIGNED_LONG);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_UNSIGNED_LONG_MIN && range[1] == VTK_UNSIGNED_LONG_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_FLOAT);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_FLOAT_MIN && range[1] == VTK_FLOAT_MAX );
  //////////////////////////////////////////////////////////////////////////
  array->SetDataType(VTK_DOUBLE);
  array->GetDataTypeRange(range);
  CPPUNIT_ASSERT( range[0] == VTK_DOUBLE_MIN && range[1] == VTK_DOUBLE_MAX );
  //////////////////////////////////////////////////////////////////////////
}
