#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkGenericRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkDataSetReader.h"
#include "vtkHistogram.h"

int main( int argc, char *argv[] ) { 

// --------------------------------------------------------------
// Create a Renderer, a RenderWindow and a RenderWindowInteractor
// --------------------------------------------------------------
vtkRenderer *ren1 = vtkRenderer::New();
   ren1->SetBackground(0.2 , 0.3 , 0.5);

vtkRenderWindow *renWin = vtkRenderWindow::New();
   renWin->AddRenderer(ren1);
   renWin->SetSize(500 , 400);

vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
   iren->SetRenderWindow(renWin);
   iren->SetLightFollowCamera(1);

vtkInteractorStyleTrackballCamera *is = vtkInteractorStyleTrackballCamera::New();
   iren->SetInteractorStyle(is);

// -----------------------
// Create the VTK pipeline
// -----------------------
vtkDataSetReader *reader = vtkDataSetReader::New();
   //reader->SetFileName("F:\\MAF2\\Data\\Volume.vtk");  //Set your own vtkImageData to load befor running the example!!!
   reader->Update();

vtkHistogram *A2D = vtkHistogram::New();
   A2D->SetColor(1,1,1);
   A2D->SetImageData(vtkImageData::SafeDownCast(reader->GetOutput()));
//   A2D->SetHisctogramRepresentation(vtkHistogram::BAR_REPRESENTATION);
//   A2D->SetHisctogramRepresentation(vtkHistogram::POINT_REPRESENTATION);
   
// -----------------------------------
// Insert all actors into the renderer
// -----------------------------------

   ren1->AddActor( A2D );

// -------------------------------
// Reset the camera and initialize
// -------------------------------
ren1->ResetCamera();
ren1->ResetCameraClippingRange();
renWin->Render();
iren->Initialize();
iren->Start();

// ----------------------------------------
// Perform cleanup of created VTK instances
// ----------------------------------------

reader->Delete();
A2D->Delete();
iren->Delete();
ren1->Delete();
renWin->Delete();

return 0;
}
