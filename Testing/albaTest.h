/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaTests_H__
#define __CPP_UNIT_albaTests_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h>

#include "albaString.h"

class vtkImageData;
class vtkPointData;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkDataSet;

// Helper class used to build a fake application needed by the wxConfig
// to store settings into the registry with the same name of the application
// ===============================================================================
class TestApp : public wxApp
// ===============================================================================
{
public:
	bool OnInit();
	int  OnExit();
	bool Yield(bool onlyIfNeeded = false);
};

DECLARE_APP(TestApp)

#define COMPARE_IMAGES(imageName, ...) CompareImage(getTestNamer__().getFixtureName().c_str(), imageName, ##__VA_ARGS__)
#define COMPARE_VTK_IMAGES(vtkImg, imageName, ...) CompareVTKImage(vtkImg, getTestNamer__().getFixtureName().c_str(), imageName, ##__VA_ARGS__)
#define GET_TEST_DATA_DIR() GetTestDataDir(getTestNamer__().getFixtureName().c_str())

class albaTest : public CPPUNIT_NS::TestFixture
{
public: 
	albaTest();
  
	/** CPPUNIT fixture: executed before each test, prepares an application for test runs
			DO NOT OVERLOAD THIS METHOD, use BeforeTest instead */
  void setUp();

	/** CPPUNIT fixture: executed after each test, clean an application for test runs
			DO NOT OVERLOAD THIS METHOD, use AfterTest instead */
  void tearDown();

	/** Test specific stuff executed before each test */
	virtual void BeforeTest() {};

	/** Test specific stuff executed after each test */
	virtual void AfterTest() {};

	static void PauseBeforeExit();

protected:
	void CompareImage(albaString suiteName, albaString imageName, int index = -1);

	void CompareVTKImage(vtkImageData *imDataComp, albaString suiteName, albaString imageName, int index=-1);

	void InitializeRenderWindow();

	void RenderData(vtkDataSet *data);
	
	albaString GetTestDataDir(albaString suiteName);

	TestApp *m_App;
	vtkRenderer *m_Renderer;
	vtkRenderWindow *m_RenderWindow;
	vtkRenderWindowInteractor *m_RenderWindowInteractor;

	albaString m_WorkingDir;
};

#endif
