/*=========================================================================

 Program: MAF2
 Module: vtkMAFCollisionDetectionFilterTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFCollisionDetectionFilterTEST_H__
#define __CPP_UNIT_vtkMAFCollisionDetectionFilterTEST_H__

#include "mafTest.h"

class vtkActor;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkProperty;

class vtkMAFCollisionDetectionFilterTest : public mafTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( vtkMAFCollisionDetectionFilterTest );
  /*CPPUNIT_TEST( TestDynamicAllocation );*/
  CPPUNIT_TEST( Test );
  /*CPPUNIT_TEST( TestChangingMatrix );*/
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void Test();
  void TestChangingMatrix();
  void CompareImages(int index , wxString folder);

  void Visualize(vtkActor *actor);
  void AddPolydataToVisualize(vtkPolyData *data, vtkProperty *property = NULL);

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;

};

#endif