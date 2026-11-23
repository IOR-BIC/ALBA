/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProgressBarHelperTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>
#include "albaProgressBarHelperTest.h"
#include "albaProgressBarHelper.h"
#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyData.h"

// The tests have no main window (albaProgressBarHelper::GetMainWindow() is NULL): the state of the helper is checked

//-------------------------------------------------------------------------
void albaProgressBarHelperTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
	//Create and Destroy albaProgressBarHelper object
	CPPUNIT_ASSERT(albaProgressBarHelper::GetMainWindow() == NULL);
	albaProgressBarHelper *helper;
	helper = new albaProgressBarHelper();
	CPPUNIT_ASSERT(!helper->IsInited() && helper->GetProgress() == 0);
	delete helper;
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::InitProgressBarTest()
{
	albaProgressBarHelper helper;

	helper.InitProgressBar("",false);
	CPPUNIT_ASSERT(helper.IsInited());
	CPPUNIT_ASSERT(helper.GetProgress() == 0);

	//Double Init: the bar is closed and opened again
	helper.UpdateProgressBar(30);
	helper.InitProgressBar("",false);
	CPPUNIT_ASSERT(helper.IsInited());
	CPPUNIT_ASSERT(helper.GetProgress() == 0);
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::UpdateProgressBarTest()
{
	albaProgressBarHelper helper;

	helper.InitProgressBar("",false);

	helper.UpdateProgressBar(10);
	CPPUNIT_ASSERT(helper.GetProgress() == 10);

	helper.UpdateProgressBar(15);
	CPPUNIT_ASSERT(helper.GetProgress() == 15);

	//A progress over 100 is refused
	helper.UpdateProgressBar(150);
	CPPUNIT_ASSERT(helper.GetProgress() == 15);

	helper.ResetProgress();
	CPPUNIT_ASSERT(helper.GetProgress() == 0);
	CPPUNIT_ASSERT(helper.IsInited());
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::CloseProgressBarTest()
{
	albaProgressBarHelper helper;

	helper.InitProgressBar("",false);
	helper.UpdateProgressBar(50);
	helper.CloseProgressBar();
	CPPUNIT_ASSERT(!helper.IsInited());
	CPPUNIT_ASSERT(helper.GetProgress() == 0);

	//Auto close on destroy test
	albaProgressBarHelper *helper2;
	helper2 = new albaProgressBarHelper();
	helper2->InitProgressBar("",false);
	delete helper2;
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::GetSetTextModeTest()
{
	albaProgressBarHelper helper;

	//Default mode should be false
	CPPUNIT_ASSERT(helper.GetTextMode()==false);
		
	//Text mode true test
	helper.SetTextMode(true);
	CPPUNIT_ASSERT(helper.GetTextMode()==true);

	//text mode false test
	helper.SetTextMode(false);
	CPPUNIT_ASSERT(helper.GetTextMode()==false);
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::TextModeTest()
{
	//In text mode the progress advances by steps of 2
	albaProgressBarHelper helper;

	helper.SetTextMode(true);
	helper.InitProgressBar("",false);
	helper.UpdateProgressBar(11);
	CPPUNIT_ASSERT(helper.GetProgress() == 10);
	helper.CloseProgressBar();
	CPPUNIT_ASSERT(!helper.IsInited());
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::BindToVTKFilterTest()
{
	vtkALBASmartPointer<vtkSphereSource> sphere;

	//The filter opens and closes the bar
	{
		albaProgressBarHelper helper;
		helper.BindToVTKFilter(sphere);
		sphere->Update();
		CPPUNIT_ASSERT(!helper.IsInited());
	}

	//A bar already open is moved by the filter but it is not closed
	{
		albaProgressBarHelper helper;
		helper.InitProgressBar("",false);
		helper.BindToVTKFilter(sphere);
		sphere->Modified();
		sphere->Update();
		CPPUNIT_ASSERT(helper.IsInited());
	}

	//The helpers are destroyed: the filter can still run
	sphere->Modified();
	sphere->Update();
	CPPUNIT_ASSERT(sphere->GetOutput()->GetNumberOfPoints() > 0);
}
