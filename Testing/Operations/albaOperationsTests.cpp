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

#include "albaOperationsTests.h"

#include "albaASCIIImporterUtilityTest.h"
#include "albaDicomClassesTest.h"
#include "albaFakeLogicForTest.h"
#include "albaGeometryEditorPolylineGraphTest.h"
#include "albaOpAddLandmarkTest.h"
#include "albaOpApplyTrajectoryTest.h"
#include "albaOpBooleanSurfaceTest.h"
#include "albaOpClassicICPRegistrationTest.h"
#include "albaOpCleanSurfaceTest.h"
#include "albaOpComputeInertialTensorTest.h"
#include "albaOpComputeWrappingTest.h"
#include "albaOpConnectivitySurfaceTest.h"
#include "albaOpCreateEditSkeletonTest.h"
#include "albaOpCreateGroupTest.h"
#include "albaOpCreateLabeledVolumeTest.h"
#include "albaOpCreateMeterTest.h"
#include "albaOpCreateProberTest.h"
#include "albaOpCreateRefSysTest.h"
#include "albaOpCreateSlicerTest.h"
#include "albaOpCreateSplineTest.h"
#include "albaOpCreateSurfaceParametricTest.h"
#include "albaOpCreateSurfaceTest.h"
#include "albaOpCreateVolumeTest.h"
#include "albaOpCreateWrappedMeterTest.h"
#include "albaOpCropDeformableROITest.h"
#include "albaOpCropTest.h"
#include "albaOpDecimateSurfaceTest.h"
#include "albaOpDecomposeTimeVarVMETest.h"
#include "albaOpEditMetadataTest.h"
#include "albaOpEditNormalsTest.h"
#include "albaOpEqualizeHistogramTest.h"
#include "albaOpExporterAbaqusFileTest.h"
#include "albaOpExporterAnalogWSTest.h"
#include "albaOpExporterAnsysCDBFileTest.h"
#include "albaOpExporterAnsysInputFileTest.h" 
#include "albaOpExporterBMPTest.h"
#include "albaOpExporterGRFWSTest.h"
#include "albaOpExporterLandmarkTest.h"
#include "albaOpExporterLandmarkWSTest.h"
#include "albaOpExporterMSFTest.h"
#include "albaOpExporterMeshTest.h"
#include "albaOpExporterMetersTest.h"
#include "albaOpExporterRAWTest.h"
#include "albaOpExporterSTLTest.h"
#include "albaOpExporterVTKTest.h"
#include "albaOpExporterWrappedMeterTest.h"
#include "albaOpExtractGeometryTest.h"
#include "albaOpExtractIsosurfaceTest.h"
#include "albaOpExtrusionHolesTest.h"
#include "albaOpFillHolesTest.h"
#include "albaOpFilterSurfaceTest.h"
#include "albaOpFilterVolumeTest.h"
#include "albaOpFlipNormalsTest.h"
#include "albaOpFreezeVMETest.h"
#include "albaOpGarbageCollectMSFDirTest.h"
#include "albaOpImporterAbaqusFileTest.h"
#include "albaOpImporterASCIITest.h"
#include "albaOpImporterAnalogWSTest.h"
#include "albaOpImporterAnsysCDBFileTest.h"
#include "albaOpImporterAnsysInputFileTest.h"
#include "albaOpImporterC3DTest.h"
#include "albaOpImporterDicomTest.h"
#include "albaOpImporterExternalFileTest.h"
#include "albaOpImporterGRFWSTest.h"
#include "albaOpImporterImageTest.h"
#include "albaOpImporterLandmarkTXTTest.h"
#include "albaOpImporterLandmarkTest.h"
#include "albaOpImporterLandmarkWSTest.h"
#include "albaOpImporterMSF1xTest.h"
#include "albaOpImporterMSFTest.h"
#include "albaOpImporterMeshTest.h"
#include "albaOpImporterMotionDataTest.h"
#include "albaOpImporterRAWImagesTest.h"
#include "albaOpImporterRAWVolumeTest.h"
#include "albaOpImporterSTLTest.h"
#include "albaOpImporterVMEDataSetAttributesTest.h"
#include "albaOpImporterVRMLTest.h"
#include "albaOpImporterVTKTest.h"
#include "albaOpImporterVTKXMLTest.h"
#include "albaOpInteractionDebuggerTest.h"
#include "albaOpInteractiveClipSurfaceTest.h"
#include "albaOpIterativeRegistrationTest.h"
#include "albaOpLabelExtractorTest.h"
#include "albaOpLabelizeSurfaceTest.h"
#include "albaOpTransformOldTest.h"
#include "albaOpMML3ParameterViewTest.h"
#include "albaOpMML3Test.h"
#include "albaOpMakeVMETimevaryingTest.h"
#include "albaOpMatrixVectorMathTest.h"
#include "albaOpMeshQualityTest.h"
#include "albaOpMoveTest.h"
#include "albaOpOpenExternalFileTest.h"
#include "albaOpRegisterClustersTest.h"
#include "albaOpRemoveCellsTest.h"
#include "albaOpReparentToTest.h"
#include "albaOpScalarToSurfaceTest.h"
#include "albaOpScaleDatasetTest.h"
#include "albaOpSegmentationRegionGrowingConnectedThresholdTest.h"
#include "albaOpSmoothSurfaceCellsTest.h"
#include "albaOpSmoothSurfaceTest.h"
#include "albaOpSplitSurfaceTest.h"
#include "albaOpSubdivideTest.h"
#include "albaOpSurfaceMirrorTest.h"
#include "albaOpVolumeMirrorTest.h"
#include "albaOpTransformInterfaceTest.h"
#include "albaOpTransformTest.h"
#include "albaOpTriangulateSurfaceTest.h"
#include "albaOpVOIDensityEditorTestTest.h"
#include "albaOpVOIDensityTest.h"
#include "albaOpValidateTreeTest.h"
#include "albaOpVolumeMeasureTest.h"
#include "albaServiceLocator.h"
#include "albaVMEDataSetAttributesImporterTest.h"
#include "albaVMEMeshAnsysTextExporterTest.h"
#include "albaVMEMeshAnsysTextImporterTest.h"
#include "albaOpImporterDICFileTest.h"
#include "albaOpImporterPointCloudTest.h"
#include "albaOpFilterImageTest.h"

#include "itkRawMotionImporterUtilityTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkFileOutputWindow.h"
#include "vtkALBASmartPointer.h"
#include "albaVMEOutputNULL.h"

// Visual Leak Detector
//#include <vld.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(DummyVme);

//-------------------------------------------------------------------------
albaVMEOutput *DummyVme::GetOutput()
{
	// allocate the right type of output on demand
	if (m_Output==NULL)
	{
		SetOutput(albaVMEOutputNULL::New()); // create the output
	}
	return m_Output;
}

//----------------------------------------------------------------------------
void DummyObserver::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetId() == VME_CHOOSE)
		{
			e->SetVme(m_DummyVme);
		}
	}
}

//----------------------------------------------------------------------------
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

	//runner.addTest(albaOpImporterMSF1xTest::suite()); 
	runner.addTest(albaOpImporterSTLTest::suite());
	runner.addTest(albaOpExporterSTLTest::suite());
	runner.addTest(albaOpCropTest::suite());
	runner.addTest(albaOpVOIDensityTest::suite());
	runner.addTest(albaOpImporterImageTest::suite()); 
	runner.addTest(albaOpImporterRAWVolumeTest::suite());
	runner.addTest(albaOpImporterVRMLTest::suite());
	runner.addTest(albaOpExporterRAWTest::suite());
	runner.addTest(albaOpCreateGroupTest::suite());
	runner.addTest(albaOpReparentToTest::suite());
	runner.addTest(albaOpDecimateSurfaceTest::suite());
	runner.addTest(albaOpConnectivitySurfaceTest::suite());
	runner.addTest(albaOpRemoveCellsTest::suite());
	runner.addTest(albaOpBooleanSurfaceTest::suite());
	runner.addTest(albaOpEditNormalsTest::suite()); 
	runner.addTest(albaOpExporterBMPTest::suite()); 
	runner.addTest(albaOpImporterMSFTest::suite());
	runner.addTest(albaOpImporterExternalFileTest::suite());
	runner.addTest(albaOpOpenExternalFileTest::suite());
	runner.addTest(albaOpDecomposeTimeVarVMETest::suite());
	runner.addTest(albaOpLabelExtractorTest::suite());
	runner.addTest(albaOpImporterVTKTest::suite());
	runner.addTest(albaOpCreateVolumeTest::suite());
	runner.addTest(albaOpVOIDensityEditorTestTest::suite());
	runner.addTest(albaOpAddLandmarkTest::suite());
	runner.addTest(albaOpApplyTrajectoryTest::suite());
	runner.addTest(albaOpExtractIsosurfaceTest::suite());
	runner.addTest(albaOpImporterASCIITest::suite());
 	runner.addTest(albaOpExporterMSFTest::suite());
	runner.addTest(albaOpFilterSurfaceTest::suite());
	runner.addTest(albaOpValidateTreeTest::suite());
	runner.addTest(albaOpGarbageCollectMSFDirTest::suite());
	runner.addTest(albaOpScalarToSurfaceTest::suite());
	runner.addTest(albaASCIIImporterUtilityTest::suite());
	runner.addTest(albaOpEditMetadataTest::suite());
	runner.addTest(albaOpFilterVolumeTest::suite());
	runner.addTest(albaOpTransformOldTest::suite());
 	runner.addTest(albaOpTransformInterfaceTest::suite());
	runner.addTest(albaVMEMeshAnsysTextImporterTest::suite());  
	runner.addTest(albaVMEMeshAnsysTextExporterTest::suite()); 
	runner.addTest(albaOpImporterMeshTest::suite()); 
	runner.addTest(albaOpExporterMeshTest::suite());  
	runner.addTest(albaVMEDataSetAttributesImporterTest::suite());
	runner.addTest(albaOpImporterVMEDataSetAttributesTest::suite());
	runner.addTest(albaOpCreateMeterTest::suite());
	runner.addTest(albaOpCreateProberTest::suite());
	runner.addTest(albaOpCreateRefSysTest::suite());
	runner.addTest(albaOpCreateSlicerTest::suite());
	runner.addTest(albaOpCreateSplineTest::suite());
	runner.addTest(albaOpCreateSurfaceParametricTest::suite());
	runner.addTest(albaOpExporterVTKTest::suite());
	runner.addTest(albaOpExporterLandmarkTest::suite());
	runner.addTest(albaOpImporterLandmarkTest::suite());
	runner.addTest(albaOpImporterGRFWSTest::suite());
	runner.addTest(albaOpImporterAnalogWSTest::suite());
	runner.addTest(albaGeometryEditorPolylineGraphTest::suite());
	runner.addTest(albaOpMatrixVectorMathTest::suite());
	runner.addTest(albaOpVolumeMeasureTest::suite());
	runner.addTest(albaOpImporterRAWImagesTest::suite());
	runner.addTest(albaOpImporterLandmarkTXTTest::suite());
	runner.addTest(albaOpImporterLandmarkWSTest::suite());
	runner.addTest(albaOpExtrusionHolesTest::suite());
	runner.addTest(albaOpExporterWrappedMeterTest::suite());
	runner.addTest(albaOpFreezeVMETest::suite());
	runner.addTest(albaOpLabelizeSurfaceTest::suite());
	runner.addTest(albaOpCleanSurfaceTest::suite());
	runner.addTest(albaOpSmoothSurfaceTest::suite());
	runner.addTest(albaOpTriangulateSurfaceTest::suite());
	runner.addTest(albaOpSurfaceMirrorTest::suite());
	runner.addTest(albaOpVolumeMirrorTest::suite());
	runner.addTest(albaOpExporterMetersTest::suite());
	runner.addTest(albaOpSubdivideTest::suite());
	runner.addTest(albaOpCreateSurfaceTest::suite());
	runner.addTest(albaOpSegmentationRegionGrowingConnectedThresholdTest::suite());
	runner.addTest(albaOpScaleDatasetTest::suite());
	runner.addTest(albaOpCreateLabeledVolumeTest::suite());
	runner.addTest(albaOpComputeWrappingTest::suite());
	runner.addTest(albaOpComputeInertialTensorTest::suite());
	runner.addTest(albaOpSplitSurfaceTest::suite());
	runner.addTest(albaOpMML3Test::suite());
	runner.addTest(albaOpExtractGeometryTest::suite());
	runner.addTest(albaOpImporterDicomTest::suite());
	runner.addTest(albaItkRawMotionImporterUtilityTest::suite());
	runner.addTest(albaOpMakeVMETimevaryingTest::suite());
	runner.addTest(albaOpEqualizeHistogramTest::suite());
	runner.addTest(albaOpCreateWrappedMeterTest::suite());
	runner.addTest(albaOpFlipNormalsTest::suite());
	runner.addTest(albaOpSmoothSurfaceCellsTest::suite());
	runner.addTest(albaOpMeshQualityTest::suite());
	runner.addTest(albaOpCropDeformableROITest::suite());
	runner.addTest(albaOpMoveTest::suite());
	runner.addTest(albaOpTransformTest::suite());
	runner.addTest(albaOpExporterAnalogWSTest::suite());
	runner.addTest(albaOpExporterGRFWSTest::suite());
	runner.addTest(albaOpExporterLandmarkWSTest::suite()); 
	runner.addTest(albaDicomClassesTest::suite()); 
	runner.addTest(albaOpImporterC3DTest::suite());
	runner.addTest(albaOpImporterMotionDataTest::suite());
	runner.addTest(albaOpFillHolesTest::suite());
	runner.addTest(albaOpImporterVTKXMLTest::suite());
	runner.addTest(albaOpInteractiveClipSurfaceTest::suite());
	runner.addTest(albaOpCreateEditSkeletonTest::suite());
	runner.addTest(albaOpRegisterClustersTest::suite());
	runner.addTest(albaOpClassicICPRegistrationTest::suite());
	runner.addTest(albaOpMML3ParameterViewTest::suite());
	runner.addTest(albaOpInteractionDebuggerTest::suite());
	runner.addTest(albaOpIterativeRegistrationTest::suite()); 
	runner.addTest(albaOpImporterAnsysCDBFileTest::suite()); 
 	runner.addTest(albaOpImporterAnsysInputFileTest::suite());
 	runner.addTest(albaOpExporterAnsysCDBFileTest::suite()); 
 	runner.addTest(albaOpExporterAnsysInputFileTest::suite()); 
	runner.addTest(albaOpImporterAbaqusFileTest::suite());
	runner.addTest(albaOpExporterAbaqusFileTest::suite());
	runner.addTest(albaOpImporterDICFileTest::suite());
	runner.addTest(albaOpImporterPointCloudTest::suite());
	runner.addTest(albaOpFilterImageTest::suite()); /* */

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	cppDEL(logic);

	albaTest::PauseBeforeExit();

	return result.wasSuccessful() ? 0 : 1;
}