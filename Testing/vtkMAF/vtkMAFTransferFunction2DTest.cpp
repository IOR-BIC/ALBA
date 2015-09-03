/*=========================================================================

 Program: MAF2
 Module: vtkMAFTransferFunction2DTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFTransferFunction2D.h"
#include "vtkMAFTransferFunction2DTest.h"

//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test1 = vtkMAFTransferFunction2D::New();
  test1->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TetsInitialize()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();

  test->Initialize();

  // Nothing to test

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test1 = vtkMAFTransferFunction2D::New();
  vtkMAFTransferFunction2D *test2 = vtkMAFTransferFunction2D::New();

  test1->Initialize();
  test2->Initialize();

  double color[3] = {.2,.2,.2};

  // Sets some properties
  tfWidget widget;
  widget.Ratio = 1;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 2;
  widget.Range[0][2] = 1;
  widget.Range[1][0] = 0;
  widget.Range[1][1] = 10;
  widget.Range[1][2] = 1;
  widget.Opacity = 1;
  widget.Diffuse = 1;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test1->AddWidget(widget);

  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 3;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 5;
  widget.Range[1][1] = 15;
  widget.Range[1][2] = 7;
  widget.Opacity = 1;
  widget.Diffuse = 1;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test1->AddWidget(widget);

  test2->DeepCopy(test1);

  const char *to_string = test1->SaveToString();

  CPPUNIT_ASSERT(test1->GetNumberOfWidgets() == test2->GetNumberOfWidgets());

  const char *new_string = test2->SaveToString();

  // Equals is not defined so the strings representing the objects are compared
  CPPUNIT_ASSERT(strcmp(new_string,to_string) == 0);

  delete to_string;
  delete new_string;
  test1->Delete();
  test2->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestShallowCopy()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test1 = vtkMAFTransferFunction2D::New();
  vtkMAFTransferFunction2D *test2 = vtkMAFTransferFunction2D::New();

  test1->Initialize();

  double color[3] = {.2,.2,.2};

  // Sets some properties
  tfWidget widget;
  widget.Ratio = 1;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 2;
  widget.Range[0][2] = 1;
  widget.Range[1][0] = 0;
  widget.Range[1][1] = 10;
  widget.Range[1][2] = 1;
  widget.Opacity = 1;
  widget.Diffuse = 1;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test1->AddWidget(widget);

  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 3;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 5;
  widget.Range[1][1] = 15;
  widget.Range[1][2] = 7;
  widget.Opacity = 1;
  widget.Diffuse = 1;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test1->AddWidget(widget);

  test2->ShallowCopy(test1);

  const char *to_string = test1->SaveToString();

  CPPUNIT_ASSERT(test1->GetNumberOfWidgets() == test2->GetNumberOfWidgets());

  const char *new_string = test2->SaveToString();

  // Equals is not defined so the strings representing the objects are compared
  CPPUNIT_ASSERT(strcmp(new_string,to_string) == 0);

  delete to_string;
  delete new_string;
  
  test1->Delete();
  test2->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestGetDataObjectType()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();

  CPPUNIT_ASSERT(test->GetDataObjectType() == VTK_TRANSFER_FUNCTION_2D);

  test->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestGetNumberOfWidgets()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  CPPUNIT_ASSERT(test->GetNumberOfWidgets() == 0);

  tfWidget widget;

  // Set some widget properties to get it accepted
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);
  test->AddWidget(widget);
  test->AddWidget(widget);

  CPPUNIT_ASSERT(test->GetNumberOfWidgets() == 3);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestAddRemoveWidget()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;

  // Set some widget properties to get it accepted
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);
  CPPUNIT_ASSERT(test->GetNumberOfWidgets() == 1);

  test->RemoveWidget(0);
  CPPUNIT_ASSERT(test->GetNumberOfWidgets() == 0);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidget()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);
  test->SetWidget(0, widget);

  tfWidget ret_widget = test->GetWidget(0);

  // equals is not defined so check all widget attributes
  CPPUNIT_ASSERT(ret_widget.Ratio == widget.Ratio);
  CPPUNIT_ASSERT(ret_widget.Range[0][0] == widget.Range[0][0]);
  CPPUNIT_ASSERT(ret_widget.Range[0][1] == widget.Range[0][1]);
  CPPUNIT_ASSERT(ret_widget.Range[0][2] == widget.Range[0][2]);
  CPPUNIT_ASSERT(ret_widget.Range[1][0] == widget.Range[1][0]);
  CPPUNIT_ASSERT(ret_widget.Range[1][1] == widget.Range[1][1]);
  CPPUNIT_ASSERT(ret_widget.Range[1][2] == widget.Range[1][2]);
  CPPUNIT_ASSERT(ret_widget.Color[0] == widget.Color[0]);
  CPPUNIT_ASSERT(ret_widget.Color[1] == widget.Color[1]);
  CPPUNIT_ASSERT(ret_widget.Color[2] == widget.Color[2]);
  CPPUNIT_ASSERT(ret_widget.Opacity == widget.Opacity);
  CPPUNIT_ASSERT(ret_widget.Diffuse == widget.Diffuse);
  CPPUNIT_ASSERT(ret_widget.Visible == widget.Visible);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetName()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  test->SetWidgetName(0,"test_widget_name");
  CPPUNIT_ASSERT(strcmp(test->GetWidgetName(0),"test_widget_name") == 0);

  test->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetVisibility()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  test->SetWidgetVisibility(0,false);
  CPPUNIT_ASSERT(test->GetWidgetVisibility(0) == false);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetOpacity()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  test->SetWidgetOpacity(0,.3);
  CPPUNIT_ASSERT(test->GetWidgetOpacity(0) == .3);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetColor()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  double color[3];
  color[0] = .1;
  color[1] = .2;
  color[2] = .3;

  test->SetWidgetColor(0,color);

  const double *ret_color = test->GetWidgetColor(0);
  CPPUNIT_ASSERT(ret_color[0] == color[0]);
  CPPUNIT_ASSERT(ret_color[1] == color[1]);
  CPPUNIT_ASSERT(ret_color[2] == color[2]);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetDiffuse()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  test->SetWidgetDiffuse(0,.3);
  CPPUNIT_ASSERT(test->GetWidgetDiffuse(0) == .3);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetValueRatio()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  test->SetWidgetValueRatio(0,.3);
  CPPUNIT_ASSERT(test->GetWidgetValueRatio(0) == .3);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetValueRange()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  double range[3];
  range[0] = .1;
  range[1] = .3;
  range[2] = .2;

  test->SetWidgetValueRange(0,range);

  const double *ret_range;
  ret_range = test->GetWidgetValueRange(0);

  CPPUNIT_ASSERT(ret_range[0] == range[0]);
  CPPUNIT_ASSERT(ret_range[1] == range[1]);
  CPPUNIT_ASSERT(ret_range[2] == range[2]);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetGradientRange()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  double range[3];
  range[0] = .6;
  range[1] = .8;
  range[2] = .7;

  test->SetWidgetGradientRange(0,range);

  const double *ret_range;
  ret_range = test->GetWidgetGradientRange(0);

  CPPUNIT_ASSERT(ret_range[0] == range[0]);
  CPPUNIT_ASSERT(ret_range[1] == range[1]);
  CPPUNIT_ASSERT(ret_range[2] == range[2]);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSetGetWidgetGradientInterpolation()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  double range[3];
  range[0] = .6;
  range[1] = .8;
  range[2] = .7;

  test->SetWidgetGradientInterpolation(0,1);

  CPPUNIT_ASSERT(test->GetWidgetGradientInterpolation(0) == 1);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestCheckWidget()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  // all right
  CPPUNIT_ASSERT(test->CheckWidget(widget) == true);

  // wrong ratio
  widget.Ratio = -1;
  CPPUNIT_ASSERT(test->CheckWidget(widget) == false);

  // wrong range
  widget.Ratio = .5;
  widget.Range[0][0] = 2;
  widget.Range[0][1] = 1;
  widget.Range[0][2] = -1;
  widget.Range[1][0] = -2;
  widget.Range[1][1] = .3;
  widget.Range[1][2] = -5.;
  CPPUNIT_ASSERT(test->CheckWidget(widget) == false);

  // wrong opacity
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = 8;
  CPPUNIT_ASSERT(test->CheckWidget(widget) == false);

  // wrong diffuse
  widget.Opacity = .5;
  widget.Diffuse = -1;
  CPPUNIT_ASSERT(test->CheckWidget(widget) == false);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestGetRange()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 2;
  widget.Range[0][2] = 1;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 1;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  widget.Ratio = .5;
  widget.Range[0][0] = 1;
  widget.Range[0][1] = 5;
  widget.Range[0][2] = 2;
  widget.Range[1][0] = 6;
  widget.Range[1][1] = 6;
  widget.Range[1][2] = 6;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  const double * range = test->GetRange();
  CPPUNIT_ASSERT(range[0] == 0);
  CPPUNIT_ASSERT(range[1] == 5);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestGetGradientRange()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  
  test->Initialize();

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 1;
  widget.Range[1][1] = 2;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 0;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 2;
  widget.Range[1][1] = 6;
  widget.Range[1][2] = 5;
  widget.Opacity = .5;
  widget.Diffuse = .5;

  test->AddWidget(widget);

  const double * range = test->GetGradientRange();
  CPPUNIT_ASSERT(range[0] == 1);
  CPPUNIT_ASSERT(range[1] == 6);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestGetValue()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  
  test->Initialize();

  double color[3] = {.2,.2,.2};

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 2;
  widget.Range[0][2] = 1;
  widget.Range[1][0] = 0;
  widget.Range[1][1] = 10;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test->AddWidget(widget);

  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 3;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 5;
  widget.Range[1][1] = 15;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test->AddWidget(widget);

  double opacity = .5;
  double diffuse = .5;
  CPPUNIT_ASSERT(test->GetValue(2,5.2,opacity,color,diffuse) == 0.f);
  CPPUNIT_ASSERT(test->GetValue(1.5,5.2,opacity,color,diffuse) == 0.17105263157894735);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestGetTable()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test = vtkMAFTransferFunction2D::New();
  
  test->Initialize();

  double color[3] = {.2,.2,.2};

  tfWidget widget;
  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 2;
  widget.Range[0][2] = 1;
  widget.Range[1][0] = 0;
  widget.Range[1][1] = 10;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test->AddWidget(widget);

  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 3;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 5;
  widget.Range[1][1] = 15;
  widget.Range[1][2] = 1;
  widget.Opacity = .5;
  widget.Diffuse = .5;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test->AddWidget(widget);

  double vTable[2] = {1.5,1.6};
  double gTable[2] = {5.2,5.3};
  double opacityTable[4];
  unsigned char rgbTable[16];
  CPPUNIT_ASSERT(test->GetTable(2,vTable,2,gTable,opacityTable,rgbTable) == true);

  CPPUNIT_ASSERT(opacityTable[0] == 0.17105263157894735); // the same as before
  CPPUNIT_ASSERT(opacityTable[1] == 0.10526315789473678);
  CPPUNIT_ASSERT(opacityTable[2] == 0.17320261437908496);
  CPPUNIT_ASSERT(opacityTable[3] == 0.10784313725490191);

  test->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTransferFunction2DTest::TestSaveToStringLoadFromString()
//----------------------------------------------------------------------------
{
  vtkMAFTransferFunction2D *test1 = vtkMAFTransferFunction2D::New();
  vtkMAFTransferFunction2D *test2 = vtkMAFTransferFunction2D::New();

  test1->Initialize();
  test2->Initialize();

  double color[3] = {.2,.2,.2};

  // Sets some properties
  tfWidget widget;
  widget.Ratio = 1;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 2;
  widget.Range[0][2] = 1;
  widget.Range[1][0] = 0;
  widget.Range[1][1] = 10;
  widget.Range[1][2] = 1;
  widget.Opacity = 1;
  widget.Diffuse = 1;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test1->AddWidget(widget);

  widget.Ratio = .5;
  widget.Range[0][0] = 0;
  widget.Range[0][1] = 3;
  widget.Range[0][2] = 0;
  widget.Range[1][0] = 5;
  widget.Range[1][1] = 15;
  widget.Range[1][2] = 7;
  widget.Opacity = 1;
  widget.Diffuse = 1;
  widget.Color[0] = color[0];
  widget.Color[1] = color[1];
  widget.Color[2] = color[2];

  test1->AddWidget(widget);

  const char *to_string = test1->SaveToString();
  test2->LoadFromString(to_string);

  CPPUNIT_ASSERT(test1->GetNumberOfWidgets() == test2->GetNumberOfWidgets());

  const char *new_string = test2->SaveToString();

  CPPUNIT_ASSERT(strcmp(new_string,to_string) == 0);

  delete to_string; // this strings are allocated with new
  delete new_string;
  test1->Delete();
  test2->Delete();
}