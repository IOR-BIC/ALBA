// Test mafVMEGeneric class

#include "mafVMEGeneric.h"
#include "mafVMERoot.h"
#include "mafTransform.h"
#include "mafVMEOutput.h"
//#include "mafDataVector.h"
//#include "mafMatrixVector.h"
#include "mafNodeIterator.h"
//#include "mafTagArray.h"
//#include "mafTagItem.h"
#include "mafOBB.h"

#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
//#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
//#include "vtkConeSource.h"
//#include "vtkCylinderSource.h"
//#include "vtkAxes.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyData.h"
//#include "vtkGlyph3D.h"
#include "vtkOutlineSource.h"
#include "vtkCamera.h"
#include "vtkProperty.h"

#include <iostream>

#ifdef WIN32
  #define SLEEP(a) Sleep(a)
#else
  #include <unistd.h>
  #define SLEEP(a) usleep(a*10)
#endif

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
  vtkMAFSmartPointer<vtkRenderWindowInteractor> iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  //renderer->SetBackground(0.1, 0.1, 0.1);
  renWin->SetSize(640, 480);
  renWin->SetPosition(400,0);
  //renWin->StereoCapableWindowOn();
  //renWin->StereoRenderOn();

  // create a tree of VMEs composed of 2 boxes under a root box.
  mafVMEGeneric *box0=mafVMEGeneric::New();
  mafVMEGeneric *box1=mafVMEGeneric::New();
  mafVMEGeneric *box2=mafVMEGeneric::New();

  box0->SetName("box0");
  box1->SetName("box1");
  box2->SetName("box2");

  // Let's create an animation which makes the children boxes to move 
  // from a starting central position to lateral position.
  // Later on the central box moves up.
  // Lase part of animation will see the boxes to contemporary increase their size.

  root->AddChild(box0);
  box0->AddChild(box1);
  box0->AddChild(box2);

  vtkMAFSmartPointer<vtkCubeSource> box_source;
  box_source->SetCenter(0,0,0);
  box_source->SetXLength(1);
  box_source->SetYLength(1);
  box_source->SetZLength(1);

  // set data for the different boxes at time 0
  box0->SetData(box_source->GetOutput(),0);
  box1->SetData(box_source->GetOutput(),0);
  box2->SetData(box_source->GetOutput(),0);

  mafTransform trans;

  // must store cube bounds since vtkCubeSource has a bug and generates
  // a cube of wrong size.
  mafOBB cube_bounds[201];

  //
  // Fill in the tree poses to make the boxes to move.
  // From 0 to 49 lateral boxes move aside.
  // From 50 to 99 central box moves up.
  //
  box0->SetPose(trans.GetMatrix(),0);
  int i;
  for (i=0;i<=100;i++)
  {
    if (i<=50)
    {
      // for i<50 move lateral boxes aside
      trans.Identity();
      trans.SetPosition(i,0,0);
      box1->SetPose(trans.GetMatrix(),i);
      trans.SetPosition(-i,0,0);
      box2->SetPose(trans.GetMatrix(),i);
    }
    else
    {
      
      // for i>=50 move the central box up
      trans.Identity();
      trans.SetPosition(0,i-50,0);
      box0->SetPose(trans.GetMatrix(),i);
    }
    cube_bounds[i].DeepCopy(box_source->GetOutput()->GetBounds());
  }
    
  for (i=100;i<200;i++)
  {
    // make the boxes to gown asymmetrically along the 3 axes
    box_source->SetXLength(1.0+((((double)i-100))/100.0)*3.0); 
    box_source->SetYLength(1.0+((((double)i-100))/100.0)*2.0);
    box_source->SetYLength(1.0+((((double)i-100))/100.0)*4.0);

    box0->SetData(box_source->GetOutput(),i);
    box1->SetData(box_source->GetOutput(),i);
    box2->SetData(box_source->GetOutput(),i);

    cube_bounds[i].DeepCopy(box_source->GetOutput()->GetBounds());
  }


  //
  // Test bounds of single VMEs
  //
  for (i=0;i<200;i++)
  {
    root->SetTreeTime(i);
    
    mafOBB local_bounds0,local_bounds1,local_bounds2;
    mafOBB bounds0,bounds1,bounds2;
    box0->GetOutput()->GetVMELocalBounds(local_bounds0);
    box1->GetOutput()->GetVMELocalBounds(local_bounds1);
    box2->GetOutput()->GetVMELocalBounds(local_bounds2);

    box0->GetOutput()->GetVMEBounds(bounds0);
    box1->GetOutput()->GetVMEBounds(bounds1);
    box2->GetOutput()->GetVMEBounds(bounds2);

    if (i<100)
    {
      // Local VME bounds
      for (int j=0;j<6;j++)
      {
        MAF_TEST(local_bounds0.m_Bounds[j]==cube_bounds[i].m_Bounds[j]);
        MAF_TEST(local_bounds1.m_Bounds[j]==cube_bounds[i].m_Bounds[j]);
        MAF_TEST(local_bounds2.m_Bounds[j]==cube_bounds[i].m_Bounds[j]);
      }
      
      if (i<50)
      {
        // VME bounds
        MAF_TEST(bounds0.m_Bounds[0]==cube_bounds[i].m_Bounds[0]);
        MAF_TEST(bounds0.m_Bounds[1]==cube_bounds[i].m_Bounds[1]);
        MAF_TEST(bounds0.m_Bounds[2]==cube_bounds[i].m_Bounds[2]);
        MAF_TEST(bounds0.m_Bounds[3]==cube_bounds[i].m_Bounds[3]);
        MAF_TEST(bounds0.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
        MAF_TEST(bounds0.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);

        MAF_TEST(bounds1.m_Bounds[0]==cube_bounds[i].m_Bounds[0]+i);
        MAF_TEST(bounds1.m_Bounds[1]==cube_bounds[i].m_Bounds[1]+i);
        MAF_TEST(bounds1.m_Bounds[2]==cube_bounds[i].m_Bounds[2]);
        MAF_TEST(bounds1.m_Bounds[3]==cube_bounds[i].m_Bounds[3]);
        MAF_TEST(bounds1.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
        MAF_TEST(bounds1.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);

        MAF_TEST(bounds2.m_Bounds[0]==cube_bounds[i].m_Bounds[0]-i);
        MAF_TEST(bounds2.m_Bounds[1]==cube_bounds[i].m_Bounds[1]-i);
        MAF_TEST(bounds2.m_Bounds[2]==cube_bounds[i].m_Bounds[2]);
        MAF_TEST(bounds2.m_Bounds[3]==cube_bounds[i].m_Bounds[3]);
        MAF_TEST(bounds2.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
        MAF_TEST(bounds2.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);

        // tree bounds
        mafOBB tree_bounds,test_tree_bounds;
        test_tree_bounds.m_Bounds[0]=cube_bounds[i].m_Bounds[0]-i;
        test_tree_bounds.m_Bounds[1]=cube_bounds[i].m_Bounds[1]+i;
        test_tree_bounds.m_Bounds[2]=cube_bounds[i].m_Bounds[2];
        test_tree_bounds.m_Bounds[3]=cube_bounds[i].m_Bounds[3];
        test_tree_bounds.m_Bounds[4]=cube_bounds[i].m_Bounds[4];
        test_tree_bounds.m_Bounds[5]=cube_bounds[i].m_Bounds[5];

        root->GetOutput()->GetBounds(tree_bounds);
        //MAF_TEST(tree_bounds.Equals(test_tree_bounds));
      }
      else
      {
        // VME bounds
        MAF_TEST(bounds0.m_Bounds[0]==cube_bounds[i].m_Bounds[0]);
        MAF_TEST(bounds0.m_Bounds[1]==cube_bounds[i].m_Bounds[1]);
        MAF_TEST(bounds0.m_Bounds[2]==cube_bounds[i].m_Bounds[2]+i-50);
        MAF_TEST(bounds0.m_Bounds[3]==cube_bounds[i].m_Bounds[3]+i-50);
        MAF_TEST(bounds0.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
        MAF_TEST(bounds0.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);

        MAF_TEST(bounds1.m_Bounds[0]==cube_bounds[i].m_Bounds[0]+50);
        MAF_TEST(bounds1.m_Bounds[1]==cube_bounds[i].m_Bounds[1]+50);
        MAF_TEST(bounds1.m_Bounds[2]==cube_bounds[i].m_Bounds[2]+i-50);
        MAF_TEST(bounds1.m_Bounds[3]==cube_bounds[i].m_Bounds[3]+i-50);
        MAF_TEST(bounds1.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
        MAF_TEST(bounds1.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);

        MAF_TEST(bounds2.m_Bounds[0]==cube_bounds[i].m_Bounds[0]-50);
        MAF_TEST(bounds2.m_Bounds[1]==cube_bounds[i].m_Bounds[1]-50);
        MAF_TEST(bounds2.m_Bounds[2]==cube_bounds[i].m_Bounds[2]+i-50);
        MAF_TEST(bounds2.m_Bounds[3]==cube_bounds[i].m_Bounds[3]+i-50);
        MAF_TEST(bounds2.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
        MAF_TEST(bounds2.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);


        // tree bounds
        mafOBB tree_bounds,test_tree_bounds;
        test_tree_bounds.m_Bounds[0]=cube_bounds[i].m_Bounds[0]-50;
        test_tree_bounds.m_Bounds[1]=cube_bounds[i].m_Bounds[1]+50;
        test_tree_bounds.m_Bounds[2]=cube_bounds[i].m_Bounds[2]+i-50;
        test_tree_bounds.m_Bounds[3]=cube_bounds[i].m_Bounds[3]+i-50;
        test_tree_bounds.m_Bounds[4]=cube_bounds[i].m_Bounds[4];
        test_tree_bounds.m_Bounds[5]=cube_bounds[i].m_Bounds[5];

        root->GetOutput()->GetBounds(tree_bounds);
        MAF_TEST(tree_bounds.Equals(test_tree_bounds));
      }
    }
    else
    {
      // Local VME bounds
      for (int j=0;j<6;j++)
      {
        MAF_TEST(local_bounds0.m_Bounds[j]==cube_bounds[i].m_Bounds[j]);
        MAF_TEST(local_bounds1.m_Bounds[j]==cube_bounds[i].m_Bounds[j]);
        MAF_TEST(local_bounds2.m_Bounds[j]==cube_bounds[i].m_Bounds[j]);
      }

      // VME bounds
      MAF_TEST(bounds0.m_Bounds[0]==cube_bounds[i].m_Bounds[0]);
      MAF_TEST(bounds0.m_Bounds[1]==cube_bounds[i].m_Bounds[1]);
      MAF_TEST(bounds0.m_Bounds[2]==cube_bounds[i].m_Bounds[2]+100-50);
      MAF_TEST(bounds0.m_Bounds[3]==cube_bounds[i].m_Bounds[3]+100-50);
      MAF_TEST(bounds0.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
      MAF_TEST(bounds0.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);

      MAF_TEST(bounds1.m_Bounds[0]==cube_bounds[i].m_Bounds[0]+50);
      MAF_TEST(bounds1.m_Bounds[1]==cube_bounds[i].m_Bounds[1]+50);
      MAF_TEST(bounds1.m_Bounds[2]==cube_bounds[i].m_Bounds[2]+100-50);
      MAF_TEST(bounds1.m_Bounds[3]==cube_bounds[i].m_Bounds[3]+100-50);
      MAF_TEST(bounds1.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
      MAF_TEST(bounds1.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);

      MAF_TEST(bounds2.m_Bounds[0]==cube_bounds[i].m_Bounds[0]-50);
      MAF_TEST(bounds2.m_Bounds[1]==cube_bounds[i].m_Bounds[1]-50);
      MAF_TEST(bounds2.m_Bounds[2]==cube_bounds[i].m_Bounds[2]+100-50);
      MAF_TEST(bounds2.m_Bounds[3]==cube_bounds[i].m_Bounds[3]+100-50);
      MAF_TEST(bounds2.m_Bounds[4]==cube_bounds[i].m_Bounds[4]);
      MAF_TEST(bounds2.m_Bounds[5]==cube_bounds[i].m_Bounds[5]);
    } 
  }
  

/*  
  //---------------------------------------------------------------------------------------
  //        A visual feedback on the animation
  //---------------------------------------------------------------------------------------

  // here it should be good to add a regression test

  cerr << "Testing VME time dynamic behavior\n";

  // Test display of generic VME tree of surfaces
  mafNodeIterator *iter=root->NewIterator();

  root->SetTreeTime(0);
  //renderer->GetActiveCamera()->Zoom(.5);
  renderer->Render();
  renderer->ResetCamera();
  renderer->GetActiveCamera()->SetViewAngle(0);

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
      else
      {
        vtkDataSet *data=vme->GetOutput()->GetVTKData();
        MAF_TEST(data!=NULL);

        MAF_TEST(data->IsA("vtkPolyData")!=0);

        vtkDataSetMapper *mapper=vtkDataSetMapper::New();

        mapper->SetInput((vtkPolyData *)data);
  
        vtkMAFSmartPointer<vtkActor> vmeact;
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

  renderer->GetActiveCamera()->Zoom(.5);
  renderer->Render();
  renderer->ResetCamera();
  
  
  //renderer->GetActiveCamera()->SetViewAngle(0);



  int t;
  for (t = 0; t < 100; t++) 
  {   
    // Update time into all the tree
    root->SetTreeTime(t);
  
    renderer->ResetCameraClippingRange();
    //renderer->ResetCamera();
    renWin->Render();
    //SLEEP(100);
  }

 
/*  
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

  vtkDataSetMapper *treeBoundsMapper=vtkDataSetMapper::New();
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

  vtkDataSetMapper *tree4DBoundsMapper=vtkDataSetMapper::New();
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

  vtkDataSetMapper *asmBoundsMapper=vtkDataSetMapper::New();
  asmBoundsMapper->SetInput(asmBoundsBox->GetOutput());
  asmBoundsBox->Delete();

  vtkActor *asmBoundsActor=vtkActor::New();
  asmBoundsActor->SetMapper(asmBoundsMapper);
  asmBoundsMapper->Delete();

  asmBoundsActor->GetProperty()->SetColor(0,1,1);


  rootAsm->GetBounds(asmBounds);

  MAF_TEST(treeBounds.Equals(asmBounds)!=0);

  renderer->AddActor(tree4DBoundsActor);

  //SLEEP(500);

  //renderer->ResetCamera();
  renWin->Render();

  renderer->AddActor(treeBoundsActor);
  renderer->AddActor(asmBoundsActor);

  tree4DBoundsActor->Delete();
  treeBoundsActor->Delete();
  asmBoundsActor->Delete();
  
  //renderer->GetActiveCamera()->SetViewAngle(30);

  for (t = 0; t < 100; t++) 
  {
    // Update time into all the tree
    root->SetTreeTime(t);

    renderer->ResetCameraClippingRange();
    //renderer->ResetCamera();
    //renderer->GetActiveCamera()->Yaw(.003);
    renWin->Render();

    SLEEP(300);

    root->GetOutput()->GetBounds(treeBounds);
    vtitle->GetOutput()->GetBounds(treeBounds);

    rootAsm->GetBounds(asmBounds);

    treeBoundsBox->SetBounds(treeBounds.m_Bounds[0],treeBounds.m_Bounds[1],treeBounds.m_Bounds[2], \
    treeBounds.m_Bounds[3],treeBounds.m_Bounds[4],treeBounds.m_Bounds[5]);

    asmBoundsBox->SetBounds(asmBounds);    

    //MAF_TEST(treeBounds.Equals(asmBounds));
  
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

  mafVMERoot *newroot=mafVMERoot::SafeDownCast(root->CopyTree());

  */
  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
