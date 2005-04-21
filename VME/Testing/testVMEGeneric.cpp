// Test mafVMEGeneric class

#include "mafVMEGeneric.h"
#include "mafVMERoot.h"
#include "mafTransform.h"
#include "mafAttribute.h"
#include "mafVMEOutput.h"
#include "mafDataVector.h"
#include "mafMatrixVector.h"
#include "mafNodeIterator.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafStorageElement.h"

#include "vtkMAFSmartPointer.h"
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

/** attribute class for attaching vtkActor to VME */
class mafClientData : public mafAttribute
{
public:
  mafTypeMacro(mafClientData,mafAttribute);

  vtkMAFAutoPointer<vtkAssembly> m_Prop3D;

  virtual void DeepCopy(const mafAttribute *a) {Superclass::DeepCopy(a); m_Prop3D=((mafClientData *)a)->m_Prop3D;}
  virtual bool Equals(const mafAttribute *a) const {return Superclass::Equals(a)&&m_Prop3D==((mafClientData *)a)->m_Prop3D;}
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafClientData);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------
{
  // create a small tree with 
  mafSmartPointer<mafVMERoot> root;
  root->SetName("ROOT");

  // create windows
  vtkMAFSmartPointer<vtkRenderer> renderer;
  vtkMAFSmartPointer<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);

  renWin->SetSize(1000,1000);
  renderer->SetBackground( 0.1, 0.2, 0.4 );


  vtkMAFSmartPointer<vtkConeSource> test_cone;
  test_cone->SetResolution(30);
  test_cone->SetHeight(2);
  test_cone->SetRadius( 1.0 );
  //test_cone->CappingOn();
  vtkMAFSmartPointer<vtkPolyDataMapper> test_mapper;
  test_mapper->SetInput(test_cone->GetOutput());

  vtkMAFSmartPointer<vtkActor> test_actor;
  test_actor->SetMapper(test_mapper);
  test_actor->SetPosition(4,0,0);
  //test_actor->GetProperty()->SetColor(1,0,0);
  renderer->AddActor(test_actor);

  vtkMAFSmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);

  // create a tree of VMEs: notice ref. counter is initially set to 0,
  // so noneed to unregister VME's when they are attached to the tree!
  mafVMEGeneric *vtitle=mafVMEGeneric::New();
  mafVMEGeneric *vsphere=mafVMEGeneric::New();
  mafVMEGeneric *vcone=mafVMEGeneric::New();
  mafVMEGeneric *vmorph=mafVMEGeneric::New();

  vtitle->SetName("title");
  vsphere->SetName("sphere");
  vcone->SetName("cone");
  vmorph->SetName("morph");

  mafTransform trans;
  
  vtitle->SetData(axes->GetOutput(),0);

  root->AddChild(vtitle);
  
  vtitle->AddChild(vsphere);
  vtitle->AddChild(vcone);
  vtitle->AddChild(vmorph);


  // arrays used to store time stamps for later testings
  std::vector<mafTimeStamp> data_time_stamps;
  std::vector<mafTimeStamp> matrix_time_stamps;

  // arrays used to store output data and pose matrices for later testing
  vtkMAFAutoPointer<vtkSphereSource> test_sphere[10];
  mafMatrix test_vtitle_pose[10],test_sphere_pose[10],test_sphere_abspose[10];

  //
  // Fill in VME-tree with data and pose matrices to create a beautiful animation
  //

  vmorph->SetPose(-1,0,0,0,0,90,0);

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
    vtkMAFSmartPointer<vtkSphereSource> sphere;
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
    vtkMAFSmartPointer<vtkConeSource> cone;
    cone->SetResolution(103-i);
    cone->Update();

    vcone->SetData(cone->GetOutput(),200-i*2);
    data_time_stamps.insert(data_time_stamps.begin(),200-i*2);

    trans.Identity();
    trans.Translate(2-(double)i/50.0,0,0,POST_MULTIPLY);

    vcone->SetPose(trans.GetMatrix(),i*.5+75.11); //
    matrix_time_stamps.push_back(i*.5+75.11);

    vtkSmartPointer<vtkPolyDataSource> morph;

    // the morphing tube
    if (i<50)
    {
      vtkMAFSmartPointer<vtkCylinderSource> cyl;
      cyl->SetResolution(52-i);
      morph=cyl;
    }
    else
    {
      vtkMAFSmartPointer<vtkCubeSource> cube;
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
    MAF_TEST(pdata!=NULL);

    // does data generated by the data pipe store the right content?
    pdata->Update();
    MAF_TEST(pdata->GetNumberOfCells()==test_sphere[i]->GetOutput()->GetNumberOfCells());
    MAF_TEST(pdata->GetNumberOfPoints()==test_sphere[i]->GetOutput()->GetNumberOfPoints());

    // Test Point Values
    for (int n=0;n<pdata->GetNumberOfPoints();n++)
    {
      double x[3],y[3];
      pdata->GetPoints()->GetPoint(n,x);
      test_sphere[i]->GetOutput()->GetPoints()->GetPoint(n,y);
      MAF_TEST(x[0]==y[0] && x[1]==y[1] && x[2]==y[2]);
    }
  }


  for (i=0;i<10;i++)
  {
    root->SetTreeTime(i*10);
    // is matrix generated by matrix pipe correct
    mafMatrix *pose=vtitle->GetOutput()->GetMatrix();
    MAF_TEST(pose!=NULL);
    MAF_TEST(*pose==test_vtitle_pose[i]);
  }
  

  for (i=0;i<10;i++)
  {
    root->SetTreeTime(i*10*.5+25);
    // is matrix generated by matrix pipe correct
    mafMatrix *pose=vsphere->GetOutput()->GetMatrix();
    MAF_TEST(pose!=NULL);
    MAF_TEST(*pose==test_sphere_pose[i]);  
  }

  // test random matrix access
  mafMatrix tmp_mat;
  vsphere->GetOutput()->GetMatrix(tmp_mat,50*.5+25);
  MAF_TEST(tmp_mat==test_sphere_pose[5]);

  //vsphere->GetOutput()->GetAbsMatrix(tmp_mat,50*.5+25);
  //MAF_TEST(tmp_mat==test_sphere_abspose[5]);

  //---------------------------------------------------------------------------------------
  //        Perform a generic TestRun
  //---------------------------------------------------------------------------------------

  cerr << "Testing VME time dynamic behavior\n";

  // Test display of generic VME tree of surfaces
  mafNodeIterator *iter=root->NewIterator();

  root->SetTreeTime(0);

  mafNode *node;

  // connect VME to assemblies and put root assembly into the renderer
  for (node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    if (mafVME *vme=mafVME::SafeDownCast(node))
    {
    
      if (vme->IsA("mafVMERoot")) 
      {
        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        mafSmartPointer<mafClientData> attr;
        attr->m_Prop3D=vmeasm;
        vme->SetAttribute("ClientData",attr);
        renderer->AddActor(vmeasm);
      }
      else if (vme->IsA("mafVMEPointSet"))
      {
        vtkDataSet *data=vme->GetOutput()->GetVTKData();
        MAF_TEST(data!=NULL);

        MAF_TEST(data->IsA("vtkPolyData")!=0);

        vtkMAFSmartPointer<vtkGlyph3D> glyph;
        glyph->SetInput((vtkPolyData *)data);

        vtkMAFSmartPointer<vtkPolyDataMapper> mapper;

        mapper->SetInput(glyph->GetOutput());

        vtkMAFSmartPointer<vtkSphereSource> sphere;
        sphere->SetRadius(.01);
  

        glyph->SetSource(sphere->GetOutput());

        glyph->SetScaleModeToScaleByScalar();
        glyph->SetColorModeToColorByScale();
  
        vtkMAFSmartPointer<vtkActor> vmeact;
        vmeact->SetMapper(mapper);
        vmeact->GetProperty()->SetColor(0,1,0);
        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        vmeasm->AddPart(vmeact);
        mafSmartPointer<mafClientData> attr;
        attr->m_Prop3D=vmeasm;
        vme->SetAttribute("ClientData",attr);

        vmeasm->SetUserTransform(vme->GetOutput()->GetTransform()->GetVTKTransform());

        MAF_TEST(vme->GetAttribute("ClientData")==attr.GetPointer());
      
        mafClientData *pattr=mafClientData::SafeDownCast(vme->GetParent()->GetAttribute("ClientData"));
        vtkAssembly *pvmeasm=pattr->m_Prop3D;
      
        MAF_TEST(pvmeasm!=NULL);
        MAF_TEST(pvmeasm->IsA("vtkAssembly")!=0);
        pvmeasm->AddPart(vmeasm);
      }
      else
      {
        vtkDataSet *data=vme->GetOutput()->GetVTKData();
        MAF_TEST(data!=NULL);

        MAF_TEST(data->IsA("vtkPolyData")!=0);

        vtkPolyDataMapper *mapper=vtkPolyDataMapper::New();

        mapper->SetInput((vtkPolyData *)data);
  
        vtkMAFSmartPointer<vtkActor> vmeact;
        vmeact->GetProperty()->SetColor(0,1,0);
        vmeact->GetProperty()->SetOpacity(1);
        vmeact->SetMapper(mapper);
        mapper->Delete();

        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        vmeasm->AddPart(vmeact);

        vmeasm->SetUserTransform(vme->GetOutput()->GetTransform()->GetVTKTransform());

        mafSmartPointer<mafClientData> attr;
        attr->m_Prop3D=vmeasm;
        vme->SetAttribute("ClientData",attr);

        MAF_TEST(vme->GetAttribute("ClientData")==attr.GetPointer());

        mafClientData *pattr=mafClientData::SafeDownCast(vme->GetParent()->GetAttribute("ClientData"));
        vtkAssembly *pvmeasm=pattr->m_Prop3D;
      
        MAF_TEST(pvmeasm!=NULL);
        MAF_TEST(pvmeasm->IsA("vtkAssembly")!=0);
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
    mafSleep(10);
  }

  //--------------------------------------------------------------------------
  //          Test the GetTimeBounds function of the VME tree
  //--------------------------------------------------------------------------

  cerr << "Testing GetTimeBounds()\n";

  mafTimeStamp treeTimeBounds[2],vtitleTimeBounds[2], vsphereTimeBounds[2],vconeTimeBounds[2],vmorphTimeBounds[2];

  root->GetOutput()->GetTimeBounds(treeTimeBounds);

  vtitle->GetOutput()->GetLocalTimeBounds(vtitleTimeBounds);
  vsphere->GetOutput()->GetLocalTimeBounds(vsphereTimeBounds);
  vcone->GetOutput()->GetLocalTimeBounds(vconeTimeBounds);
  vmorph->GetOutput()->GetLocalTimeBounds(vmorphTimeBounds);
  int sphereAllTimeStamps=vsphere->GetNumberOfTimeStamps();
  int sphereDataTimeStamps=vsphere->GetDataVector()->GetNumberOfItems();
  int sphereMatrixTimeStamps=vsphere->GetMatrixVector()->GetNumberOfItems();

  int treeTimesStamps=vtitle->GetNumberOfTimeStamps();

  //MAF_TEST(treeTimesStamps==376);

  // time bases for data and matrixes do not overlap
  MAF_TEST(sphereAllTimeStamps==150);

  cerr << "Tree TBounds = [" << treeTimeBounds[0] <<"," << treeTimeBounds[1] << "]\n";
  cerr << "Title TBounds = [" << vtitleTimeBounds[0] <<"," << vtitleTimeBounds[1] << "]\n";
  cerr << "Sphere TBounds = [" << vsphereTimeBounds[0] <<"," << vsphereTimeBounds[1] << "]\n";
  cerr << "Cone TBounds = [" << vconeTimeBounds[0] <<"," << vconeTimeBounds[1] << "]\n";
  cerr << "Morph TBounds = [" << vmorphTimeBounds[0] <<"," << vmorphTimeBounds[1] << "]\n";
  
  std::vector<mafTimeStamp> dataTimeStamps;
  std::vector<mafTimeStamp> matrixTimeStamps;
  
  vcone->GetDataVector()->GetTimeStamps(dataTimeStamps);
  vcone->GetMatrixVector()->GetTimeStamps(matrixTimeStamps);

  MAF_TEST(dataTimeStamps.size()==data_time_stamps.size());
  MAF_TEST(matrixTimeStamps.size()==matrix_time_stamps.size());

  cerr << "VME-Cone data_time_stamps={";
  for (int n=0;n<dataTimeStamps.size();n++)
  {
    mafTimeStamp t1=dataTimeStamps[n];

    mafTimeStamp t2=data_time_stamps[n];

    //MAF_TEST(t1==t2);
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
    mafTimeStamp t1=matrixTimeStamps[m];

    mafTimeStamp t2=matrix_time_stamps[m];

    MAF_TEST(t1==t2);

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

  
  mafOBB treeBounds,tree4DBounds;
  root->GetOutput()->GetBounds(treeBounds);

  root->GetOutput()->Get4DBounds(tree4DBounds);

  // Bounding box linked to root->GetBounds()
  vtkOutlineSource *treeBoundsBox=vtkOutlineSource::New();
  treeBoundsBox->SetBounds(treeBounds.m_Bounds[0],treeBounds.m_Bounds[1],treeBounds.m_Bounds[2], \
    treeBounds.m_Bounds[3],treeBounds.m_Bounds[4],treeBounds.m_Bounds[5]);

  vtkPolyDataMapper *treeBoundsMapper=vtkPolyDataMapper::New();
  treeBoundsMapper->SetInput(treeBoundsBox->GetOutput());
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
  tree4DBoundsMapper->SetInput(tree4DBoundsBox->GetOutput());
  tree4DBoundsBox->Delete();

  vtkActor *tree4DBoundsActor=vtkActor::New();
  tree4DBoundsActor->SetMapper(tree4DBoundsMapper);
  tree4DBoundsMapper->Delete();

  tree4DBoundsActor->GetProperty()->SetColor(1,0,0);

  mafClientData *attr=mafClientData::SafeDownCast(root->GetAttribute("ClientData")); 
  vtkAssembly *rootAsm=attr->m_Prop3D;

  double asmBounds[6];
  rootAsm->GetBounds(asmBounds);

  // Bounding box linked to rootAsm->vtkAssembly::GetBounds()
  vtkOutlineSource *asmBoundsBox=vtkOutlineSource::New();
  treeBoundsBox->SetBounds(treeBounds.m_Bounds[0],treeBounds.m_Bounds[1],treeBounds.m_Bounds[2], \
    treeBounds.m_Bounds[3],treeBounds.m_Bounds[4],treeBounds.m_Bounds[5]);

  asmBoundsBox->SetBounds(asmBounds);

  vtkPolyDataMapper *asmBoundsMapper=vtkPolyDataMapper::New();
  asmBoundsMapper->SetInput(asmBoundsBox->GetOutput());
  asmBoundsBox->Delete();

  vtkActor *asmBoundsActor=vtkActor::New();
  asmBoundsActor->SetMapper(asmBoundsMapper);
  asmBoundsMapper->Delete();

  asmBoundsActor->GetProperty()->SetColor(0,1,1);


  rootAsm->GetBounds(asmBounds);

  MAF_TEST(treeBounds.Equals(asmBounds)!=0);

  renderer->AddActor(tree4DBoundsActor);

  //mafSleep(500);

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

    mafSleep(10);

    root->GetOutput()->GetBounds(treeBounds);
    //vtitle->GetOutput()->GetBounds(treeBounds);

    rootAsm->GetBounds(asmBounds);

    treeBoundsBox->SetBounds(treeBounds.m_Bounds[0],treeBounds.m_Bounds[1],treeBounds.m_Bounds[2], \
    treeBounds.m_Bounds[3],treeBounds.m_Bounds[4],treeBounds.m_Bounds[5]);

    asmBoundsBox->SetBounds(asmBounds);    

    MAF_TEST(treeBounds.Equals(asmBounds));
  
  }


  mafClientData *root_attr=mafClientData::SafeDownCast(root->GetAttribute("ClientData"));
  vtkAssembly *rasm=root_attr->m_Prop3D;
  renderer->RemoveActor(rasm);
  
  for (node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    mafVME *vme=mafVME::SafeDownCast(node);
    MAF_TEST(vme);
    vme->RemoveAttribute("ClientData");
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
	mafTransform v_t;

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

  mafMatrix test_frame;

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
  vtitle->GetTagArray()->SetTag(mafTagItem("TestTag1","1"));
  const char *multcomp[3]={"100","200","300"};
  vtitle->GetTagArray()->SetTag(mafTagItem("TestTag2",multcomp,3));
  vtitle->GetTagArray()->SetTag(mafTagItem("TestTag1",5.5555));

  //---------------------------
  // try to copy the sub tree
  //---------------------------

  mafAutoPointer<mafVMERoot> newroot=mafVMERoot::SafeDownCast(root->CopyTree());

  mafVMEItem::GlobalCompareDataFlagOn();
  //---------------------------------------
  // Compare the new tree with the old one
  //---------------------------------------
  MAF_TEST(root->CompareTree(newroot));
  MAF_TEST(newroot->CompareTree(root));

  //------------------------------
  // compare test that should fail
  //------------------------------
  mafAutoPointer<mafVME> badVME=mafVME::SafeDownCast(vmorph->MakeCopy());

  mafMatrix mat;
  mat.SetTimeStamp(23.4455667788);
  badVME->SetMatrix(mat);

  MAF_TEST(!vmorph->Equals(badVME));


  mafAutoPointer<mafVME> badTree=mafVME::SafeDownCast(root->GetFirstChild()->CopyTree());
  badTree->AddChild(badVME);
  
  MAF_TEST(!root->GetFirstChild()->CompareTree(badTree));

  MAF_TEST(newroot->GetReferenceCount()==1);

  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
