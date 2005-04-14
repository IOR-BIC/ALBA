/** Test for VME related classes.  */

// - Improve Test for Copy of VME
// - Implement Test for SaveAs
// - Implement Test for Load


#include "mafDefines.h"

#include "mafVMEFactory.h"
#include "mafNodeIterator.h"
#include "mafMSFStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafTransform.h"

#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"

#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"
#include "vtkTextSource.h"
#include "vtkCylinderSource.h"
#include "vtkAxes.h"
#include "vtkFileOutputWindow.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkMAFSmartPointer.h"

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

int play_tree(mafVMERoot *root)
{
  //
  // Test display of generic VME tree of surfaces
  //
  vtkMAFSmartPointer<vtkRenderWindow> renWin;
  vtkMAFSmartPointer<vtkRenderer> renderer;
  vtkMAFSmartPointer<vtkRenderWindowInteractor> iren;

  renWin->AddRenderer(renderer);
  iren->SetRenderWindow(renWin);

  renderer->SetBackground(0.1, 0.1, 0.1);
  renWin->SetSize(640, 480);
  renWin->SetPosition(400,0);
  renWin->StereoCapableWindowOn();

  mafNodeIterator *iter=root->NewIterator();

  // connect VME to assemblies and put root assembly into the renderer
  for (mafNode *node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    if (mafVME *vme=mafVME::SafeDownCast(node))
    {
      if (vme->IsMAFType(mafVMERoot))
      {
        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        mafClientData *clientdata=mafClientData::SafeDownCast(vme->GetAttribute("ClientData"));
        if (!clientdata)
        {
          clientdata=mafClientData::New(); // note ref count already set to 0
          clientdata->SetName("ClientData");
          vme->SetAttribute("ClientData",clientdata);
        }

        MAF_TEST(clientdata!=NULL);
        clientdata->m_Prop3D=vmeasm;

        renderer->AddActor(vmeasm);
      }
      else
      {
        vtkDataSet *vmedata=vme->GetOutput()->GetVTKData();
        vtkMAFSmartPointer<vtkDataSetMapper> mapper;
        mapper->SetInput((vtkPolyData *)vmedata);
  
        vtkMAFSmartPointer<vtkActor> vmeact;
        vmeact->SetMapper(mapper);

        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        vmeasm->AddPart(vmeact);
        vmeasm->SetUserTransform(vme->GetOutput()->GetTransform()->GetVTKTransform());
        
        mafClientData *clientdata=mafClientData::SafeDownCast(vme->GetAttribute("ClientData"));
        if (!clientdata)
        {
          clientdata=mafClientData::New(); // note ref count initially set to 0
          clientdata->SetName("ClientData");
          vme->SetAttribute("ClientData",clientdata);
        }

        MAF_TEST(clientdata!=NULL);
        clientdata->m_Prop3D=vmeasm;

        mafClientData *pclientdata=mafClientData::SafeDownCast(vme->GetParent()->GetAttribute("ClientData"));
        MAF_TEST(pclientdata);
        vtkAssembly *pvmeasm=(vtkAssembly *)pclientdata->m_Prop3D;
        pvmeasm->AddPart(vmeasm);
      }
    }    
  }

  mafTimeStamp treeTimeBounds[2];
  root->GetOutput()->GetTimeBounds(treeTimeBounds);

  MAF_TEST(treeTimeBounds[0]==0);
  MAF_TEST(treeTimeBounds[1]==200);

  float spacing=(treeTimeBounds[1]-treeTimeBounds[0])/100;

  int step=1;
  for (int t = 0; t < 100; t+=step) 
  {
    if (t==100)
    {
      step=-1;
      SLEEP(500);
      continue;
    }
    
    // Update time into all the tree
    root->SetTreeTime(t*spacing);

    renderer->ResetCameraClippingRange();
    renWin->Render();
    SLEEP(10);
  }

  SLEEP(1000);

  iter->Delete();

  return MAF_OK;
}
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafClientData);
//-------------------------------------------------------------------------


int main( int argc, char *argv[] )
{
  vtkFileOutputWindow *log;
  log=vtkFileOutputWindow::New();
  log->SetInstance(log);
  log->SetFileName("testMSF.log"); // log of VTK error messages

  // initialized the VME factory
  mafVMEFactory::Initialize();

  vtkMAFSmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);

  vtkMAFSmartPointer<vtkTextSource> text;
  
  text->SetText("VME Tree Test");
  text->Update();

  mafMSFStorage storage;
  storage.SetURL("testMSF.msf");
  mafVMERoot *root=storage.GetRoot();

  mafSmartPointer<mafVMEGeneric> vtitle;
  mafSmartPointer<mafVMESurface> vsphere;
  mafSmartPointer<mafVMESurface> vcone;
  mafSmartPointer<mafVMESurface> vmorph;

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

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  vtkMAFSmartPointer<vtkConeSource> cone;

  int i;
  for (i=0;i<100;i++)
  {
    trans.Identity();
    trans.RotateY(i*1.6-180.0,POST_MULTIPLY);

    vtitle->SetPose(trans.GetMatrix(),i);

    // The sphere
    sphere->SetRadius(.1+.01*i);
    vsphere->SetData(sphere->GetOutput(),i*.5+50);

    trans.Identity();
    trans.Translate(0,.02*i,0,POST_MULTIPLY);
    vsphere->SetPose(trans.GetMatrix(),i*.5+25);

    // the cone
    cone->SetResolution(103-i);

    vcone->SetData(cone->GetOutput(),200-i*2);

    trans.Identity();
    trans.Translate(2-(double)i/50.0,0,0,POST_MULTIPLY);
    
    vcone->SetPose(trans.GetMatrix(),i*.5+75);
    
    vtkPolyDataSource *morph;

    // the morphing tube
    if (i<50)
    {
      vtkCylinderSource *cyl=vtkCylinderSource::New();
   
      cyl->SetResolution(52-i);
      morph=cyl;
    }
    else
    {
      vtkCubeSource *cube=vtkCubeSource::New();
   
      cube->SetYLength((100.0-(double)i)/100.0+.5);
      cube->SetXLength(1);
      cube->SetZLength(1);
      morph=cube;
    }

    vmorph->SetData(morph->GetOutput(),log10(100-i)*50);
  }

  trans.Identity();
  trans.RotateZ(90,POST_MULTIPLY);
  trans.Translate(-.75,0,0,POST_MULTIPLY);
  vmorph->SetPose(trans.GetMatrix(),0); // set pose at time 0

  MAF_TEST(play_tree(root)==MAF_OK);

  MAF_TEST(storage.Store()==MAF_OK);

  // store the filenames for VME items
  //...

  //
  // Test data reloading
  //
  mafMSFStorage load_storage;
  load_storage.SetURL("testMSF.msf");
  mafVMERoot *loaded_root=load_storage.GetRoot();
  load_storage.Restore();

  // test if data is loaded on demand

  // test if data is automatically released

  MAF_TEST(play_tree(loaded_root)==MAF_OK);
  MAF_TEST(root->CompareTree(loaded_root));

  //
  // Testing editing of the tree
  //
  cone->SetRadius(2.5);
 
  for (i=0;i<100;i++)
  {
    // change the cone radius
    cone->SetResolution(103-i);
  }


  // save changed data
  storage.Store();
   
  // test removal of old files
  //...
  
  mafMSFStorage reload_storage;
  reload_storage.SetURL("testMSF.msf");
  mafVMERoot *reloaded_root=reload_storage.GetRoot();
  reload_storage.Restore();
  
  MAF_TEST(root->CompareTree(reloaded_root));

  MAF_TEST(!root->CompareTree(loaded_root));

  // Test saving with different name
  storage.SetURL("testMSF_saveAs.msf");
  storage.Store();


  mafMSFStorage load_saveas_storage;
  load_saveas_storage.SetURL("testMSF_saveAs.msf");
  mafVMERoot *loaded_saveas_root=load_saveas_storage.GetRoot();
  load_saveas_storage.Restore();
  
  MAF_TEST(root->CompareTree(loaded_saveas_root));

  return 0;
}

