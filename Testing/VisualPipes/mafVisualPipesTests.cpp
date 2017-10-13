/*=========================================================================

 Program: MAF2
 Module: mafWizardWaitOpTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafVisualPipesTests.h"
#include "mafFakeLogicForTest.h"
#include "mafPipeCompoundTest.h"
#include "mafPipeCompoundVolumeDRRTest.h"
#include "mafPipeCompoundVolumeIsosurfaceTest.h"
#include "mafPipeCompoundVolumeMIPTest.h"
#include "mafPipeCompoundVolumeTest.h"
#include "mafPipeCompoundVolumeVRTest.h"
#include "mafPipeCompoundVolumefixedScalarsTest.h"
#include "mafPipeDensityDistanceTest.h"
#include "mafPipeFactoryVMETest.h"
#include "mafPipeGizmoTest.h"
#include "mafPipeGraphTest.h"
#include "mafPipeImage3DTest.h"
#include "mafPipeIsosurfaceGPUTest.h"
#include "mafPipeIsosurfaceTest.h"
#include "mafPipeLandmarkCloudTest.h"
#include "mafPipeMeshSliceTest.h"
#include "mafPipeMeshTest.h"
#include "mafPipeMeterTest.h"
#include "mafPipePointSetTest.h"
#include "mafPipePolylineGraphEditorTest.h"
#include "mafPipePolylineSliceTest.h"
#include "mafPipePolylineTest.h"
#include "mafPipeRayCastTest.h"
#include "mafPipeScalarMatrixTest.h"
#include "mafPipeScalarTest.h"
#include "mafPipeSliceTest.h"
#include "mafPipeSurfaceEditorTest.h"
#include "mafPipeSurfaceSliceTest.h"
#include "mafPipeSurfaceTest.h"
#include "mafPipeSurfaceTexturedTest.h"
#include "mafPipeTensorFieldGlyphsTest.h"
#include "mafPipeTensorFieldSliceTest.h"
#include "mafPipeTensorFieldSurfaceTest.h"
#include "mafPipeTensorFieldTest.h"
#include "mafPipeTrajectoriesTest.h"
#include "mafPipeVectorFieldGlyphsTest.h"
#include "mafPipeVectorFieldMapWithArrowsTest.h"
#include "mafPipeVectorFieldSliceTest.h"
#include "mafPipeVectorFieldSurfaceTest.h"
#include "mafPipeVectorFieldTest.h"
#include "mafPipeVectorTest.h"
#include "mafPipeVolumeDRRTest.h"
#include "mafPipeVolumeMIPTest.h"
#include "mafPipeVolumeProjectedTest.h"
#include "mafPipeVolumeArbSliceTest.h"
#include "mafPipeVolumeOrthoSliceTest.h"
#include "mafPipeWrappedMeterTest.h"
#include "mafServiceLocator.h"
#include "mafVisualPipePolylineGraphTest.h"
#include "mafVisualPipeSlicerSliceTest.h"
#include "mafVisualPipeVolumeRayCastingTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "vtkFileOutputWindow.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafClientData);
//-------------------------------------------------------------------------

//Main Test Executor
int	main( int argc, char* argv[] )
{
	// Create log of VTK error messages
	vtkMAFSmartPointer<vtkFileOutputWindow> log;
	vtkFileOutputWindow::SetInstance(log);
	mafString logPath = wxGetWorkingDirectory();
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

	mafFakeLogicForTest *logic = new mafFakeLogicForTest();
	mafServiceLocator::SetLogicManager(logic);

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;

	runner.addTest(mafPipeMeshTest::suite());
	runner.addTest(mafPipeSurfaceTest::suite());
	runner.addTest(mafPipeGizmoTest::suite());
	runner.addTest(mafPipeSurfaceTexturedTest::suite());
	runner.addTest(mafPipeScalarTest::suite());
	runner.addTest(mafPipeImage3DTest::suite());
	runner.addTest(mafPipeIsosurfaceTest::suite());
	runner.addTest(mafPipePointSetTest::suite());
	runner.addTest(mafPipeFactoryVMETest::suite());
	runner.addTest(mafPipeIsosurfaceGPUTest::suite());
	runner.addTest(mafPipePolylineTest::suite());
	runner.addTest(mafPipeLandmarkCloudTest::suite());
	runner.addTest(mafPipeVectorTest::suite());
	runner.addTest(mafPipeMeterTest::suite());
	runner.addTest(mafPipeVolumeProjectedTest::suite());
	runner.addTest(mafPipeScalarMatrixTest::suite());
	runner.addTest(mafVisualPipeVolumeRayCastingTest::suite());
	runner.addTest(mafPipeVolumeMIPTest::suite());
	runner.addTest(mafPipeVolumeDRRTest::suite());
	runner.addTest(mafPipePolylineGraphEditorTest::suite());
	runner.addTest(mafPipeMeshSliceTest::suite());
	runner.addTest(mafPipeSurfaceSliceTest::suite());
	runner.addTest(mafPipeVolumeOrthoSliceTest::suite());
 	runner.addTest(mafPipeVolumeArbSliceTest::suite());
	runner.addTest(mafPipeTrajectoriesTest::suite());
	runner.addTest(mafPipeSliceTest::suite());
	runner.addTest(mafPipePolylineSliceTest::suite());
	runner.addTest(mafPipeWrappedMeterTest::suite());
	runner.addTest(mafPipeDensityDistanceTest::suite());
	runner.addTest(mafPipeGraphTest::suite());
	runner.addTest(mafPipeCompoundTest::suite());
	runner.addTest(mafPipeCompoundVolumeTest::suite());
	runner.addTest(mafPipeCompoundVolumeFixedScalarsTest::suite());
	runner.addTest(mafVisualPipePolylineGraphTest::suite());
	runner.addTest(mafPipeSurfaceEditorTest::suite());
	runner.addTest(mafPipeTensorFieldSurfaceTest::suite());
	runner.addTest(mafPipeVectorFieldSurfaceTest::suite());
	runner.addTest(mafPipeVectorFieldGlyphsTest::suite());
	runner.addTest(mafPipeVectorFieldMapWithArrowsTest::suite());
	runner.addTest(mafPipeTensorFieldSliceTest::suite());
	runner.addTest(mafPipeVectorFieldSliceTest::suite());
	runner.addTest(mafPipeTensorFieldGlyphsTest::suite());
	runner.addTest(mafPipeTensorFieldTest::suite());
	runner.addTest(mafPipeVectorFieldTest::suite());
	runner.addTest(mafVisualPipeSlicerSliceTest::suite());
	runner.addTest(mafPipeCompoundVolumeVRTest::suite());
	runner.addTest(mafPipeCompoundVolumeMIPTest::suite());
	runner.addTest(mafPipeCompoundVolumeDRRTest::suite());
	runner.addTest(mafPipeCompoundVolumeIsosurfaceTest::suite());
	//runner.addTest(mafPipeRayCastTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 
	
	cppDEL(logic);

	return result.wasSuccessful() ? 0 : 1;
}

