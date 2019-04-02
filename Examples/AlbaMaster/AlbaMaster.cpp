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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in ALBA must include "mafDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "AlbaMaster.h"

#include "mafDecl.h"
#include "mafGUIDicomSettings.h"
#include "mafGUIMDIFrame.h"
#include "mafPics.h"
#include "mafPipeFactoryVME.h"
#include "mafServiceLocator.h"
#include "mafVMEFactory.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"

// OPERATIONS
#include "mafOp2DMeasure.h"
#include "mafOpAddLandmark.h"
#include "mafOpBooleanSurface.h"
#include "mafOpClassicICPRegistration.h"
#include "mafOpCleanSurface.h"
#include "mafOpClipSurface.h"
#include "mafOpComputeWrapping.h"
#include "mafOpConnectivitySurface.h"
#include "mafOpCreateEditSkeleton.h"
#include "mafOpCreateGroup.h"
#include "mafOpCreateLabeledVolume.h"
#include "mafOpCreateMeter.h"
#include "mafOpCreateProber.h"
#include "mafOpCreateRefSys.h"
#include "mafOpCreateSlicer.h"
#include "mafOpCreateSpline.h"
#include "mafOpCreateSurface.h"
#include "mafOpCreateSurfaceParametric.h"
#include "mafOpCreateVolume.h"
#include "mafOpCreateWrappedMeter.h"
#include "mafOpCrop.h"
#include "mafOpCropDeformableROI.h"
#include "mafOpDecimateSurface.h"
#include "mafOpEditMetadata.h"
#include "mafOpEditNormals.h"
#include "mafOpEqualizeHistogram.h"

//EXPORTERS
#include "mafOpExporterAnsysCDBFile.h"
#include "mafOpExporterAnsysInputFile.h"
#include "mafOpExporterGRFWS.h"
#include "mafOpExporterLandmark.h"
#include "mafOpExporterMSF.h"
#include "mafOpExporterMesh.h"
#include "mafOpExporterMeters.h"
#include "mafOpExporterRAW.h"
#include "mafOpExporterSTL.h"
#include "mafOpExporterVTK.h"
#include "mafOpExporterWrappedMeter.h"
#include "mafOpExtractIsosurface.h"
#include "mafOpExtrusionHoles.h"
#include "mafOpFillHoles.h"
#include "mafOpFilterSurface.h"
#include "mafOpFilterVolume.h"
#include "mafOpFlipNormals.h"
#include "mafOpFreezeVME.h"

//IMPORTERS
#include "mafOpImporterASCII.h"
#include "mafOpImporterAnalogWS.h"
#include "mafOpImporterAnsysCDBFile.h"
#include "mafOpImporterAnsysInputFile.h"
#include "mafOpImporterC3D.h"
#include "mafOpImporterDicom.h"
#include "mafOpImporterGRFWS.h"
#include "mafOpImporterImage.h"
#include "mafOpImporterLandmark.h"
#include "mafOpImporterLandmarkTXT.h"
#include "mafOpImporterLandmarkWS.h"
#include "mafOpImporterMSF.h"
#include "mafOpImporterMSF1x.h"
#include "mafOpImporterMesh.h"
#include "mafOpImporterRAWImages.h"
#include "mafOpImporterRAWVolume.h"
#include "mafOpImporterSTL.h"
#include "mafOpImporterVRML.h"
#include "mafOpImporterVTK.h"

#include "mafOpInteractiveClipSurface.h"
#include "mafOpIterativeRegistration.h"
#include "mafOpLabelizeSurface.h"
#include "mafOpMAFTransform.h"
#include "mafOpMML.h"
#include "mafOpMML3.h"
#include "mafOpMakeVMETimevarying.h"
#include "mafOpMeshDeformation.h"
#include "mafOpMeshQuality.h"
#include "mafOpMove.h"
#include "mafOpRegisterClusters.h"
#include "mafOpRemoveCells.h"
#include "mafOpScaleDataset.h"
#include "mafOpSegmentationRegionGrowingConnectedThreshold.h"
#include "mafOpSegmentationRegionGrowingLocalAndGlobalThreshold.h"
#include "mafOpSegmentation.h"
#include "mafOpSmoothSurface.h"
#include "mafOpSmoothSurfaceCells.h"
#include "mafOpSplitSurface.h"
#include "mafOpSubdivide.h"
#include "mafOpSurfaceMirror.h"
#include "mafOpTransform.h"
#include "mafOpTriangulateSurface.h"
#include "mafOpVOIDensity.h"
#include "mafOpVolumeMeasure.h"
#include "mafOpVolumeMirror.h"
#include "mafOpVolumeResample.h"

// VIEWS
#include "mafView3D.h"
#include "mafViewArbitraryOrthoSlice.h"
#include "mafViewArbitrarySlice.h"
#include "mafViewCT.h"
#include "mafViewCompound.h"
#include "mafViewCompoundWindowing.h"
#include "mafViewGlobalSlice.h"
#include "mafViewGlobalSliceCompound.h"
#include "mafViewHTML.h"
#include "mafViewImage.h"
#include "mafViewImageCompound.h"
#include "mafViewIsosurfaceCompound.h"
#include "mafViewOrthoSlice.h"
#include "mafViewPlot.h"
#include "mafViewRX.h"
#include "mafViewRXCT.h"
#include "mafViewRXCompound.h"
#include "mafViewSlice.h"
#include "mafViewSliceBlend.h"
#include "mafViewSliceBlendRX.h"
#include "mafViewSliceGlobal.h"
#include "mafViewSliceNotInterpolated.h"
#include "mafViewSliceNotInterpolatedCompound.h"
#include "mafViewSliceOnCurve.h"
#include "mafViewSliceOnCurveCompound.h"
#include "mafViewSlicer.h"
#include "mafViewVTK.h"
#include "mafViewVTKCompound.h"

//WIZARD
// Comment this line to disable wizard sample
//#define USE_WIZARD

#ifdef USE_WIZARD
#include "mafWizard.h"
#include "exWizardSample.h"
#endif

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(AlbaMaster)

//--------------------------------------------------------------------------------
bool AlbaMaster::OnInit()
{
  mafPictureFactory::GetPictureFactory()->Initialize();	

#include "Examples/AlbaMaster/FRAME_ICON16x16.xpm"
	mafADDPIC(FRAME_ICON16x16);
#include "Examples/AlbaMaster/FRAME_ICON32x32.xpm"
	mafADDPIC(FRAME_ICON32x32);
#include "Examples/MedicalIcons/MDICHILD_ICON.xpm"

	mafADDPIC(MDICHILD_ICON);

	int result;
	result = mafVMEFactory::Initialize();
	assert(result==MAF_OK);

	// Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
	result = mafPipeFactoryVME::Initialize();
	assert(result==MAF_OK);

	m_Logic = new mafLogicWithManagers();
	mafServiceLocator::SetLogicManager(m_Logic);
  
	m_Logic->GetTopWin()->SetTitle("AlbaMaster");

	//m_Logic->PlugTimebar(false);
	//m_Logic->PlugMenu(false);
	//m_Logic->PlugToolbar(false);
	//m_Logic->PlugLogbar(false);
	//m_Logic->PlugSidebar(true, mafSideBar::SINGLE_NOTEBOOK);
	//m_Logic->PlugOpManager(false);
	//m_Logic->PlugViewManager(false);
	//m_Logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005:

#ifdef USE_WIZARD
  m_Logic->PlugWizardManager(true);
#endif
	m_Logic->PlugSnapshotManager(true);
	m_Logic->Configure();

	SetTopWindow(mafGetFrame());  

	//------------------------------------------------------------
	// Importer Menu:
	//------------------------------------------------------------

	m_Logic->Plug(new mafOpImporterImage("Images"));
	m_Logic->Plug(new mafOpImporterRAWVolume("RAW Volume"));
	m_Logic->Plug(new mafOpImporterSTL("STL"));
	m_Logic->Plug(new mafOpImporterVRML("VRML"));
	m_Logic->Plug(new mafOpImporterVTK("VTK"));
	m_Logic->Plug(new mafOpImporterMSF("MSF"));
	m_Logic->Plug(new mafOpImporterMSF1x("MSF 1.x"));
	m_Logic->Plug(new mafOpImporterMesh("Mesh"));
	m_Logic->Plug(new mafOpImporterAnsysCDBFile("Ansys CDB File"), "Finite Element");
	m_Logic->Plug(new mafOpImporterAnsysInputFile("Ansys Input File"), "Finite Element");
	m_Logic->Plug(new mafOpImporterASCII("ASCII"));

	mafGUIDicomSettings *dicomSettings=new mafGUIDicomSettings(NULL,"DICOM");
	m_Logic->Plug(new mafOpImporterDicom("DICOM"), "", true, dicomSettings);
	m_Logic->Plug(new mafOpImporterLandmark("Landmark"),"Landmark Suite");
	m_Logic->Plug(new mafOpImporterLandmarkTXT("Landmark TXT"),"Landmark Suite");
	m_Logic->Plug(new mafOpImporterLandmarkWS("Landmark WS"),"Landmark Suite");
	m_Logic->Plug(new mafOpImporterC3D("C3D"),"Landmark Suite");
	m_Logic->Plug(new mafOpImporterAnalogWS("EMG"));
	m_Logic->Plug(new mafOpImporterGRFWS("GRF"));
	m_Logic->Plug(new mafOpImporterRAWImages("RAW Images"));

	//------------------------------------------------------------
	// Exporter Menu:
	//------------------------------------------------------------

	m_Logic->Plug(new mafOpExporterMSF("MSF"));
	m_Logic->Plug(new mafOpExporterSTL("STL"));
	m_Logic->Plug(new mafOpExporterVTK("VTK"));
	m_Logic->Plug(new mafOpExporterRAW("Raw"));
	m_Logic->Plug(new mafOpExporterLandmark("Landmark"));
	m_Logic->Plug(new mafOpExporterWrappedMeter());
	m_Logic->Plug(new mafOpExporterGRFWS());
	m_Logic->Plug(new mafOpExporterMeters());
	m_Logic->Plug(new mafOpExporterAnsysCDBFile("Ansys CDB File"), "Finite Element");
	m_Logic->Plug(new mafOpExporterAnsysInputFile("Ansys Input File"), "Finite Element");
	m_Logic->Plug(new mafOpExporterMesh("Generic Mesh"), "Finite Element");

	//------------------------------------------------------------
	// Operation Menu:
	//------------------------------------------------------------

	m_Logic->Plug(new mafOpCreateVolume("Create Volume"), _("Create"));
	m_Logic->Plug(new mafOpAddLandmark("Add Landmark  \tCtrl+A"), _("Create"));
	m_Logic->Plug(new mafOpCreateGroup("Group"),_("Create"));
	m_Logic->Plug(new mafOpCreateMeter("Meter"),_("Create"));
	m_Logic->Plug(new mafOpCreateRefSys("RefSys"),_("Create"));
	m_Logic->Plug(new mafOpCreateProber("Prober"),_("Create"));
	m_Logic->Plug(new mafOpCreateSlicer("Slicer"),_("Create"));
	m_Logic->Plug(new mafOpExtractIsosurface("Extract Isosurface"),_("Create"));
	m_Logic->Plug(new mafOpCreateSurfaceParametric("Parametric Surface"),_("Create"));
	m_Logic->Plug(new mafOpCreateSpline("Spline"),_("Create"));
	m_Logic->Plug(new mafOpCreateLabeledVolume(),_("Create"));
	m_Logic->Plug(new mafOpCreateEditSkeleton(),_("Create"));
	m_Logic->Plug(new mafOpCreateSurface(),_("Create"));
	m_Logic->Plug(new mafOpCreateWrappedMeter(),_("Create"));
	m_Logic->Plug(new mafOpComputeWrapping("Computing Wrapping"),_("Create"));
	m_Logic->Plug(new mafOpFreezeVME(),_("Create"));
	m_Logic->Plug(new mafOpSegmentationRegionGrowingConnectedThreshold(),_("Create"));
	m_Logic->Plug(new mafOpSegmentationRegionGrowingLocalAndGlobalThreshold(), _("Create"));
	m_Logic->Plug(new mafOpSegmentation(), _("Create"));


	m_Logic->Plug(new mafOpClipSurface("Clip Surface"),_("Modify"));
	m_Logic->Plug(new mafOpFilterSurface("Filter Surface"),_("Modify"));
	m_Logic->Plug(new mafOpFilterVolume("Filter Volume"),_("Modify"));
	m_Logic->Plug(new mafOpDecimateSurface("Decimate Surface"),_("Modify"));
	m_Logic->Plug(new mafOpConnectivitySurface("Connectivity Surface"),_("Modify"));
	m_Logic->Plug(new mafOpEditNormals("Edit Normals"),_("Modify"));
	m_Logic->Plug(new mafOpEditMetadata("Metadata Editor"),_("Modify"));
	m_Logic->Plug(new mafOpMAFTransform("Transform"),_("Modify"));
	m_Logic->Plug(new mafOpVolumeResample("Resample Volume"),_("Modify"));
	m_Logic->Plug(new mafOpCrop("Crop Volume"),_("Modify"));
	m_Logic->Plug(new mafOpBooleanSurface("Boolean Surface"),_("Modify"));
	m_Logic->Plug(new mafOpMML("MML"),_("Modify"));
	m_Logic->Plug(new mafOpMML3("MML 3"),_("Modify"));
	m_Logic->Plug(new mafOpCropDeformableROI("Crop ROI"),_("Modify"));
	m_Logic->Plug(new mafOpFlipNormals("Flip Normals"),_("Modify"));
	m_Logic->Plug(new mafOpRemoveCells("Remove Cells"),_("Modify"));
	m_Logic->Plug(new mafOpExtrusionHoles(),_("Modify"));
	m_Logic->Plug(new mafOpTransform(), _("Modify"));
	m_Logic->Plug(new mafOpScaleDataset("Scale Dataset"),_("Modify"));
	m_Logic->Plug(new mafOpMove("Move"),_("Modify"));    
	m_Logic->Plug(new mafOpSplitSurface(),_("Modify"));
	m_Logic->Plug(new mafOpInteractiveClipSurface(),_("Modify"));
	m_Logic->Plug(new mafOpLabelizeSurface(),_("Modify"));
	m_Logic->Plug(new mafOpSmoothSurface(),_("Modify"));
	m_Logic->Plug(new mafOpCleanSurface(),_("Modify"));
	m_Logic->Plug(new mafOpTriangulateSurface(),_("Modify"));
	m_Logic->Plug(new mafOpSurfaceMirror(), _("Modify"));
	m_Logic->Plug(new mafOpVolumeMirror(), _("Modify"));
	m_Logic->Plug(new mafOpSubdivide(), _("Modify"));
	m_Logic->Plug(new mafOpFillHoles(),_("Modify"));
	m_Logic->Plug(new mafOpMeshDeformation(),_("Modify"));
	m_Logic->Plug(new mafOpMakeVMETimevarying(),_("Modify"));
	m_Logic->Plug(new mafOpEqualizeHistogram(),_("Modify"));
	m_Logic->Plug(new mafOpSmoothSurfaceCells(),_("Modify"));

	m_Logic->Plug(new mafOp2DMeasure("2D Measure"), "Measure");
	m_Logic->Plug(new mafOpVOIDensity("VOI Density"), "Measure");
	m_Logic->Plug(new mafOpVolumeMeasure("Volume"), "Measure");
	m_Logic->Plug(new mafOpMeshQuality("Mesh Quality"), "Measure");

	m_Logic->Plug(new mafOpIterativeRegistration(),_("Register"));
	m_Logic->Plug(new mafOpRegisterClusters("Clusters"),_("Register"));
	m_Logic->Plug(new mafOpClassicICPRegistration("Surface"),_("Register"));
	
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
	m_Logic->Plug(new mafViewVTK("Surface"));

	//View Arbitrary Slice
	mafViewArbitrarySlice *varbitrary= new mafViewArbitrarySlice("Arbitrary Slice");
	varbitrary->PackageView();
	m_Logic->Plug(varbitrary);

 	//View Orthoslice
	mafViewOrthoSlice *vortho= new mafViewOrthoSlice("Orthoslice");
	vortho->PackageView();
	m_Logic->Plug(vortho);

 	//View RX Compound
	mafViewRXCompound *vrx=new mafViewRXCompound("RX");
	vrx->PackageView();
	m_Logic->Plug(vrx);

 	//View Isosurface
	mafViewVTK *viso = new mafViewVTK("Isosurface");
	viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
	m_Logic->Plug(viso);

	//View Isosurface Compound
	mafViewIsosurfaceCompound *visoc = new mafViewIsosurfaceCompound("Isosurface Compound");
	visoc->PackageView();
	m_Logic->Plug(visoc);

 	//View RXCT
	mafViewRXCT *vrxct=new mafViewRXCT("RX-CT");
	vrxct->PackageView();
	m_Logic->Plug(vrxct);

 	//View Global Slice Compound
	mafViewGlobalSliceCompound *vglobal=new mafViewGlobalSliceCompound("Global Slice");
	vglobal->PackageView();
	m_Logic->Plug(vglobal);

  //View Image Compound
	mafViewImageCompound *vimageC=new mafViewImageCompound("Image");
	vimageC->PackageView();
	m_Logic->Plug(vimageC);

	//View 3D
	m_Logic->Plug(new mafView3D("3D"));

	//View Arbitrary Orthoslice
	mafViewArbitraryOrthoSlice *varbitraryortho=new mafViewArbitraryOrthoSlice("Arbitrary Orthoslice");
	varbitraryortho->PackageView();
	m_Logic->Plug(varbitraryortho);

	//View Slice On Curve Compound
	mafViewSliceOnCurveCompound *vsliceoncurveC=new mafViewSliceOnCurveCompound("Slice On Curve");
	vsliceoncurveC->PackageView();
	m_Logic->Plug(vsliceoncurveC);

	//View Image
	mafViewImage *vimage = new mafViewImage("Image");
	//vimage->PackageView();
	m_Logic->Plug(vimage);

// 	//View Compound Windowing 
// 	mafViewCompoundWindowing *vCompWind = new mafViewCompoundWindowing("Compound Windowing (mafViewCompoundWindowing)");
// 	vCompWind->PackageView();
// 	m_Logic->Plug(vCompWind);

	//View CT 
	mafViewCT *vCT = new mafViewCT("CT");
	vCT->PackageView();
	m_Logic->Plug(vCT);

	//View Global Slice 
	mafViewGlobalSlice *vGlobalS = new mafViewGlobalSlice("Global Slice");
	//vGlobalS->PackageView();
	m_Logic->Plug(vGlobalS);

	//View RX 
	mafViewRX *vRX = new mafViewRX("RX");
	//vRX->PackageView();
	m_Logic->Plug(vRX);

	//View Slice
	mafViewSlice *vslice = new mafViewSlice("Slice");
	//vslice->PackageView();
	m_Logic->Plug(vslice);

	//View Slice Blend
	mafViewSliceBlend *vSliceBlend = new mafViewSliceBlend("Slice Blend");
	//vSliceBlend->PackageView();
	m_Logic->Plug(vSliceBlend);

	//View Slice Blend RX
	mafViewSliceBlendRX *vSliceBlendRX = new mafViewSliceBlendRX("Slice Blend RX");
	vSliceBlendRX->PackageView();
	m_Logic->Plug(vSliceBlendRX);

	//View Slice Global
	mafViewSliceGlobal *vSliceGlobal = new mafViewSliceGlobal("Slice Slice Global");
	//vSliceGlobal->PackageView();
	m_Logic->Plug(vSliceGlobal);

	//View Slice Not Interpolated
	mafViewSliceNotInterpolated *vSliceNoInterp = new mafViewSliceNotInterpolated("Slice Not Interpolated");
	vSliceNoInterp->PackageView();
	m_Logic->Plug(vSliceNoInterp);

	//View Slice Not Interpolated Compound
	mafViewSliceNotInterpolatedCompound *vSliceNoInterpC = new mafViewSliceNotInterpolatedCompound("Slice Not Interpolated Compound");
	vSliceNoInterpC->PackageView();
	m_Logic->Plug(vSliceNoInterpC);

	//View Slice On Curve
	mafViewSliceOnCurve *vsliceoncurve = new mafViewSliceOnCurve("Slice On Curve");
	vsliceoncurve->PackageView();
	m_Logic->Plug(vsliceoncurve);

	//View Slicer
	mafViewSlicer *vslicer = new mafViewSlicer("Slicer");
	vslicer->PackageView();
	m_Logic->Plug(vslicer);

	//View RayCast
	mafViewVTK *vRayCast = new mafViewVTK("3D Volume Rendering");
	vRayCast->PlugVisualPipe("mafVMEVolumeGray", "mafPipeRayCast", MUTEX);
	m_Logic->Plug(vRayCast);

	//View HTML
	mafViewHTML *vHtml = new mafViewHTML("HTML");
	//vHtml->PackageView();
	m_Logic->Plug(vHtml);

	wxHandleFatalExceptions();

	//m_Logic->ShowSplashScreen();
	m_Logic->Show();
	m_Logic->Init(0,NULL); // calls FileNew - which create the root
	return TRUE;
}
//--------------------------------------------------------------------------------
int AlbaMaster::OnExit()
{
	cppDEL(m_Logic);
	return 0;
}
//--------------------------------------------------------------------------------
void AlbaMaster::OnFatalException()
{
	m_Logic->HandleException();
}
