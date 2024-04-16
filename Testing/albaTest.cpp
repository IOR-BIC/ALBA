/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "albaDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaTest.h"

#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
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
#include "vtkALBASmartPointer.h"
#include "vtkImageCast.h"



IMPLEMENT_APP(TestApp)

//----------------------------------------------------------------------------
albaTest::albaTest()
{
	m_Renderer = NULL;
	m_RenderWindow = NULL;

	m_WorkingDir = wxGetCwd();
}

//--------------------------------------------------------------------------------
bool TestApp::OnInit()
{
	return true;
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
void albaTest::setUp()
{
	m_App = new TestApp();  // Instantiate the application class
	m_App->argc = 0;        // set the number of input argument to 0
	m_App->argv = NULL;     // set to NULL the input argument's parameters
	wxTheApp->SetAppName("albaUserTest"); // Set the name for the application

	//NOTE, wxLog produces some memory leaks, set false during test
	wxLog::EnableLogging(true);

	//Resets Random Color for test repeatability
	albaResetRandomColor();

	//Run Test Specific Stuff
	BeforeTest();
}
//----------------------------------------------------------------------------
void albaTest::tearDown()
{
	//Clean Test Spefic Stuff
	AfterTest();
		
	cppDEL(m_App);  // Destroy the application
	wxAppConsole::SetInstance(NULL);	
}

//----------------------------------------------------------------------------
void albaTest::PauseBeforeExit()
{
	int x;
	scanf("%d", &x);
	return;
}

//----------------------------------------------------------------------------
void albaTest::CompareImage(albaString suiteName, albaString imageName, int index)
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

	vtkImageData *imDataComp = windowToImage->GetOutput();

	CompareVTKImage(imDataComp, suiteName, imageName, index);

	vtkDEL(windowToImage);
}

//----------------------------------------------------------------------------
void albaTest::CompareVTKImage(vtkImageData *imDataComp, albaString suiteName, albaString imageName, int index)
{
	vtkImageData *imgToComp;
	vtkALBASmartPointer<vtkImageCast> vtkImageToUChar;


	if (imDataComp->GetScalarType() != VTK_UNSIGNED_CHAR)
	{
		albaLogMessage("Converting image tho unsigned short in order to save it as PNG format");
		vtkImageToUChar->SetOutputScalarTypeToUnsignedChar();
		vtkImageToUChar->SetInputData(imDataComp);
		vtkImageToUChar->Modified();
		vtkImageToUChar->Update();
		imgToComp = vtkImageToUChar->GetOutput();
	}
	else
	{
		imgToComp = imDataComp;
	}

	wxString imageFolder = GetTestDataDir(suiteName);
	albaString imageFileBase;
	if (index >= 0)
	{
		imageFileBase.Printf("%s\\%s%d", imageFolder.ToAscii(), imageName.GetCStr(), index);
	}
	else
	{
		imageFileBase.Printf("%s\\%s", imageFolder.ToAscii(), imageName.GetCStr());
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


		// Compare
		vtkImageMathematics *imageMath = vtkImageMathematics::New();
		imageMath->SetInput1Data(imDataOrig);
		imageMath->SetInput2Data(imgToComp);
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
			imageWriter->SetInputData(imageMath->GetOutput());
			imageWriter->SetFileName(imageFileDiff);
			imageWriter->Write();
			imageWriter->SetInputData(imgToComp);
			imageWriter->SetFileName(imageFileNew);
			imageWriter->Write();

			albaLogMessage("CompareImages has found differences. File %s stored.", imageFileNew);
		}

		CPPUNIT_ASSERT(result);

		// End visualization control
		vtkDEL(imageMath);
		vtkDEL(imageReader);
	}
	else
	{
		//First run storing file
		imageWriter->SetInputData(imgToComp);
		imageWriter->SetFileName(imageFileStored);
		imageWriter->Write();
		return;
	}

	vtkDEL(imageWriter);
}

//----------------------------------------------------------------------------
albaString albaTest::GetTestDataDir(albaString suiteName)
{
	wxString testDataFolder = m_WorkingDir;
	testDataFolder += "\\" + suiteName;

	if (!wxDirExists(testDataFolder))
	{
		wxMkdir(testDataFolder);
	}

	return testDataFolder;
}