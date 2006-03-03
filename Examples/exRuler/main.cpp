// ----------------------------------
// File: cow.cxx
// Created by vtkgui v 1.0
// ----------------------------------

// ------------------------------------------------------------
// Call the VTK header files to make available all VTK commands
// ------------------------------------------------------------

#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkGenericRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkPlaneSource.h"

#include "vtkRulerActor2D.h"
#include "vtkSimpleRulerActor2D.h"

int main( int argc, char *argv[] ) { 

// --------------------------------------------------------------
// Create a Renderer, a RenderWindow and a RenderWindowInteractor
// --------------------------------------------------------------

vtkCamera *C = vtkCamera::New();
   C->SetFocalPoint(0, 0, 0);
   C->SetPosition(0 ,0, 1);
   C->SetViewUp(0 , 1 , 0);
   
   //C->SetPosition(0 ,0, -1);
   //C->SetViewUp(0 , 1 , 0);
   C->SetParallelProjection(1);

vtkRenderer *ren1 = vtkRenderer::New();
   ren1->SetActiveCamera(C);
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

vtkPlaneSource *PS = vtkPlaneSource::New();
   PS->SetCenter(1 , 1 , 0);
   PS->SetNormal(0 , 0 , 1);
   PS->SetOrigin(0 , 0 , 0);
   PS->SetPoint1(2 , 0 , 0);
   PS->SetPoint2(0 , 2 , 0);
   PS->SetXResolution(10);
   PS->SetYResolution(10);

vtkPolyDataMapper *PDM2 = vtkPolyDataMapper::New();
   PDM2->SetInput(PS->GetOutput());

vtkActor *A2 = vtkActor::New();
   A2->SetMapper(PDM2);
   A2->GetProperty()->SetRepresentation(1);
   A2->GetProperty()->SetColor(1,0,0);
   A2->GetProperty()->SetInterpolationToFlat();
   //A2->SetScale(10);

   
vtkSimpleRulerActor2D *A2D = vtkSimpleRulerActor2D::New();
   A2D->SetLabelAxesVisibility();
   A2D->SetLabelScaleVisibility();
   //A2D->SetAxesVisibility(false);
   A2D->SetTickVisibility(true);
   A2D->SetLegend("mm");
   A2D->SetColor(1,1,1);

   //A2D->SetLegend("inch");
   //A2D->SetScaleFactor(25.4);

vtkRulerActor2D *A2D2 = vtkRulerActor2D::New();
   A2D2->SetLabelAxesVisibility();
   A2D2->SetLabelScaleVisibility(false);
   //A2D2->SetAxesVisibility(false);
   A2D2->SetTickVisibility(true);
   A2D2->SetLegend("mm");
   A2D2->SetColor(1,1,1);

   //A2D2->SetLegend("inch");
   //A2D2->SetScaleFactor(25.4);

   
   
// -----------------------------------
// Insert all actors into the renderer
// -----------------------------------

   ren1->AddActor( A2 );
   ren1->AddActor2D( A2D );
   ren1->AddActor2D( A2D2 );
/*   ren1->AddActor2D( A2D->GetScaleFactorLabelActor() );
   ren1->AddActor2D( A2D->GetXAxesLabelActor() );
   ren1->AddActor2D( A2D->GetYAxesLabelActor() );
*/
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

A2D->Delete();
C->Delete();
iren->Delete();
ren1->Delete();
renWin->Delete();

return 0;
}


