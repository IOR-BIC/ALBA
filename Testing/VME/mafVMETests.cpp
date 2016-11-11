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

#include "mafVMETests.h"
#include "mafVMEExternalDataTest.h"
#include "mafVMEVolumeGrayTest.h"
#include "mafVMESurfaceParametricTest.h"
#include "mafVMESurfaceTest.h"
#include "mafVMEGroupTest.h"
#include "mafVMEGizmoTest.h"
#include "mafVMEScalarMatrixTest.h"
#include "mafVMEScalarTest.h"
#include "mafVMEPolylineTest.h"
#include "mafVMEPolylineSplineTest.h"
#include "mafVMEMeshTest.h"
#include "mafPipeMeshTest.h"
#include "mafVMELandmarkTest.h"
#include "mafVMELandmarkCloudTest.h"
#include "mafMatrixVectorTest.h"
#include "mafVMEFactoryTest.h"
#include "mafDataVectorTest.h"
#include "mafVMEItemVTKTest.h"
#include "mafPipeSurfaceTest.h"
#include "mafPipeGizmoTest.h"
#include "mafPipeSurfaceTexturedTest.h"
#include "mafPipeScalarTest.h"
#include "mafPipeImage3DTest.h"
#include "mafPipeIsosurfaceTest.h"
#include "mafPipePointSetTest.h"
#include "mafVMEImageTest.h"
#include "mafVMEVolumeRGBTest.h"
#include "mafVMEInfoTextTest.h"
#include "mmaMeterTest.h"
#include "mafVMEOutputMeterTest.h"
#include "mafVMEPointSetTest.h"
#include "mafVMEVectorTest.h"
#include "mafPipeFactoryVMETest.h"
#include "mafVMEOutputPolylineTest.h"
#include "mafVMEOutputSurfaceTest.h"
#include "mafVMEOutputPointSetTest.h"
#include "mafVMEOutputScalarTest.h"
#include "mafVMEOutputLandmarkCloudTest.h"
#include "mafVMEOutputScalarMatrixTest.h"
#include "mafVMEOutputMeshTest.h"
#include "mafPipeIsosurfaceGPUTest.h"
#include "mafVMEOutputImageTest.h"
#include "mafVMEOutputVolumeTest.h"
#include "mafCryptTest.h"
#include "mafPipePolylineTest.h"
#include "mafMatrixInterpolatorTest.h"
#include "mafVMEGenericTest.h"
#include "mafPipeLandmarkCloudTest.h"
#include "mafVMEOutputVTKTest.h"
#include "mafVMEItemScalarMatrixTest.h"
#include "mafScalarVectorTest.h"
#include "mafPipeVectorTest.h"
#include "mafVMEGenericAbstractTest.h"
#include "mafPipeMeterTest.h"
#include "mafPipeVolumeProjectedTest.h"
#include "mafVMEItemTest.h"
#include "mafVMERefSysTest.h"
#include "mafVMEProberTest.h"
#include "mafVMEMeterTest.h"
#include "mafMSFImporterTest.h"
#include "mafPipeScalarMatrixTest.h"
#include "mafVisualPipeVolumeRayCastingTest.h"
#include "mafVMESlicerTest.h"
#include "mafVMERawMotionDataTest.h"
#include "mafPolylineGraphTest.h"
#include "mafVMEWrappedMeterTest.h"
#include "mafVMELabeledVolumeTest.h"
#include "mafPipeVolumeMIPTest.h"
#include "mafPipeVolumeDRRTest.h"
#include "mafVMEAnalogTest.h"
#include "mafVMEOutputSurfaceEditorTest.h"
#include "mafVMEOutputPolylineEditorTest.h"
#include "mafVMESurfaceEditorTest.h"
#include "mafPipePolylineGraphEditorTest.h"
#include "mafVMEPolylineEditorTest.h"
#include "mafPipeMeshSliceTest.h"
#include "mafPipeSurfaceSliceTest.h"
#include "mafPipeVolumeSliceTest.h"
#include "mafVMEMapsTest.h"
#include "mafPipeTrajectoriesTest.h"
#include "mafPipeSliceTest.h"
#include "mafPipePolylineSliceTest.h"
#include "mafPipeWrappedMeterTest.h"
#include "mafVMEOutputWrappedMeterTest.h"
#include "mafPipeDensityDistanceTest.h"
#include "mafVMEPolylineGraphTest.h"
#include "mafPipeGraphTest.h"
#include "mafVMESegmentationVolumeTest.h"
#include "mafPipeCompoundTest.h"
#include "mafPipeCompoundVolumeTest.h"
#include "mafPipeCompoundVolumefixedScalarsTest.h"
#include "mafVisualPipePolylineGraphTest.h"
#include "mafPipeSurfaceEditorTest.h"
#include "mafPipeTensorFieldSurfaceTest.h"
#include "mafPipeVectorFieldSurfaceTest.h"
#include "mafPipeVectorFieldGlyphsTest.h"
#include "mafPipeVectorFieldMapWithArrowsTest.h"
#include "mafAttributeSegmentationVolumeTest.h"
#include "mafPipeTensorFieldSliceTest.h"
#include "mafPipeVectorFieldSliceTest.h"
#include "mafPipeTensorFieldGlyphsTest.h"
#include "mafVMEOutputComputeWrappingTest.h"
#include "mafPipeTensorFieldTest.h"
#include "mafPipeVectorFieldTest.h"
#include "mafVisualPipeSlicerSliceTest.h"
#include "mafPipeCompoundVolumeVRTest.h"
#include "mafPipeCompoundVolumeMIPTest.h"
#include "mafPipeCompoundVolumeDRRTest.h"
#include "mafPipeCompoundVolumeIsosurfaceTest.h"
#include "mafPolylineGraphVertexTest.h"
#include "mafPolylineGraphEdgeTest.h"
#include "mafPolylineGraphBranchTest.h"
#include "mafDataPipeCustomSegmentationVolumeTest.h"
#include "mafPipeRayCastTest.h"
#include "VMEGenericPoseTest.h"
#include "VMEGenericBoundsTest.h"
#include "CustomDataPipeTest.h"
#include "VMEGenericTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
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

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
	runner.addTest(mafVMEExternalDataTest::suite());
	runner.addTest(mafVMEVolumeGrayTest::suite());
	runner.addTest(mafVMESurfaceParametricTest::suite());
	runner.addTest(mafVMESurfaceTest::suite());
	runner.addTest(mafVMEGroupTest::suite());
	runner.addTest(mafVMEGizmoTest::suite());
	runner.addTest(mafVMEScalarMatrixTest::suite());
	runner.addTest(mafVMEScalarTest::suite());
	runner.addTest(mafVMEPolylineTest::suite());
	runner.addTest(mafVMEPolylineSplineTest::suite());
	runner.addTest(mafVMEMeshTest::suite());
	runner.addTest(mafPipeMeshTest::suite());
	runner.addTest(mafVMELandmarkTest::suite());
	runner.addTest(mafVMELandmarkCloudTest::suite());
	runner.addTest(mafMatrixVectorTest::suite());
	runner.addTest(mafVMEFactoryTest::suite());
	runner.addTest(mafDataVectorTest::suite());
	runner.addTest(mafVMEItemVTKTest::suite());
	runner.addTest(mafPipeSurfaceTest::suite());
	runner.addTest(mafPipeGizmoTest::suite());
	runner.addTest(mafPipeSurfaceTexturedTest::suite());
	runner.addTest(mafPipeScalarTest::suite());
	runner.addTest(mafPipeImage3DTest::suite());
	runner.addTest(mafPipeIsosurfaceTest::suite());
	runner.addTest(mafPipePointSetTest::suite());
	runner.addTest(mafVMEImageTest::suite());
	runner.addTest(mafVMEVolumeRGBTest::suite());
	runner.addTest(mafVMEInfoTextTest::suite());
	runner.addTest(mmaMeterTest::suite());
	runner.addTest(mafVMEOutputMeterTest::suite());
	runner.addTest(mafVMEPointSetTest::suite());
	runner.addTest(mafVMEVectorTest::suite());
	runner.addTest(mafPipeFactoryVMETest::suite());
	runner.addTest(mafVMEOutputPolylineTest::suite());
	runner.addTest(mafVMEOutputSurfaceTest::suite());
	runner.addTest(mafVMEOutputPointSetTest::suite());
	runner.addTest(mafVMEOutputScalarTest::suite());
	runner.addTest(mafVMEOutputLandmarkCloudTest::suite());
	runner.addTest(mafVMEOutputScalarMatrixTest::suite());
	runner.addTest(mafVMEOutputMeshTest::suite());
	runner.addTest(mafPipeIsosurfaceGPUTest::suite());
	runner.addTest(mafVMEOutputImageTest::suite());
	runner.addTest(mafVMEOutputVolumeTest::suite());
	runner.addTest(mafCryptTest::suite());
	runner.addTest(mafPipePolylineTest::suite());
	runner.addTest(mafMatrixInterpolatorTest::suite());
	runner.addTest(mafVMEGenericTest::suite());
	runner.addTest(mafPipeLandmarkCloudTest::suite());
	runner.addTest(mafVMEOutputVTKTest::suite());
	runner.addTest(mafVMEItemScalarMatrixTest::suite());
	runner.addTest(mafScalarVectorTest::suite());
	runner.addTest(mafPipeVectorTest::suite());
	runner.addTest(mafVMEGenericAbstractTest::suite());
	runner.addTest(mafPipeMeterTest::suite());
	runner.addTest(mafPipeVolumeProjectedTest::suite());
	runner.addTest(mafVMEItemTest::suite());
	runner.addTest(mafVMERefSysTest::suite());
	runner.addTest(mafVMEProberTest::suite());
	runner.addTest(mafVMEMeterTest::suite());
	runner.addTest(mafMSFImporterTest::suite());
	runner.addTest(mafPipeScalarMatrixTest::suite());
	runner.addTest(mafVisualPipeVolumeRayCastingTest::suite());
	runner.addTest(mafVMESlicerTest::suite());
	runner.addTest(mafVMERawMotionDataTest::suite());
	runner.addTest(mafPolylineGraphTest::suite());
	runner.addTest(mafVMEWrappedMeterTest::suite());
	runner.addTest(mafVMELabeledVolumeTest::suite());
	runner.addTest(mafPipeVolumeMIPTest::suite());
	runner.addTest(mafPipeVolumeDRRTest::suite());
	runner.addTest(mafVMEAnalogTest::suite());
	runner.addTest(mafVMEOutputSurfaceEditorTest::suite());
	runner.addTest(mafVMEOutputPolylineEditorTest::suite());
	runner.addTest(mafVMESurfaceEditorTest::suite());
	runner.addTest(mafPipePolylineGraphEditorTest::suite());
	runner.addTest(mafVMEPolylineEditorTest::suite());
	runner.addTest(mafPipeMeshSliceTest::suite());
	runner.addTest(mafPipeSurfaceSliceTest::suite());
	runner.addTest(mafPipeVolumeSliceTest::suite());
	runner.addTest(mafVMEMapsTest::suite());
	runner.addTest(mafPipeTrajectoriesTest::suite());
	runner.addTest(mafPipeSliceTest::suite());
	runner.addTest(mafPipePolylineSliceTest::suite());
	runner.addTest(mafPipeWrappedMeterTest::suite());
	runner.addTest(mafVMEOutputWrappedMeterTest::suite());
	runner.addTest(mafPipeDensityDistanceTest::suite());
	runner.addTest(mafVMEPolylineGraphTest::suite());
	runner.addTest(mafPipeGraphTest::suite());
	runner.addTest(mafVMESegmentationVolumeTest::suite());
	runner.addTest(mafPipeCompoundTest::suite());
	runner.addTest(mafPipeCompoundVolumeTest::suite());
	runner.addTest(mafPipeCompoundVolumeFixedScalarsTest::suite());
	runner.addTest(mafVisualPipePolylineGraphTest::suite());
	runner.addTest(mafPipeSurfaceEditorTest::suite());
	runner.addTest(mafPipeTensorFieldSurfaceTest::suite());
	runner.addTest(mafPipeVectorFieldSurfaceTest::suite());
	runner.addTest(mafPipeVectorFieldGlyphsTest::suite());
	runner.addTest(mafPipeVectorFieldMapWithArrowsTest::suite());
	runner.addTest(mafAttributeSegmentationVolumeTest::suite());
	runner.addTest(mafPipeTensorFieldSliceTest::suite());
	runner.addTest(mafPipeVectorFieldSliceTest::suite());
	runner.addTest(mafPipeTensorFieldGlyphsTest::suite());
	runner.addTest(mafVMEOutputComputeWrappingTest::suite());
	runner.addTest(mafPipeTensorFieldTest::suite());
	runner.addTest(mafPipeVectorFieldTest::suite());
	runner.addTest(mafVisualPipeSlicerSliceTest::suite());
	runner.addTest(mafPipeCompoundVolumeVRTest::suite());
	runner.addTest(mafPipeCompoundVolumeMIPTest::suite());
	runner.addTest(mafPipeCompoundVolumeDRRTest::suite());
	runner.addTest(mafPipeCompoundVolumeIsosurfaceTest::suite());
	runner.addTest(mafPolylineGraphVertexTest::suite());
	runner.addTest(mafPolylineGraphEdgeTest::suite());
	runner.addTest(mafPolylineGraphBranchTest::suite());
	runner.addTest(mafDataPipeCustomSegmentationVolumeTest::suite());
	runner.addTest(mafPipeRayCastTest::suite());
	runner.addTest(VMEGenericPoseTest::suite());
	runner.addTest(VMEGenericBoundsTest::suite());
	runner.addTest(CustomDataPipeTest::suite());
	runner.addTest(VMEGenericTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

