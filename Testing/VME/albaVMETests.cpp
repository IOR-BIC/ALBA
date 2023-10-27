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
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaVMETests.h"

#include "CustomDataPipeTest.h"
#include "albaAttributeSegmentationVolumeTest.h"
#include "albaCryptTest.h"
#include "albaDataPipeCustomSegmentationVolumeTest.h"
#include "albaDataVectorTest.h"
#include "albaFakeLogicForTest.h"
#include "albaMSFImporterTest.h"
#include "albaMatrixInterpolatorTest.h"
#include "albaMatrixVectorTest.h"
#include "albaPolylineGraphBranchTest.h"
#include "albaPolylineGraphEdgeTest.h"
#include "albaPolylineGraphTest.h"
#include "albaPolylineGraphVertexTest.h"
#include "albaScalarVectorTest.h"
#include "albaServiceLocator.h"
#include "albaVMEAnalogTest.h"
#include "albaVMEExternalDataTest.h"
#include "albaVMEFactoryTest.h"
#include "albaVMEGenericAbstractTest.h"
#include "albaVMEGenericTest.h"
#include "albaVMEGizmoTest.h"
#include "albaVMEGroupTest.h"
#include "albaVMEImageTest.h"
#include "albaVMEInfoTextTest.h"
#include "albaVMEItemScalarMatrixTest.h"
#include "albaVMEItemTest.h"
#include "albaVMEItemVTKTest.h"
#include "albaVMELabeledVolumeTest.h"
#include "albaVMELandmarkCloudTest.h"
#include "albaVMELandmarkTest.h"
#include "albaVMEMapsTest.h"
#include "albaVMEMeshTest.h"
#include "albaVMEMeterTest.h"
#include "albaVMEOutputComputeWrappingTest.h"
#include "albaVMEOutputImageTest.h"
#include "albaVMEOutputLandmarkCloudTest.h"
#include "albaVMEOutputMeshTest.h"
#include "albaVMEOutputMeterTest.h"
#include "albaVMEPointCloudTest.h"
#include "albaVMEOutputPointSetTest.h"
#include "albaVMEOutputPolylineEditorTest.h"
#include "albaVMEOutputPolylineTest.h"
#include "albaVMEOutputScalarMatrixTest.h"
#include "albaVMEOutputScalarTest.h"
#include "albaVMEOutputSurfaceEditorTest.h"
#include "albaVMEOutputSurfaceTest.h"
#include "albaVMEOutputVTKTest.h"
#include "albaVMEOutputVolumeTest.h"
#include "albaVMEOutputWrappedMeterTest.h"
#include "albaVMEPointSetTest.h"
#include "albaVMEPolylineEditorTest.h"
#include "albaVMEPolylineGraphTest.h"
#include "albaVMEPolylineSplineTest.h"
#include "albaVMEPolylineTest.h"
#include "albaVMEProberTest.h"
#include "albaVMERawMotionDataTest.h"
#include "albaVMERefSysTest.h"
#include "albaVMEScalarMatrixTest.h"
#include "albaVMEScalarTest.h"
#include "albaVMESegmentationVolumeTest.h"
#include "albaVMESlicerTest.h"
#include "albaVMESurfaceEditorTest.h"
#include "albaVMESurfaceParametricTest.h"
#include "albaVMESurfaceTest.h"
#include "albaVMEVectorTest.h"
#include "albaVMEVolumeGrayTest.h"
#include "albaVMEVolumeRGBTest.h"
#include "albaVMEWrappedMeterTest.h"
#include "mmaMeterTest.h"

#include "VMEGenericBoundsTest.h"
#include "VMEGenericPoseTest.h"
#include "VMEGenericTest.h"

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
	vtkOutputWindow::SetInstance(log);
	albaString logPath = wxGetCwd();
	logPath << "\\vtkLog.txt";
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
	runner.addTest(albaVMEExternalDataTest::suite());
	runner.addTest(albaVMEVolumeGrayTest::suite());
	runner.addTest(albaVMESurfaceParametricTest::suite());
	runner.addTest(albaVMESurfaceTest::suite());
	runner.addTest(albaVMEGroupTest::suite());
	runner.addTest(albaVMEGizmoTest::suite());
	runner.addTest(albaVMEScalarMatrixTest::suite());
	runner.addTest(albaVMEScalarTest::suite());
	runner.addTest(albaVMEPolylineTest::suite());
	runner.addTest(albaVMEPolylineSplineTest::suite());
	runner.addTest(albaVMEMeshTest::suite());
	runner.addTest(albaVMEPointCloudTest::suite());
	runner.addTest(albaVMELandmarkTest::suite());
	runner.addTest(albaVMELandmarkCloudTest::suite());
	runner.addTest(albaMatrixVectorTest::suite());
	runner.addTest(albaVMEFactoryTest::suite());
	runner.addTest(albaDataVectorTest::suite());
	runner.addTest(albaVMEItemVTKTest::suite());
	runner.addTest(albaVMEImageTest::suite());
	runner.addTest(albaVMEVolumeRGBTest::suite());
	runner.addTest(albaVMEInfoTextTest::suite());
	runner.addTest(mmaMeterTest::suite());
	runner.addTest(albaVMEOutputMeterTest::suite());
	runner.addTest(albaVMEPointSetTest::suite());
	runner.addTest(albaVMEVectorTest::suite());
	runner.addTest(albaVMEOutputPolylineTest::suite());
	runner.addTest(albaVMEOutputSurfaceTest::suite());
	runner.addTest(albaVMEOutputPointSetTest::suite());
	runner.addTest(albaVMEOutputScalarTest::suite());
	runner.addTest(albaVMEOutputLandmarkCloudTest::suite());
	runner.addTest(albaVMEOutputScalarMatrixTest::suite());
	runner.addTest(albaVMEOutputMeshTest::suite());
	runner.addTest(albaVMEOutputImageTest::suite());
	//runner.addTest(albaVMEOutputVolumeTest::suite());
	runner.addTest(albaCryptTest::suite());
	runner.addTest(albaMatrixInterpolatorTest::suite());
	runner.addTest(albaVMEGenericTest::suite());
	runner.addTest(albaVMEOutputVTKTest::suite());
	runner.addTest(albaVMEItemScalarMatrixTest::suite());
	runner.addTest(albaScalarVectorTest::suite());
	runner.addTest(albaVMEGenericAbstractTest::suite());
	runner.addTest(albaVMEItemTest::suite());
	runner.addTest(albaVMERefSysTest::suite());
	runner.addTest(albaVMEProberTest::suite());
	runner.addTest(albaVMEMeterTest::suite());
	runner.addTest(albaMSFImporterTest::suite());
//	runner.addTest(albaVMESlicerTest::suite());
	runner.addTest(albaVMERawMotionDataTest::suite());
	runner.addTest(albaPolylineGraphTest::suite());
	runner.addTest(albaVMEWrappedMeterTest::suite());
	runner.addTest(albaVMELabeledVolumeTest::suite());
	runner.addTest(albaVMEAnalogTest::suite());
	runner.addTest(albaVMEOutputSurfaceEditorTest::suite());
	runner.addTest(albaVMEOutputPolylineEditorTest::suite());
	runner.addTest(albaVMESurfaceEditorTest::suite());
	runner.addTest(albaVMEPolylineEditorTest::suite());
	runner.addTest(albaVMEMapsTest::suite());
	runner.addTest(albaVMEOutputWrappedMeterTest::suite());
	runner.addTest(albaVMEPolylineGraphTest::suite());
	runner.addTest(albaVMESegmentationVolumeTest::suite());
	runner.addTest(albaAttributeSegmentationVolumeTest::suite());
	runner.addTest(albaVMEOutputComputeWrappingTest::suite());
	runner.addTest(albaPolylineGraphVertexTest::suite());
	runner.addTest(albaPolylineGraphEdgeTest::suite());
	runner.addTest(albaPolylineGraphBranchTest::suite());
	runner.addTest(albaDataPipeCustomSegmentationVolumeTest::suite());
	runner.addTest(VMEGenericPoseTest::suite());
	runner.addTest(VMEGenericBoundsTest::suite());
	runner.addTest(CustomDataPipeTest::suite());
	runner.addTest(VMEGenericTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 
	
	cppDEL(logic);

	return result.wasSuccessful() ? 0 : 1;
}