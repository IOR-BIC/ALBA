/*=========================================================================

 Program: MAF2
 Module: vtkMAFGridActorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFGridActortest_H__
#define __CPP_UNIT_vtkMAFGridActortest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class vtkImageData;
class vtkActor;

/**
  Class Name: vtkMAFGridActorTest.
  Test for vtkMAFGridActor.
*/
class vtkMAFGridActorTest : public mafTest
{
  public:
    /** Test IDs*/
    enum ID_TEST_LIST
    {
      ID_GRID_NORMAL_TEST = 0,
      ID_GRID_POSITION_TEST,
      ID_GRID_COLOR_TEST,
      ID_LABEL_ACTOR_TEST,
    };

    /** Test suite begin. */
    CPPUNIT_TEST_SUITE( vtkMAFGridActorTest );
    /** Instance creation and deletion test. */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test SetGridNormal. */
    CPPUNIT_TEST( TestSetGridNormal );
    /** Test SetGridPosition. */
    CPPUNIT_TEST( TestSetGridPosition );
    /** Test SetGridColor. */
    CPPUNIT_TEST( TestSetGridColor );
    /** Test SetGetLabelActor. */
    CPPUNIT_TEST( TestGetLabelActor );
    /** Test PrintSelf. */
    CPPUNIT_TEST( TestPrintSelf );
    /** Test suite end. */
    CPPUNIT_TEST_SUITE_END();

  protected:

    /** Test the object creation and deletion.*/
    void TestDynamicAllocation();
    /** Test SetGridNormal in order to see grid new perpendicular axis.*/
    void TestSetGridNormal();
    /** Test SetGridPosition in order to see grid position along its axis.*/
    void TestSetGridPosition();
    /** Test SetGridColor in order to see grid color.*/
    void TestSetGridColor();
    /** Test GetLabelActor in order to see which value of scale  as string has text actor.*/
    void TestGetLabelActor();
    /** Call PrintSelf in order to print information in console output.*/
    void TestPrintSelf();

    /** Render data in vtk render window.*/
    void RenderData(vtkActor *actor );
    /** After saving images from render window, compare them in order to find difference between test sessions. */
    void CompareImages(vtkRenderWindow * renwin);
    /** Convert an int to std::string */
    static std::string ConvertInt(int number);

    int m_TestNumber;
};

#endif
