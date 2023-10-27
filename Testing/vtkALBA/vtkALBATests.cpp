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
#include "vtkALBATests.h"

#include "vtkALBAContourVolumeMapperTest.h"
#include "vtkALBARemoveCellsFilterTest.h"
#include "vtkALBAPolyDataToSinglePolyLineTest.h"
#include "vtkALBATextOrientatorTest.h"
#include "vtkALBAGlobalAxisCoordinateTest.h"
#include "vtkALBALocalAxisCoordinateTest.h"
#include "vtkALBAVolumeResampleTest.h"
#include "vtkALBADOFMatrixTest.h"
#include "vtkALBAImplicitPolyDataTest.h"
#include "vtkALBATextActorMeterTest.h"
#include "vtkALBACellsFilterTest.h"
#include "vtkALBAGridActorTest.h"
#include "vtkALBAProjectVolumeTest.h"
#include "vtkALBAClipSurfaceBoundingBoxTest.h"
#include "vtkALBARectilinearGridToRectilinearGridFilterTest.h"
#include "vtkALBADummyRectilinearGridToRectilinearGridFilter.h"
#include "vtkALBAProfilingActorTest.h"
#include "vtkALBARulerActor2DTest.h"
#include "vtkALBASimpleRulerActor2DTest.h"
#include "vtkALBAFixedCutterTest.h"
#include "vtkALBAHistogramTest.h"
#include "vtkALBAExtendedGlyph3DTest.h"
#include "vtkALBATransferFunction2DTest.h"
#include "vtkHoleConnectivityTest.h"
#include "vtkTriangleQualityRatioTest.h"
#include "vtkMaskPolyDataFilterTest.h"
#include "vtkALBAExtrudeToCircleTest.h"
#include "vtkALBAPastValuesListTest.h"
#include "vtkALBAPolyDataMirrorTest.h"
#include "vtkALBARegionGrowingLocalGlobalThresholdTest.h"
#include "vtkImageUnPackerTest.h"
#include "vtkPackedImageTest.h"
#include "vtkALBAVolumeRayCastMapperTest.h"
#include "vtkXRayVolumeMapperTest.h"
#include "vtkALBAVolumeTextureMapper2DTest.h"
#include "vtkALBADataArrayDescriptorTest.h"
#include "vtkALBAErythrocyteSourceTest.h"
#include "vtkALBAMeshCutterTest.h"
#include "vtkALBAFillingHoleTest.h"
#include "vtkALBAFixTopologyTest.h"
#include "vtkWeightedLandmarkTransformTest.h"
#include "vtkALBALandmarkCloudOutlineCornerFilterTest.h"
#include "EdgeTest.h"
#include "TriangleTest.h"
#include "CoredPointIndexTest.h"
#include "EdgeIndexTest.h"
#include "CoredEdgeIndexTest.h"
#include "TriangleIndexTest.h"
#include "TriangulationEdgeTest.h"
#include "TriangulationTriangleTest.h"
#include "CMatrixTest.h"
#include "CMatrixTestM1.h"
#include "CMatrixTestM2.h"
#include "RingDataTest.h"
#include "MeshDataTest.h"
#include "vtkALBAExtrudeToCircleVertexDataTest.h"
#include "vtkALBAFillingHoleCTriangleTest.h"
#include "vtkALBAFillingHoleCEdgeTest.h"
#include "MemoryInfoTest.h"
#include "VectorTest.h"
#include "SparseMatrixTest.h"
#include "NVectorTest.h"
#include "TreeNodeDataTest.h"
#include "SortedTreeNodesTest.h"
#include "CubeTest.h"
#include "SquareTest.h"
#include "CSkeletonEdgeTest.h"
#include "VertexDataTest.h"
#include "TriangulationTest.h"
#include "AllocatorTest.h"
#include "BinaryNodeTest.h"
#include "PolynomialTest.h"
#include "StartingPolynomialTest.h"
#include "CSkeletonVertexTest.h"
#include "PPolynomialTest.h"
#include "CSkeletonEdgeM1Test.h"
#include "CSkeletonVertexM1Test.h"
#include "CSkeletonVertexM2Test.h"
#include "vtkALBACollisionDetectionFilterTest.h"
#include "vtkALBAImageFillHolesRemoveIslandsTest.h"
#include "vtkALBARayCastCleanerTest.h"
#include "vtkALBALineStripSourceTest.h"
#include "vtkALBATubeFilterTest.h"
#include "vtkALBARGtoSPImageFilterTest.h"
#include "vtkALBADistanceFilterTest.h"
#include "vtkALBACircleSourceTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"
#include "vtkALBASmartPointer.h"
#include "albaFakeLogicForTest.h"
#include "albaServiceLocator.h"

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

	runner.addTest(vtkALBAContourVolumeMapperTest::suite());
	runner.addTest(vtkALBARemoveCellsFilterTest::suite());
	runner.addTest(vtkALBAPolyDataToSinglePolyLineTest::suite());
	runner.addTest(vtkALBATextOrientatorTest::suite());
	runner.addTest(vtkALBAGlobalAxisCoordinateTest::suite());
	runner.addTest(vtkALBALocalAxisCoordinateTest::suite());
	runner.addTest(vtkALBAVolumeResampleTest::suite());
	runner.addTest(vtkALBADOFMatrixTest::suite());
	runner.addTest(vtkALBAImplicitPolyDataTest::suite());
	runner.addTest(vtkALBATextActorMeterTest::suite());
	runner.addTest(vtkALBACellsFilterTest::suite());
	runner.addTest(vtkALBAGridActorTest::suite());
	runner.addTest(vtkALBAProjectVolumeTest::suite());
	runner.addTest(vtkALBAClipSurfaceBoundingBoxTest::suite());
	//runner.addTest(vtkALBARectilinearGridToRectilinearGridFilterTest::suite());
	runner.addTest(vtkALBAProfilingActorTest::suite());
	runner.addTest(vtkALBARulerActor2DTest::suite());
	runner.addTest(vtkALBASimpleRulerActor2DTest::suite());
	runner.addTest(vtkALBAFixedCutterTest::suite());
	runner.addTest(vtkALBAHistogramTest::suite());
	runner.addTest(vtkALBAExtendedGlyph3DTest::suite());
	runner.addTest(vtkALBATransferFunction2DTest::suite());
	runner.addTest(vtkHoleConnectivityTest::suite());
	runner.addTest(vtkTriangleQualityRatioTest::suite());
	runner.addTest(vtkMaskPolyDataFilterTest::suite());
	runner.addTest(vtkALBAExtrudeToCircleTest::suite());
	runner.addTest(vtkALBAPastValuesListTest::suite());
	runner.addTest(vtkALBAPolyDataMirrorTest::suite());
	runner.addTest(vtkALBARegionGrowingLocalGlobalThresholdTest::suite());
	runner.addTest(vtkImageUnPackerTest::suite());
	runner.addTest(vtkPackedImageTest::suite());
	//runner.addTest(vtkALBAVolumeRayCastMapperTest::suite());
	runner.addTest(vtkXRayVolumeMapperTest::suite());
	runner.addTest(vtkALBAVolumeTextureMapper2DTest::suite());
	runner.addTest(vtkALBADataArrayDescriptorTest::suite());
	runner.addTest(vtkALBAErythrocyteSourceTest::suite());
	runner.addTest(vtkALBAMeshCutterTest::suite());
	runner.addTest(vtkALBAFillingHoleTest::suite());
	runner.addTest(vtkALBAFixTopologyTest::suite());
	runner.addTest(vtkWeightedLandmarkTransformTest::suite());
	runner.addTest(vtkALBALandmarkCloudOutlineCornerFilterTest::suite());
	runner.addTest(EdgeTest::suite());
	runner.addTest(TriangleTest::suite());
	runner.addTest(CoredPointIndexTest::suite());
	runner.addTest(EdgeIndexTest::suite());
	runner.addTest(CoredEdgeIndexTest::suite());
	runner.addTest(TriangleIndexTest::suite());
	runner.addTest(TriangulationEdgeTest::suite());
	runner.addTest(TriangulationTriangleTest::suite());
	runner.addTest(CMatrixTest::suite());
	runner.addTest(CMatrixTestM1::suite());
	runner.addTest(CMatrixTestM2::suite());
	runner.addTest(RingDataTest::suite());
	runner.addTest(MeshDataTest::suite());
	runner.addTest(vtkALBAExtrudeToCircleVertexDataTest::suite());
	runner.addTest(vtkALBAFillingHoleCTriangleTest::suite());
	runner.addTest(vtkALBAFillingHoleCEdgeTest::suite());
	runner.addTest(MemoryInfoTest::suite());
	runner.addTest(VectorTest::suite());
	runner.addTest(SparseMatrixTest::suite());
	runner.addTest(NVectorTest::suite());
	runner.addTest(TreeNodeDataTest::suite());
	runner.addTest(SortedTreeNodesTest::suite());
	runner.addTest(CubeTest::suite());
	runner.addTest(SquareTest::suite());
	runner.addTest(CSkeletonEdgeTest::suite());
	runner.addTest(VertexDataTest::suite());
	runner.addTest(TriangulationTest::suite());
	runner.addTest(AllocatorTest::suite());
	runner.addTest(BinaryNodeTest::suite());
	runner.addTest(PolynomialTest::suite());
	runner.addTest(StartingPolynomialTest::suite());
	runner.addTest(CSkeletonVertexTest::suite());
	runner.addTest(PPolynomialTest::suite());
	runner.addTest(CSkeletonEdgeM1Test::suite());
	runner.addTest(CSkeletonVertexM1Test::suite());
	runner.addTest(CSkeletonVertexM2Test::suite());
	runner.addTest(vtkALBACollisionDetectionFilterTest::suite());
	runner.addTest(vtkALBAImageFillHolesRemoveIslandsTest::suite());
	runner.addTest(vtkALBARayCastCleanerTest::suite());
	runner.addTest(vtkALBALineStripSourceTest::suite());
	runner.addTest(vtkALBATubeFilterTest::suite());
	runner.addTest(vtkALBARGtoSPImageFilterTest::suite());
	runner.addTest(vtkALBADistanceFilterTest::suite());
	runner.addTest(vtkALBACircleSourceTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 
 	return result.wasSuccessful() ? 0 : 1;
}