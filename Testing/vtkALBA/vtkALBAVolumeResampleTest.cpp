/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAVolumeResampleTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>
#include "vtkALBAVolumeResampleTest.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAVolumeResample.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"

#include "vtkRectilinearGridReader.h"
#include "vtkRectilinearGrid.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkDataSetWriter.h"

using namespace std;


void vtkALBAVolumeResampleTest::TestResample()
{
  const char *inFileName = "volumeRG_dim_10_10_10_bounds_1_10_1_10_1_10.vtk";
   
  const char *outVTKFileName= "resampled_volumeRG_dim_10_10_10_bounds_1_10_1_10_1_10.vtk";

  TestResampleInternal(inFileName,  outVTKFileName );

  inFileName = "volumeRG_dim_10_10_10_bounds_m5_5_m5_5_m5_5.vtk";

  outVTKFileName= "resampled_volumeRG_dim_10_10_10_bounds_m5_5_m5_5_m5_5.vtk";
  
  TestResampleInternal(inFileName,  outVTKFileName );
  
}


void vtkALBAVolumeResampleTest::TestResampleInternal( const char *inFileName , const char *outVTKFileName )
{  
  std::string absPathFilename=ALBA_DATA_ROOT;
  absPathFilename += "/Test_VolumeResample/";
  absPathFilename.append(inFileName);

  vtkRectilinearGridReader *reader = vtkRectilinearGridReader::New();

  reader->SetFileName(absPathFilename.c_str());
  reader->Update();

  double inputDataSpacing[3];
  vtkRectilinearGrid *rg = reader->GetOutput();

  inputDataSpacing[0] = rg->GetXCoordinates()->GetComponent(1,0)-rg->GetXCoordinates()->GetComponent(0,0);
  inputDataSpacing[1] = rg->GetYCoordinates()->GetComponent(1,0)-rg->GetYCoordinates()->GetComponent(0,0);
  inputDataSpacing[2] = rg->GetZCoordinates()->GetComponent(1,0)-rg->GetZCoordinates()->GetComponent(0,0);
 
  PrintDouble3(cout, inputDataSpacing, "inputDataSpacing");

  double inputDataOrigin[3];
  inputDataOrigin[0] = rg->GetXCoordinates()->GetComponent(0,0);
  inputDataOrigin[1] = rg->GetYCoordinates()->GetComponent(0,0);
  inputDataOrigin[2] = rg->GetZCoordinates()->GetComponent(0,0);
  
  PrintDouble3(cout, inputDataOrigin, "inputDataOrigin");

  vtkALBAVolumeResample *resample = vtkALBAVolumeResample::New();
  resample->SetZeroValue(0);
  resample->SetInputConnection(reader->GetOutputPort());

  resample->SetVolumeOrigin(inputDataOrigin);
  PrintDouble3(cout, inputDataOrigin, "inputDataOrigin");

  vtkTransform *transform = vtkTransform::New();
  transform->Identity();
  transform->Update();
  
  double xAxis[3] = {0,0,0};
  for (int i = 0; i < 3; i++)
  {
    xAxis[i] = transform->GetMatrix()->GetElement(i, 0);
  }
  resample->SetVolumeAxisX(xAxis);
  PrintDouble3(cout, xAxis, "xAxis");

  double yAxis[3] = {0,0,0};
  for (int i = 0; i < 3; i++)
  {
    yAxis[i] = transform->GetMatrix()->GetElement(i, 1);
  }
  resample->SetVolumeAxisY(yAxis);
  PrintDouble3(cout, yAxis, "yAxis");

  double sr[2];
  rg->GetScalarRange(sr);

  double w = sr[1] - sr[0];
  double l = (sr[1] + sr[0]) * 0.5;

  resample->SetWindow(w);
  resample->SetLevel(l);
  resample->AutoSpacingOff();
  
  
  
  double resamplingBoxBounds[6];
  rg->GetBounds(resamplingBoxBounds);
  int outputSPExtent[6];
  outputSPExtent[0] = 0;
  outputSPExtent[1] = (resamplingBoxBounds[1] - resamplingBoxBounds[0]) / inputDataSpacing[0];
  outputSPExtent[2] = 0;
  outputSPExtent[3] = (resamplingBoxBounds[3] - resamplingBoxBounds[2]) / inputDataSpacing[1];
  outputSPExtent[4] = 0;
  outputSPExtent[5] = (resamplingBoxBounds[5] - resamplingBoxBounds[4]) / inputDataSpacing[2];

	resample->SetOutputSpacing(inputDataSpacing);
	resample->SetOutputExtent(outputSPExtent);

  //resample->SetOutput(outputSP);
  resample->Update();

	vtkImageData *outputSP = vtkImageData::SafeDownCast(resample->GetOutput());
  
  double inBounds[6] = {0,0,0};
  rg->GetBounds(inBounds);

  double checkBounds[6];
  outputSP->GetBounds(checkBounds);
  
  CPPUNIT_ASSERT(checkBounds[0]==inBounds[0] && checkBounds[1]==inBounds[1] && checkBounds[2]==inBounds[2]);
  CPPUNIT_ASSERT(checkBounds[3]==inBounds[3] && checkBounds[4]==inBounds[4] && checkBounds[5]==inBounds[5]);

  WriteVTKDatasetToFile(outputSP, outVTKFileName);
  
  reader->Delete();
  resample->Delete(); 
  transform->Delete();
}

void vtkALBAVolumeResampleTest::WriteVTKDatasetToFile( vtkDataSet * outputVolumeVTKData, const char *outputFilename )
{
  vtkALBASmartPointer<vtkDataSetWriter> writer;
  writer->SetInputData(outputVolumeVTKData);

  string fullPathOutputFilename;
  fullPathOutputFilename.append(GET_TEST_DATA_DIR());
  fullPathOutputFilename.append("//");
  fullPathOutputFilename.append(outputFilename);
  cout << fullPathOutputFilename;

  writer->SetFileName(fullPathOutputFilename.c_str());
  writer->SetFileTypeToASCII();
  writer->Write();
}


void vtkALBAVolumeResampleTest::PrintDouble6( ostream& os, double array[6], const char *logMessage /*= NULL */ )
{

  if (logMessage) os << logMessage << std::endl;
  os << "xmin, xmax [" << array[0] << " , " << array[1] << "]" << std::endl;
  os << "ymin, ymax [" << array[2] << " , " << array[3] << "]" << std::endl;
  os << "zmin, zmax [" << array[4] << " , " << array[5] << "]" << std::endl;
  os << std::endl;
}

void vtkALBAVolumeResampleTest::PrintDouble3( ostream& os, double array[3], const char *logMessage /*= NULL*/ )
{
  if (logMessage) os << logMessage << " [" << array[0] << " , " << array[1] << " , " << array[2] << " ]" << std::endl;
  os << std::endl;
}


void vtkALBAVolumeResampleTest::PrintInt3( ostream& os, int array[3], const char *logMessage /*= NULL*/ )
{
  if (logMessage) os << logMessage << " [" << array[0] << " , " << array[1] << " , " << array[2] << " ]" << std::endl;
  os << std::endl;
}

void vtkALBAVolumeResampleTest::TestSetGetVolumeOrigin()
{
  double volumeOrigin[3] = {1,2,3};
  vtkALBAVolumeResample *resample = vtkALBAVolumeResample::New();
  resample->SetVolumeOrigin(volumeOrigin);

  double checkVolumeOrigin[3] = {1,2,3};
  resample->GetVolumeOrigin(checkVolumeOrigin);

  for (int i = 0; i < 3; i++) 
  {
    CPPUNIT_ASSERT(checkVolumeOrigin[i] == volumeOrigin[i]);
  }
  
  resample->Delete();
}

void vtkALBAVolumeResampleTest::TestSetGetVolumeAxisX()
{
  double volumeAxisX[3] = {0,0,1};
  vtkALBAVolumeResample *resample = vtkALBAVolumeResample::New();
  resample->SetVolumeAxisX(volumeAxisX);

  double checkVolumeAxisX[3] = {0,0,1};
  resample->GetVolumeAxisX(checkVolumeAxisX);

  for (int i = 0; i < 3; i++) 
  {
    CPPUNIT_ASSERT(checkVolumeAxisX[i] == volumeAxisX[i]);
  }

  resample->Delete();
}


void vtkALBAVolumeResampleTest::TestSetGetVolumeAxisY()
{
  double volumeAxisY[3] = {0,0,1};
  vtkALBAVolumeResample *resample = vtkALBAVolumeResample::New();
  resample->SetVolumeAxisY(volumeAxisY);

  double checkVolumeAxisY[3] = {0,0,1};
  resample->GetVolumeAxisY(checkVolumeAxisY);

  for (int i = 0; i < 3; i++) 
  {
    CPPUNIT_ASSERT(checkVolumeAxisY[i] == volumeAxisY[i]);
  }

  resample->Delete();
}

void vtkALBAVolumeResampleTest::TestSetGetWindow()
{
  double window = 10;
  vtkALBAVolumeResample *resample = vtkALBAVolumeResample::New();
  resample->SetWindow(window);

  double checkWindow = 10;
  resample->GetWindow();

  CPPUNIT_ASSERT(checkWindow == window);

  resample->Delete();
}

void vtkALBAVolumeResampleTest::TestSetGetLevel()
{
  double level = 10;
  vtkALBAVolumeResample *resample = vtkALBAVolumeResample::New();
  resample->SetLevel(level);

  double checkLevel = 10;
  resample->GetLevel();

  CPPUNIT_ASSERT(checkLevel == level);
  
  resample->Delete();
}