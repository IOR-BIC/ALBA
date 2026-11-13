/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLogicStandardPlugs
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
#include "albaDecl.h"
#include "albaLogicStandardPlugs.h"
#include "albaLogicWithManagers.h"

//IMPORTERS
#include "albaOpImporterAnsysCDBFile.h"
#include "albaOpImporterAnsysInputFile.h"
#ifdef ALBA_USE_GDCM
#include "albaGUIDicomSettings.h"
#include "albaOpImporterDicom.h"
#endif
#include "albaOpImporterImage.h"
#include "albaOpImporterLandmark.h"
#include "albaOpImporterMSF.h"
#include "albaOpImporterMSF1x.h"
#include "albaOpImporterMesh.h"
#include "albaOpImporterRAWImages.h"
#include "albaOpImporterRAWVolume.h"
#include "albaOpImporterSTL.h"
#include "albaOpImporterPLY.h"
#include "albaOpImporterVRML.h"
#include "albaOpImporterVTK.h"
#include "albaOpImporterMetaImage.h"
#include "albaOpImporterVTKXML.h"
#include "albaOpImporterAbaqusFile.h"
#include "albaOpMeshScarlarsImporter.h"
#include "albaOpImporterDicFile.h"
#include "albaOpImporterPointCloud.h"
#include "albaOpImporterScancoImageIO.h"

//EXPORTERS
#include "albaOpExporterAnsysCDBFile.h"
#include "albaOpExporterAnsysInputFile.h"
#include "albaOpExporterImages.h"
#ifdef ALBA_USE_GDCM
#include "albaOpExporterDicom.h"
#endif 
#include "albaOpExporterLandmark.h"
#include "albaOpExporterMSF.h"
#include "albaOpExporterMesh.h"
#include "albaOpExporterMeters.h"
#include "albaOpExporterRAW.h"
#include "albaOpExporterSTL.h"
#include "albaOpExporterPLY.h"
#include "albaOpExporterVTK.h"
#include "albaOpExporterVTKXML.h"
#include "albaOpExporterMetaImage.h"
#include "albaOpExporterAbaqusFile.h"
#include "albaOpExporterPointCloud.h"
#include "albaOpExporterDicFile.h"

// OPERATIONS
#include "albaOp2DMeasure.h"
#include "albaOpMeasure2D.h"
#include "albaOpComputeInertialTensor.h"
#include "albaOpTransformOld.h"
#include "albaOpAddLandmark.h"
#include "albaOpBooleanSurface.h"
#include "albaOpClassicICPRegistration.h"
#include "albaOpCleanSurface.h"
#include "albaOpClipSurface.h"
#include "albaOpConnectivitySurface.h"
#include "albaOpCreateAverageLandmark.h"
#include "albaOpCreateAveragePlane.h"
#include "albaOpCreateEditSkeleton.h"
#include "albaOpCreateGroup.h"
#include "albaOpCreateLabeledVolume.h"
#include "albaOpCreateMeter.h"
#include "albaOpCreateRefSys.h"
#include "albaOpCreateRefSysFromViewSlice.h"
#include "albaOpCreatePolyline.h"
#include "albaOpPolylineFromLandmarkCloud.h"
#include "albaOpCreateSpline.h"
#include "albaOpCreateSurfaceParametric.h"
#include "albaOpCreateVolume.h"
#include "albaOpCreateInfoText.h"
#include "albaOpCreateWrappedMeter.h"
#include "albaOpCrop.h"
#include "albaOpCropDeformableROI.h"
#include "albaOpDecimateSurface.h"
#include "albaOpEditMetadata.h"
#include "albaOpEditNormals.h"
#include "albaOpExtractIsosurface.h"
#include "albaOpExtrusionHoles.h"
#include "albaOpFillHoles.h"
#include "albaOpFilterSurface.h"
#include "albaOpFilterVolume.h"
#include "albaOpFilterImage.h"
#include "albaOpFlipNormals.h"
#include "albaOpFreezeVME.h"
#include "albaOpInteractiveClipSurface.h"
#include "albaOpIterativeRegistration.h"
#include "albaOpLabelizeSurface.h"
#include "albaOpMML.h"
#include "albaOpMML3.h"
#include "albaOpMakeVMETimevarying.h"
#include "albaOpMeshDeformation.h"
#include "albaOpMeshQuality.h"
#include "albaOpMove.h"
#include "albaOpRegisterClusters.h"
#include "albaOpRemoveCells.h"
#include "albaOpScaleDataset.h"
#include "albaOpSegmentation.h"
#include "albaOpSegmentationRegionGrowingConnectedThreshold.h"
#include "albaOpSegmentationRegionGrowingLocalAndGlobalThreshold.h"
#include "albaOpSmoothSurface.h"
#include "albaOpSmoothSurfaceCells.h"
#include "albaOpSplitSurface.h"
#include "albaOpSubdivide.h"
#include "albaOpSurfaceMirror.h"
#include "albaOpTransform.h"
#include "albaOpTriangulateSurface.h"
#include "albaOpVOIDensity.h"
#include "albaOpVolumeMeasure.h"
#include "albaOpVolumeMirror.h"
#include "albaOpVolumeResample.h"
#include "albaOpESPCalibration.h"
#include "albaOpComputeHausdorffDistance.h"
#include "albaOpCreateCentroid.h"
#include "albaOpTransformAtoB.h"
#include "albaOpImageCalibration.h"

//TMP
#include "albaOpImporterExternalFile.h"
#include "albaOpApplyTrajectory.h"
#include "albaOpCreateProsthesis.h"
#include "albaOpDecomposeTimeVarVME.h"
#include "albaOpExtractGeometry.h"
#include "albaOpGarbageCollectMSFDir.h"
#include "albaOpLabelExtractor.h"
#include "albaOpInteractionDebugger.h"
#include "albaOpOpenExternalFile.h"
#include "albaOpScalarToSurface.h"
#include "albaOpVOIDensityEditor.h"
#include "albaOpValidateTree.h"
#include "albaOpVolumeUnion.h"
#include "albaOpInteractorSliderSample.h"

// VIEWS
#include "albaView3D.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaViewArbitrarySlice.h"
#include "albaViewGlobalSliceCompound.h"
#include "albaViewImageCompound.h"
#include "albaViewOrthoSlice.h"
#include "albaViewRXCT.h"
#include "albaViewSliceBlendRX.h"
#include "albaViewSliceOnCurveCompound.h"
#include "albaViewVTK.h"
#include "albaViewVirtualRX.h"
#include "albaOpNearestLandmark.h"
#include "albaOpMeasureAxisRotations.h"



void albaLogicStandardPlugs::PlugStandardOperations(albaLogicWithManagers *logic)
{
	//------------------------------------------------------------
	// Importer Menu:
	//------------------------------------------------------------

	logic->Plug(new albaOpImporterImage("Images"));
	logic->Plug(new albaOpImporterRAWVolume("RAW Volume"));
	logic->Plug(new albaOpImporterRAWImages("RAW Images"));
	logic->Plug(new albaOpImporterSTL("STL"));
	logic->Plug(new albaOpImporterPLY("PLY"));
	logic->Plug(new albaOpImporterVRML("VRML"));
	logic->Plug(new albaOpImporterVTK("VTK"));
	logic->Plug(new albaOpImporterVTKXML("VTK xml"));
	logic->Plug(new albaOpImporterMetaImage());
	logic->Plug(new albaOpImporterMSF("ALBA"));
	logic->Plug(new albaOpImporterMSF1x("MSF 1.x"));
	logic->Plug(new albaOpImporterAnsysCDBFile("Ansys CDB File"), "Finite Element");
	logic->Plug(new albaOpImporterAnsysInputFile("Ansys Input File"), "Finite Element");
	logic->Plug(new albaOpImporterAbaqusFile("Abaqus File"), "Finite Element");
	logic->Plug(new albaOpImporterMesh("Generic Mesh"), "Finite Element");
	logic->Plug(new albaOpMeshScarlarsImporter("Mesh Scalars from Lis files"), ("Finite Element"));

#ifdef ALBA_USE_GDCM
	albaGUIDicomSettings *dicomSettings = new albaGUIDicomSettings(NULL, "DICOM");
	logic->Plug(new albaOpImporterDicom("DICOM"), "", true, dicomSettings);
#endif
	logic->Plug(new albaOpImporterLandmark("Landmark"));

	logic->Plug(new albaOpImporterDicFile("Digital Image Correlation (DIC)"));
	logic->Plug(new albaOpImporterPointCloud("Generic Point Cloud"));
	logic->Plug(new albaOpImporterScancoImageIO("Scanco IO")); 
	logic->Plug(new albaOpImporterExternalFile());


	//------------------------------------------------------------
	// Exporter Menu:
	//------------------------------------------------------------

	logic->Plug(new albaOpExporterMSF());
	logic->Plug(new albaOpExporterSTL("STL"));
	logic->Plug(new albaOpExporterPLY("PLY"));
	logic->Plug(new albaOpExporterVTK("VTK"));
	logic->Plug(new albaOpExporterVTKXML("VTK xml"));
	logic->Plug(new albaOpExporterMetaImage());
#ifdef ALBA_USE_GDCM
	logic->Plug(new albaOpExporterDicom("Dicom"), "", true, dicomSettings);
#endif
	logic->Plug(new albaOpExporterRAW("Raw"));
	logic->Plug(new albaOpExporterImages("Images"));
	logic->Plug(new albaOpExporterLandmark("Landmark"));
	logic->Plug(new albaOpExporterPointCloud());
	logic->Plug(new albaOpExporterMeters());
	logic->Plug(new albaOpExporterAnsysCDBFile("Ansys CDB File"), "Finite Element");
	logic->Plug(new albaOpExporterAnsysInputFile("Ansys Input File"), "Finite Element");
	logic->Plug(new albaOpExporterAbaqusFile("Abaqus File"), "Finite Element");
	logic->Plug(new albaOpExporterMesh("Generic Mesh"), "Finite Element");
	logic->Plug(new albaOpExporterDicFile("Digital Image Correlation (DIC)"));

	//------------------------------------------------------------
	// Operation Menu:
	//------------------------------------------------------------

	logic->Plug(new albaOpCreateVolume(), _("Create"));
	logic->Plug(new albaOpAddLandmark("Add Landmark  \tCtrl+A"), _("Create"));
	logic->Plug(new albaOpCreateAverageLandmark("Create Average Landmark"), _("Create"));
	logic->Plug(new albaOpCreateAveragePlane("Create Average Plane"), _("Create"));
	logic->Plug(new albaOpCreateGroup("Group"), _("Create"));
	logic->Plug(new albaOpCreateMeter("Meter"), _("Create"));
	logic->Plug(new albaOpCreateRefSys("RefSys"), _("Create"));
	logic->Plug(new albaOpCreateInfoText("Info Text"), _("Create"));
	logic->Plug(new albaOpCreateRefSysFromViewSlice("RefSys from View Slice"), _("Create"));
	logic->Plug(new albaOpExtractIsosurface("Extract Isosurface"), _("Create"));
	logic->Plug(new albaOpCreateSurfaceParametric("Parametric Surface"), _("Create"));
	logic->Plug(new albaOpCreatePolyline("Polyline"), _("Create"));
	logic->Plug(new albaOpPolylineFromLandmarkCloud(), _("Create"));
	logic->Plug(new albaOpCreateSpline("Spline"), _("Create"));
	logic->Plug(new albaOpCreateEditSkeleton(), _("Create"));
	logic->Plug(new albaOpCreateWrappedMeter(), _("Create"));
	logic->Plug(new albaOpFreezeVME(), _("Create"));
	logic->Plug(new albaOpNearestLandmark(), _("Create"));
	logic->Plug(new albaOpCreateCentroid(), _("Create"));


	logic->Plug(new albaOpClipSurface("Clip Surface"), _("Modify"));
	logic->Plug(new albaOpFilterSurface("Filter Surface"), _("Modify"));
	logic->Plug(new albaOpFilterVolume("Filter Volume"), _("Modify"));
	logic->Plug(new albaOpFilterImage("Filter Image"), _("Modify"));
	logic->Plug(new albaOpDecimateSurface("Decimate Surface"), _("Modify"));
	logic->Plug(new albaOpConnectivitySurface("Connectivity Surface"), _("Modify"));
	logic->Plug(new albaOpEditNormals("Edit Normals"), _("Modify"));
	logic->Plug(new albaOpEditMetadata("Metadata Editor"), _("Modify"));
	logic->Plug(new albaOpTransformOld("Transform"), _("Modify"));
	logic->Plug(new albaOpVolumeResample("Resample Volume"), _("Modify"));
	logic->Plug(new albaOpCrop("Crop Volume"), _("Modify"));
	logic->Plug(new albaOpBooleanSurface("Boolean Surface"), _("Modify"));
	logic->Plug(new albaOpMML("MML"), _("Modify"));
	logic->Plug(new albaOpMML3("MML 3"), _("Modify"));
	logic->Plug(new albaOpCropDeformableROI("Crop ROI"), _("Modify"));
	logic->Plug(new albaOpFlipNormals("Flip Normals"), _("Modify"));
	logic->Plug(new albaOpRemoveCells("Remove Cells"), _("Modify"));
	logic->Plug(new albaOpExtrusionHoles(), _("Modify"));
	logic->Plug(new albaOpTransform(), _("Modify"));
	logic->Plug(new albaOpScaleDataset("Scale Dataset"), _("Modify"));
	logic->Plug(new albaOpMove("Move"), _("Modify"));
	logic->Plug(new albaOpSplitSurface(), _("Modify"));
	logic->Plug(new albaOpInteractiveClipSurface(), _("Modify"));
	logic->Plug(new albaOpLabelizeSurface(), _("Modify"));
	logic->Plug(new albaOpSmoothSurface(), _("Modify"));
	logic->Plug(new albaOpCleanSurface(), _("Modify"));
	logic->Plug(new albaOpTriangulateSurface(), _("Modify"));
	logic->Plug(new albaOpSurfaceMirror(), _("Modify"));
	logic->Plug(new albaOpVolumeMirror(), _("Modify"));
	logic->Plug(new albaOpSubdivide(), _("Modify"));
	logic->Plug(new albaOpFillHoles(), _("Modify"));
	logic->Plug(new albaOpMeshDeformation(), _("Modify"));
	logic->Plug(new albaOpMakeVMETimevarying(), _("Modify"));
	logic->Plug(new albaOpSmoothSurfaceCells(), _("Modify"));
	logic->Plug(new albaOpTransformAtoB(), "Modify");

	logic->Plug(new albaOpMeasure2D("2D Measure"), "Measure");
	logic->Plug(new albaOp2DMeasure("2D Measure (Old)"), "Measure");
	logic->Plug(new albaOpVOIDensity("VOI Density"), "Measure");
	logic->Plug(new albaOpVolumeMeasure("Volume"), "Measure");
	logic->Plug(new albaOpMeshQuality("Mesh Quality"), "Measure");
	logic->Plug(new albaOpComputeInertialTensor(), "Measure");
	logic->Plug(new albaOpComputeHausdorffDistance(), "Measure");
	logic->Plug(new albaOpMeasureAxisRotations(), "Measure");

	logic->Plug(new albaOpSegmentation(), _("Segmentation"));
	logic->Plug(new albaOpSegmentationRegionGrowingConnectedThreshold(), _("Segmentation"));
	logic->Plug(new albaOpSegmentationRegionGrowingLocalAndGlobalThreshold(), _("Segmentation"));
	logic->Plug(new albaOpCreateLabeledVolume(), _("Segmentation"));
	logic->Plug(new albaOpLabelExtractor(), _("Segmentation"));

	logic->Plug(new albaOpIterativeRegistration(), _("Register"));
	logic->Plug(new albaOpRegisterClusters("Clusters"), _("Register"));
	logic->Plug(new albaOpClassicICPRegistration("Surface"), _("Register"));

	logic->Plug(new albaOpESPCalibration("Calibrate ESP Phantom"), _("Calibration"));
	logic->Plug(new albaOpImageCalibration(), _("Calibration"));


	logic->Plug(new albaOpApplyTrajectory(), "TMP to test");
	logic->Plug(new albaOpCreateProsthesis(), "TMP to test");
	logic->Plug(new albaOpDecomposeTimeVarVME(), "TMP to test");
	logic->Plug(new albaOpExtractGeometry(), "TMP to test");
	logic->Plug(new albaOpGarbageCollectMSFDir(), "TMP to test");
	logic->Plug(new albaOpInteractionDebugger(), "TMP to test");
	logic->Plug(new albaOpOpenExternalFile(), "TMP to test");
	logic->Plug(new albaOpScalarToSurface(), "TMP to test");
	logic->Plug(new albaOpVOIDensityEditor(), "TMP to test");
	logic->Plug(new albaOpValidateTree(), "TMP to test");
	logic->Plug(new albaOpVolumeUnion(), "TMP to test");
	logic->Plug(new albaOpInteractorSliderSample(), "TMP to test");

}

//----------------------------------------------------------------------------
void albaLogicStandardPlugs::PlugStandardViews(albaLogicWithManagers *logic)
{
	//------------------------------------------------------------
	// View Menu:
	//------------------------------------------------------------

	//View VTK
	logic->Plug(new albaViewVTK("Surface"));

	
	//View Isosurface
	albaViewVTK *viso = new albaViewVTK("Isosurface");
	viso->PlugVisualPipe("albaVMEVolumeGray", "albaPipeIsosurface", MUTEX);
	logic->Plug(viso);

	//View Orthoslice
	albaViewOrthoSlice *vortho = new albaViewOrthoSlice("Orthoslice");
	vortho->PackageView();
	logic->Plug(vortho);

	//View Arbitrary Slice
	albaViewArbitrarySlice *varbitrary = new albaViewArbitrarySlice("Arbitrary Slice");
	varbitrary->PackageView();
	logic->Plug(varbitrary);

	//View Arbitrary Orthoslice
	albaViewArbitraryOrthoSlice *varbitraryortho = new albaViewArbitraryOrthoSlice("Arbitrary Orthoslice", albaAxes::BODY);
	varbitraryortho->PackageView();
	logic->Plug(varbitraryortho);

	//View Global Slice Compound
	albaViewGlobalSliceCompound *vglobal = new albaViewGlobalSliceCompound("Global Slice");
	vglobal->PackageView();
	logic->Plug(vglobal);

	//View Slice Blend RX
	albaViewSliceBlendRX *vSliceBlendRX = new albaViewSliceBlendRX("Slice Blend RX");
	vSliceBlendRX->PackageView();
	logic->Plug(vSliceBlendRX);

	//View Slice On Curve Compound
	albaViewSliceOnCurveCompound *vsliceoncurveC = new albaViewSliceOnCurveCompound("Slice On Curve");
	vsliceoncurveC->PackageView();
	logic->Plug(vsliceoncurveC);

	//View virtual RX
	albaViewVirtualRX *vvrx = new albaViewVirtualRX("Virtual RX");
	vvrx->PackageView();
	logic->Plug(vvrx);

	//View RXCT
	albaViewRXCT *vrxct = new albaViewRXCT("RX-CT");
	vrxct->PackageView();
	logic->Plug(vrxct);



	//View Image Compound
	albaViewImageCompound *vimageC = new albaViewImageCompound("Image");
	vimageC->PackageView();
	logic->Plug(vimageC);


	//View 3D
	logic->Plug(new albaView3D("3D"));

	//View RayCast
	albaViewVTK *vRayCast = new albaViewVTK("3D Volume Rendering");
	vRayCast->PlugVisualPipe("albaVMEVolumeGray", "albaPipeRayCast", MUTEX);
	logic->Plug(vRayCast);
}
