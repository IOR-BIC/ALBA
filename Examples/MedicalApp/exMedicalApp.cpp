/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: exMedicalApp.cpp,v $
Language:  C++
Date:      $Date: 2008-04-28 09:04:28 $
Version:   $Revision: 1.14 $
Authors:   Matteo Giacomoni - Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "exMedicalApp.h"
#include "mafDecl.h"

#include "mafVMEFactory.h"
#include "medVMEFactory.h"
#include "mafPics.h"
#include "mmgMDIFrame.h"

#include "mafNodeFactory.h"
#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafPipeFactoryVME.h"
#include "medPipeFactoryVME.h"

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
		#include "medOpImporterEMGWS.h"
		#include "medOpImporterGRFWS.h"
	#endif
	#include "mmoDICOMImporter.h"
	#include "medOpImporterCTA.h"
	#include "medOpImporterCTMRI.h"
	#include "medOpImporterLandmark.h"
	#include "medOpImporterLandmarkTXT.h"
	#include "medOpImporterLandmarkWS.h"
	#include "medOpImporterRAWImages.h"
	#include "medOpImporterC3D.h"
#endif
#ifndef _DEBUG
	//EXPORTERS
	#include "mafOpExporterMSF.h"
	#include "mafOpExporterRAW.h"
	#include "mafOpExporterSTL.h"
	#include "mafOpExporterVTK.h"
	#include "medOpExporterLandmark.h"
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
	#include "mafOpExplodeCollapse.h"
	#include "mafOpExtractIsosurface.h"
	#include "mafOpFilterSurface.h"
	#include "mafOpFilterVolume.h"
	#include "mafOpMAFTransform.h"
	#include "mafOpReparentTo.h"
	#include "mafOpVolumeResample.h"
	#include "mafOpDecimateSurface.h"
	#include "mafOpConnectivitySurface.h"
	#include "mafOpBooleanSurface.h"
	#include "mafOpEditNormals.h"
	#include "mafOpCreateSurfaceParametric.h"
	#include "mafOpVOIDensity.h"
  #include "medOpScaleDataset.h"
  #include "medOpMove.h"
#ifdef MAF_USE_ITK
		#include "medOpClassicICPRegistration.h"
#endif
	#include "medOpCropDeformableROI.h"
	#include "medOpMML.h"
	#include "medOpMeshQuality.h"
	#include "medOpVolumeMeasure.h"
	#include "medOpRegisterClusters.h"
	#include "medOpFlipNormals.h"
	#include "mafOpCreateSpline.h"
	#include "mafOpRemoveCells.h"
	#include "medOpExtrusionHoles.h"
#endif

#ifndef _DEBUG
	//VIEWS
	#include "mafViewVTK.h"
	#include "mafViewCompound.h"
	#include "mafViewRXCompound.h"
	#include "mafViewRXCT.h"
	#include "mafViewOrthoSlice.h"
	#include "mafViewArbitrarySlice.h"
	#include "mafViewGlobalSliceCompound.h"
	#include "mafViewSingleSliceCompound.h"
	#include "mafViewImageCompound.h"
	#include "mafView3D.h"
#endif
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(exMedicalApp)

//--------------------------------------------------------------------------------
bool exMedicalApp::OnInit()
//--------------------------------------------------------------------------------
{
	mafPics.Initialize();	

	int result;
	result = medVMEFactory::Initialize();
	assert(result==MAF_OK);

	// Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
	result = medPipeFactoryVME::Initialize();
	assert(result==MAF_OK);

	m_Logic = new medLogicWithManagers();
	m_Logic->GetTopWin()->SetTitle("Medical example");

	//m_Logic->PlugTimebar(false);
	//m_Logic->PlugMenu(false);
	//m_Logic->PlugToolbar(false);
	//m_Logic->PlugLogbar(false);
	//m_Logic->PlugSidebar(true, mafSideBar::SINGLE_NOTEBOOK);
	//m_Logic->PlugOpManager(false);
	//m_Logic->PlugViewManager(false);
	//m_Logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005: 
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
	m_Logic->Plug(new mafOpImporterMSF1x("MSF 1.x"));
	m_Logic->Plug(new mafOpImporterMesh("Mesh"));
#ifdef MAF_USE_ITK
		m_Logic->Plug(new mafOpImporterASCII("ASCII"));
#endif
	m_Logic->Plug(new mmoDICOMImporter("DICOM"),"DICOM Suite");
	m_Logic->Plug(new medOpImporterCTA("CTA-DSA"),"DICOM Suite");
	m_Logic->Plug(new medOpImporterCTMRI("CT"),"DICOM Suite");
	m_Logic->Plug(new medOpImporterCTMRI("MRI"),"DICOM Suite");
	m_Logic->Plug(new medOpImporterLandmark("Landmark"),"Landmark Suite");
	m_Logic->Plug(new medOpImporterLandmarkTXT("Landmark TXT"),"Landmark Suite");
	m_Logic->Plug(new medOpImporterLandmarkWS("Landmark WS"),"Landmark Suite");
	m_Logic->Plug(new medOpImporterC3D("C3D"),"Landmark Suite");
#ifdef MAF_USE_ITK
		m_Logic->Plug(new medOpImporterEMGWS("EMG"));
		m_Logic->Plug(new medOpImporterGRFWS("GRF"));
#endif
	m_Logic->Plug(new medOpImporterRAWImages("RAW Images"));
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
	m_Logic->Plug(new medOpExporterLandmark("Landmark"));
#endif
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Operation Menu':
	//------------------------------------------------------------
#ifndef _DEBUG
	m_Logic->Plug(new mafOp2DMeasure("2D Measure"),"Measure");
	m_Logic->Plug(new mafOpVOIDensity("VOI Density"),"Measure");
	m_Logic->Plug(new medOpVolumeMeasure("Volume"),"Measure");
	m_Logic->Plug(new medOpMeshQuality("Mesh Quality"),"Measure");

	m_Logic->Plug(new mafOpAddLandmark("Add Landmark"),"Create");
	m_Logic->Plug(new mafOpCreateGroup("Group"),"Create");
	m_Logic->Plug(new mafOpCreateMeter("Meter"),"Create");
	m_Logic->Plug(new mafOpCreateRefSys("RefSys"),"Create");
	m_Logic->Plug(new mafOpCreateProber("Prober"),"Create");
	m_Logic->Plug(new mafOpCreateSlicer("Slicer"),"Create");
	m_Logic->Plug(new mafOpExtractIsosurface("Extract Isosurface"),"Create");
	m_Logic->Plug(new mafOpCreateSurfaceParametric("Parametric Surface"),"Create");
	m_Logic->Plug(new mafOpCreateSpline("Spline"),"Create");

	m_Logic->Plug(new mafOpClipSurface("Clip Surface"),"Modify");
	m_Logic->Plug(new mafOpFilterSurface("Filter Surface"),"Modify");
	m_Logic->Plug(new mafOpFilterVolume("Filter Volume"),"Modify");
	m_Logic->Plug(new mafOpDecimateSurface("Decimate Surface"),"Modify");
	m_Logic->Plug(new mafOpConnectivitySurface("Connectivity Surface"),"Modify");
	m_Logic->Plug(new mafOpEditNormals("Edit Normals"),"Modify");
	m_Logic->Plug(new mafOpEditMetadata("Metadata Editor"),"Modify");
	m_Logic->Plug(new mafOpExplodeCollapse("Explode/Collapse cloud"),"Modify");
	m_Logic->Plug(new mafOpMAFTransform("Transform"),"Modify");
	m_Logic->Plug(new mafOpReparentTo("Reparent to..."),"Modify");
	m_Logic->Plug(new mafOpVolumeResample("Resample Volume"),"Modify");
	m_Logic->Plug(new mafOpCrop("Crop Volume"),"Modify");
	m_Logic->Plug(new mafOpBooleanSurface("Boolean Surface"),"Modify");
	m_Logic->Plug(new medOpMML("MML"),"Modify");
	m_Logic->Plug(new medOpCropDeformableROI("Crop ROI"),"Modify");
	m_Logic->Plug(new medOpFlipNormals("Flip Normals"),"Modify");
	m_Logic->Plug(new mafOpRemoveCells("Remove Cells"),"Modify");
	m_Logic->Plug(new medOpExtrusionHoles(),"Modify");
  m_Logic->Plug(new medOpScaleDataset("Scale Dataset"),"Modify");
  m_Logic->Plug(new medOpMove("Move"),"Modify");    
	m_Logic->Plug(new medOpRegisterClusters("Clusters"),"Register");
#ifdef MAF_USE_ITK
		m_Logic->Plug(new medOpClassicICPRegistration("Surface"),"Register");
#endif
#endif
	//------------------------------------------------------------

	//------------------------------------------------------------
	// View Menu':
	//------------------------------------------------------------
#ifndef _DEBUG

	//View VTK
	m_Logic->Plug(new mafViewVTK("VTK view"));
	//View Arbitrary Slice
	mafViewArbitrarySlice *varbitrary= new mafViewArbitrarySlice("Arbitrary Slice");
	varbitrary->PackageView();
	m_Logic->Plug(varbitrary);
	//View Orthoslice
	mafViewOrthoSlice *vortho= new mafViewOrthoSlice("Orthoslice");
	vortho->PackageView();
	m_Logic->Plug(vortho);
	//View RX
	mafViewRXCompound *vrx=new mafViewRXCompound("RX");
	vrx->PackageView();
	m_Logic->Plug(vrx);
	//View Isosurface
	mafViewVTK *viso = new mafViewVTK("Isosurface");
	viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
	m_Logic->Plug(viso);
	//View RXCT
	mafViewRXCT *vrxct=new mafViewRXCT("RX-CT");
	vrxct->PackageView();
	m_Logic->Plug(vrxct);
	//View Global Slice
	mafViewGlobalSliceCompound *vglobal=new mafViewGlobalSliceCompound("Global Slice");
	vglobal->PackageView();
	m_Logic->Plug(vglobal);
	//View Image
	mafViewImageCompound *vimage=new mafViewImageCompound("Image");
	vimage->PackageView();
	m_Logic->Plug(vimage);
	//View Single Slice
	mafViewSingleSliceCompound *vsingleslice=new mafViewSingleSliceCompound("Single Slice");
	vsingleslice->PackageView();
	m_Logic->Plug(vsingleslice);
	//View 3D
	mafView3D *v3d=new mafView3D("3D");
	m_Logic->Plug(v3d);
#endif
	//------------------------------------------------------------

	//wxHandleFatalExceptions();

	//m_Logic->ShowSplashScreen();
	m_Logic->Show();
	mafString app_stamp;
	app_stamp = "OPEN_ALL_DATA";
	m_Logic->SetApplicationStamp(app_stamp);
	m_Logic->Init(0,NULL); // calls FileNew - which create the root
	return TRUE;
}
//--------------------------------------------------------------------------------
int exMedicalApp::OnExit()
//--------------------------------------------------------------------------------
{
	cppDEL(m_Logic);
	return 0;
}
//--------------------------------------------------------------------------------
void exMedicalApp::OnFatalException()
//--------------------------------------------------------------------------------
{
	m_Logic->HandleException();
}
