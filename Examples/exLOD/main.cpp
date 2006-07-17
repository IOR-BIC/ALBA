// ----------------------------------
// File: main.cxx
// Created by vtkgui v 1.0
// ----------------------------------

// ------------------------------------------------------------
// Call the VTK header files to make available all VTK commands
// ------------------------------------------------------------

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkGenericRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkLight.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPropCollection.h"
#include "vtkPropPicker.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkSphereSource.h"

#include "mafLODActor.h"


int main( int argc, char *argv[] ) { 

// --------------------------------------------------------------
// Create a Renderer, a RenderWindow and a RenderWindowInteractor
// --------------------------------------------------------------

vtkRenderer *R = vtkRenderer::New();
   R->SetBackground(0.5 , 0.5 , 0.5);

vtkRenderWindow *RW = vtkRenderWindow::New();
   RW->AddRenderer(R);
   RW->AddRenderer(R);
   RW->SetSize(500 , 406);

vtkRenderWindowInteractor *RWI = vtkRenderWindowInteractor::New();
   RWI->SetRenderWindow(RW);
   RWI->SetLightFollowCamera(1);


// -----------------------
// Create the VTK pipeline
// -----------------------

vtkSphereSource *SS = vtkSphereSource::New();
   SS->SetCenter(0 , 0 , 0);
   SS->SetEndPhi(180);
   SS->SetPhiResolution(8);
   SS->SetRadius(0.5);
   SS->SetStartPhi(0);
   SS->SetThetaResolution(8);

vtkPolyDataMapper *PDM = vtkPolyDataMapper::New();
   PDM->SetInput((vtkPolyData *) SS->GetOutput());
   PDM->SetNumberOfPieces(1);
   PDM->SetScalarRange(0 , 1);
   PDM->SetColorMode(0);
   PDM->SetResolveCoincidentTopology(0);
   PDM->SetScalarMode(0);
   PDM->SetImmediateModeRendering(0);
   PDM->SetScalarVisibility(1);
   PDM->SetUseLookupTableScalarRange(0);

mafLODActor *A = mafLODActor::New();
   A->SetMapper(PDM);
   A->SetOrigin(0 , 0 , 0);
   A->SetPosition(0 , 0 , 0);
   A->SetScale(1 , 1 , 1);
   A->SetPickable(1);
   A->SetVisibility(1);


// -----------------------------------
// Insert all actors into the renderer
// -----------------------------------

   R->AddActor( A );

// -------------------------------
// Reset the camera and initialize
// -------------------------------

R->ResetCamera();
R->ResetCameraClippingRange();
RW->Render();
RWI->Initialize();
RWI->Start();

// ----------------------------------------
// Perform cleanup of created VTK instances
// ----------------------------------------

A->Delete();
PDM->Delete();
R->Delete();
RW->Delete();
RWI->Delete();
SS->Delete();

   return 0;
}
