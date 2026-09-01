/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEStorageTest
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaCoreTests.h"
#include "albaVMEStorageTest.h"

#include "albaVMEFactory.h"
#include "albaCoreFactory.h"
#include "albaVMEIterator.h"
#include "albaVMESurface.h"
#include "albaVMEPolyline.h"
#include "albaTransform.h"
#include "albaVMEItem.h"
#include "albaDataVector.h"
#include "albaDirectory.h"

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
#include "vtkALBASmartPointer.h"

#include <wx/dir.h>

#include <iostream>
#include <set>
#include "vtkPolyDataAlgorithm.h"

const char *testMSFDir = "test_MSF";
const char *testMSFCopyDir = "testMSF_copy";
const char *testMSFSaveAgainDir = "testMSF_saveAgain";
const char *testMSFSaveAsDir = "testMSF_saveAs";

/** attribute class for attaching vtkActor to VME */
//-------------------------------------------------------------------------
class albaClientData : public albaAttribute
//-------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaClientData,albaAttribute);

  vtkALBAAutoPointer<vtkAssembly> m_Prop3D;

  virtual void DeepCopy(const albaAttribute *a) {Superclass::DeepCopy(a); m_Prop3D=((albaClientData *)a)->m_Prop3D;}
  virtual bool Equals(const albaAttribute *a) const {return Superclass::Equals(a)&&m_Prop3D==((albaClientData *)a)->m_Prop3D;}
};

//-------------------------------------------------------------------------
int play_tree(albaVMERoot *m_StorageRoot)
//-------------------------------------------------------------------------
{
  //
  // Test display of generic VME tree of surfaces
  //
  vtkALBASmartPointer<vtkRenderWindow> renWin;
  vtkALBASmartPointer<vtkRenderer> renderer;
  vtkALBASmartPointer<vtkRenderWindowInteractor> iren;

  renWin->AddRenderer(renderer);
  iren->SetRenderWindow(renWin);

  renderer->SetBackground(0.1, 0.1, 0.1);
  renWin->SetSize(640, 480);
  renWin->SetPosition(400,0);
  renWin->StereoCapableWindowOn();

  albaVMEIterator *iter=m_StorageRoot->NewIterator();

  // connect VME to assemblies and put m_StorageRoot assembly into the renderer
  for (albaVME *node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    if (node->IsALBAType(albaVMERoot))
    {
      vtkALBASmartPointer<vtkAssembly> vmeasm;
      albaClientData *clientdata=albaClientData::SafeDownCast(node->GetAttribute("ClientData"));
      if (!clientdata)
      {
        clientdata=albaClientData::New(); // note ref count already set to 0
        clientdata->SetName("ClientData");
 
        node->SetAttribute("ClientData",clientdata);
      }

      CPPUNIT_ASSERT(clientdata!=NULL);
      clientdata->m_Prop3D=vmeasm;

      renderer->AddActor(vmeasm);
    }
    else
    {
      vtkDataSet *vmedata=node->GetOutput()->GetVTKData();
      vtkALBASmartPointer<vtkDataSetMapper> mapper;
        mapper->SetInputData((vtkPolyData *)vmedata);

      vtkALBASmartPointer<vtkActor> vmeact;
      vmeact->SetMapper(mapper);

      vtkALBASmartPointer<vtkAssembly> vmeasm;
      vmeasm->AddPart(vmeact);
      vmeasm->SetUserTransform(node->GetOutput()->GetTransform()->GetVTKTransform());
      albaClientData *clientdata=albaClientData::SafeDownCast(node->GetAttribute("ClientData"));
      if (!clientdata)
      {
        clientdata=albaClientData::New(); // note ref count initially set to 0
        clientdata->SetName("ClientData");
        node->SetAttribute("ClientData",clientdata);
      }

      CPPUNIT_ASSERT(clientdata!=NULL);
      clientdata->m_Prop3D=vmeasm;

      albaClientData *pclientdata=albaClientData::SafeDownCast(node->GetParent()->GetAttribute("ClientData"));
      CPPUNIT_ASSERT(pclientdata);
      vtkAssembly *pvmeasm=(vtkAssembly *)pclientdata->m_Prop3D;
      pvmeasm->AddPart(vmeasm);
    }
  }

  albaTimeStamp treeTimeBounds[2];
  m_StorageRoot->GetOutput()->GetTimeBounds(treeTimeBounds);

  CPPUNIT_ASSERT(treeTimeBounds[0] == 0);
  CPPUNIT_ASSERT(treeTimeBounds[1] == 200);

  double spacing = (treeTimeBounds[1]-treeTimeBounds[0])/100;

  int step = 1;
  for (int t = 0; t < 100; t+=step) 
  {
    if (t==100)
    {
      step = -1;
      continue;
    }

    // Update time into all the tree
    m_StorageRoot->SetTreeTime(t*spacing);

    renderer->ResetCameraClippingRange();
    renWin->Render();
  }

  iter->Delete();

  return ALBA_OK;
}

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaClientData);
//-------------------------------------------------------------------------


//----------------------------------------------------------------------------
void albaVMEStorageTest::TestFixture()
//----------------------------------------------------------------------------
{
  // display the tree animation we are going to save
  CPPUNIT_ASSERT(play_tree(m_StorageRoot)==ALBA_OK);
}

//----------------------------------------------------------------------------
void albaVMEStorageTest::BeforeTest()
//----------------------------------------------------------------------------
{
  albaVMEItem::GlobalCompareDataFlagOn();

	m_Storage=new albaVMEStorage();
  m_StorageRoot = NULL;
  m_RootVme = NULL;
  
  // initialized the VME factory
  albaVMEFactory::Initialize();

  // create test vme tree by filing m_StorageRoot children  
  CreateVMETestTree();

  // create folder for test file
  wxMkdir(testMSFDir);

  // create folder for a new file to be saved with a different name
  wxMkdir(testMSFCopyDir);

  // create folder for a new file to be saved with a different name
  wxMkdir(testMSFSaveAsDir);

  // also create a new file saving the loaded tree to a new file name
  wxMkdir(testMSFSaveAgainDir);
}

//----------------------------------------------------------------------------
void albaVMEStorageTest::AfterTest()
//----------------------------------------------------------------------------
{
  // cleanup created directories   
  bool removeSuccessful = true;

  removeSuccessful = RemoveDir(testMSFDir);
  CPPUNIT_ASSERT(removeSuccessful);

  removeSuccessful = RemoveDir(testMSFCopyDir);
  CPPUNIT_ASSERT(removeSuccessful);

  removeSuccessful = RemoveDir(testMSFSaveAgainDir);
  CPPUNIT_ASSERT(removeSuccessful);

  removeSuccessful = RemoveDir(testMSFSaveAsDir);
  CPPUNIT_ASSERT(removeSuccessful);

  std::cerr << "Test Completed Successfully!\n";

  albaDEL(m_RootVme);
	albaDEL(m_Storage);
}

//----------------------------------------------------------------------------
void albaVMEStorageTest::TestSaveAs()
//----------------------------------------------------------------------------
{
  // save the tree to test to disk to default location
  CPPUNIT_ASSERT(m_Storage->Store()==ALBA_OK);

  // save the tree to another location with a different name 
  m_Storage->SetURL("testMSF_saveAs/testMSF_saveAs.msf");
  CPPUNIT_ASSERT(m_Storage->Store()==ALBA_OK);

  // test if the file has been saved and reloaded correctly
  
  // create the "Save as" storage to reload the saved as tree   
  albaVMEStorage load_saveas_storage;
  load_saveas_storage.SetURL("testMSF_saveAs/testMSF_saveAs.msf");
  
  albaVMERoot *loaded_saveas_root=load_saveas_storage.GetRoot();
  
  // reload the saved as tree     
  CPPUNIT_ASSERT(load_saveas_storage.Restore()==ALBA_OK);
  CPPUNIT_ASSERT(load_saveas_storage.GetErrorCode()==albaStorage::IO_OK);

  // compare the original tree with the save as one
  CPPUNIT_ASSERT(m_StorageRoot->CompareTree(loaded_saveas_root));

  // save again with a different name using the second storage
  load_saveas_storage.SetURL("testMSF_saveAgain/testMSF_saveAgain.msf");

  // store to disk
  CPPUNIT_ASSERT(load_saveas_storage.Store()==ALBA_OK);

  // reload the save again MSF file
  albaVMEStorage load_saveAgain_storage;
  load_saveAgain_storage.SetURL("testMSF_saveAgain/testMSF_saveAgain.msf");

  albaVMERoot *loaded_saveAgain_root=load_saveAgain_storage.GetRoot();
  CPPUNIT_ASSERT(load_saveAgain_storage.Restore()==ALBA_OK);
  CPPUNIT_ASSERT(load_saveAgain_storage.GetErrorCode()==albaStorage::IO_OK);

  int num = loaded_saveAgain_root->GetFirstChild()->GetNumberOfChildren();

  // compare the save again tree with the original one
  bool treeEquals = m_StorageRoot->CompareTree(loaded_saveAgain_root);
  CPPUNIT_ASSERT(treeEquals);
}

//----------------------------------------------------------------------------
void albaVMEStorageTest::CreateVMETestTree()
//----------------------------------------------------------------------------
{
  // plug the custom attribute in the Node Factory
  albaPlugAttribute<albaClientData>("Simple attribute for attaching actors to VMEs");

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);

  vtkALBASmartPointer<vtkTextSource> text;

  text->SetText("VME Tree Test");
  text->Update();

  // create a Storage, extract the m_StorageRoot, create a tree
  // and save it to disk
  m_Storage->SetURL("test_MSF/test_MSF.msf");
  m_StorageRoot=m_Storage->GetRoot();

  // this will be destroyed when the tree is destroyed: no need to call delete... 
  m_RootVme = albaVMEGeneric::New();

  albaSmartPointer<albaVMESurface> vsphere;
  albaSmartPointer<albaVMESurface> vcone;
  albaSmartPointer<albaVMESurface> vmorph;

  m_RootVme->SetName("title");
  vsphere->SetName("sphere");
  vcone->SetName("cone");
  vmorph->SetName("morph");

  albaTransform trans;
  m_RootVme->SetData(axes->GetOutput(),0);
  
  m_StorageRoot->AddChild(m_RootVme);

  m_RootVme->AddChild(vsphere);
  m_RootVme->AddChild(vcone);
  m_RootVme->AddChild(vmorph);

  vtkALBASmartPointer<vtkSphereSource> sphere;
  vtkALBASmartPointer<vtkConeSource> cone;

  int i;
  for (i=0;i<100;i++)
  {
    trans.Identity();
    trans.RotateY(i*1.6-180.0,POST_MULTIPLY);

    m_RootVme->SetPose(trans.GetMatrix(),i);

    // The sphere
    sphere->SetRadius(.1+.01*i);
		sphere->Update();
    vsphere->SetData(sphere->GetOutput(),i*.5+50);

    trans.Identity();
    trans.Translate(0,.02*i,0,POST_MULTIPLY);
    vsphere->SetPose(trans.GetMatrix(),i*.5+25);

    // the cone
    cone->SetResolution(103-i);
		cone->Update();
    vcone->SetData(cone->GetOutput(),200-i*2);
    trans.Identity();
    trans.Translate(2-(double)i/50.0,0,0,POST_MULTIPLY);

    vcone->SetPose(trans.GetMatrix(),i*.5+75);

    vtkPolyDataAlgorithm *morph;

    // the morphing tube
    if (i<50)
    {

      vtkCylinderSource *cyl = vtkCylinderSource::New();
      cyl->SetResolution(52-i);
      morph=cyl;
    }
    else
    {
      vtkCubeSource *cube = vtkCubeSource::New();
      cube->SetYLength((100.0-(double)i)/100.0+.5);
      cube->SetXLength(1);
      cube->SetZLength(1);
      morph=cube;
    }

		morph->Update();
    vmorph->SetData(morph->GetOutput(),log10((double)(100-i))*50);
    morph->Delete();
  }

  trans.Identity();
  trans.RotateZ(90,POST_MULTIPLY);
  trans.Translate(-.75,0,0,POST_MULTIPLY);
  vmorph->SetPose(trans.GetMatrix(),0); // set pose at time 0
}

//----------------------------------------------------------------------------
void albaVMEStorageTest::TestSave()
//----------------------------------------------------------------------------
{
  // save the tree to disk
  int res = m_Storage->Store();
  CPPUNIT_ASSERT(res == ALBA_OK);
}

//----------------------------------------------------------------------------
void albaVMEStorageTest::TestSaveAndReload()
//----------------------------------------------------------------------------
{
  // save the tree to test to disk
  CPPUNIT_ASSERT(m_Storage->Store()==ALBA_OK);

  //
  // Test data reloading:
  // THIS PART HAS LEAKS! 
  // Something in the restore part seems leaked... 
  albaVMEStorage load_storage;
  load_storage.SetURL("test_MSF/test_MSF.msf");

  albaVMERoot *loaded_root = load_storage.GetRoot();
  load_storage.Restore();

  // test if data is loaded on demand
  // NOT YET IMPLEMENTED

  // test if data is automatically released
  // NOT YET IMPLEMENTED

  // test if tree has been saved and reloaded correctly
  bool treeEquals = m_StorageRoot->CompareTree(loaded_root);
  CPPUNIT_ASSERT(treeEquals);

  // display tree animation again (for debug purposes)
  CPPUNIT_ASSERT(play_tree(loaded_root)==ALBA_OK);

}
//----------------------------------------------------------------------------
void albaVMEStorageTest::TestTreeEditAndGarbageCollection()
{
  // save the tree to test to disk
  CPPUNIT_ASSERT(m_Storage->Store()==ALBA_OK);

  //
  // Test data reloading:
  albaVMEStorage load_storage;
  load_storage.SetURL("test_MSF/test_MSF.msf");

  albaVMERoot *loaded_root=load_storage.GetRoot();
  load_storage.Restore();

  //
  // Testing editing of the tree
  //
  vtkALBASmartPointer<vtkConeSource> cone;
  cone->SetRadius(2.5);
	cone->Update();

  std::vector<albaString> cone_items_fname;
  cone_items_fname.resize(100);

  CPPUNIT_ASSERT(!strcmp(m_RootVme->GetChild(1)->GetName(),"cone"));

  albaSmartPointer<albaVMESurface> vcone = albaVMESurface::SafeDownCast(m_RootVme->GetChild(1));
	vcone->Update();
	vcone->GetOutput()->Update();
  // modify some data of existing VMEs...
  int i=0;
  for (;i<100;i++)
  {

    cone_items_fname[i]=albaVMESurface::SafeDownCast(m_RootVme->GetChild(1))->GetDataVector()->GetItemByIndex(i)->GetURL();
    // change the cone radius
    cone->SetResolution(103-i);
    vcone->SetData(cone->GetOutput(),200-i*2);
  }

  {
    // also add a new sub-node of title
    vtkALBASmartPointer<vtkCubeSource> new_cube;
    albaSmartPointer<albaVMESurface> new_cube_vme;
		new_cube->Update();
    new_cube_vme->SetData(new_cube->GetOutput(),0);

    m_RootVme->AddChild(new_cube_vme);
  } // make local objects reference to be removed

  // save changed data
  m_Storage->Store();

  //
  // check if data files garbage collection works
  //
  albaDirectory dir;
  std::set<albaString> file_list;
  dir.Load(testMSFDir);
  for (i=0;i<dir.GetNumberOfFiles();i++)
  {
    file_list.insert(dir.GetFile(i));
  }

  for (i=0;i<100;i++)
  {
    // test if data files has been changed
    albaString new_fname=vcone->GetDataVector()->GetItemByIndex(i)->GetURL();
    CPPUNIT_ASSERT(new_fname!=cone_items_fname[i]);

    // test removal of old files
    CPPUNIT_ASSERT(file_list.find(cone_items_fname[i])==file_list.end());
    // commented line below because now the single file mode manage
    // by itself the compression, so old test is not valid anymore
    //CPPUNIT_ASSERT(file_list.find(new_fname)!=file_list.end());
  }

  // now reload the MSF file in a new tree
  albaVMEStorage reload_storage;
  reload_storage.SetURL("test_MSF/test_MSF.msf");
  albaVMERoot *reloaded_root=reload_storage.GetRoot();
  reload_storage.Restore();

  albaTimeStamp tb[2],rtb[2];
  m_StorageRoot->GetOutput()->GetTimeBounds(tb);
  reloaded_root->GetOutput()->GetTimeBounds(rtb);
  CPPUNIT_ASSERT(albaEquals(tb[0],rtb[0])&&albaEquals(tb[1],rtb[1]));

  // test if the new tree is equal to the old one
  CPPUNIT_ASSERT(m_StorageRoot->CompareTree(reloaded_root));

  // notice that the loaded_root tree is not aligned with the disk 
  // image, since it was loaded before editing the orginal tree
  // and saving it. Since the tree data has already been loaded into
  // memory ALBA do not notice anything. BEWARE to never do this in real
  // code: two storages pointing to the same MSF file can be very dangerous!!!
  CPPUNIT_ASSERT(!m_StorageRoot->CompareTree(loaded_root));

  // display tree animationa again (for debug purposes)
  CPPUNIT_ASSERT(play_tree(reloaded_root)==ALBA_OK);  
}
