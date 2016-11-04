/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "mafDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafTest.h"

#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageSource.h"

#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPNGWriter.h"
#include "vtkPNGReader.h"
#include "vtkBMPReader.h"
#include "vtkBMPWriter.h"


IMPLEMENT_APP(TestApp)

//----------------------------------------------------------------------------
mafTest::mafTest()
{
	m_Renderer = NULL;
	m_RenderWindow = NULL;

	m_WorkingDir = wxGetWorkingDirectory();
}

//--------------------------------------------------------------------------------
bool TestApp::OnInit()
{
	return TRUE;
}

//--------------------------------------------------------------------------------
int TestApp::OnExit()
{
	wxApp::CleanUp();
	return 0;
}

//--------------------------------------------------------------------------------
bool TestApp::Yield(bool onlyIfNeeded){
	return true;
};

//----------------------------------------------------------------------------
void mafTest::setUp()
{
	m_App = new TestApp();  // Instantiate the application class
	m_App->argc = 0;        // set the number of input argument to 0
	m_App->argv = NULL;     // set to NULL the input argument's parameters
	wxTheApp->SetAppName("mafUserTest"); // Set the name for the application

	//NOTE, wxLog produces some memory leaks, set false during test
	wxLog::EnableLogging(true);

	//Run Test Specific Stuff
	BeforeTest();
}
//----------------------------------------------------------------------------
void mafTest::tearDown()
{
	//Clean Test Spefic Stuff
	AfterTest();
		
	cppDEL(m_App);  // Destroy the application
	wxAppConsole::SetInstance(NULL);	
}

//----------------------------------------------------------------------------
void mafTest::CompareImage(mafString suiteName, mafString imageName, int index)
{
	if (!m_RenderWindow) 
		return;

	// Visualization control
	m_RenderWindow->OffScreenRenderingOn();
	vtkWindowToImageFilter *windowToImage;
	vtkNEW(windowToImage);
	windowToImage->SetInput(m_RenderWindow);
	//w2i->SetMagnification(magnification);
	windowToImage->Update();
	m_RenderWindow->OffScreenRenderingOff();

	wxString imageFolder = GetTestDataDir(suiteName);
	mafString imageFileBase;
	if (index >= 0)
	{
		imageFileBase.Printf("%s\\%s%d", imageFolder.c_str(), imageName.GetCStr(), index);
	}
	else
	{
		imageFileBase.Printf("%s\\%s", imageFolder.c_str(), imageName.GetCStr());
	}

	wxString ext = ".png";
	vtkPNGWriter *imageWriter;
	vtkNEW(imageWriter);
	
	// Write comparing image
	wxString imageFileStored = imageFileBase + ext;

	if (wxFileExists(imageFileStored))
	{
		// Read original Image
		vtkPNGReader *imageReader;
		vtkNEW(imageReader);
		imageReader->SetFileName(imageFileStored);
		imageReader->Update();
		vtkImageData *imDataOrig = imageReader->GetOutput();

		vtkImageData *imDataComp = windowToImage->GetOutput(); // imageReader2->GetOutput();

		// Compare
		vtkImageMathematics *imageMath = vtkImageMathematics::New();
		imageMath->SetInput1(imDataOrig);
		imageMath->SetInput2(imDataComp);
		imageMath->SetOperationToSubtract();
		imageMath->Update();

		double srR[2] = { -1,1 };
		imageMath->GetOutput()->GetPointData()->GetScalars()->GetRange(srR);

		bool result = srR[0] == 0.0 && srR[1] == 0.0;

		wxString imageFileDiff = imageFileBase + "_DIFF" + ext;
		wxString imageFileNew = imageFileBase + "_NEW" + ext;

		if (wxFileExists(imageFileDiff))
			wxRemoveFile(imageFileDiff);
		if (wxFileExists(imageFileNew))
			wxRemoveFile(imageFileNew);
		
		if (!result)
		{
			imageWriter->SetInput(imageMath->GetOutput());
			imageWriter->SetFileName(imageFileDiff);
			imageWriter->Write();
			imageWriter->SetInput(imDataComp);
			imageWriter->SetFileName(imageFileNew);
			imageWriter->Write();

			mafLogMessage("CompareImages has found differences. File %s stored.", imageFileNew);
		}

		CPPUNIT_ASSERT(result);

		// End visualization control
		vtkDEL(imageMath);
		vtkDEL(imageReader);
	}
	else
	{
		//First run storing file
		imageWriter->SetInput(windowToImage->GetOutput());
		imageWriter->SetFileName(imageFileStored);
		imageWriter->Write();
		return;
	}

	vtkDEL(imageWriter);
	vtkDEL(windowToImage);
}

//----------------------------------------------------------------------------
mafString mafTest::GetTestDataDir(mafString suiteName)
{
	wxString testDataFolder = m_WorkingDir;
	testDataFolder += "\\" + suiteName;

	if (!wxDirExists(testDataFolder))
	{
		wxMkdir(testDataFolder);
	}

	return testDataFolder;
}