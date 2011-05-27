/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: iPoseApp.cpp,v $
  Language:  C++
  Date:      $Date: 2011-05-27 07:40:46 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "iPoseApp.h"

#include <wx/datetime.h>
#include <wx/config.h>

#include "mafDecl.h"
#include "mafLogicWithManagers.h"
#include "mafPics.h"
#include "mafGUIMDIFrame.h"
#include "mafInteractionFactory.h"
#include "medPipeFactoryVME.h"
#include "medVMEFactory.h"
#include "medOpImporterDicomOffis.h"
#include "mafOpImporterSTL.h"
#include "mafOpExporterSTL.h"
#include "medOpMove.h"
#include "mafViewVTK.h"
#include "mafViewOrthoSlice.h"
#include "medGUIDicomSettings.h"
#include "mafOp2DMeasure.h"

#include "vtkTimerLog.h"


// TODO: REFACTOR THIS 
// this component is used only to override the Accept,  
// and it`s the minimal amount of code in order to override the method
// it could go in a separate file with other redefined Accept`s

class iposeOpMove : public medOpMove
{
public:

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme)
  {
    bool accepted = false;
    
    accepted = vme->IsA("mafVMESurface");

    if (accepted == false)
    {
      return false;
    }
    else
    {
      return medOpMove::Accept(vme);
    }
  }
};
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------

IMPLEMENT_APP(iPoseApp)

//--------------------------------------------------------------------------------
bool iPoseApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPictureFactory::GetPictureFactory()->Initialize();
  #include "iPose/pic/FRAME_ICON16x16.xpm"
  mafADDPIC(FRAME_ICON16x16);
  
  #include "iPose/pic/FRAME_ICON32x32.xpm"
  mafADDPIC(FRAME_ICON32x32);
    
  #include "iPose/pic/MDICHILD_ICON.xpm"
  mafADDPIC(MDICHILD_ICON);

  int result;
 
  result = medVMEFactory::Initialize();
  assert(result == MAF_OK);

	result = medPipeFactoryVME::Initialize();
	assert(result==MAF_OK);

  result = mafInteractionFactory::Initialize();
  assert(result==MAF_OK);

  m_Logic = new mafLogicWithManagers();
  m_Logic->GetTopWin()->SetTitle("iPose");
  m_Logic->Configure();
  SetTopWindow(mafGetFrame());  

	wxRegKey RegKey(wxString("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\iPose"));
	if(RegKey.Exists())
	{
		RegKey.Create();
		wxString revision;
		RegKey.QueryValue(wxString("DisplayVersion"), revision);
		m_Logic->SetRevision(revision);
	}
	else
	{
		wxString revision="0.1";
		m_Logic->SetRevision(revision);
	}

  //------------------------- Importers -------------------------
  medGUIDicomSettings *dicomSettings=new medGUIDicomSettings(NULL,"DICOM");
  m_Logic->Plug(new medOpImporterDicomOffis("DICOM"),"Images",true,dicomSettings);
  m_Logic->Plug(new mafOpImporterSTL("STL"),"Geometries");
  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  m_Logic->Plug(new mafOpExporterSTL("STL"),"Geometries");
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  m_Logic->Plug(new iposeOpMove(),"Move");
  m_Logic->Plug(new mafOp2DMeasure(),"Measure");
  //-------------------------------------------------------------

  //------------------------- Views -------------------------
  mafViewVTK *viso = new mafViewVTK("Isosurface");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("mafVMEVolumeLarge","mafPipeIsosurface",MUTEX);
  m_Logic->Plug(viso);

  mafViewOrthoSlice *viewOrthoSlice = new mafViewOrthoSlice("OrthoSlice");
  viewOrthoSlice->PackageView();
  m_Logic->Plug(viewOrthoSlice);
  //-------------------------------------------------------------

  wxBitmap splashBitmap;
  splashBitmap.LoadFile("../Splash/iposeSplash.bmp", wxBITMAP_TYPE_BMP);
  m_Logic->ShowSplashScreen(splashBitmap); 

  // show the application
  m_Logic->Show();

  m_Logic->Init(0,NULL); // calls FileNew - which create the root

  return TRUE;
}
//--------------------------------------------------------------------------------
int iPoseApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_Logic);

  //this hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  return 0;
}
