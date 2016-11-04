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

#include "mafOperationsTests.h"
#include "mafOpImporterMSF1xTest.h"
#include "mafOpImporterSTLTest.h"
#include "mafOpExporterSTLTest.h"
#include "mafOpCropTest.h"
#include "mafOpVOIDensityTest.h"
#include "mafOpImporterImageTest.h"
#include "mafOpImporterRAWVolumeTest.h"
#include "mafOpImporterVRMLTest.h"
#include "mafOpExporterRAWTest.h"
#include "mafOpCreateGroupTest.h"
#include "mafOpReparentToTest.h"
#include "mafOpVolumeResampleTest.h"
#include "mafOpDecimateSurfaceTest.h"
#include "mafOpConnectivitySurfaceTest.h"
#include "mafOpRemoveCellsTest.h"
#include "mafOpBooleanSurfaceTest.h"
#include "mafOpEditNormalsTest.h"
#include "mafOpExporterBMPTest.h"
#include "mafOpImporterMSFTest.h"
#include "mafOpImporterExternalFileTest.h"
#include "mafOpOpenExternalFileTest.h"
#include "mafOpDecomposeTimeVarVMETest.h"
#include "mafOpLabelExtractorTest.h"
#include "mafOpImporterVTKTest.h"
#include "mafOpCreateVolumeTest.h"
#include "mafOpVOIDensityEditorTestTest.h"
#include "mafOpAddLandmarkTest.h"
#include "mafOpApplyTrajectoryTest.h"
#include "mafOpExtractIsosurfaceTest.h"
#include "mafOpImporterASCIITest.h"
#include "mafOpExporterMSFTest.h"
#include "mafOpFilterSurfaceTest.h"
#include "mafOpValidateTreeTest.h"
#include "mafOpGarbageCollectMSFDirTest.h"
#include "mafOpScalarToSurfaceTest.h"
#include "mafASCIIImporterUtilityTest.h"
#include "mafOpEditMetadataTest.h"
#include "mafOpFilterVolumeTest.h"
#include "mafOpMAFTransformTest.h"
#include "mafOpTransformInterfaceTest.h"
#include "mafVMEMeshAnsysTextImporterTest.h"
#include "mafVMEMeshAnsysTextExporterTest.h"
#include "mafOpImporterMeshTest.h"
#include "mafOpExporterMeshTest.h"
#include "mafVMEDataSetAttributesImporterTest.h"
#include "mafOpImporterVMEDataSetAttributesTest.h"
#include "mafOpCreateMeterTest.h"
#include "mafOpCreateProberTest.h"
#include "mafOpCreateRefSysTest.h"
#include "mafOpCreateSlicerTest.h"
#include "mafOpCreateSplineTest.h"
#include "mafOpCreateSurfaceParametricTest.h"
#include "mafOpExporterVTKTest.h"
#include "mafOpExporterLandmarkTest.h"
#include "mafOpImporterLandmarkTest.h"
#include "mafOpImporterGRFWSTest.h"
#include "mafOpImporterAnalogWSTest.h"
#include "mafGeometryEditorPolylineGraphTest.h"
#include "mafOpMatrixVectorMathTest.h"
#include "mafOpVolumeMeasureTest.h"
#include "mafOpImporterRAWImagesTest.h"
#include "mafOpImporterLandmarkTXTTest.h"
#include "mafOpImporterLandmarkWSTest.h"
#include "mafOpExtrusionHolesTest.h"
#include "mafOpExporterWrappedMeterTest.h"
#include "mafOpFreezeVMETest.h"
#include "mafOpLabelizeSurfaceTest.h"
#include "mafOpCleanSurfaceTest.h"
#include "mafOpSmoothSurfaceTest.h"
#include "mafOpTriangulateSurfaceTest.h"
#include "mafOpSurfaceMirrorTest.h"
#include "medOpImporterVTKTest.h"
#include "mafOpExporterMetersTest.h"
#include "mafOpSubdivideTest.h"
#include "mafOpCreateSurfaceTest.h"
#include "mafOpSegmentationRegionGrowingConnectedThresholdTest.h"
#include "mafOpScaleDatasetTest.h"
#include "mafOpCreateLabeledVolumeTest.h"
#include "mafOpComputeWrappingTest.h"
#include "mafOpComputeInertialTensorTest.h"
#include "mafOpSplitSurfaceTest.h"
#include "mafOpMML3Test.h"
#include "mafOpExtractGeometryTest.h"
#include "mafOpImporterDicomOffisTest.h"
#include "itkRawMotionImporterUtilityTest.h"
#include "mafOpMakeVMETimevaryingTest.h"
#include "mafOpEqualizeHistogramTest.h"
#include "mafOpCreateWrappedMeterTest.h"
#include "mafOpFlipNormalsTest.h"
#include "mafOpSmoothSurfaceCellsTest.h"
#include "mafOpMeshQualityTest.h"
#include "mafOpCropDeformableROITest.h"
#include "mafOpMoveTest.h"
#include "mafOpExporterAnalogWSTest.h"
#include "mafOpExporterGRFWSTest.h"
#include "mafOpExporterLandmarkWSTest.h"
#include "mafDicomClassesTest.h"
#include "mafOpImporterC3DTest.h"
#include "mafOpImporterMotionDataTest.h"
#include "mafOpFillHolesTest.h"
#include "mafOpImporterVTKXMLTest.h"
#include "mafOpInteractiveClipSurfaceTest.h"
#include "mafOpCreateEditSkeletonTest.h"
#include "mafOpRegisterClustersTest.h"
#include "mafOpClassicICPRegistrationTest.h"
#include "mafOpMML3ParameterViewTest.h"
#include "mafOpInteractionDebuggerTest.h"
#include "mafOpIterativeRegistrationTest.h"
#include "mafOpTransformTest.h"
#include "mafOpImporterAnsysCDBFileTest.h"
#include "mafOpImporterAnsysInputFileTest.h"
#include "mafOpExporterAnsysCDBFileTest.h"
#include "mafOpExporterAnsysInputFileTest.h" 

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"
#include "vtkMAFSmartPointer.h"

// Visual Leak Detector
//#include <vld.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(DummyVme);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutput *DummyVme::GetOutput()
//-------------------------------------------------------------------------
{
	// allocate the right type of output on demand
	if (m_Output==NULL)
	{
		SetOutput(mafVMEOutputNULL::New()); // create the output
	}
	return m_Output;
}


//----------------------------------------------------------------------------
void DummyObserver::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		if(e->GetId() == VME_REMOVE)
		{
			e->GetVme()->ReparentTo(NULL);
		}
		else if (e->GetId() == VME_CHOOSE)
		{
			e->SetVme(m_DummyVme);
		}
	}
}


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

	//runner.addTest(mafOpImporterMSF1xTest::suite());
	runner.addTest(mafOpImporterSTLTest::suite());
	runner.addTest(mafOpExporterSTLTest::suite());
	runner.addTest(mafOpCropTest::suite());
	runner.addTest(mafOpVOIDensityTest::suite());
	runner.addTest(mafOpImporterImageTest::suite());
	runner.addTest(mafOpImporterRAWVolumeTest::suite());
	runner.addTest(mafOpImporterVRMLTest::suite());
	runner.addTest(mafOpExporterRAWTest::suite());
	runner.addTest(mafOpCreateGroupTest::suite());
	runner.addTest(mafOpReparentToTest::suite());
	runner.addTest(mafOpVolumeResampleTest::suite());
	runner.addTest(mafOpDecimateSurfaceTest::suite());
	runner.addTest(mafOpConnectivitySurfaceTest::suite());
	runner.addTest(mafOpRemoveCellsTest::suite());
	runner.addTest(mafOpBooleanSurfaceTest::suite());
	runner.addTest(mafOpEditNormalsTest::suite());
	runner.addTest(mafOpExporterBMPTest::suite());
	runner.addTest(mafOpImporterMSFTest::suite());
	runner.addTest(mafOpImporterExternalFileTest::suite());
	runner.addTest(mafOpOpenExternalFileTest::suite());
	runner.addTest(mafOpDecomposeTimeVarVMETest::suite());
	runner.addTest(mafOpLabelExtractorTest::suite());
	runner.addTest(mafOpImporterVTKTest::suite());
	runner.addTest(mafOpCreateVolumeTest::suite());
	runner.addTest(mafOpVOIDensityEditorTestTest::suite());
	runner.addTest(mafOpAddLandmarkTest::suite());
	runner.addTest(mafOpApplyTrajectoryTest::suite());
	runner.addTest(mafOpExtractIsosurfaceTest::suite());
	runner.addTest(mafOpImporterASCIITest::suite());
	runner.addTest(mafOpExporterMSFTest::suite());
	runner.addTest(mafOpFilterSurfaceTest::suite());
	runner.addTest(mafOpValidateTreeTest::suite());
	runner.addTest(mafOpGarbageCollectMSFDirTest::suite());
	runner.addTest(mafOpScalarToSurfaceTest::suite());
	runner.addTest(mafASCIIImporterUtilityTest::suite());
	runner.addTest(mafOpEditMetadataTest::suite());
	runner.addTest(mafOpFilterVolumeTest::suite());
	runner.addTest(mafOpMAFTransformTest::suite());
	runner.addTest(mafOpTransformInterfaceTest::suite());
	runner.addTest(mafVMEMeshAnsysTextImporterTest::suite());
	runner.addTest(mafVMEMeshAnsysTextExporterTest::suite());
	runner.addTest(mafOpImporterMeshTest::suite());
	runner.addTest(mafOpExporterMeshTest::suite());
	runner.addTest(mafVMEDataSetAttributesImporterTest::suite());
	runner.addTest(mafOpImporterVMEDataSetAttributesTest::suite());
	runner.addTest(mafOpCreateMeterTest::suite());
	runner.addTest(mafOpCreateProberTest::suite());
	runner.addTest(mafOpCreateRefSysTest::suite());
	runner.addTest(mafOpCreateSlicerTest::suite());
	runner.addTest(mafOpCreateSplineTest::suite());
	runner.addTest(mafOpCreateSurfaceParametricTest::suite());
	runner.addTest(mafOpExporterVTKTest::suite());
	runner.addTest(mafOpExporterLandmarkTest::suite());
	runner.addTest(mafOpImporterLandmarkTest::suite());
	runner.addTest(mafOpImporterGRFWSTest::suite());
	runner.addTest(mafOpImporterAnalogWSTest::suite());
	runner.addTest(mafGeometryEditorPolylineGraphTest::suite());
	runner.addTest(mafOpMatrixVectorMathTest::suite());
	runner.addTest(mafOpVolumeMeasureTest::suite());
	runner.addTest(mafOpImporterRAWImagesTest::suite());
	runner.addTest(mafOpImporterLandmarkTXTTest::suite());
	runner.addTest(mafOpImporterLandmarkWSTest::suite());
	runner.addTest(mafOpExtrusionHolesTest::suite());
	runner.addTest(mafOpExporterWrappedMeterTest::suite());
	runner.addTest(mafOpFreezeVMETest::suite());
	runner.addTest(mafOpLabelizeSurfaceTest::suite());
	runner.addTest(mafOpCleanSurfaceTest::suite());
	runner.addTest(mafOpSmoothSurfaceTest::suite());
	runner.addTest(mafOpTriangulateSurfaceTest::suite());
	runner.addTest(mafOpSurfaceMirrorTest::suite());
	runner.addTest(medOpImporterVTKTest::suite());
	runner.addTest(mafOpExporterMetersTest::suite());
	runner.addTest(mafOpSubdivideTest::suite());
	runner.addTest(mafOpCreateSurfaceTest::suite());
	runner.addTest(mafOpSegmentationRegionGrowingConnectedThresholdTest::suite());
	runner.addTest(mafOpScaleDatasetTest::suite());
	runner.addTest(mafOpCreateLabeledVolumeTest::suite());
	runner.addTest(mafOpComputeWrappingTest::suite());
	runner.addTest(mafOpComputeInertialTensorTest::suite());
	runner.addTest(mafOpSplitSurfaceTest::suite());
	runner.addTest(mafOpMML3Test::suite());
	runner.addTest(mafOpExtractGeometryTest::suite());
	runner.addTest(mafOpImporterDicomOffisTest::suite());
	runner.addTest(mafItkRawMotionImporterUtilityTest::suite());
	runner.addTest(mafOpMakeVMETimevaryingTest::suite());
	runner.addTest(mafOpEqualizeHistogramTest::suite());
	runner.addTest(mafOpCreateWrappedMeterTest::suite());
	runner.addTest(mafOpFlipNormalsTest::suite());
	runner.addTest(mafOpSmoothSurfaceCellsTest::suite());
	runner.addTest(mafOpMeshQualityTest::suite());
	runner.addTest(mafOpCropDeformableROITest::suite());
	runner.addTest(mafOpMoveTest::suite());
	runner.addTest(mafOpTransformTest::suite());
	runner.addTest(mafOpExporterAnalogWSTest::suite());
	runner.addTest(mafOpExporterGRFWSTest::suite());
	runner.addTest(mafOpExporterLandmarkWSTest::suite());
	runner.addTest(mafDicomClassesTest::suite());
	runner.addTest(mafOpImporterC3DTest::suite());
	runner.addTest(mafOpImporterMotionDataTest::suite());
	runner.addTest(mafOpFillHolesTest::suite());
	runner.addTest(mafOpImporterVTKXMLTest::suite());
	runner.addTest(mafOpInteractiveClipSurfaceTest::suite());
	runner.addTest(mafOpCreateEditSkeletonTest::suite());
	runner.addTest(mafOpRegisterClustersTest::suite());
	runner.addTest(mafOpClassicICPRegistrationTest::suite());
	runner.addTest(mafOpMML3ParameterViewTest::suite());
	runner.addTest(mafOpInteractionDebuggerTest::suite());
	runner.addTest(mafOpIterativeRegistrationTest::suite());
	runner.addTest(mafOpImporterAnsysCDBFileTest::suite());
 	runner.addTest(mafOpImporterAnsysInputFileTest::suite());
 	runner.addTest(mafOpExporterAnsysCDBFileTest::suite());
 	runner.addTest(mafOpExporterAnsysInputFileTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

