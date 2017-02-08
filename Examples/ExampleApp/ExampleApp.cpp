/*=========================================================================

 Program: MAF2
 Module: ExampleApp
 Authors: Matteo Giacomoni - Daniele Giunchi - Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//Enable This for memory leak detection
//#include <vld.h>

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "ExampleApp.h"
#include "mafDecl.h"

#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mafGUIMDIFrame.h"

#include "mafVMEFactory.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafPipeFactoryVME.h"

#include "mafServiceLocator.h"

//Comment this line to disable wizard sample
#define USE_WIZARD

#undef _DEBUG

#ifdef USE_WIZARD
  #include "mafWizard.h"
  #include "exWizardSample.h"
#endif

#ifndef _DEBUG
//IMPORTERS
#include "mafOpImporterImage.h"
#include "mafOpImporterVRML.h"
#include "mafOpImporterVTK.h"
#include "mafOpImporterSTL.h"
#include "mafOpImporterMSF1x.h"
#include "mafOpImporterRAWVolume.h"
#include "mafOpImporterMesh.h"
#ifdef MAF_USE_ITK
#include "mafOpImporterASCII.h"
#include "mafOpImporterAnalogWS.h"
#include "mafOpImporterGRFWS.h"
#endif
#include "mafGUIDicomSettings.h"
#include "mafOpImporterDicomOffis.h"
#include "mafOpImporterLandmark.h"
#include "mafOpImporterLandmarkTXT.h"
#include "mafOpImporterLandmarkWS.h"
#include "mafOpImporterRAWImages.h"
#include "mafOpImporterC3D.h"
#include "mafOpImporterAnsysCDBFile.h"
#include "mafOpImporterAnsysInputFile.h"
#include "mafOpCreateVolume.h"
#endif
#ifndef _DEBUG
//EXPORTERS
#include "mafOpExporterMSF.h"
#include "mafOpExporterRAW.h"
#include "mafOpExporterSTL.h"
#include "mafOpExporterVTK.h"
#include "mafOpExporterLandmark.h"
#include "mafOpExporterWrappedMeter.h"
#include "mafOpExporterGRFWS.h"
#include "mafOpExporterMeters.h"
#include "mafOpExporterAnsysCDBFile.h"
#include "mafOpExporterAnsysInputFile.h"
#endif
#ifndef _DEBUG
//OPERATIONS
#include "mafOp2DMeasure.h"
#include "mafOpAddLandmark.h"
#include "mafOpClipSurface.h"
#include "mafOpCreateGroup.h"
#include "mafOpCreateMeter.h"
#include "mafOpCreateProber.h"
#include "mafOpCreateRefSys.h"
#include "mafOpCreateSlicer.h"
#include "mafOpCrop.h"
#include "mafOpEditMetadata.h"
#include "mafOpExtractIsosurface.h"
#include "mafOpFilterSurface.h"
#include "mafOpFilterVolume.h"
#include "mafOpMAFTransform.h"
#include "mafOpVolumeResample.h"
#include "mafOpDecimateSurface.h"
#include "mafOpConnectivitySurface.h"
#include "mafOpBooleanSurface.h"
#include "mafOpEditNormals.h"
#include "mafOpCreateSurfaceParametric.h"
#include "mafOpVOIDensity.h"
#include "mafOpScaleDataset.h"
#include "mafOpTransform.h"
#include "mafOpMove.h"
#ifdef MAF_USE_ITK
#include "mafOpClassicICPRegistration.h"
#endif
#include "mafOpCropDeformableROI.h"
#include "mafOpMML.h"
#include "mafOpMeshQuality.h"
#include "mafOpVolumeMeasure.h"
#include "mafOpRegisterClusters.h"
#include "mafOpFlipNormals.h"
#include "mafOpCreateSpline.h"
#include "mafOpRemoveCells.h"
#include "mafOpExtrusionHoles.h"
#include "mafOpCreateLabeledVolume.h"
#include "mafOpSplitSurface.h"
#include "mafOpInteractiveClipSurface.h"
#include "mafOpCreateEditSkeleton.h"
#include "mafOpCreateSurface.h"
#include "mafOpCreateWrappedMeter.h"
#include "mafOpComputeWrapping.h"
#include "mafOpLabelizeSurface.h"
#include "mafOpFreezeVME.h"
#include "mafOpIterativeRegistration.h"
#include "mafOpCleanSurface.h"
#include "mafOpSmoothSurface.h"
#include "mafOpTriangulateSurface.h"
#include "mafOpSurfaceMirror.h"
#include "mafOpVolumeMirror.h"
#include "mafOpSubdivide.h"
#include "mafOpFillHoles.h"
#include "mafOpMeshDeformation.h"
#include "mafOpMakeVMETimevarying.h"
#include "mafOpSmoothSurfaceCells.h"
#include "mafOpEqualizeHistogram.h"
#include "mafOpMML3.h"
#include "mafOpSegmentationRegionGrowingConnectedThreshold.h"
#include "mafOpSegmentationRegionGrowingLocalAndGlobalThreshold.h"
#include "mafOpMergeDicomSeries.h"
#endif

#include "mafViewVTK.h"

#ifndef _DEBUG
//VIEWS

#include "mafViewCompound.h"
#include "mafViewRXCompound.h"
#include "mafViewRXCT.h"
#include "mafViewOrthoSlice.h"
#include "mafViewArbitrarySlice.h"
#include "mafViewGlobalSliceCompound.h"
#include "mafViewSingleSliceCompound.h"
#include "mafViewImageCompound.h"
#include "mafView3D.h"
#include "mafViewArbitraryOrthoSlice.h"
#include "mafViewSliceOnCurveCompound.h"

#include "mafViewCompoundWindowing.h"
#include "mafViewImage.h"
#include "mafViewCT.h"
#include "mafViewGlobalSlice.h"
#include "mafViewRX.h"
#include "mafViewSingleSlice.h"
#include "mafViewSlice.h"
#include "mafViewSliceBlend.h"
#include "mafViewSliceBlendRX.h"
#include "mafViewSliceGlobal.h"
#include "mafViewSliceNotInterpolated.h"
#include "mafViewSliceNotInterpolatedCompound.h"
#include "mafViewSliceOnCurve.h"
#include "mafViewSlicer.h"
#include "mafViewVTKCompound.h"
#include "mafViewPlot.h"
#include "mafViewHTML.h"
#include "mafViewIsosurfaceCompound.h"
#include "mafOpImporterMSF.h"
#endif
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(ExampleApp)

//--------------------------------------------------------------------------------
bool ExampleApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPictureFactory::GetPictureFactory()->Initialize();	

#include "Examples/MedicalIcons/FRAME_ICON16x16.xpm"
	mafADDPIC(FRAME_ICON16x16);
#include "Examples/MedicalIcons/FRAME_ICON32x32.xpm"
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
  
	m_Logic->GetTopWin()->SetTitle("ExampleApp");

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
	m_Logic->Configure();

	SetTopWindow(mafGetFrame());  

	//------------------------------------------------------------
	// Importer Menu':
	//------------------------------------------------------------
#ifndef _DEBUG
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
#ifdef MAF_USE_ITK
	m_Logic->Plug(new mafOpImporterASCII("ASCII"));
#endif
	mafGUIDicomSettings *dicomSettings=new mafGUIDicomSettings(NULL,"DICOM");
	m_Logic->Plug(new mafOpImporterDicomOffis("DICOM"),"DICOM Suite",true,dicomSettings);
	m_Logic->Plug(new mafOpImporterLandmark("Landmark"),"Landmark Suite");
	m_Logic->Plug(new mafOpImporterLandmarkTXT("Landmark TXT"),"Landmark Suite");
	m_Logic->Plug(new mafOpImporterLandmarkWS("Landmark WS"),"Landmark Suite");
	m_Logic->Plug(new mafOpImporterC3D("C3D"),"Landmark Suite");
#ifdef MAF_USE_ITK
	m_Logic->Plug(new mafOpImporterAnalogWS("EMG"));
	m_Logic->Plug(new mafOpImporterGRFWS("GRF"));
#endif
	m_Logic->Plug(new mafOpImporterRAWImages("RAW Images"));
#endif
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Exporter Menu':
	//------------------------------------------------------------
#ifndef _DEBUG
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
#endif
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Operation Menu':
	//------------------------------------------------------------
#ifndef _DEBUG
	m_Logic->Plug(new mafOp2DMeasure("2D Measure"),"Measure");
	m_Logic->Plug(new mafOpVOIDensity("VOI Density"),"Measure");
	m_Logic->Plug(new mafOpVolumeMeasure("Volume"),"Measure");
	m_Logic->Plug(new mafOpMeshQuality("Mesh Quality"),"Measure");

	m_Logic->Plug(new mafOpCreateVolume("Create Volume"), _("Create"));
	m_Logic->Plug(new mafOpAddLandmark("Add Landmark"), _("Create"));
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
	m_Logic->Plug(new mafOpSegmentationRegionGrowingLocalAndGlobalThreshold(),_("Create"));

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

	m_Logic->Plug(new mafOpIterativeRegistration(),_("Register"));
	m_Logic->Plug(new mafOpRegisterClusters("Clusters"),_("Register"));
#ifdef MAF_USE_ITK
	m_Logic->Plug(new mafOpClassicICPRegistration("Surface"),_("Register"));
#endif
	m_Logic->Plug(new mafOpMergeDicomSeries("Merge Dicom"), "");
#endif
	//------------------------------------------------------------


#ifdef USE_WIZARD
  //------------------------------------------------------------
  // Wizard Menu':
  //------------------------------------------------------------

  //A simple wizard sample
  m_Logic->Plug(new exWizardSample("Import loop"),"");
#endif

	//------------------------------------------------------------
	// View Menu':
	//------------------------------------------------------------
	//View VTK

	//View VTK
	m_Logic->Plug(new mafViewVTK("Surface (mafViewVTK)"));

	//View Arbitrary Slice
	mafViewArbitrarySlice *varbitrary= new mafViewArbitrarySlice("Arbitrary Slice (mafViewArbitrarySlice)");
	varbitrary->PackageView();
	m_Logic->Plug(varbitrary);

 	//View Orthoslice
	mafViewOrthoSlice *vortho= new mafViewOrthoSlice("Orthoslice (mafViewOrthoSlice)");
	vortho->PackageView();
	m_Logic->Plug(vortho);

 	//View RX Compound
	mafViewRXCompound *vrx=new mafViewRXCompound("RX (mafViewRXCompound)");
	vrx->PackageView();
	m_Logic->Plug(vrx);

 	//View Isosurface
	mafViewVTK *viso = new mafViewVTK("Isosurface (mafViewVTK)");
	viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
	m_Logic->Plug(viso);

	//View Isosurface Compound
	mafViewIsosurfaceCompound *visoc = new mafViewIsosurfaceCompound("Isosurface Compound (mafViewIsosurfaceCompound)");
	visoc->PackageView();
	m_Logic->Plug(visoc);

 	//View RXCT
	mafViewRXCT *vrxct=new mafViewRXCT("RX-CT (mafViewRXCT)");
	vrxct->PackageView();
	m_Logic->Plug(vrxct);

 	//View Global Slice Compound
	mafViewGlobalSliceCompound *vglobal=new mafViewGlobalSliceCompound("Global Slice (mafViewGlobalSliceCompound)");
	vglobal->PackageView();
	m_Logic->Plug(vglobal);

  //View Image Compound
	mafViewImageCompound *vimageC=new mafViewImageCompound("Image (mafViewImageCompound)");
	vimageC->PackageView();
	m_Logic->Plug(vimageC);

	//View Single Slice Compound
	mafViewSingleSliceCompound *vsinglesliceC=new mafViewSingleSliceCompound("Single Slice (mafViewSingleSliceCompound)");
	vsinglesliceC->PackageView();
	m_Logic->Plug(vsinglesliceC);

	//View 3D
	m_Logic->Plug(new mafView3D("3D (mafView3D)"));

	//View Arbitrary Orthoslice
	mafViewArbitraryOrthoSlice *varbitraryortho=new mafViewArbitraryOrthoSlice("Arbitrary Orthoslice (mafViewArbitraryOrthoSlice)");
	varbitraryortho->PackageView();
	m_Logic->Plug(varbitraryortho);

	//View Slice On Curve Compound
	mafViewSliceOnCurveCompound *vsliceoncurveC=new mafViewSliceOnCurveCompound("Slice On Curve (mafViewSliceOnCurveCompound)");
	vsliceoncurveC->PackageView();
	m_Logic->Plug(vsliceoncurveC);

	//

	//View Image
	mafViewImage *vimage = new mafViewImage("Image (mafViewImage)");
	//vimage->PackageView();
	m_Logic->Plug(vimage);

// 	//View Compound Windowing 
// 	mafViewCompoundWindowing *vCompWind = new mafViewCompoundWindowing("Compound Windowing (mafViewCompoundWindowing)");
// 	vCompWind->PackageView();
// 	m_Logic->Plug(vCompWind);

	//View CT 
	mafViewCT *vCT = new mafViewCT("CT (mafViewCT)");
	vCT->PackageView();
	m_Logic->Plug(vCT);

	//View Global Slice 
	mafViewGlobalSlice *vGlobalS = new mafViewGlobalSlice("Global Slice (mafViewGlobalSlice)");
	//vGlobalS->PackageView();
	m_Logic->Plug(vGlobalS);

	//View RX 
	mafViewRX *vRX = new mafViewRX("RX (mafViewRX)");
	//vRX->PackageView();
	m_Logic->Plug(vRX);

	//View Single Slice
	mafViewSingleSlice*vsingleslice = new mafViewSingleSlice("Single Slice (mafViewSingleSlice)");
	//vsingleslice->PackageView();
	m_Logic->Plug(vsingleslice);

	//View Slice
	mafViewSlice *vslice = new mafViewSlice("Slice (mafViewSlice)");
	//vslice->PackageView();
	m_Logic->Plug(vslice);

	//View Slice Blend
	mafViewSliceBlend *vSliceBlend = new mafViewSliceBlend("Slice Blend (mafViewSliceBlend)");
	//vSliceBlend->PackageView();
	m_Logic->Plug(vSliceBlend);

	//View Slice Blend RX
	mafViewSliceBlendRX *vSliceBlendRX = new mafViewSliceBlendRX("Slice Blend RX (mafViewSliceBlendRX)");
	vSliceBlendRX->PackageView();
	m_Logic->Plug(vSliceBlendRX);

	//View Slice Global
	mafViewSliceGlobal *vSliceGlobal = new mafViewSliceGlobal("Slice Slice Global (mafViewSliceGlobal)");
	//vSliceGlobal->PackageView();
	m_Logic->Plug(vSliceGlobal);

	//View Slice Not Interpolated
	mafViewSliceNotInterpolated *vSliceNoInterp = new mafViewSliceNotInterpolated("Slice Not Interpolated (mafViewSliceNotInterpolated)");
	vSliceNoInterp->PackageView();
	m_Logic->Plug(vSliceNoInterp);

	//View Slice Not Interpolated Compound
	mafViewSliceNotInterpolatedCompound *vSliceNoInterpC = new mafViewSliceNotInterpolatedCompound("Slice Not Interpolated (mafViewSliceNotInterpolatedCompound)");
	vSliceNoInterpC->PackageView();
	m_Logic->Plug(vSliceNoInterpC);

	//View Slice On Curve
	mafViewSliceOnCurve *vsliceoncurve = new mafViewSliceOnCurve("Slice On Curve (mafViewSliceOnCurve)");
	vsliceoncurve->PackageView();
	m_Logic->Plug(vsliceoncurve);

	//View Slicer
	mafViewSlicer *vslicer = new mafViewSlicer("Slicer (mafViewSlicer)");
	vslicer->PackageView();
	m_Logic->Plug(vslicer);

	//View VTK Compound
// 	mafViewVTKCompound *vVTKC = new mafViewVTKCompound("VTK (mafViewVTKCompound)");
// 	vVTKC->PackageView();
// 	m_Logic->Plug(vVTKC);

	//View Plot
	mafViewPlot *vplot = new mafViewPlot("Plot (mafViewPlot)");
	//vplot->PackageView();
	m_Logic->Plug(vplot);

	//View HTML
	mafViewHTML *vHtml = new mafViewHTML("HTML (mafViewHTML)");
	//vHtml->PackageView();
	m_Logic->Plug(vHtml);

	//View Compound
	mafViewCompound *vComp = new mafViewCompound("Compound (mafViewCompound)");
	vComp->PackageView();
	m_Logic->Plug(vComp);

	//wxHandleFatalExceptions();

	//m_Logic->ShowSplashScreen();
	m_Logic->Show();
	m_Logic->Init(0,NULL); // calls FileNew - which create the root
	return TRUE;
}
//--------------------------------------------------------------------------------
int ExampleApp::OnExit()
//--------------------------------------------------------------------------------
{
	cppDEL(m_Logic);
	return 0;
}
//--------------------------------------------------------------------------------
void ExampleApp::OnFatalException()
//--------------------------------------------------------------------------------
{
	m_Logic->HandleException();
}
