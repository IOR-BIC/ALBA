/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: multiThreaderTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Test albaVMEGeneric class
#include "albaVMETests.h"
#include "VMEGenericTest.h"

#include "albaVMEGeneric.h"
#include "albaVMERoot.h"
#include "albaTransform.h"
#include "albaAttribute.h"
#include "albaVMEOutput.h"
#include "albaDataVector.h"
#include "albaMatrixVector.h"
#include "albaVMEIterator.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaStorageElement.h"

#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkAxes.h"
//#include "vtkDataSetMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkGlyph3D.h"
#include "vtkOutlineSource.h"
#include "vtkCamera.h"
#include "vtkProperty.h"

#include <iostream>
#include "albaVMEStorage.h"
#include "albaDataPipeInterpolatorVTK.h"
#include "vtkALBADataPipe.h"


//-------------------------------------------------------------------------
void VMEGenericTest::VMEGenericMainTest()
//-------------------------------------------------------------------------
{
  // create a small tree with 
  albaSmartPointer<albaVMERoot> root;
  root->SetName("ROOT");

  // create windows
  vtkALBASmartPointer<vtkRenderer> renderer;
  vtkALBASmartPointer<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);

  renWin->SetSize(500,500);
  renWin->SetPosition(600,400);
  renderer->SetBackground( 0.1, 0.2, 0.4 );


  vtkALBASmartPointer<vtkConeSource> test_cone;
  test_cone->SetResolution(30);
  test_cone->SetHeight(2);
  test_cone->SetRadius( 1.0 );
  //test_cone->CappingOn();
  vtkALBASmartPointer<vtkPolyDataMapper> test_mapper;
  test_mapper->SetInputConnection(test_cone->GetOutputPort());

  vtkALBASmartPointer<vtkActor> test_actor;
  test_actor->SetMapper(test_mapper);
  test_actor->SetPosition(4,0,0);
  //test_actor->GetProperty()->SetColor(1,0,0);
  renderer->AddActor(test_actor);

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);
	axes->Update();

  // create a tree of VMEs: notice ref. counter is initially set to 0,
  // so noneed to unregister VME's when they are attached to the tree!
  albaVMEGeneric *vtitle=albaVMEGeneric::New();
  albaVMEGeneric *vsphere=albaVMEGeneric::New();
  albaVMEGeneric *vcone=albaVMEGeneric::New();
  albaVMEGeneric *vmorph=albaVMEGeneric::New();

  vtitle->SetName("title");
  vsphere->SetName("sphere");
  vcone->SetName("cone");
  vmorph->SetName("morph");

  CPPUNIT_ASSERT(vtitle->GetOutput()->GetVTKData()==NULL);

  albaTransform trans;
  
  vtitle->SetData(axes->GetOutput(),0);

  root->AddChild(vtitle);
  
  vtitle->AddChild(vsphere);
  vtitle->AddChild(vcone);
  vtitle->AddChild(vmorph);


  albaMatrix test_matrix;
  
  // some preliminary tests to check if matrix vector and matrix pipe are working corretly
  test_matrix.Equals( vtitle->GetOutput()->GetAbsMatrix());

  albaTransform::SetPosition(test_matrix,10,10,10);
  vtitle->SetAbsMatrix(test_matrix,0);
  
  test_matrix.Equals( vtitle->GetOutput()->GetAbsMatrix());

  albaTransform::SetPosition(test_matrix,20,20,20);
  vcone->SetAbsMatrix(test_matrix,0);

  albaTransform::SetPosition(test_matrix,10,10,10);
  test_matrix.Equals( vcone->GetOutput()->GetAbsMatrix());

  test_matrix.Identity();
  vtitle->SetPose(test_matrix,0);
  vcone->SetPose(test_matrix,0);

  // arrays used to store time stamps for later testings
  std::vector<albaTimeStamp> data_time_stamps;
  std::vector<albaTimeStamp> matrix_time_stamps;

  // arrays used to store output data and pose matrices for later testing
  vtkALBAAutoPointer<vtkSphereSource> test_sphere[10];
  albaMatrix test_vtitle_pose[10],test_sphere_pose[10],test_sphere_abspose[10];

  //
  // Fill in VME-tree with data and pose matrices to create a beautiful animation
  //

  vmorph->SetPose(-1,0,0,0,0,90,0);
  // add first time stamp used for previous testing
  matrix_time_stamps.push_back(0);

  int i;
  for (i=0;i<100;i++)
  {
    trans.Identity();

    //trans.Translate(i*0.05,i*0.02,.05*i,POST_MULTIPLY);

    trans.RotateY(i*1.6-180.0,POST_MULTIPLY);

    //trans.RotateY(45);
    trans.RotateX(45,POST_MULTIPLY);

    vtitle->SetPose(trans.GetMatrix(),i); //

    // The sphere
    vtkALBASmartPointer<vtkSphereSource> sphere;
    sphere->SetRadius(.1+.01*i);
    sphere->Update();

    if ((i%10)==0)
    {
      test_sphere[i/10] = sphere;
      test_vtitle_pose[i/10]=trans.GetMatrix();
      test_vtitle_pose[i/10].SetTimeStamp(i);
    }

    vsphere->SetData(sphere->GetOutput(),i*.5+50);

    trans.Identity();
    trans.Translate(0,.02*i,0,POST_MULTIPLY);
    vsphere->SetPose(trans.GetMatrix(),i*.5+25); //

    // every ten iteration stores the output and the pose
    if ((i%10)==0)
    {
      test_sphere_pose[i/10]=trans.GetMatrix();
      test_sphere_pose[i/10].SetTimeStamp(i*.5+25);
    }

    // the cone
    vtkALBASmartPointer<vtkConeSource> cone;
    cone->SetResolution(103-i);
    cone->Update();

    vcone->SetData(cone->GetOutput(),200-i*2);
    data_time_stamps.insert(data_time_stamps.begin(),200-i*2);

    trans.Identity();
    trans.Translate(2-(double)i/50.0,0,0,POST_MULTIPLY);

    vcone->SetPose(trans.GetMatrix(),i*.5+75.11); //
    matrix_time_stamps.push_back(i*.5+75.11);

    vtkSmartPointer<vtkPolyDataAlgorithm> morph;

    // the morphing tube
    if (i<50)
    {
      vtkALBASmartPointer<vtkCylinderSource> cyl;
      cyl->SetResolution(52-i);
      morph=cyl;
    }
    else
    {
      vtkALBASmartPointer<vtkCubeSource> cube;
      cube->SetYLength((100.0-(double)i)/100.0+.5);
      cube->SetXLength(1);
      cube->SetZLength(1);
      morph=cube;
    }

    morph->Update();

    vmorph->SetData(morph->GetOutput(),(100-i));
  }

  //---------------------------------------------------------------------------------------
  // Test matrix pipe and Data Pipe
  //---------------------------------------------------------------------------------------

  for (i=0;i<10;i++)
  {
    root->SetTreeTime(i*10*.5+50);
    vtkPolyData *pdata=vtkPolyData::SafeDownCast(vsphere->GetOutput()->GetVTKData());

    // data has been generated by data pipe?
    CPPUNIT_ASSERT(pdata!=NULL);

    // does data generated by the data pipe store the right content?
    CPPUNIT_ASSERT(pdata->GetNumberOfCells()==test_sphere[i]->GetOutput()->GetNumberOfCells());
    CPPUNIT_ASSERT(pdata->GetNumberOfPoints()==test_sphere[i]->GetOutput()->GetNumberOfPoints());

    // Test Point Values
    for (int n=0;n<pdata->GetNumberOfPoints();n++)
    {
      double x[3],y[3];
      pdata->GetPoints()->GetPoint(n,x);
      test_sphere[i]->GetOutput()->GetPoints()->GetPoint(n,y);
      CPPUNIT_ASSERT(x[0]==y[0] && x[1]==y[1] && x[2]==y[2]);
    }
  }


  for (i=0;i<10;i++)
  {
    root->SetTreeTime(i*10);
    // is matrix generated by matrix pipe correct
    albaMatrix *pose=vtitle->GetOutput()->GetMatrix();
    CPPUNIT_ASSERT(pose!=NULL);
    CPPUNIT_ASSERT(*pose==test_vtitle_pose[i]);
  }
  

  for (i=0;i<10;i++)
  {
    root->SetTreeTime(i*10*.5+25);
    // is matrix generated by matrix pipe correct
    albaMatrix *pose=vsphere->GetOutput()->GetMatrix();
    CPPUNIT_ASSERT(pose!=NULL);
    CPPUNIT_ASSERT(*pose==test_sphere_pose[i]);  
  }

  // test random matrix access
  albaMatrix tmp_mat;
  vsphere->GetOutput()->GetMatrix(tmp_mat,50*.5+25);
  CPPUNIT_ASSERT(tmp_mat==test_sphere_pose[5]);

  //vsphere->GetOutput()->GetAbsMatrix(tmp_mat,50*.5+25);
  //CPPUNIT_ASSERT(tmp_mat==test_sphere_abspose[5]);

  //---------------------------------------------------------------------------------------
  //        Perform a generic TestRun
  //---------------------------------------------------------------------------------------

  cerr << "Testing VME time dynamic behavior\n";

  // Test display of generic VME tree of surfaces
  albaVMEIterator *iter=root->NewIterator();

  root->SetTreeTime(0);

  albaVME *node;

  // connect VME to assemblies and put root assembly into the renderer
  for (node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    if (node)
    {
    
      if (node->IsA("albaVMERoot")) 
      {
        vtkALBASmartPointer<vtkAssembly> vmeasm;
        albaSmartPointer<albaClientData> attr;
        attr->m_Prop3D=vmeasm;
        node->SetAttribute("ClientData",attr);
        renderer->AddActor(vmeasm);
      }
      else if (node->IsA("albaVMEPointSet"))
      {
        vtkDataSet *data=node->GetOutput()->GetVTKData();
        CPPUNIT_ASSERT(data!=NULL);

        CPPUNIT_ASSERT(data->IsA("vtkPolyData")!=0);

        vtkALBASmartPointer<vtkGlyph3D> glyph;
        glyph->SetInputData((vtkPolyData *)data);

        vtkALBASmartPointer<vtkPolyDataMapper> mapper;

        mapper->SetInputConnection(glyph->GetOutputPort());

        vtkALBASmartPointer<vtkSphereSource> sphere;
        sphere->SetRadius(.01);
  

        glyph->SetSourceConnection(sphere->GetOutputPort());

        glyph->SetScaleModeToScaleByScalar();
        glyph->SetColorModeToColorByScale();
  
        vtkALBASmartPointer<vtkActor> vmeact;
        vmeact->SetMapper(mapper);
        vmeact->GetProperty()->SetColor(0,1,0);
        vtkALBASmartPointer<vtkAssembly> vmeasm;
        vmeasm->AddPart(vmeact);
        albaSmartPointer<albaClientData> attr;
        attr->m_Prop3D=vmeasm;
        node->SetAttribute("ClientData",attr);

        vmeasm->SetUserTransform(node->GetOutput()->GetTransform()->GetVTKTransform());

        CPPUNIT_ASSERT(node->GetAttribute("ClientData")==attr.GetPointer());
      
        albaClientData *pattr=albaClientData::SafeDownCast(node->GetParent()->GetAttribute("ClientData"));
        vtkAssembly *pvmeasm=pattr->m_Prop3D;
      
        CPPUNIT_ASSERT(pvmeasm!=NULL);
        CPPUNIT_ASSERT(pvmeasm->IsA("vtkAssembly")!=0);
        pvmeasm->AddPart(vmeasm);
      }
      else
      {
        vtkDataSet *data=node->GetOutput()->GetVTKData();
        CPPUNIT_ASSERT(data!=NULL);

        CPPUNIT_ASSERT(data->IsA("vtkPolyData")!=0);

				albaDataPipeInterpolatorVTK *interpolator = albaDataPipeInterpolatorVTK::SafeDownCast(node->GetDataPipe());

				vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();

				mapper->SetInputConnection(interpolator->GetVTKDataPipe()->GetOutputPort());
  
        vtkALBASmartPointer<vtkActor> vmeact;
        vmeact->GetProperty()->SetColor(0,1,0);
        vmeact->GetProperty()->SetOpacity(1);
        vmeact->SetMapper(mapper);
        mapper->Delete();

        vtkALBASmartPointer<vtkAssembly> vmeasm;
        vmeasm->AddPart(vmeact);

        vmeasm->SetUserTransform(node->GetOutput()->GetTransform()->GetVTKTransform());

        albaSmartPointer<albaClientData> attr;
        attr->m_Prop3D=vmeasm;
        node->SetAttribute("ClientData",attr);

        CPPUNIT_ASSERT(node->GetAttribute("ClientData")==attr.GetPointer());

        albaClientData *pattr=albaClientData::SafeDownCast(node->GetParent()->GetAttribute("ClientData"));
        vtkAssembly *pvmeasm=pattr->m_Prop3D;
      
        CPPUNIT_ASSERT(pvmeasm!=NULL);
        CPPUNIT_ASSERT(pvmeasm->IsA("vtkAssembly")!=0);
        pvmeasm->AddPart(vmeasm);
      }
    }
  }

  renderer->ResetCamera();

  int t;
  for (t = 0; t < 200; t++) 
  {   
    // Update time into all the tree
    root->SetTreeTime(t);
  
    renderer->GetActiveCamera()->Azimuth(-1);
    renderer->ResetCameraClippingRange();
    renWin->Render();
  }

  //--------------------------------------------------------------------------
  //          Test the GetTimeBounds function of the VME tree
  //--------------------------------------------------------------------------

  cerr << "Testing GetTimeBounds()\n";

  albaTimeStamp treeTimeBounds[2],vtitleTimeBounds[2], vsphereTimeBounds[2],vconeTimeBounds[2],vmorphTimeBounds[2];

  root->GetOutput()->GetTimeBounds(treeTimeBounds);

  vtitle->GetOutput()->GetLocalTimeBounds(vtitleTimeBounds);
  vsphere->GetOutput()->GetLocalTimeBounds(vsphereTimeBounds);
  vcone->GetOutput()->GetLocalTimeBounds(vconeTimeBounds);
  vmorph->GetOutput()->GetLocalTimeBounds(vmorphTimeBounds);
  int sphereAllTimeStamps=vsphere->GetNumberOfTimeStamps();
  int sphereDataTimeStamps=vsphere->GetDataVector()->GetNumberOfItems();
  int sphereMatrixTimeStamps=vsphere->GetMatrixVector()->GetNumberOfItems();

  int treeTimesStamps=vtitle->GetNumberOfTimeStamps();

  //CPPUNIT_ASSERT(treeTimesStamps==376);

  // time bases for data and matrixes do not overlap
  CPPUNIT_ASSERT(sphereAllTimeStamps==150);

  cerr << "Tree TBounds = [" << treeTimeBounds[0] <<"," << treeTimeBounds[1] << "]\n";
  cerr << "Title TBounds = [" << vtitleTimeBounds[0] <<"," << vtitleTimeBounds[1] << "]\n";
  cerr << "Sphere TBounds = [" << vsphereTimeBounds[0] <<"," << vsphereTimeBounds[1] << "]\n";
  cerr << "Cone TBounds = [" << vconeTimeBounds[0] <<"," << vconeTimeBounds[1] << "]\n";
  cerr << "Morph TBounds = [" << vmorphTimeBounds[0] <<"," << vmorphTimeBounds[1] << "]\n";
  
  std::vector<albaTimeStamp> dataTimeStamps;
  std::vector<albaTimeStamp> matrixTimeStamps;
  
  vcone->GetDataVector()->GetTimeStamps(dataTimeStamps);
  vcone->GetMatrixVector()->GetTimeStamps(matrixTimeStamps);

  CPPUNIT_ASSERT(dataTimeStamps.size()==data_time_stamps.size());
  CPPUNIT_ASSERT(matrixTimeStamps.size()==matrix_time_stamps.size());

  cerr << "VME-Cone data_time_stamps={";
  for (int n=0;n<dataTimeStamps.size();n++)
  {
    albaTimeStamp t1=dataTimeStamps[n];

    albaTimeStamp t2=data_time_stamps[n];

    //CPPUNIT_ASSERT(t1==t2);
    cerr << t1 ;

    if (n<dataTimeStamps.size()-1)
    {
      cerr << ",";
    }
  }
  cerr << "}\n";

  cerr << "VME-cone matrix_time_stamps={";

  for (int m=0;m<matrixTimeStamps.size();m++)
  {
    albaTimeStamp t1=matrixTimeStamps[m];

    albaTimeStamp t2=matrix_time_stamps[m];

    CPPUNIT_ASSERT(t1==t2);

    cerr << t1 ;

    if (m<matrixTimeStamps.size()-1)
    {
      cerr << ",";
    }
  }

  cerr << "}\n";

  iter->Delete();

  //--------------------------------------------------------------------------
  //          Test the GetBounds Get4DBounds and GetLocal4DBounds function of the VME tree
  //--------------------------------------------------------------------------
  cerr << "Testing GetBounds() Get4DBounds() and GetLocal4DBounds()\n";

  // Iterate through the tree to create a single big geometry of the whole tree  
  iter=root->NewIterator();
  root->SetTreeTime(0);

  
  albaOBB treeBounds,tree4DBounds;
  root->GetOutput()->GetBounds(treeBounds);

  root->GetOutput()->Get4DBounds(tree4DBounds);

  // Bounding box linked to root->GetBounds()
  vtkOutlineSource *treeBoundsBox=vtkOutlineSource::New();
  treeBoundsBox->SetBounds(treeBounds.m_Bounds[0],treeBounds.m_Bounds[1],treeBounds.m_Bounds[2], \
    treeBounds.m_Bounds[3],treeBounds.m_Bounds[4],treeBounds.m_Bounds[5]);

  vtkPolyDataMapper *treeBoundsMapper=vtkPolyDataMapper::New();
  treeBoundsMapper->SetInputConnection(treeBoundsBox->GetOutputPort());
  treeBoundsBox->Delete();

  vtkActor *treeBoundsActor=vtkActor::New();
  treeBoundsActor->SetMapper(treeBoundsMapper);
  treeBoundsMapper->Delete();

  treeBoundsActor->GetProperty()->SetColor(1,1,0);

  // 4D Bounding box linked to root->Get4DBounds()
  vtkOutlineSource *tree4DBoundsBox=vtkOutlineSource::New();
  tree4DBoundsBox->SetBounds(tree4DBounds.m_Bounds[0],tree4DBounds.m_Bounds[1],tree4DBounds.m_Bounds[2], \
    tree4DBounds.m_Bounds[3],tree4DBounds.m_Bounds[4],tree4DBounds.m_Bounds[5]);

  vtkPolyDataMapper *tree4DBoundsMapper=vtkPolyDataMapper::New();
  tree4DBoundsMapper->SetInputConnection(tree4DBoundsBox->GetOutputPort());
  tree4DBoundsBox->Delete();

  vtkActor *tree4DBoundsActor=vtkActor::New();
  tree4DBoundsActor->SetMapper(tree4DBoundsMapper);
  tree4DBoundsMapper->Delete();

  tree4DBoundsActor->GetProperty()->SetColor(1,0,0);

  albaClientData *attr=albaClientData::SafeDownCast(root->GetAttribute("ClientData")); 
  vtkAssembly *rootAsm=attr->m_Prop3D;

  double asmBounds[6];
  rootAsm->GetBounds(asmBounds);

  // Bounding box linked to rootAsm->vtkAssembly::GetBounds()
  vtkOutlineSource *asmBoundsBox=vtkOutlineSource::New();
  treeBoundsBox->SetBounds(treeBounds.m_Bounds[0],treeBounds.m_Bounds[1],treeBounds.m_Bounds[2], \
    treeBounds.m_Bounds[3],treeBounds.m_Bounds[4],treeBounds.m_Bounds[5]);

  asmBoundsBox->SetBounds(asmBounds);

  vtkPolyDataMapper *asmBoundsMapper=vtkPolyDataMapper::New();
  asmBoundsMapper->SetInputConnection(asmBoundsBox->GetOutputPort());
  asmBoundsBox->Delete();

  vtkActor *asmBoundsActor=vtkActor::New();
  asmBoundsActor->SetMapper(asmBoundsMapper);
  asmBoundsMapper->Delete();

  asmBoundsActor->GetProperty()->SetColor(0,1,1);


  rootAsm->GetBounds(asmBounds);

  CPPUNIT_ASSERT(treeBounds.Equals(asmBounds)!=0);

  renderer->AddActor(tree4DBoundsActor);

  //renderer->ResetCamera();
  renWin->Render();

  renderer->AddActor(treeBoundsActor);
  renderer->AddActor(asmBoundsActor);

  tree4DBoundsActor->Delete();
  treeBoundsActor->Delete();
  asmBoundsActor->Delete();


  for (t = 0; t < 200; t++) 
  {
    // Update time into all the tree
    root->SetTreeTime(t);

    renderer->ResetCameraClippingRange();
		renderer->GetActiveCamera()->Azimuth(1);
		renWin->Render();

    root->GetOutput()->GetBounds(treeBounds);
    //vtitle->GetOutput()->GetBounds(treeBounds);

    rootAsm->GetBounds(asmBounds);

    treeBoundsBox->SetBounds(treeBounds.m_Bounds[0],treeBounds.m_Bounds[1],treeBounds.m_Bounds[2], \
    treeBounds.m_Bounds[3],treeBounds.m_Bounds[4],treeBounds.m_Bounds[5]);

    asmBoundsBox->SetBounds(asmBounds);    

    CPPUNIT_ASSERT(treeBounds.Equals(asmBounds));
	
  }


  albaClientData *root_attr=albaClientData::SafeDownCast(root->GetAttribute("ClientData"));
  vtkAssembly *rasm=root_attr->m_Prop3D;
  renderer->RemoveActor(rasm);
  
  for (node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    CPPUNIT_ASSERT(node);
    node->RemoveAttribute("ClientData");
  }

  iter->Delete();

/*
  //--------------------------------------------------------------------------
  //          Test the Auxiliary Reference System
  //--------------------------------------------------------------------------
		
	cerr << "\n";
	cerr << "\n";
  cerr << "Testing Auxiliary Reference System Set/Get\n";
	cerr << "\n";
	
	//create random transform
	albaTransform v_t;

	float translation[3] = {vtkMath::Random(0.0, 10.0), 
													vtkMath::Random(0.0, 10.0), 
													vtkMath::Random(0.0, 10.0) };

	float angle[3] = {vtkMath::Random(0.0, 90.0), 
									 vtkMath::Random(0.0, 90.0), 
									 vtkMath::Random(0.0, 90.0) };

	float scale[3]	= {vtkMath::Random(0.0, 10),
							 vtkMath::Random(0.0, 10),
							 vtkMath::Random(0.0, 10)};

  v_t.Translate(translation);
  v_t.RotateX(angle[0]);
  v_t.RotateY(angle[1]);
  v_t.RotateZ(angle[2]);
  v_t.Scale(scale);

  cerr << "##################################################" << "\n";
  cerr << "Creating PIPPO local auxiliary ref sys..." << "\n";
  cerr << "##################################################" << "\n";

  cerr << "Auxiliary ref sys name:" << "\n";
  cerr << "PIPPO REFSYS" << "\n";


  cerr << "Setting auxiliary ref sys..." << "\n";
  vsphere->SetAuxiliaryRefSys(v_t.GetMatrix(), "PIPPO");
  cerr << "done" << "\n";

  cerr << "\n";

  albaMatrix test_frame;

  cerr << "##################################################" << "\n";
  cerr << "Getting PIPPO Auxiliary Ref Sys..." << "\n";
  cerr << "##################################################" << "\n";
  cerr << "\n";

  cerr << "testing GetAuxiliaryRefSys(test_frame, PIPPO);" << "\n";
  vsphere->GetAuxiliaryRefSys(test_frame, "PIPPO");

  test_frame->Print(cerr);

  test_frame->Identity();

  cerr << "testing GetAuxiliaryRefSys(test_frame, PIPPO, MFL_LOCAL_FRAME_TAG);" << "\n";
  vsphere->GetAuxiliaryRefSys(test_frame, "PIPPO");

  test_frame->Print(cerr);

  test_frame->Identity();

  cerr << "testing GetAuxiliaryRefSys(test_frame, PIPPO, MFL_GLOBAL_FRAME_TAG);" << "\n";
  vsphere->GetAuxiliaryRefSys(test_frame, "PIPPO", MFL_GLOBAL_FRAME_TAG);

  test_frame->Print(cerr);

  //create a global ref sys
  cerr << "##################################################" << "\n";
  cerr << "Creating PLUTO global auxiliary ref sys..." << "\n";
  cerr << "##################################################" << "\n";

  cerr << "Auxiliary ref sys name:" << "\n";
  cerr << "PLUTO REFSYS" << "\n";


  cerr << "Setting global auxiliary ref sys..." << "\n";
  vsphere->SetAuxiliaryRefSys(v_t.GetMatrix(), "PLUTO", MFL_GLOBAL_FRAME_TAG);
  cerr << "done" << "\n";

  cerr << "\n";
  cerr << "##################################################" << "\n";
  cerr << "Getting PLUTO Auxiliary Ref Sys..." << "\n";
  cerr << "##################################################" << "\n";cerr << "\n";

  cerr << "testing GetAuxiliaryRefSys(test_frame, PLUTO);" << "\n";
  vsphere->GetAuxiliaryRefSys(test_frame, "PLUTO");

  test_frame->Print(cerr);

  test_frame->Identity();


  cerr << "testing GetAuxiliaryRefSys(test_frame, PLUTO, MFL_LOCAL_FRAME_TAG);" << "\n";
  vsphere->GetAuxiliaryRefSys(test_frame, "PLUTO");

  test_frame->Print(cerr);

  test_frame->Identity();

  cerr << "testing GetAuxiliaryRefSys(test_frame, PLUTO, MFL_GLOBAL_FRAME_TAG);" << "\n";
  vsphere->GetAuxiliaryRefSys(test_frame, "PLUTO", MFL_GLOBAL_FRAME_TAG);

  test_frame->Print(cerr);


*/

  //---------------------------------------------------------------------------------------
  //        Perform a Copy Test    
  //---------------------------------------------------------------------------------------


  cerr << "Testing VME DeepCopy(), CopyTree() and CompareTree()\n";

  //------------------------
  // Create a simple tree
  //------------------------
  vtitle->GetTagArray()->SetTag(albaTagItem("TestTag1","1"));
  const char *multcomp[3]={"100","200","300"};
  vtitle->GetTagArray()->SetTag(albaTagItem("TestTag2",multcomp,3));
  vtitle->GetTagArray()->SetTag(albaTagItem("TestTag1",5.5555));

  //---------------------------
  // try to copy the sub tree
  //---------------------------

  albaAutoPointer<albaVMERoot> newroot=albaVMERoot::SafeDownCast(root->CopyTree());

  albaVMEItem::GlobalCompareDataFlagOn();
  //---------------------------------------
  // Compare the new tree with the old one
  //---------------------------------------
  CPPUNIT_ASSERT(root->CompareTree(newroot));
  CPPUNIT_ASSERT(newroot->CompareTree(root));

  //------------------------------
  // compare test that should fail
  //------------------------------
  albaAutoPointer<albaVME> badVME=vmorph->MakeCopy();

  albaMatrix mat;
  mat.SetTimeStamp(23.4455667788);
  badVME->SetMatrix(mat);

  CPPUNIT_ASSERT(!vmorph->Equals(badVME));


  albaAutoPointer<albaVME> badTree=root->GetFirstChild()->CopyTree();
  badTree->AddChild(badVME);
  
  CPPUNIT_ASSERT(!root->GetFirstChild()->CompareTree(badTree));

  CPPUNIT_ASSERT(newroot->GetReferenceCount()==1);
  CPPUNIT_ASSERT(badTree->GetReferenceCount()==1);
  CPPUNIT_ASSERT(badVME->GetReferenceCount()==2);

  std::cerr<<"Test completed successfully!"<<std::endl;


  albaDEL(vtitle);
  albaDEL(vsphere);
  albaDEL(vcone);
  albaDEL(vmorph);
}
