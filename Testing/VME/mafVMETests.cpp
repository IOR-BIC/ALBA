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

#include "CustomDataPipeTest.h"

#include "mafAttributeSegmentationVolumeTest.h"
#include "mafCryptTest.h"
#include "mafDataPipeCustomSegmentationVolumeTest.h"
#include "mafDataVectorTest.h"
#include "mafFakeLogicForTest.h"
#include "mafMSFImporterTest.h"
#include "mafMatrixInterpolatorTest.h"
#include "mafMatrixVectorTest.h"
#include "mafPolylineGraphBranchTest.h"
#include "mafPolylineGraphEdgeTest.h"
#include "mafPolylineGraphTest.h"
#include "mafPolylineGraphVertexTest.h"
#include "mafScalarVectorTest.h"
#include "mafServiceLocator.h"
#include "mafVMEAnalogTest.h"
#include "mafVMEExternalDataTest.h"
#include "mafVMEFactoryTest.h"
#include "mafVMEGenericAbstractTest.h"
#include "mafVMEGenericTest.h"
#include "mafVMEGizmoTest.h"
#include "mafVMEGroupTest.h"
#include "mafVMEImageTest.h"
#include "mafVMEInfoTextTest.h"
#include "mafVMEItemScalarMatrixTest.h"
#include "mafVMEItemTest.h"
#include "mafVMEItemVTKTest.h"
#include "mafVMELabeledVolumeTest.h"
#include "mafVMELandmarkCloudTest.h"
#include "mafVMELandmarkTest.h"
#include "mafVMEMapsTest.h"
#include "mafVMEMeshTest.h"
#include "mafVMEMeterTest.h"
#include "mafVMEOutputComputeWrappingTest.h"
#include "mafVMEOutputImageTest.h"
#include "mafVMEOutputLandmarkCloudTest.h"
#include "mafVMEOutputMeshTest.h"
#include "mafVMEOutputMeterTest.h"
#include "mafVMEOutputPointSetTest.h"
#include "mafVMEOutputPolylineEditorTest.h"
#include "mafVMEOutputPolylineTest.h"
#include "mafVMEOutputScalarMatrixTest.h"
#include "mafVMEOutputScalarTest.h"
#include "mafVMEOutputSurfaceEditorTest.h"
#include "mafVMEOutputSurfaceTest.h"
#include "mafVMEOutputVTKTest.h"
#include "mafVMEOutputVolumeTest.h"
#include "mafVMEOutputWrappedMeterTest.h"
#include "mafVMEPointSetTest.h"
#include "mafVMEPolylineEditorTest.h"
#include "mafVMEPolylineGraphTest.h"
#include "mafVMEPolylineSplineTest.h"
#include "mafVMEPolylineTest.h"
#include "mafVMEProberTest.h"
#include "mafVMERawMotionDataTest.h"
#include "mafVMERefSysTest.h"
#include "mafVMEScalarMatrixTest.h"
#include "mafVMEScalarTest.h"
#include "mafVMESegmentationVolumeTest.h"
#include "mafVMESlicerTest.h"
#include "mafVMESurfaceEditorTest.h"
#include "mafVMESurfaceParametricTest.h"
#include "mafVMESurfaceTest.h"
#include "mafVMETests.h"
#include "mafVMEVectorTest.h"
#include "mafVMEVolumeGrayTest.h"
#include "mafVMEVolumeRGBTest.h"
#include "mafVMEWrappedMeterTest.h"
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
mafCxxTypeMacro(mafClientData);
//-------------------------------------------------------------------------

//Main Test Executor
int	main( int argc, char* argv[] )
{
	// Create log of VTK error messages
	vtkMAFSmartPointer<vtkFileOutputWindow> log;
	vtkOutputWindow::SetInstance(log);
	mafString logPath = wxGetWorkingDirectory();
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

	mafFakeLogicForTest *logic = new mafFakeLogicForTest();
	mafServiceLocator::SetLogicManager(logic);

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
// 	runner.addTest(mafVMEExternalDataTest::suite());
// 	runner.addTest(mafVMEVolumeGrayTest::suite());
// 	runner.addTest(mafVMESurfaceParametricTest::suite());
// 	runner.addTest(mafVMESurfaceTest::suite());
// 	runner.addTest(mafVMEGroupTest::suite());
// 	runner.addTest(mafVMEGizmoTest::suite());
// 	runner.addTest(mafVMEScalarMatrixTest::suite());
// 	runner.addTest(mafVMEScalarTest::suite());
// 	runner.addTest(mafVMEPolylineTest::suite());
// 	runner.addTest(mafVMEPolylineSplineTest::suite());
// 	runner.addTest(mafVMEMeshTest::suite());
// 	runner.addTest(mafVMELandmarkTest::suite());
// 	runner.addTest(mafVMELandmarkCloudTest::suite());
// 	runner.addTest(mafMatrixVectorTest::suite());
// 	runner.addTest(mafVMEFactoryTest::suite());
// 	runner.addTest(mafDataVectorTest::suite());
// 	runner.addTest(mafVMEItemVTKTest::suite());
// 	runner.addTest(mafVMEImageTest::suite());
// 	runner.addTest(mafVMEVolumeRGBTest::suite());
// 	runner.addTest(mafVMEInfoTextTest::suite());
// 	runner.addTest(mmaMeterTest::suite());
// 	runner.addTest(mafVMEOutputMeterTest::suite());
// 	runner.addTest(mafVMEPointSetTest::suite());
// 	runner.addTest(mafVMEVectorTest::suite());
// 	runner.addTest(mafVMEOutputPolylineTest::suite());
// 	runner.addTest(mafVMEOutputSurfaceTest::suite());
// 	runner.addTest(mafVMEOutputPointSetTest::suite());
// 	runner.addTest(mafVMEOutputScalarTest::suite());
// 	runner.addTest(mafVMEOutputLandmarkCloudTest::suite());
// 	runner.addTest(mafVMEOutputScalarMatrixTest::suite());
// 	runner.addTest(mafVMEOutputMeshTest::suite());
// 	runner.addTest(mafVMEOutputImageTest::suite());
// 	runner.addTest(mafVMEOutputVolumeTest::suite());
// 	runner.addTest(mafCryptTest::suite());
// 	runner.addTest(mafMatrixInterpolatorTest::suite());
// 	runner.addTest(mafVMEGenericTest::suite());
// 	runner.addTest(mafVMEOutputVTKTest::suite());
// 	runner.addTest(mafVMEItemScalarMatrixTest::suite());
// 	runner.addTest(mafScalarVectorTest::suite());
// 	runner.addTest(mafVMEGenericAbstractTest::suite());
// 	runner.addTest(mafVMEItemTest::suite());
// 	runner.addTest(mafVMERefSysTest::suite());
// 	runner.addTest(mafVMEProberTest::suite());
// 	runner.addTest(mafVMEMeterTest::suite());
// 	runner.addTest(mafMSFImporterTest::suite());
// 	runner.addTest(mafVMESlicerTest::suite());
// 	runner.addTest(mafVMERawMotionDataTest::suite());
// 	runner.addTest(mafPolylineGraphTest::suite());
// 	runner.addTest(mafVMEWrappedMeterTest::suite());
// 	runner.addTest(mafVMELabeledVolumeTest::suite());
	runner.addTest(mafVMEAnalogTest::suite());
	runner.addTest(mafVMEOutputSurfaceEditorTest::suite());
	runner.addTest(mafVMEOutputPolylineEditorTest::suite());
	runner.addTest(mafVMESurfaceEditorTest::suite());
	runner.addTest(mafVMEPolylineEditorTest::suite());
	runner.addTest(mafVMEMapsTest::suite());
	runner.addTest(mafVMEOutputWrappedMeterTest::suite());
	runner.addTest(mafVMEPolylineGraphTest::suite());
	runner.addTest(mafVMESegmentationVolumeTest::suite());
	runner.addTest(mafAttributeSegmentationVolumeTest::suite());
	runner.addTest(mafVMEOutputComputeWrappingTest::suite());
	runner.addTest(mafPolylineGraphVertexTest::suite());
	runner.addTest(mafPolylineGraphEdgeTest::suite());
	runner.addTest(mafPolylineGraphBranchTest::suite());
	runner.addTest(mafDataPipeCustomSegmentationVolumeTest::suite());
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

