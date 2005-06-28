#include "mafDefines.h"

#include "vtkMAFSmartPointer.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetReader.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkStructuredPointsReader.h"
#include "vtkRectilinearGridReader.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkFloatArray.h"
#include "vtkShortArray.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"
#include "vtkCamera.h"
#include "vtkPlane.h"
#include "vtkVolumeSlicer.h"
#include "vtkTexture.h"
#include "vtkProperty.h"

#include <iostream>

//--------------------------------------------------------------------
int VolumeSlicerTest()
  //--------------------------------------------------------------------
{  
  vtkVolumeSlicer         *slicer;
  vtkVolumeSlicer         *pslicer;

  slicer = vtkVolumeSlicer::New();
  pslicer = vtkVolumeSlicer::New();

  // create windows
  vtkMAFSmartPointer<vtkRenderer> renderer;
  vtkMAFSmartPointer<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);

  vtkMAFSmartPointer<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // prepare for rendering
  iren->SetStillUpdateRate(0.05f);
  iren->SetDesiredUpdateRate(15.f);
  vtkMAFSmartPointer<vtkInteractorStyleSwitch> style;
  iren->SetInteractorStyle(style);
  style->SetCurrentStyleToTrackballActor();

  renderer->SetBackground(0.05f, 0.05f, 0.05f);
  renWin->SetSize(800, 600);

  //------------------ load (create) data
  int dataExtent[6];
  vtkMAFSmartPointer<vtkImageData> data;
  data->SetScalarTypeToUnsignedChar();
  data->SetExtent(0, 255, 0, 255, 0, 255);
  data->AllocateScalars();
  data->SetSpacing(1, 1, 1);
  data->GetExtent(dataExtent);
  data->SetWholeExtent(dataExtent);
  unsigned char* ptr = (unsigned char*)data->GetScalarPointer();
  const int R2 = 127 * 127;
  for (int z = dataExtent[4]; z <= dataExtent[5]; z++) 
  {
    for (int y = dataExtent[2]; y <= dataExtent[3]; y++) 
    {
      for (int x = dataExtent[0]; x <= dataExtent[1]; x++) 
      {
        int r2 = (x - 127) * (x - 127) + (y - 127)* (y - 127) + (z - 127) * (z - 127);
        if (r2 > R2)
          *ptr = 0;
        else if (r2 < 2500)
          *ptr = 64;
        else
          *ptr = 255;
        ptr++;
      }
    }
  }

  double bounds[6];
  data->GetBounds(bounds);
  slicer->SetPlaneOrigin(bounds[0], bounds[2], (bounds[4] + bounds[5])*.5f);
  pslicer->SetPlaneOrigin(slicer->GetPlaneOrigin());
  double x[3] = {1, 0, 0}, y[3] = {0, 1, 0}, normal[3];
  vtkMath::Normalize(x);
  vtkMath::Normalize(y);
  vtkMath::Cross(y, x, normal);
  vtkMath::Normalize(normal);
  vtkMath::Cross(normal, x, y);
  vtkMath::Normalize(y);

  slicer->SetPlaneAxisX(x);
  pslicer->SetPlaneAxisX(x);
  slicer->SetPlaneAxisY(y);
  pslicer->SetPlaneAxisY(y);
  
  slicer->SetInput(data);
  pslicer->SetInput(data);

  vtkMAFSmartPointer<vtkImageData> idata;
  idata->SetScalarTypeToUnsignedChar();
  idata->SetNumberOfScalarComponents(4);
  idata->SetExtent(0, 1023, 0, 1023, 0, 0);
  idata->SetSpacing(0.5f, 0.5f, 1.f);
  slicer->SetOutput(idata);
  slicer->SetWindow(800.f);
  slicer->SetLevel(-100.f);

  MAF_TEST(slicer->GetWindow() == 800);
  MAF_TEST(slicer->GetLevel() == -100);

  vtkMAFSmartPointer<vtkTexture> texture;
  texture->RepeatOff();
  texture->InterpolateOn();
  texture->SetQualityTo32Bit();
  texture->SetInput(idata);

  vtkMAFSmartPointer<vtkPolyDataMapper> smapper;
  vtkMAFSmartPointer<vtkPolyData>       polydata;
  pslicer->SetOutput(polydata);
  pslicer->SetTexture(idata);
  smapper->SetInput(polydata);
  vtkMAFSmartPointer<vtkActor> sactor;
  sactor->SetMapper(smapper);
  sactor->VisibilityOn();
  sactor->PickableOff();
  renderer->AddActor(sactor);
  sactor->GetProperty()->SetColor(1.f, 0.2f, 0.2f);
  sactor->GetProperty()->SetAmbient(1.f);
  renderer->SetAmbient(1.f, 1.f, 1.f);
  sactor->GetProperty()->SetDiffuse(0.f);
  sactor->SetTexture(texture);
  smapper->ScalarVisibilityOff();

  //sactor->SetUserMatrix(volume->GetMatrix());
  renderer->ResetCamera(smapper->GetBounds());

  renderer->GetActiveCamera()->SetViewAngle(vtkMath::RadiansToDegrees() * 2.f * atan(renWin->GetSize()[1] * 0.27 / (2.f * 700.f)));
  renderer->GetActiveCamera()->SetEyeAngle(2.5);
  renderer->ResetCamera();
  renWin->Render();

  vtkCamera *camera = renderer->GetActiveCamera();
  camera->Azimuth(45);
  renderer->ResetCamera(data->GetBounds());

  for (float roll = 0; roll < 30.f; roll += .1f ) 
  {
    camera->Roll(1.f);
    camera->Azimuth(((int(roll) % 20) - 10) * .2f);
    renWin->Render();
  }

  slicer->Delete();
  pslicer->Delete();


  return MAF_OK;
}

int main()
{
  return VolumeSlicerTest();
}
