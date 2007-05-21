/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: exMedicalApp.cpp,v $
Language:  C++
Date:      $Date: 2007-05-21 08:35:14 $
Version:   $Revision: 1.5 $
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

//IMPORTERS
#include "mmoImageImporter.h"
#include "mmoVRMLImporter.h"
#include "mmoVTKImporter.h"
#include "mmoSTLImporter.h"
#include "mmoMSF1xImporter.h"
#include "mmoRAWImporterVolume.h"
#include "mmoMeshImporter.h"
#ifdef MAF_USE_ITK
	#include "mmoASCIIImporter.h"
	#include "mmoEMGImporterWS.h"
	#include "mmoGRFImporterWS.h"
#endif
#include "mmoDICOMImporter.h"
#include "mmoCTAImporter.h"
#include "mmoCTMRIImporter.h"
#include "mmoLandmarkImporter.h"
#include "mmoLandmarkImporterTXT.h"
#include "mmoLandmarkImporterWS.h"
#include "mmoRAWImporterImages.h"
#include "mmoC3DImporter.h"
//EXPORTERS
#include "mmoMSFExporter.h"
#include "mmoRAWExporter.h"
#include "mmoSTLExporter.h"
#include "mmoVTKExporter.h"
#include "mmoLandmarkExporter.h"
//OPERATIONS
#include "mmo2DMeasure.h"
#include "mmoAddLandmark.h"
#include "mmoClipSurface.h"
#include "mmoCreateGroup.h"
#include "mmoCreateMeter.h"
#include "mmoCreateProber.h"
#include "mmoCreateRefSys.h"
#include "mmoCreateSlicer.h"
#include "mmoCrop.h"
#include "mmoEditMetadata.h"
#include "mmoExplodeCollapse.h"
#include "mmoExtractIsosurface.h"
#include "mmoFilterSurface.h"
#include "mmoFilterVolume.h"
#include "mmoMAFTransform.h"
#include "mmoReparentTo.h"
#include "mmoVolumeResample.h"
#include "mmoDecimateSurface.h"
#include "mmoConnectivitySurface.h"
#include "mmoBooleanSurface.h"
#include "mmoEditNormals.h"
#include "mmoCreateSurfaceParametric.h"
#include "mmoVOIDensity.h"
#ifdef MAF_USE_ITK
	#include "mmoClassicICPRegistration.h"
#endif
#include "mmoCropDeformableROI.h"
#include "mmoMML.h"
#include "mmoMeshQuality.h"
#include "mmoVolumeMeasure.h"
#include "mmoRegisterClusters.h"
#include "mmoFlipNormals.h"
#include "mmoCreateSpline.h"
#include "mmoRemoveCells.h"
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

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(exMedicalApp)

//--------------------------------------------------------------------------------
bool exMedicalApp::OnInit()
//--------------------------------------------------------------------------------
{
	mafPics.Initialize();	

	int result = mafVMEFactory::Initialize();
	assert(result==MAF_OK);

	result = medVMEFactory::Initialize();
	assert(result==MAF_OK);

	// Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
	result = mafPipeFactoryVME::Initialize();
	assert(result==MAF_OK);

	result = medPipeFactoryVME::Initialize();
	assert(result==MAF_OK);

	m_Logic = new mafLogicWithManagers();
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
#ifdef _DEBUG
	m_Logic->Plug(new mmoImageImporter("Images"));
	m_Logic->Plug(new mmoRAWImporterVolume("RAW Volume"));
	m_Logic->Plug(new mmoSTLImporter("STL"));
	m_Logic->Plug(new mmoVRMLImporter("VRML"));
	m_Logic->Plug(new mmoVTKImporter("VTK"));
	m_Logic->Plug(new mmoMSF1xImporter("MSF 1.x"));
	m_Logic->Plug(new mmoMeshImporter("Mesh"));
	#ifdef MAF_USE_ITK
		m_Logic->Plug(new mmoASCIIImporter("ASCII"));
	#endif
	m_Logic->Plug(new mmoDICOMImporter("DICOM"),"DICOM Suite");
	m_Logic->Plug(new mmoCTAImporter("CTA-DSA"),"DICOM Suite");
	m_Logic->Plug(new mmoCTMRIImporter("CT"),"DICOM Suite");
	m_Logic->Plug(new mmoCTMRIImporter("MRI"),"DICOM Suite");
	m_Logic->Plug(new mmoLandmarkImporter("Landmark"),"Landmark Suite");
	m_Logic->Plug(new mmoLandmarkImporterTXT("Landmark TXT"),"Landmark Suite");
	m_Logic->Plug(new mmoLandmarkImporterWS("Landmark WS"),"Landmark Suite");
	m_Logic->Plug(new mmoC3DImporter("C3D"),"Landmark Suite");
	#ifdef MAF_USE_ITK
		m_Logic->Plug(new mmoEMGImporterWS("EMG"));
		m_Logic->Plug(new mmoGRFImporterWS("GRF"));
	#endif
	m_Logic->Plug(new mmoRAWImporterImages("RAW Images"));
#endif
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Exporter Menu':
	//------------------------------------------------------------
#ifdef _DEBUG
	m_Logic->Plug(new mmoMSFExporter("MSF"));
	m_Logic->Plug(new mmoSTLExporter("STL"));
	m_Logic->Plug(new mmoVTKExporter("VTK"));
	m_Logic->Plug(new mmoRAWExporter("Raw"));
	m_Logic->Plug(new mmoLandmarkExporter("Landmark"));
#endif
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Operation Menu':
	//------------------------------------------------------------
	//MAF
#ifdef _DEBUG
	m_Logic->Plug(new mmo2DMeasure("2D Measure"),"Measure");
	m_Logic->Plug(new mmoVOIDensity("VOI Density"),"Measure");
	m_Logic->Plug(new mmoVolumeMeasure("Volume"),"Measure");
	m_Logic->Plug(new mmoMeshQuality("Mesh Quality"),"Measure");

	m_Logic->Plug(new mmoAddLandmark("Add Landmark"),"Create");
	m_Logic->Plug(new mmoCreateGroup("Group"),"Create");
	m_Logic->Plug(new mmoCreateMeter("Meter"),"Create");
	m_Logic->Plug(new mmoCreateRefSys("RefSys"),"Create");
	m_Logic->Plug(new mmoCreateProber("Prober"),"Create");
	m_Logic->Plug(new mmoCreateSlicer("Slicer"),"Create");
	m_Logic->Plug(new mmoExtractIsosurface("Extract Isosurface"),"Create");
	m_Logic->Plug(new mmoCreateSurfaceParametric("Parametric Surface"),"Create");
	m_Logic->Plug(new mmoCreateSpline("Spline"),"Create");

	m_Logic->Plug(new mmoClipSurface("Clip Surface"),"Modify");
	m_Logic->Plug(new mmoFilterSurface("Filter Surface"),"Modify");
	m_Logic->Plug(new mmoFilterVolume("Filter Volume"),"Modify");
	m_Logic->Plug(new mmoDecimateSurface("Decimate Surface"),"Modify");
	m_Logic->Plug(new mmoConnectivitySurface("Connectivity Surface"),"Modify");
	m_Logic->Plug(new mmoEditNormals("Edit Normals"),"Modify");
	m_Logic->Plug(new mmoEditMetadata("Metadata Editor"),"Modify");
	m_Logic->Plug(new mmoExplodeCollapse("Explode/Collapse cloud"),"Modify");
	m_Logic->Plug(new mmoMAFTransform("Transform"),"Modify");
	m_Logic->Plug(new mmoReparentTo("Reparent to..."),"Modify");
	m_Logic->Plug(new mmoVolumeResample("Resample Volume"),"Modify");
	m_Logic->Plug(new mmoCrop("Crop Volume"),"Modify");
	m_Logic->Plug(new mmoBooleanSurface("Boolean Surface"),"Modify");
	m_Logic->Plug(new mmoMML("MML"),"Modify");
	m_Logic->Plug(new mmoCropDeformableROI("Crop ROI"),"Modify");
	m_Logic->Plug(new mmoFlipNormals("Flip Normals"),"Modify");
	m_Logic->Plug(new mmoRemoveCells("Remove Cells"),"Modify");

	m_Logic->Plug(new mmoRegisterClusters("Clusters"),"Register");
	#ifdef MAF_USE_ITK
		m_Logic->Plug(new mmoClassicICPRegistration("Surface"),"Register");
	#endif
#endif
	//------------------------------------------------------------

	//------------------------------------------------------------
	// View Menu':
	//------------------------------------------------------------

#ifdef _DEBUG
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
