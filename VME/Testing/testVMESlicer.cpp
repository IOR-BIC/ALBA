// Test mafVMEGeneric class

#include "mafVMERoot.h"
#include "mafVMESlicer.h"
#include "mafVMEVolumeGray.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkOutlineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkTexture.h"
#include "vtkDataSetReader.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageActor.h"
#include "vtkDataSetWriter.h"
#include "vtkPlaneSource.h"

#include <iostream>

#ifdef WIN32
  #define SLEEP(a) Sleep(a)
#else
  #include <unistd.h>
  #define SLEEP(a) usleep(a*1000)
#endif

//-------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------
{
  // create a small tree with a root, a volume and a slicer 
  mafSmartPointer<mafVMERoot> root;
  root->SetName("ROOT");

  mafVMESlicer *vslicer=mafVMESlicer::New();
  mafVMEVolumeGray *vvol=mafVMEVolumeGray::New();

  vslicer->SetName("slicer");
  vvol->SetName("volume");
  
  root->AddChild(vvol);
  vvol->AddChild(vslicer);

  //set data to VME volume
  vtkMAFSmartPointer<vtkDataSetReader> reader;
  mafString filename=MAF_DATA_ROOT;
  //filename<<"/VTK_Volumes/mummy_head.vtk";
  filename<<"/VTK_Volumes/femur_r.vtk";
  reader->SetFileName(filename);

  reader->Update();

  //vvol->SetDataByReference(reader->GetStructuredPointsOutput(),0);
  vvol->SetDataByReference(reader->GetRectilinearGridOutput(),0);

  mafTransform trans;

  // create windows
  vtkMAFSmartPointer<vtkRenderer> renderer;
  vtkMAFSmartPointer<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);

  renWin->SetSize(1000,1000);
  renderer->SetBackground( 0.1, 0.2, 0.4 );

  // Bounding box surrounding the volume
  vtkOutlineSource *volBoundsBox=vtkOutlineSource::New();
  mafOBB volBounds;
  vvol->GetOutput()->GetVMELocalBounds(volBounds);
  
  volBoundsBox->SetBounds(volBounds.m_Bounds);

  vtkMAFSmartPointer<vtkPolyDataMapper> vol_mapper;
  vol_mapper->SetInput(volBoundsBox->GetOutput());

  vtkMAFSmartPointer<vtkActor> vol_actor;
  vol_actor->SetMapper(vol_mapper);
  vol_actor->GetProperty()->SetColor(1,0,0);
  renderer->AddActor(vol_actor);
  
  // create actor to display the slicer plane
  vtkMAFSmartPointer<vtkPolyDataMapper> slicer_mapper;
  slicer_mapper->SetInput(vslicer->GetSurfaceOutput()->GetSurfaceData());
  
  vtkMAFSmartPointer<vtkTexture> slicer_tex;
  slicer_tex->SetInput(vslicer->GetSurfaceOutput()->GetTexture());

  vtkMAFSmartPointer<vtkActor> slicer_actor;
  slicer_actor->SetMapper(slicer_mapper);
  slicer_actor->SetTexture(slicer_tex);

  renderer->AddActor(slicer_actor);

  double center[3];
  volBounds.GetCenter(center);

  trans.SetPosition(center);
  trans.SetOrientation(5,5,0);

  vslicer->SetMatrix(trans.GetMatrix());

  renderer->ResetCamera();
  renderer->Render();
  
  //vtkMAFSmartPointer<vtkDataSetWriter> writer;
  //writer->SetInput(vslicer->GetSurfaceOutput()->GetTexture());

  int t;
  int steps=50;
  for (t = 0; t < steps; t++) 
  {
    trans.SetPosition(center[0],center[1],(volBounds.m_Bounds[5]-volBounds.m_Bounds[4])/(double)steps*t);
    vslicer->SetMatrix(trans.GetMatrix());
  
    //renderer->GetActiveCamera()->Azimuth(-(180.0/steps));
    renderer->ResetCameraClippingRange();
    renWin->Render();

    double srange[2];
    vslicer->GetSurfaceOutput()->GetTexture()->GetScalarRange(srange);
    cerr<<"bounds=["<<srange[0]<<" , "<<srange[1]<<"]\n";

    //mafString filename="slice_";
    //filename<<t;
    //filename<<".vtk";
    //writer->SetFileName(filename);
    //writer->Write();
    //SLEEP(250);
  }

  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
