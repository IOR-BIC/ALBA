#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"

/*
#include "vtkAxes.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"
#include "vtkInteractorStyleTrackballCamera.h"
*/
/*
int main()
{

		vtkTransform *transform;
		mafMatrix M;
		mafMatrix Q;
		mafMatrix S;
		mafMatrix QxS;
	  mafMatrix invM; 
	  mafMatrix invMxQxS;

 this->transform = vtkTransform::New();
 this->M = vtkMatrix4x4::New(); 
 this->Q = vtkMatrix4x4::New();
 this->S = vtkMatrix4x4::New();
 this->QxS = vtkMatrix4x4::New();
 this->invM = vtkMatrix4x4::New();
 this->invMxQxS = vtkMatrix4x4::New();
}

//-------------------------------------------------------------------------
void mflTransformTestCase::tearDown()
//-------------------------------------------------------------------------
{
 this->transform->Delete();
 this->M->Delete();
 this->Q->Delete();
 this->S->Delete();
 this->QxS->Delete();	
 this->invM->Delete();
 this->invMxQxS->Delete();
}

//-------------------------------------------------------------------------
void mflTransformTestCase::PolarDecompTest()
//-------------------------------------------------------------------------
{


 float translation[3] = {vtkMath::Random(0.0, 10.0), 
	 vtkMath::Random(0.0, 10.0), 
	 vtkMath::Random(0.0, 10.0) };

 float angle[3] = {vtkMath::Random(0.0, 90.0), 
	vtkMath::Random(0.0, 90.0), 
	vtkMath::Random(0.0, 90.0) };
	
 float scale[3]	= {vtkMath::Random(0.0, 10),
   vtkMath::Random(0.0, 10),
	 vtkMath::Random(0.0, 10)};

    transform->Translate(translation);
    transform->RotateX(angle[0]);
    transform->RotateY(angle[1]);
    transform->RotateZ(angle[2]);
		transform->Scale(scale);
		transform->GetMatrix(M);


float T[3];

strstream str;

str << "Creating mflTransform instance..." << endl;

mflTransform *mfl_t = mflTransform::New();

str << "done!" << endl << endl;

str << "Applying polar decomposition to random matrix:" << endl;
str << "M = Q*S; the translation column T is extracted."<< endl;



mfl_t->PolarDecomp(M, Q, S, T);

str << "done!" << endl << endl;


str << "############# M ###########" << endl;
M->Print(str);
str << "############# Q ###########" << endl;
Q->Print(str);
str << "############# S ###########" << endl;
S->Print(str);
str << "############# T ###########" << endl;
str << T[0] << " " << T[1] << " " << T[2] << endl;



vtkMatrix4x4::Multiply4x4(Q, S, QxS);

str << "############# QxS ###########" << endl;

QxS->Print(str);



for (int i = 0; i < 3; i++)
{
M->SetElement(i, 3, 0);
}

vtkMatrix4x4::Invert(M, invM);
vtkMatrix4x4::Multiply4x4(invM, QxS, invMxQxS);

str << " ############# invMxQxS ########### " << endl;
str << " ######(should be identity)######## " << endl;

invMxQxS->Print(str);

str << "Deleting mflTransform instance..." << endl;
 
mfl_t->Delete();

str << "done!" << endl << endl;


cout << str.str();
}

//-------------------------------------------------------------------------
void mflTransformTestCase::ChangeRefSys()
//-------------------------------------------------------------------------
{
		//create input frame transform
		vtkTransform *v_ift = vtkTransform::New();
		v_ift->Translate(1, 2, 3);
		v_ift->RotateX(10);
		v_ift->RotateY(20);
		v_ift->RotateZ(30);

		//cerr << "#### Input Frame refsys ####" << endl; // Please, remove unuseful the banners...
    //v_ift->GetMatrix()->Print(ss);
    // Stefano, place an ASSERT here!
		
		//create target frame transform
		vtkTransform *v_tft = vtkTransform::New();
		v_tft->RotateX(30);
		v_tft->RotateY(60);
		v_tft->RotateZ(70);

		
		//cerr << "#### Target Frame refsys ####" << endl; // Please, remove unuseful the banners...
		//v_tft->GetMatrix()->Print(ss);
    // Stefano, place an ASSERT here!

		//create input matrix
		vtkTransform *v_imt = vtkTransform::New();
		v_imt->RotateX(5);
		v_imt->RotateY(10);
		v_imt->RotateZ(15);
		v_imt->Translate(5, 10, 2);

		cerr << "#### Input matrix in Input Frame RefSys ####" << endl; // Please, remove unuseful the banners...
		v_imt->GetMatrix()->Print(ss);
    // Stefano, place an ASSERT here!

		//create mflTransform
		mflTransform *mfl_t = mflTransform::New();
		mfl_t->SetInput(v_imt);
		mfl_t->SetInputFrame(mfl_if);
		mfl_t->SetTargetFrame(mfl_tf);
		mfl_t->Update();

		cerr << "#### Input matrix in Target Frame RefSys ####" << endl; // Please, remove unuseful the banners...
		mfl_t->GetMatrix()->Print(ss);
    // Stefano, place an ASSERT here!

		cerr << "####### Test palla e cono ########" << endl;
		cerr << "" << endl;

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

		vtkMatrix4x4::Multiply4x4(mfl_t->GetMatrix(), mfl_tf->GetMatrix(), M);
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


		cerr << endl;
		cerr << "#### posa cono in input ref sys ####" << endl;

		v_imt->GetMatrix()->Print(str);

		cerr << "#### posa cubo in target ref sys ####" << endl;
		
		mfl_t->GetMatrix()->Print(str);

		cerr << str.str();
}

//----------------------------------------------------------------------------
//                                main
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {

  std::string s;

  if (argc==2 && (stricmp(argv[1], "-?") == 0))
  {
    printf("Usage:       'test [-wait] %s for non-interactive test\n",TestNameStr);
    printf("             'test -?' to print this help\n");

    return -1;
  }

  TestRunner runner;
  runner.addTest(TestNameStr, mflTransformTestCase::suite());  
  bool result=runner.run(argc, argv, s);

  return (result?0:-1);
}
//-------------------------------------------------------------------------

  */