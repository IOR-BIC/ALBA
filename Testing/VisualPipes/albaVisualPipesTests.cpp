/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardWaitOpTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h"
#include "albaTestDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "albaVisualPipesTests.h"

#include "albaFakeLogicForTest.h"
#include "albaPipeCompoundTest.h"
#include "albaPipeCompoundVolumeIsosurfaceTest.h"
#include "albaPipeCompoundVolumeTest.h"
#include "albaPipeCompoundVolumefixedScalarsTest.h"
#include "albaPipeDensityDistanceTest.h"
#include "albaPipeFactoryVMETest.h"
#include "albaPipeGizmoTest.h"
#include "albaPipeGraphTest.h"
#include "albaPipeImage3DTest.h"
#include "albaPipeIsosurfaceTest.h"
#include "albaPipeLandmarkCloudTest.h"
#include "albaPipeMeshSliceTest.h"
#include "albaPipeMeshTest.h"
#include "albaPipeMeterTest.h"
#include "albaPipePointSetTest.h"
#include "albaPipePolylineGraphEditorTest.h"
#include "albaPipePolylineSliceTest.h"
#include "albaPipePolylineTest.h"
#include "albaPipeScalarMatrixTest.h"
#include "albaPipeScalarTest.h"
#include "albaPipeSliceTest.h"
#include "albaPipeSurfaceEditorTest.h"
#include "albaPipeSurfaceSliceTest.h"
#include "albaPipeSurfaceTest.h"
#include "albaPipeSurfaceTexturedTest.h"
#include "albaPipeTensorFieldGlyphsTest.h"
#include "albaPipeTensorFieldSliceTest.h"
#include "albaPipeTensorFieldSurfaceTest.h"
#include "albaPipeTensorFieldTest.h"
#include "albaPipeTrajectoriesTest.h"
#include "albaPipeVectorFieldGlyphsTest.h"
#include "albaPipeVectorFieldMapWithArrowsTest.h"
#include "albaPipeVectorFieldSliceTest.h"
#include "albaPipeVectorFieldSurfaceTest.h"
#include "albaPipeVectorFieldTest.h"
#include "albaPipeVectorTest.h"
#include "albaPipeVolumeProjectedTest.h"
#include "albaPipeVolumeArbSliceTest.h"
#include "albaPipeVolumeOrthoSliceTest.h"
#include "albaPipeWrappedMeterTest.h"
#include "albaServiceLocator.h"
#include "albaVisualPipePolylineGraphTest.h"
#include "albaVisualPipeSlicerSliceTest.h"
#include "albaPipePointCloudTest.h"

#ifndef DISABLE_GPU_VOLUME_RENDERING_TESTS
#include "albaPipeVolumeDRRTest.h"
#include "albaPipeVolumeMIPTest.h"
#include "albaPipeCompoundVolumeDRRTest.h"
#include "albaPipeCompoundVolumeMIPTest.h"
#include "albaPipeCompoundVolumeVRTest.h"
#include "albaPipeRayCastTest.h"
#endif

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "vtkFileOutputWindow.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaClientData);
//-------------------------------------------------------------------------

//Main Test Executor
int	main( int argc, char* argv[] )
{
	// Create log of VTK error messages
	vtkALBASmartPointer<vtkFileOutputWindow> log;
	vtkFileOutputWindow::SetInstance(log);
	albaString logPath = wxGetCwd();
	logPath << "\\VTKTest.log";
	log->SetFileName(logPath);

	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that collects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );        

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );      

	albaFakeLogicForTest *logic = new albaFakeLogicForTest();
	albaServiceLocator::SetLogicManager(logic);

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
	/*  */
#ifndef DISABLE_GPU_VOLUME_RENDERING_TESTS
	runner.addTest(albaPipeVolumeMIPTest::suite());
	runner.addTest(albaPipeVolumeDRRTest::suite());
	runner.addTest(albaPipeCompoundVolumeVRTest::suite());
	runner.addTest(albaPipeCompoundVolumeMIPTest::suite());
	runner.addTest(albaPipeCompoundVolumeDRRTest::suite());
	runner.addTest(albaPipeRayCastTest::suite());
#endif
	runner.addTest(albaPipeMeshTest::suite());
	runner.addTest(albaPipeSurfaceTest::suite()); 
	runner.addTest(albaPipePointCloudTest::suite());
	runner.addTest(albaPipeGizmoTest::suite()); 
	runner.addTest(albaPipeSurfaceTexturedTest::suite()); 
	runner.addTest(albaPipeScalarTest::suite());
	runner.addTest(albaPipeImage3DTest::suite());
	runner.addTest(albaPipeIsosurfaceTest::suite());
	runner.addTest(albaPipePointSetTest::suite());
	runner.addTest(albaPipeFactoryVMETest::suite());
	runner.addTest(albaPipePolylineTest::suite()); 
	runner.addTest(albaPipeLandmarkCloudTest::suite()); 
	runner.addTest(albaPipeVectorTest::suite()); 
	runner.addTest(albaPipeMeterTest::suite());
	runner.addTest(albaPipeVolumeProjectedTest::suite());
	runner.addTest(albaPipeScalarMatrixTest::suite()); 
	runner.addTest(albaPipePolylineGraphEditorTest::suite()); 
	runner.addTest(albaPipeMeshSliceTest::suite());
	runner.addTest(albaPipeSurfaceSliceTest::suite());
	runner.addTest(albaPipeVolumeOrthoSliceTest::suite());
 	runner.addTest(albaPipeVolumeArbSliceTest::suite()); 
	runner.addTest(albaPipeTrajectoriesTest::suite());
	runner.addTest(albaPipeSliceTest::suite()); 
	runner.addTest(albaPipePolylineSliceTest::suite()); 
	runner.addTest(albaPipeWrappedMeterTest::suite()); 
	runner.addTest(albaPipeDensityDistanceTest::suite());
	runner.addTest(albaPipeGraphTest::suite());
	runner.addTest(albaPipeCompoundTest::suite());
	runner.addTest(albaPipeCompoundVolumeTest::suite());
	runner.addTest(albaPipeCompoundVolumeTest::suite());
	runner.addTest(albaPipeCompoundVolumeFixedScalarsTest::suite());
	runner.addTest(albaVisualPipePolylineGraphTest::suite());
	runner.addTest(albaPipeSurfaceEditorTest::suite()); 
	runner.addTest(albaPipeTensorFieldSurfaceTest::suite()); 
	runner.addTest(albaPipeVectorFieldSurfaceTest::suite());
	runner.addTest(albaPipeVectorFieldGlyphsTest::suite());
	runner.addTest(albaPipeVectorFieldMapWithArrowsTest::suite());
	runner.addTest(albaPipeTensorFieldSliceTest::suite());
	runner.addTest(albaPipeVectorFieldSliceTest::suite());
	runner.addTest(albaPipeTensorFieldGlyphsTest::suite());
	runner.addTest(albaPipeTensorFieldTest::suite());
	runner.addTest(albaPipeVectorFieldTest::suite());
	runner.addTest(albaVisualPipeSlicerSliceTest::suite());
	runner.addTest(albaPipeCompoundVolumeIsosurfaceTest::suite());
	/* */


	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 
	
	cppDEL(logic);

	albaTest::PauseBeforeExit();

	return result.wasSuccessful() ? 0 : 1;
}