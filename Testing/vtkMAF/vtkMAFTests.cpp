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
#include "vtkMAFTests.h"

#include "vtkMAFContourVolumeMapperTest.h"
#include "vtkMAFRemoveCellsFilterTest.h"
#include "vtkMAFPolyDataToSinglePolyLineTest.h"
#include "vtkMAFTextOrientatorTest.h"
#include "vtkMAFGlobalAxisCoordinateTest.h"
#include "vtkMAFLocalAxisCoordinateTest.h"
#include "vtkMAFVolumeResampleTest.h"
#include "vtkMAFDOFMatrixTest.h"
#include "vtkMAFImplicitPolyDataTest.h"
#include "vtkMAFTextActorMeterTest.h"
#include "vtkMAFCellsFilterTest.h"
#include "vtkMAFGridActorTest.h"
#include "vtkMAFProjectVolumeTest.h"
#include "vtkMAFClipSurfaceBoundingBoxTest.h"
#include "vtkMAFRGSliceAccumulateTest.h"
#include "vtkMAFRectilinearGridToRectilinearGridFilterTest.h"
#include "vtkMAFDummyRectilinearGridToRectilinearGridFilter.h"
#include "vtkMAFProfilingActorTest.h"
#include "vtkMAFRulerActor2DTest.h"
#include "vtkMAFSimpleRulerActor2DTest.h"
#include "vtkMAFFixedCutterTest.h"
#include "vtkMAFHistogramTest.h"
#include "vtkMAFExtendedGlyph3DTest.h"
#include "vtkMAFTransferFunction2DTest.h"
#include "vtkHoleConnectivityTest.h"
#include "vtkDicomUnPackerTest.h"
#include "vtkTriangleQualityRatioTest.h"
#include "vtkMaskPolyDataFilterTest.h"
#include "vtkMAFExtrudeToCircleTest.h"
#include "vtkMAFPastValuesListTest.h"
#include "vtkMAFPolyDataMirrorTest.h"
#include "vtkMAFRegionGrowingLocalGlobalThresholdTest.h"
#include "vtkImageUnPackerTest.h"
#include "vtkPackedImageTest.h"
#include "vtkMAFVolumeRayCastMapperTest.h"
#include "vtkXRayVolumeMapperTest.h"
#include "vtkMAFVolumeTextureMapper2DTest.h"
#include "vtkMAFDataArrayDescriptorTest.h"
#include "vtkMAFErythrocyteSourceTest.h"
#include "vtkMAFMeshCutterTest.h"
#include "vtkMAFFillingHoleTest.h"
#include "vtkMAFFixTopologyTest.h"
#include "vtkWeightedLandmarkTransformTest.h"
#include "vtkMAFLandmarkCloudOutlineCornerFilterTest.h"
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
#include "vtkMAFExtrudeToCircleVertexDataTest.h"
#include "vtkMAFFillingHoleCTriangleTest.h"
#include "vtkMAFFillingHoleCEdgeTest.h"
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
#include "vtkMAFCollisionDetectionFilterTest.h"
#include "vtkMAFImageFillHolesRemoveIslandsTest.h"
#include "vtkMAFRayCastCleanerTest.h"
#include "vtkMAFLineStripSourceTest.h"
#include "vtkMAFTubeFilterTest.h"
#include "vtkMafRGtoSPImageFilterTest.h"
#include "vtkMAFDistanceFilterTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"
#include "vtkMAFSmartPointer.h"
#include "mafFakeLogicForTest.h"
#include "mafServiceLocator.h"

//Main Test Executor
int
	main( int argc, char* argv[] )
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

	runner.addTest(vtkMAFContourVolumeMapperTest::suite());
	runner.addTest(vtkMAFRemoveCellsFilterTest::suite());
	runner.addTest(vtkMAFPolyDataToSinglePolyLineTest::suite());
	runner.addTest(vtkMAFTextOrientatorTest::suite());
	runner.addTest(vtkMAFGlobalAxisCoordinateTest::suite());
	runner.addTest(vtkMAFLocalAxisCoordinateTest::suite());
	runner.addTest(vtkMAFVolumeResampleTest::suite());
	runner.addTest(vtkMAFDOFMatrixTest::suite());
	runner.addTest(vtkMAFImplicitPolyDataTest::suite());
	runner.addTest(vtkMAFTextActorMeterTest::suite());
	runner.addTest(vtkMAFCellsFilterTest::suite());
	runner.addTest(vtkMAFGridActorTest::suite());
	runner.addTest(vtkMAFProjectVolumeTest::suite());
	runner.addTest(vtkMAFClipSurfaceBoundingBoxTest::suite());
	runner.addTest(vtkMAFRGSliceAccumulateTest::suite());
	runner.addTest(vtkMAFRectilinearGridToRectilinearGridFilterTest::suite());
	runner.addTest(vtkMAFProfilingActorTest::suite());
	runner.addTest(vtkMAFRulerActor2DTest::suite());
	runner.addTest(vtkMAFSimpleRulerActor2DTest::suite());
	runner.addTest(vtkMAFFixedCutterTest::suite());
	runner.addTest(vtkMAFHistogramTest::suite());
	runner.addTest(vtkMAFExtendedGlyph3DTest::suite());
	runner.addTest(vtkMAFTransferFunction2DTest::suite());
	runner.addTest(vtkHoleConnectivityTest::suite());
	runner.addTest(vtkDicomUnPackerTest::suite());
	runner.addTest(vtkTriangleQualityRatioTest::suite());
	runner.addTest(vtkMaskPolyDataFilterTest::suite());
	runner.addTest(vtkMAFExtrudeToCircleTest::suite());
	runner.addTest(vtkMAFPastValuesListTest::suite());
	runner.addTest(vtkMAFPolyDataMirrorTest::suite());
	runner.addTest(vtkMAFRegionGrowingLocalGlobalThresholdTest::suite());
	runner.addTest(vtkImageUnPackerTest::suite());
	runner.addTest(vtkPackedImageTest::suite());
	runner.addTest(vtkMAFVolumeRayCastMapperTest::suite());
	runner.addTest(vtkXRayVolumeMapperTest::suite());
	runner.addTest(vtkMAFVolumeTextureMapper2DTest::suite());
	runner.addTest(vtkMAFDataArrayDescriptorTest::suite());
	runner.addTest(vtkMAFErythrocyteSourceTest::suite());
	runner.addTest(vtkMAFMeshCutterTest::suite());
	runner.addTest(vtkMAFFillingHoleTest::suite());
	runner.addTest(vtkMAFFixTopologyTest::suite());
	runner.addTest(vtkWeightedLandmarkTransformTest::suite());
	runner.addTest(vtkMAFLandmarkCloudOutlineCornerFilterTest::suite());
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
	runner.addTest(vtkMAFExtrudeToCircleVertexDataTest::suite());
	runner.addTest(vtkMAFFillingHoleCTriangleTest::suite());
	runner.addTest(vtkMAFFillingHoleCEdgeTest::suite());
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
	runner.addTest(vtkMAFCollisionDetectionFilterTest::suite());
	runner.addTest(vtkMAFImageFillHolesRemoveIslandsTest::suite());
	runner.addTest(vtkMAFRayCastCleanerTest::suite());
	runner.addTest(vtkMAFLineStripSourceTest::suite());
	runner.addTest(vtkMAFTubeFilterTest::suite());
	runner.addTest(vtkMafRGtoSPImageFilterTest::suite());
	runner.addTest(vtkMAFDistanceFilterTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 
	return result.wasSuccessful() ? 0 : 1;
}

