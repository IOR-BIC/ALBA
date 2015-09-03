/*=========================================================================

 Program: MAF2
 Module: mafTransformTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafTransformTest.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"

#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkSmartPointer.h"

#include <iostream>

//----------------------------------------------------------------------------
void mafTransformTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafTransformTest::TestTranslateRotateScale()
//----------------------------------------------------------------------------
{
  mafTransform transform ;
  mafMatrix M;

  vtkTransform *vtk_transform=vtkTransform::New();

  double translation[3] = {vtkMath::Random(0.0, 10.0), 
    vtkMath::Random(0.0, 10.0), 
    vtkMath::Random(0.0, 10.0) };

  double angle[3] = {vtkMath::Random(0.0, 90.0), 
    vtkMath::Random(0.0, 90.0), 
    vtkMath::Random(0.0, 90.0) };

  double scale[3]	= {vtkMath::Random(0.0, 10),
    vtkMath::Random(0.0, 10),
    vtkMath::Random(0.0, 10)};

  transform.Translate(translation,PRE_MULTIPLY);
  transform.RotateX(angle[0],PRE_MULTIPLY);
  transform.RotateY(angle[1],PRE_MULTIPLY);
  transform.RotateZ(angle[2],PRE_MULTIPLY);
  transform.Scale(scale[0],scale[1],scale[2],PRE_MULTIPLY);

  M=transform.GetMatrix();

  // test using a VTK trasnform for comparison
  vtk_transform->Translate(translation);
  vtk_transform->RotateX(angle[0]);
  vtk_transform->RotateY(angle[1]);
  vtk_transform->RotateZ(angle[2]);
  vtk_transform->Scale(scale);

  CPPUNIT_ASSERT(transform.GetMatrix()==vtk_transform->GetMatrix());

  vtk_transform->Delete();

}

//----------------------------------------------------------------------------
void mafTransformTest::TestPolarDecomposition()
//----------------------------------------------------------------------------
{
  mafMatrix M;
  mafMatrix Q;
  mafMatrix S;
  mafMatrix QxS;
  mafMatrix invM; 
  mafMatrix invMxQxS;
  
  vtkTransform *vtk_transform=vtkTransform::New();

  double translation[3] = {vtkMath::Random(0.0, 10.0), 
    vtkMath::Random(0.0, 10.0), 
    vtkMath::Random(0.0, 10.0) };

  double angle[3] = {vtkMath::Random(0.0, 90.0), 
    vtkMath::Random(0.0, 90.0), 
    vtkMath::Random(0.0, 90.0) };

  double scale[3]	= {vtkMath::Random(0.0, 10),
    vtkMath::Random(0.0, 10),
    vtkMath::Random(0.0, 10)};

  // creating a simple transform
  vtk_transform->Translate(translation);
  vtk_transform->RotateX(angle[0]);
  vtk_transform->RotateY(angle[1]);
  vtk_transform->RotateZ(angle[2]);
  vtk_transform->Scale(scale);

  M = vtk_transform->GetMatrix();


  double T[3];

  std::cerr << "Creating mflTransform instance..." << std::endl;

  mafTransform mfl_t;

  std::cerr << "done!" << std::endl << std::endl;

  std::cerr << "Applying polar decomposition to random matrix:" << std::endl;
  std::cerr << "M = Q*S; the translation column T is extracted."<< std::endl;



  mfl_t.PolarDecomp(M, Q, S, T);

  std::cerr << "done!" << std::endl << std::endl;

  std::cerr << "############# M ###########" << std::endl;
  M.Print(std::cerr);
  std::cerr << "############# Q ###########" << std::endl;
  Q.Print(std::cerr);
  std::cerr << "############# S ###########" << std::endl;
  S.Print(std::cerr);
  std::cerr << "############# T ###########" << std::endl;
  std::cerr << T[0] << " " << T[1] << " " << T[2] << std::endl;



  mafMatrix::Multiply4x4(Q, S, QxS);

  std::cerr << "############# QxS ###########" << std::endl;

  QxS.Print(std::cerr);

  for (int i = 0; i < 3; i++)
  {
    M.SetElement(i, 3, 0);
  }

  mafMatrix::Invert(M, invM);
  mafMatrix::Multiply4x4(invM, QxS, invMxQxS);

  std::cerr << " ############# invMxQxS ########### " << std::endl;
  std::cerr << " ######(should be identity)######## " << std::endl;

  invMxQxS.Print(std::cerr);
  mafMatrix identity;
  double *mat1=*(identity.GetElements());
  double *mat2=*(invMxQxS.GetElements());
  for (int idx=0;idx<16;idx++)
  {  
    double diff = fabs(mat1[idx]-mat2[idx]);
    CPPUNIT_ASSERT(diff<.000000001);
  }

  mafSmartPointer<mafTransform> pose1;
  pose1->SetPosition(10,10,10);


  mafSmartPointer<mafTransform> pose_parent;
  pose_parent->SetPosition(-10,-10,-10);

  mafSmartPointer<mafTransform> pose2;
  pose2->SetPosition(20,20,20);
  
  mafTransformFrame change_frame;
  change_frame.SetTargetFrame(pose_parent);
  change_frame.SetInput(pose1);

  CPPUNIT_ASSERT(change_frame.GetMatrix()==pose2->GetMatrix());

  // cleanup
  vtk_transform->Delete();
}

/*
// TO BE PARTED!
//-------------------------------------------------------------------------
void mafTransformTest::ChangeRefSys()
//-------------------------------------------------------------------------
{
  //create input frame transform
  vtkTransform *v_ift = vtkTransform::New();
  v_ift->Translate(1, 2, 3);
  v_ift->RotateX(10);
  v_ift->RotateY(20);
  v_ift->RotateZ(30);

  //cerr << "#### Input Frame refsys ####" << std::endl; 
  //v_ift->GetMatrix()->Print(ss);
  // Place an ASSERT here!

  //create target frame transform
  vtkTransform *v_tft = vtkTransform::New();
  v_tft->RotateX(30);
  v_tft->RotateY(60);
  v_tft->RotateZ(70);


  //cerr << "#### Target Frame refsys ####" << std::endl; 
  //v_tft->GetMatrix()->Print(ss);
  // Place an ASSERT here!

  //create input matrix
  vtkTransform *v_imt = vtkTransform::New();
  v_imt->RotateX(5);
  v_imt->RotateY(10);
  v_imt->RotateZ(15);
  v_imt->Translate(5, 10, 2);

  cerr << "#### Input matrix in Input Frame RefSys ####" << std::endl; 
  v_imt->GetMatrix()->Print(ss);
  // Place an ASSERT here!

  //create mflTransform
  mflTransform *mfl_t = mflTransform::New();
  mfl_t.SetInput(v_imt);
  mfl_t.SetInputFrame(mfl_if);
  mfl_t.SetTargetFrame(mfl_tf);
  mfl_t.Update();

  cerr << "#### Input matrix in Target Frame RefSys ####" << std::endl; 
  mfl_t.GetMatrix()->Print(ss);
  // Place an ASSERT here!

  cerr << "####### Test palla e cono ########" << std::endl;
  cerr << "" << std::endl;

  //crea assi if
  vtkAxes *v_ifa = vtkAxes::New();

  //crea assi tf
  vtkAxes *v_tfa = vtkAxes::New();

  //crea cubo
  vtkCubeSource *v_cube = vtkCubeSource::New();
  v_cube->SetXLength(0.5);
  v_cube->SetYLength(0.5);
  v_cube->SetZLength(0.5);

  //crea cono
  vtkConeSource *v_cone = vtkConeSource::New();

  vtkPolyDataMapper *v_ifpdm = vtkPolyDataMapper::New();
  vtkPolyDataMapper *v_tfpdm = vtkPolyDataMapper::New();
  vtkPolyDataMapper *v_cubepdm = vtkPolyDataMapper::New();
  vtkPolyDataMapper *v_conepdm = vtkPolyDataMapper::New();
  v_ifpdm->SetInput(v_ifa->GetOutput());
  v_tfpdm->SetInput(v_tfa->GetOutput());
  v_cubepdm->SetInput(v_cube->GetOutput());
  v_conepdm->SetInput(v_cone->GetOutput());

  vtkActor *v_ifaa = vtkActor::New();
  vtkActor *v_tfaa = vtkActor::New();
  vtkActor *v_cube_actor = vtkActor::New();
  vtkActor *v_cone_actor = vtkActor::New();
  v_ifaa->SetMapper(v_ifpdm);
  v_tfaa->SetMapper(v_tfpdm);
  v_cube_actor->SetMapper(v_cubepdm);
  v_cone_actor->SetMapper(v_conepdm);
  v_ifaa->SetUserMatrix(mfl_if->GetMatrix());
  v_tfaa->SetUserMatrix(mfl_tf->GetMatrix());



  vtkMatrix4x4::Multiply4x4(v_imt->GetMatrix(), mfl_if->GetMatrix(), M);
  v_cone_actor->SetUserMatrix(M);

  vtkMatrix4x4::Multiply4x4(mfl_t.GetMatrix(), mfl_tf->GetMatrix(), M);
  v_cube_actor->SetUserMatrix(M);

  vtkRenderer *v_r = vtkRenderer::New();
  v_r->AddActor(v_ifaa);
  v_r->AddActor(v_tfaa);
  v_r->AddActor(v_cube_actor);
  v_r->AddActor(v_cone_actor);


  vtkRenderWindow *v_rw = vtkRenderWindow::New();
  v_rw->AddRenderer(v_r);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(v_rw);

  vtkInteractorStyleTrackballCamera *v_istc = vtkInteractorStyleTrackballCamera::New();
  iren->SetInteractorStyle(v_istc);

  v_rw->Render();
  iren->Start();


  cerr << std::endl;
  cerr << "#### posa cono in input ref sys ####" << std::endl;

  v_imt->GetMatrix()->Print(std::cerr);

  cerr << "#### posa cubo in target ref sys ####" << std::endl;

  mfl_t.GetMatrix()->Print(std::cerr);

  cerr << std::cerr.std::cerr();
}
*/
