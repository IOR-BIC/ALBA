/*=========================================================================
Program:   AlbaMaster
Module:    AlbaMaster.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

//Enable This for memory leak detection
//#include <vld.h>

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in ALBA must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "AlbaMaster.h"

#include "albaDecl.h"
#ifdef ALBA_USE_GDCM
#include "albaGUIDicomSettings.h"
#endif
#include "albaGUIMDIFrame.h"
#include "albaPics.h"
#include "albaPipeFactoryVME.h"
#include "albaServiceLocator.h"
#include "albaVMEFactory.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"

//IMPORTERS
#include "albaOpImporterASCII.h"
#include "albaOpImporterAnalogWS.h"
#include "albaOpImporterAnsysCDBFile.h"
#include "albaOpImporterAnsysInputFile.h"
#include "albaOpImporterC3D.h"
#ifdef ALBA_USE_GDCM
#include "albaOpImporterDicom.h"
#endif
#include "albaOpImporterGRFWS.h"
#include "albaOpImporterImage.h"
#include "albaOpImporterLandmark.h"
#include "albaOpImporterLandmarkTXT.h"
#include "albaOpImporterLandmarkWS.h"
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
#include "albaOpImporterDicFile.h"
#include "albaOpImporterPointCloud.h"

//EXPORTERS
#include "albaOpExporterAnsysCDBFile.h"
#include "albaOpExporterAnsysInputFile.h"
#include "albaOpExporterBmp.h"
#ifdef ALBA_USE_GDCM
#include "albaOpExporterDicom.h"
#endif 
#include "albaOpExporterGRFWS.h"
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
#include "albaOpExporterWrappedMeter.h"
#include "albaOpExporterAbaqusFile.h"

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
#include "albaOpComputeWrapping.h"
#include "albaOpConnectivitySurface.h"
#include "albaOpCreateAverageLandmark.h"
#include "albaOpCreateEditSkeleton.h"
#include "albaOpCreateGroup.h"
#include "albaOpCreateLabeledVolume.h"
#include "albaOpCreateMeter.h"
#include "albaOpCreateProber.h"
#include "albaOpCreateRefSys.h"
#include "albaOpCreateRefSysFromViewSlice.h"
#include "albaOpCreateSlicer.h"
#include "albaOpCreateSpline.h"
#include "albaOpCreateSurface.h"
#include "albaOpCreateSurfaceParametric.h"
#include "albaOpCreateVolume.h"
#include "albaOpCreateWrappedMeter.h"
#include "albaOpCrop.h"
#include "albaOpCropDeformableROI.h"
#include "albaOpDecimateSurface.h"
#include "albaOpEditMetadata.h"
#include "albaOpEditNormals.h"
#include "albaOpExtractIsosurface.h"
#include "albaOpExtractImageFromArbitraryView.h"
#include "albaOpExtrusionHoles.h"
#include "albaOpEqualizeHistogram.h"
#include "albaOpFillHoles.h"
#include "albaOpFilterSurface.h"
#include "albaOpFilterVolume.h"
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

// VIEWS
#include "albaView3D.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaViewArbitrarySlice.h"
#include "albaViewCT.h"
#include "albaViewCompound.h"
#include "albaViewCompoundWindowing.h"
#include "albaViewGlobalSlice.h"
#include "albaViewGlobalSliceCompound.h"
#include "albaViewHTML.h"
#include "albaViewImage.h"
#include "albaViewImageCompound.h"
#include "albaViewIsosurfaceCompound.h"
#include "albaViewOrthoSlice.h"
#include "albaViewPlot.h"
#include "albaViewRX.h"
#include "albaViewRXCT.h"
#include "albaViewRXCompound.h"
#include "albaViewSlice.h"
#include "albaViewSliceBlend.h"
#include "albaViewSliceBlendRX.h"
#include "albaViewSliceGlobal.h"
#include "albaViewSliceOnCurve.h"
#include "albaViewSliceOnCurveCompound.h"
#include "albaViewSlicer.h"
#include "albaViewVTK.h"
#include "albaOpCreateInfoText.h"
#include "albaViewVTKCompound.h"

//WIZARD
// Comment this line to disable wizard sample
//#define USE_WIZARD

#ifdef USE_WIZARD
#include "albaWizard.h"
#include "exWizardSample.h"
#endif

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(AlbaMaster)

//--------------------------------------------------------------------------------
bool AlbaMaster::OnInit()
{
  albaPictureFactory::GetPictureFactory()->Initialize();	

#include "Examples/AlbaMaster/FRAME_ICON16x16.xpm"
	albaADDPIC(FRAME_ICON16x16);
#include "Examples/AlbaMaster/FRAME_ICON32x32.xpm"
	albaADDPIC(FRAME_ICON32x32);
#include "Examples/AlbaMaster/FRAME_ICON64x64.xpm"
	albaADDPIC(FRAME_ICON64x64);
#include "Examples/AlbaMaster/FRAME_ICON128x128.xpm"
	albaADDPIC(FRAME_ICON128x128);
#include "Examples/AlbaMaster/FRAME_ICON256x256.xpm"
	albaADDPIC(FRAME_ICON256x256);
#include "Examples/AlbaMaster/MDICHILD_ICON.xpm"
	albaADDPIC(MDICHILD_ICON);

#include "pic/GIZMO_TRANSLATE_ICON.xpm"
	albaADDPIC(GIZMO_TRANSLATE_ICON);
#include "pic/GIZMO_ROTATE_ICON.xpm"
	albaADDPIC(GIZMO_ROTATE_ICON);

	int result;
	result = albaVMEFactory::Initialize();
	assert(result==ALBA_OK);

	// Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
	result = albaPipeFactoryVME::Initialize();
	assert(result==ALBA_OK);

	m_LogicInitialized = false;
	m_Logic = new albaLogicWithManagers();
	albaServiceLocator::SetLogicManager(m_Logic);
  
	m_Logic->GetTopWin()->SetTitle("AlbaMaster");

	//m_Logic->PlugTimebar(false);
	//m_Logic->PlugMenu(false);
	//m_Logic->PlugToolbar(false);
	//m_Logic->PlugLogbar(false);
	//m_Logic->PlugSidebar(true, albaSideBar::SINGLE_NOTEBOOK);
	//m_Logic->PlugOpManager(false);
	//m_Logic->PlugViewManager(false);
	//m_Logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005:

#ifdef USE_WIZARD
  m_Logic->PlugWizardManager(true);
#endif
	m_Logic->PlugSnapshotManager(true);
	m_Logic->Configure();

	SetTopWindow(albaGetFrame());  

	//------------------------------------------------------------
	// Importer Menu:
	//------------------------------------------------------------

	m_Logic->Plug(new albaOpImporterImage("Images"));
	m_Logic->Plug(new albaOpImporterRAWVolume("RAW Volume"));
	m_Logic->Plug(new albaOpImporterRAWImages("RAW Images"));
	m_Logic->Plug(new albaOpImporterSTL("STL"));
	m_Logic->Plug(new albaOpImporterPLY("PLY"));
	m_Logic->Plug(new albaOpImporterVRML("VRML"));
	m_Logic->Plug(new albaOpImporterVTK("VTK"));
	m_Logic->Plug(new albaOpImporterVTKXML("VTK xml"));
	m_Logic->Plug(new albaOpImporterMetaImage("ITK MetaImage"));
	m_Logic->Plug(new albaOpImporterMSF("ALBA"));
	m_Logic->Plug(new albaOpImporterMSF1x("MSF 1.x"));
	m_Logic->Plug(new albaOpImporterAnsysCDBFile("Ansys CDB File"), "Finite Element");
	m_Logic->Plug(new albaOpImporterAnsysInputFile("Ansys Input File"), "Finite Element");
	m_Logic->Plug(new albaOpImporterAbaqusFile("Abaqus File"), "Finite Element");
	m_Logic->Plug(new albaOpImporterMesh("Generic Mesh"), "Finite Element");
	m_Logic->Plug(new albaOpImporterASCII("ASCII"));

#ifdef ALBA_USE_GDCM
	albaGUIDicomSettings *dicomSettings=new albaGUIDicomSettings(NULL,"DICOM");
	m_Logic->Plug(new albaOpImporterDicom("DICOM"), "", true, dicomSettings);
#endif
	m_Logic->Plug(new albaOpImporterLandmark("Landmark"),"Landmark Suite");
	m_Logic->Plug(new albaOpImporterLandmarkTXT("Landmark TXT"),"Landmark Suite");
	m_Logic->Plug(new albaOpImporterLandmarkWS("Landmark WS"),"Landmark Suite");
	m_Logic->Plug(new albaOpImporterC3D("C3D"),"Landmark Suite");
	m_Logic->Plug(new albaOpImporterAnalogWS("EMG"));
	m_Logic->Plug(new albaOpImporterGRFWS("GRF"));
	m_Logic->Plug(new albaOpImporterDicFile("Digital Image Correlation (DIC)"));
	m_Logic->Plug(new albaOpImporterPointCloud("Generic Point Cloud"));

	//------------------------------------------------------------
	// Exporter Menu:
	//------------------------------------------------------------

	m_Logic->Plug(new albaOpExporterMSF("ALBA"));
	m_Logic->Plug(new albaOpExporterSTL("STL"));
	m_Logic->Plug(new albaOpExporterPLY("PLY"));
	m_Logic->Plug(new albaOpExporterVTK("VTK"));
	m_Logic->Plug(new albaOpExporterVTKXML("VTK"));
	m_Logic->Plug(new albaOpExporterMetaImage("ITK MetaImage"));
#ifdef ALBA_USE_GDCM
	m_Logic->Plug(new albaOpExporterDicom("Dicom"),"",true,dicomSettings);
#endif
	m_Logic->Plug(new albaOpExporterRAW("Raw"));
	m_Logic->Plug(new albaOpExporterBmp("Bmp"));
	m_Logic->Plug(new albaOpExporterLandmark("Landmark"));
	m_Logic->Plug(new albaOpExporterWrappedMeter());
	m_Logic->Plug(new albaOpExporterGRFWS());
	m_Logic->Plug(new albaOpExporterMeters());
	m_Logic->Plug(new albaOpExporterAnsysCDBFile("Ansys CDB File"), "Finite Element");
	m_Logic->Plug(new albaOpExporterAnsysInputFile("Ansys Input File"), "Finite Element");
	m_Logic->Plug(new albaOpExporterAbaqusFile("Abaqus File"), "Finite Element");
	m_Logic->Plug(new albaOpExporterMesh("Generic Mesh"), "Finite Element");
	

	//------------------------------------------------------------
	// Operation Menu:
	//------------------------------------------------------------

	m_Logic->Plug(new albaOpCreateVolume("Create Volume"), _("Create"));
	m_Logic->Plug(new albaOpAddLandmark("Add Landmark  \tCtrl+A"), _("Create"));
	m_Logic->Plug(new albaOpCreateAverageLandmark("Create Average Landmark"), _("Create"));
	m_Logic->Plug(new albaOpCreateGroup("Group"),_("Create"));
	m_Logic->Plug(new albaOpCreateMeter("Meter"),_("Create"));
	m_Logic->Plug(new albaOpCreateRefSys("RefSys"),_("Create"));
	m_Logic->Plug(new albaOpCreateInfoText("Info Text"), _("Create"));
	m_Logic->Plug(new albaOpCreateRefSysFromViewSlice("RefSys from View Slice"), _("Create"));
	m_Logic->Plug(new albaOpCreateProber("Prober"),_("Create"));
	m_Logic->Plug(new albaOpCreateSlicer("Slicer"),_("Create"));
	m_Logic->Plug(new albaOpExtractIsosurface("Extract Isosurface"),_("Create"));
	m_Logic->Plug(new albaOpExtractImageFromArbitraryView("Extract Image from Arbitrary View"), _("Create"));
	m_Logic->Plug(new albaOpCreateSurfaceParametric("Parametric Surface"),_("Create"));
	m_Logic->Plug(new albaOpCreateSpline("Spline"),_("Create"));
	m_Logic->Plug(new albaOpCreateLabeledVolume(),_("Create"));
	m_Logic->Plug(new albaOpCreateEditSkeleton(),_("Create"));
	m_Logic->Plug(new albaOpCreateSurface(),_("Create"));
	m_Logic->Plug(new albaOpCreateWrappedMeter(),_("Create"));
	m_Logic->Plug(new albaOpComputeWrapping("Computing Wrapping"),_("Create"));
	m_Logic->Plug(new albaOpFreezeVME(),_("Create"));
	m_Logic->Plug(new albaOpSegmentationRegionGrowingConnectedThreshold(),_("Create"));
	m_Logic->Plug(new albaOpSegmentationRegionGrowingLocalAndGlobalThreshold(), _("Create"));
	m_Logic->Plug(new albaOpSegmentation(), _("Create"));


	m_Logic->Plug(new albaOpClipSurface("Clip Surface"),_("Modify"));
	m_Logic->Plug(new albaOpFilterSurface("Filter Surface"),_("Modify"));
	m_Logic->Plug(new albaOpFilterVolume("Filter Volume"),_("Modify"));
	m_Logic->Plug(new albaOpDecimateSurface("Decimate Surface"),_("Modify"));
	m_Logic->Plug(new albaOpConnectivitySurface("Connectivity Surface"),_("Modify"));
	m_Logic->Plug(new albaOpEditNormals("Edit Normals"),_("Modify"));
	m_Logic->Plug(new albaOpEditMetadata("Metadata Editor"),_("Modify"));
	m_Logic->Plug(new albaOpTransformOld("Transform"),_("Modify"));
	m_Logic->Plug(new albaOpVolumeResample("Resample Volume"),_("Modify"));
	m_Logic->Plug(new albaOpCrop("Crop Volume"),_("Modify"));
	m_Logic->Plug(new albaOpBooleanSurface("Boolean Surface"),_("Modify"));
	m_Logic->Plug(new albaOpMML("MML"),_("Modify"));
	m_Logic->Plug(new albaOpMML3("MML 3"),_("Modify"));
	m_Logic->Plug(new albaOpCropDeformableROI("Crop ROI"),_("Modify"));
	m_Logic->Plug(new albaOpFlipNormals("Flip Normals"),_("Modify"));
	m_Logic->Plug(new albaOpRemoveCells("Remove Cells"),_("Modify"));
	m_Logic->Plug(new albaOpExtrusionHoles(),_("Modify"));
	m_Logic->Plug(new albaOpTransform(), _("Modify"));
	m_Logic->Plug(new albaOpScaleDataset("Scale Dataset"),_("Modify"));
	m_Logic->Plug(new albaOpMove("Move"),_("Modify"));    
	m_Logic->Plug(new albaOpSplitSurface(),_("Modify"));
	m_Logic->Plug(new albaOpInteractiveClipSurface(),_("Modify"));
	m_Logic->Plug(new albaOpLabelizeSurface(),_("Modify"));
	m_Logic->Plug(new albaOpSmoothSurface(),_("Modify"));
	m_Logic->Plug(new albaOpCleanSurface(),_("Modify"));
	m_Logic->Plug(new albaOpTriangulateSurface(),_("Modify"));
	m_Logic->Plug(new albaOpSurfaceMirror(), _("Modify"));
	m_Logic->Plug(new albaOpVolumeMirror(), _("Modify"));
	m_Logic->Plug(new albaOpSubdivide(), _("Modify"));
	m_Logic->Plug(new albaOpFillHoles(),_("Modify"));
	m_Logic->Plug(new albaOpMeshDeformation(),_("Modify"));
	m_Logic->Plug(new albaOpMakeVMETimevarying(),_("Modify"));
	m_Logic->Plug(new albaOpEqualizeHistogram(),_("Modify"));
	m_Logic->Plug(new albaOpSmoothSurfaceCells(),_("Modify"));

	m_Logic->Plug(new albaOpMeasure2D("2D Measure"), "Measure");
	m_Logic->Plug(new albaOp2DMeasure("2D Measure (Old)"), "Measure");
	m_Logic->Plug(new albaOpVOIDensity("VOI Density"), "Measure");
	m_Logic->Plug(new albaOpVolumeMeasure("Volume"), "Measure");
	m_Logic->Plug(new albaOpMeshQuality("Mesh Quality"), "Measure");
	m_Logic->Plug(new albaOpComputeInertialTensor(), "Measure");
	m_Logic->Plug(new albaOpComputeHausdorffDistance(), "Measure");
	
	m_Logic->Plug(new albaOpIterativeRegistration(),_("Register"));
	m_Logic->Plug(new albaOpRegisterClusters("Clusters"),_("Register"));
	m_Logic->Plug(new albaOpClassicICPRegistration("Surface"),_("Register"));

	m_Logic->Plug(new albaOpESPCalibration("Calibrate ESP Phantom"), _("Calibration"));
	//------------------------------------------------------------
	
#ifdef USE_WIZARD
  //------------------------------------------------------------
  // Wizard Menu:
  //------------------------------------------------------------

  //A simple wizard sample
  m_Logic->Plug(new exWizardSample("Import loop"),"");
#endif

	//------------------------------------------------------------
	// View Menu:
	//------------------------------------------------------------

	//View VTK
	m_Logic->Plug(new albaViewVTK("Surface"));

	//View Arbitrary Slice
	albaViewArbitrarySlice *varbitrary= new albaViewArbitrarySlice("Arbitrary Slice");
	varbitrary->PackageView();
	m_Logic->Plug(varbitrary);

 	//View Orthoslice
	albaViewOrthoSlice *vortho= new albaViewOrthoSlice("Orthoslice");
	vortho->PackageView();
	m_Logic->Plug(vortho);

 	//View RX Compound
	albaViewRXCompound *vrx=new albaViewRXCompound("RX");
	vrx->PackageView();
	m_Logic->Plug(vrx);

 	//View Isosurface
	albaViewVTK *viso = new albaViewVTK("Isosurface");
	viso->PlugVisualPipe("albaVMEVolumeGray", "albaPipeIsosurface",MUTEX);
	m_Logic->Plug(viso);

	//View Isosurface Compound
	albaViewIsosurfaceCompound *visoc = new albaViewIsosurfaceCompound("Isosurface Compound");
	visoc->PackageView();
	m_Logic->Plug(visoc);

 	//View RXCT
	albaViewRXCT *vrxct=new albaViewRXCT("RX-CT");
	vrxct->PackageView();
	m_Logic->Plug(vrxct);

 	//View Global Slice Compound
	albaViewGlobalSliceCompound *vglobal=new albaViewGlobalSliceCompound("Global Slice");
	vglobal->PackageView();
	m_Logic->Plug(vglobal);

  //View Image Compound
	albaViewImageCompound *vimageC=new albaViewImageCompound("Image");
	vimageC->PackageView();
	m_Logic->Plug(vimageC);

	//View 3D
	m_Logic->Plug(new albaView3D("3D"));

	//View Arbitrary Orthoslice
	albaViewArbitraryOrthoSlice *varbitraryortho=new albaViewArbitraryOrthoSlice("Arbitrary Orthoslice", albaAxes::BODY);
	varbitraryortho->PackageView();
	m_Logic->Plug(varbitraryortho);

	//View Slice On Curve Compound
	albaViewSliceOnCurveCompound *vsliceoncurveC=new albaViewSliceOnCurveCompound("Slice On Curve");
	vsliceoncurveC->PackageView();
	m_Logic->Plug(vsliceoncurveC);

	//View Image
	albaViewImage *vimage = new albaViewImage("Image");
	//vimage->PackageView();
	m_Logic->Plug(vimage);

// 	//View Compound Windowing 
// 	albaViewCompoundWindowing *vCompWind = new albaViewCompoundWindowing("Compound Windowing (albaViewCompoundWindowing)");
// 	vCompWind->PackageView();
// 	m_Logic->Plug(vCompWind);

	//View CT 
	albaViewCT *vCT = new albaViewCT("CT");
	vCT->PackageView();
	m_Logic->Plug(vCT);

	//View Global Slice 
	albaViewGlobalSlice *vGlobalS = new albaViewGlobalSlice("Global Slice");
	//vGlobalS->PackageView();
	m_Logic->Plug(vGlobalS);

	//View RX 
	albaViewRX *vRX = new albaViewRX("RX");
	//vRX->PackageView();
	m_Logic->Plug(vRX);

	//View Slice
	albaViewSlice *vslice = new albaViewSlice("Slice");
	//vslice->PackageView();
	m_Logic->Plug(vslice);

	//View Slice Blend
	albaViewSliceBlend *vSliceBlend = new albaViewSliceBlend("Slice Blend");
	//vSliceBlend->PackageView();
	m_Logic->Plug(vSliceBlend);

	//View Slice Blend RX
	albaViewSliceBlendRX *vSliceBlendRX = new albaViewSliceBlendRX("Slice Blend RX");
	vSliceBlendRX->PackageView();
	m_Logic->Plug(vSliceBlendRX);

	//View Slice Global
	albaViewSliceGlobal *vSliceGlobal = new albaViewSliceGlobal("Slice Slice Global");
	//vSliceGlobal->PackageView();
	m_Logic->Plug(vSliceGlobal);

	//View Slice On Curve
	albaViewSliceOnCurve *vsliceoncurve = new albaViewSliceOnCurve("Slice On Curve");
	vsliceoncurve->PackageView();
	m_Logic->Plug(vsliceoncurve);

	//View Slicer
	albaViewSlicer *vslicer = new albaViewSlicer("Slicer");
	vslicer->PackageView();
	m_Logic->Plug(vslicer);

	//View RayCast
	albaViewVTK *vRayCast = new albaViewVTK("3D Volume Rendering");
	vRayCast->PlugVisualPipe("albaVMEVolumeGray", "albaPipeRayCast", MUTEX);
	m_Logic->Plug(vRayCast);

	//View HTML
	albaViewHTML *vHtml = new albaViewHTML("HTML");
	//vHtml->PackageView();
	m_Logic->Plug(vHtml);

	//////////////////////////////////////////////////////////////////////////
	// Splash Screen
	albaString splashImageName = "AlbaMasterSplash.bmp";

	wxString splashDir = albaGetApplicationDirectory().c_str();
	wxBitmap splashBitmap;
	splashBitmap.LoadFile(splashDir + "\\Config\\" + splashImageName, wxBITMAP_TYPE_BMP);
	m_Logic->ShowSplashScreen(splashBitmap);

	//////////////////////////////////////////////////////////////////////////
	wxHandleFatalExceptions();

	//m_Logic->ShowSplashScreen();
	m_Logic->Show();
	m_Logic->Init(argc, argv); // calls FileNew - which create the root

	m_LogicInitialized = true;

	return TRUE;
}
//--------------------------------------------------------------------------------
int AlbaMaster::OnExit()
{
	m_LogicInitialized = false;

	cppDEL(m_Logic);
	return 0;
}

//--------------------------------------------------------------------------------
void AlbaMaster::OnFatalException()
{
	m_Logic->HandleException();
}

//--------------------------------------------------------------------------------
int AlbaMaster::FilterEvent(wxEvent& event)
{
	if (m_LogicInitialized && m_Logic)
		return m_Logic->AppEventFilter(event);

	return -1;
}
