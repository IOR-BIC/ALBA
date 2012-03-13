/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEStorageTest.cpp,v $
Language:  C++
Date:      $Date: 2008-07-03 11:32:34 $
Version:   $Revision: 1.2 $
Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEStorageTest.h"

#include "mafVMEFactory.h"
#include "mafCoreFactory.h"
#include "mafNodeIterator.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafTransform.h"
#include "mafVMEItem.h"
#include "mafDataVector.h"
#include "mafDirectory.h"

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

#include <wx/dir.h>

#include <iostream>
#include <set>

const char *testMSFDir = "test_MSF";
const char *testMSFCopyDir = "testMSF_copy";
const char *testMSFSaveAgainDir = "testMSF_saveAgain";
const char *testMSFSaveAsDir = "testMSF_saveAs";

/** attribute class for attaching vtkActor to VME */
//-------------------------------------------------------------------------
class mafClientData : public mafAttribute
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafClientData,mafAttribute);

  vtkMAFAutoPointer<vtkAssembly> m_Prop3D;

  virtual void DeepCopy(const mafAttribute *a) {Superclass::DeepCopy(a); m_Prop3D=((mafClientData *)a)->m_Prop3D;}
  virtual bool Equals(const mafAttribute *a) const {return Superclass::Equals(a)&&m_Prop3D==((mafClientData *)a)->m_Prop3D;}
};

//-------------------------------------------------------------------------
int play_tree(mafVMERoot *m_StorageRoot)
//-------------------------------------------------------------------------
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

  mafNodeIterator *iter=m_StorageRoot->NewIterator();

  // connect VME to assemblies and put m_StorageRoot assembly into the renderer
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

        CPPUNIT_ASSERT(clientdata!=NULL);
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

        CPPUNIT_ASSERT(clientdata!=NULL);
        clientdata->m_Prop3D=vmeasm;

        mafClientData *pclientdata=mafClientData::SafeDownCast(vme->GetParent()->GetAttribute("ClientData"));
        CPPUNIT_ASSERT(pclientdata);
        vtkAssembly *pvmeasm=(vtkAssembly *)pclientdata->m_Prop3D;
        pvmeasm->AddPart(vmeasm);
      }
    }    
  }

  mafTimeStamp treeTimeBounds[2];
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
      mafSleep(500);
      continue;
    }

    // Update time into all the tree
    m_StorageRoot->SetTreeTime(t*spacing);

    renderer->ResetCameraClippingRange();
    renWin->Render();
    mafSleep(10);
  }

  mafSleep(1000);

  iter->Delete();

  return MAF_OK;
}

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafClientData);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool RemoveDir(const char *dirName)
//-------------------------------------------------------------------------
{
  wxArrayString filenameArray;
  wxDir::GetAllFiles(dirName,&filenameArray);
  for (int index = 0; index < filenameArray.size(); index++)
  {
    wxRemoveFile(filenameArray.Item(index));
  }

  bool result = TRUE;

  result = wxRmdir(dirName);
  if (result == false)
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
void mafVMEStorageTest::TestFixture()
//----------------------------------------------------------------------------
{
  // display the tree animation we are going to save
  CPPUNIT_ASSERT(play_tree(m_StorageRoot)==MAF_OK);
}

//----------------------------------------------------------------------------
void mafVMEStorageTest::setUp()
//----------------------------------------------------------------------------
{
  mafVMEItem::GlobalCompareDataFlagOn();

  m_StorageRoot = NULL;
  m_RootVme = NULL;
  
  // initialized the VME factory
  mafVMEFactory::Initialize();

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
void mafVMEStorageTest::tearDown()
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

  mafDEL(m_RootVme);
}

//----------------------------------------------------------------------------
void mafVMEStorageTest::TestSaveAs()
//----------------------------------------------------------------------------
{
  // save the tree to test to disk to default location
  CPPUNIT_ASSERT(m_Storage.Store()==MAF_OK);

  // save the tree to another location with a different name 
  m_Storage.SetURL("testMSF_saveAs/testMSF_saveAs.msf");
  CPPUNIT_ASSERT(m_Storage.Store()==MAF_OK);

  // test if the file has been saved and reloaded correctly
  
  // create the "Save as" storage to reload the saved as tree   
  mafVMEStorage load_saveas_storage;
  load_saveas_storage.SetURL("testMSF_saveAs/testMSF_saveAs.msf");
  
  mafVMERoot *loaded_saveas_root=load_saveas_storage.GetRoot();
  
  // reload the saved as tree     
  CPPUNIT_ASSERT(load_saveas_storage.Restore()==MAF_OK);
  CPPUNIT_ASSERT(load_saveas_storage.GetErrorCode()==mafStorage::IO_OK);

  // compare the original tree with the save as one
  CPPUNIT_ASSERT(m_StorageRoot->CompareTree(loaded_saveas_root));

  // save again with a different name using the second storage
  load_saveas_storage.SetURL("testMSF_saveAgain/testMSF_saveAgain.msf");

  // store to disk
  CPPUNIT_ASSERT(load_saveas_storage.Store()==MAF_OK);

  // reload the save again MSF file
  mafVMEStorage load_saveAgain_storage;
  load_saveAgain_storage.SetURL("testMSF_saveAgain/testMSF_saveAgain.msf");

  mafVMERoot *loaded_saveAgain_root=load_saveAgain_storage.GetRoot();
  CPPUNIT_ASSERT(load_saveAgain_storage.Restore()==MAF_OK);
  CPPUNIT_ASSERT(load_saveAgain_storage.GetErrorCode()==mafStorage::IO_OK);

  int num = loaded_saveAgain_root->GetFirstChild()->GetNumberOfChildren();

  // compare the save again tree with the original one
  bool treeEquals = m_StorageRoot->CompareTree(loaded_saveAgain_root);
  CPPUNIT_ASSERT(treeEquals);
}

//----------------------------------------------------------------------------
void mafVMEStorageTest::CreateVMETestTree()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkFileOutputWindow> log;
  log->SetInstance(log);
  log->SetFileName("testMSF.log"); // log of VTK error messages

  // plug the custom attribute in the Node Factory
  mafPlugAttribute<mafClientData>("Simple attribute for attaching actors to VMEs");

  vtkMAFSmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);

  vtkMAFSmartPointer<vtkTextSource> text;

  text->SetText("VME Tree Test");
  text->Update();

  // create a Storage, extract the m_StorageRoot, create a tree
  // and save it to disk
  m_Storage.SetURL("test_MSF/test_MSF.msf");
  m_StorageRoot=m_Storage.GetRoot();

  // this will be destroyed when the tree is destroyed: no need to call delete... 
  m_RootVme = mafVMEGeneric::New();

  mafSmartPointer<mafVMESurface> vsphere;
  mafSmartPointer<mafVMESurface> vcone;
  mafSmartPointer<mafVMESurface> vmorph;

  m_RootVme->SetName("title");
  vsphere->SetName("sphere");
  vcone->SetName("cone");
  vmorph->SetName("morph");

  mafTransform trans;
  m_RootVme->SetData(axes->GetOutput(),0);
  
  m_StorageRoot->AddChild(m_RootVme);

  m_RootVme->AddChild(vsphere);
  m_RootVme->AddChild(vcone);
  m_RootVme->AddChild(vmorph);

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  vtkMAFSmartPointer<vtkConeSource> cone;

  int i;
  for (i=0;i<100;i++)
  {
    trans.Identity();
    trans.RotateY(i*1.6-180.0,POST_MULTIPLY);

    m_RootVme->SetPose(trans.GetMatrix(),i);

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

    vmorph->SetData(morph->GetOutput(),log10((double)(100-i))*50);
    morph->Delete();
  }

  trans.Identity();
  trans.RotateZ(90,POST_MULTIPLY);
  trans.Translate(-.75,0,0,POST_MULTIPLY);
  vmorph->SetPose(trans.GetMatrix(),0); // set pose at time 0
}

//----------------------------------------------------------------------------
void mafVMEStorageTest::TestSave()
//----------------------------------------------------------------------------
{
  // save the tree to disk
  int res = m_Storage.Store();
  CPPUNIT_ASSERT(res == MAF_OK);
}

//----------------------------------------------------------------------------
void mafVMEStorageTest::TestSaveAndReload()
//----------------------------------------------------------------------------
{
  // save the tree to test to disk
  CPPUNIT_ASSERT(m_Storage.Store()==MAF_OK);

  //
  // Test data reloading:
  // THIS PART HAS LEAKS! 
  // Something in the restore part seems leaked... 
  mafVMEStorage load_storage;
  load_storage.SetURL("test_MSF/test_MSF.msf");

  mafVMERoot *loaded_root = load_storage.GetRoot();
  load_storage.Restore();

  // test if data is loaded on demand
  // NOT YET IMPLEMENTED

  // test if data is automatically released
  // NOT YET IMPLEMENTED

  // test if tree has been saved and reloaded correctly
  bool treeEquals = m_StorageRoot->CompareTree(loaded_root);
  CPPUNIT_ASSERT(treeEquals);

  // display tree animation again (for debug purposes)
  CPPUNIT_ASSERT(play_tree(loaded_root)==MAF_OK);

}
//----------------------------------------------------------------------------
void mafVMEStorageTest::TestTreeEditAndGarbageCollection()
//----------------------------------------------------------------------------
{
  // save the tree to test to disk
  CPPUNIT_ASSERT(m_Storage.Store()==MAF_OK);

  //
  // Test data reloading:
  mafVMEStorage load_storage;
  load_storage.SetURL("test_MSF/test_MSF.msf");

  mafVMERoot *loaded_root=load_storage.GetRoot();
  load_storage.Restore();

  //
  // Testing editing of the tree
  //
  vtkMAFSmartPointer<vtkConeSource> cone;
  cone->SetRadius(2.5);

  std::vector<mafString> cone_items_fname;
  cone_items_fname.resize(100);

  CPPUNIT_ASSERT(!strcmp(m_RootVme->GetChild(1)->GetName(),"cone"));

  mafSmartPointer<mafVMESurface> vcone = mafVMESurface::SafeDownCast(m_RootVme->GetChild(1));

  // modify some data of existing VMEs...
  int i=0;
  for (;i<100;i++)
  {

    cone_items_fname[i]=mafVMESurface::SafeDownCast(m_RootVme->GetChild(1))->GetDataVector()->GetItemByIndex(i)->GetURL();
    // change the cone radius
    cone->SetResolution(103-i);
    vcone->SetData(cone->GetOutput(),200-i*2);
  }

  {
    // also add a new sub-node of title
    vtkMAFSmartPointer<vtkCubeSource> new_cube;
    mafSmartPointer<mafVMESurface> new_cube_vme;
    new_cube_vme->SetData(new_cube->GetOutput(),0);

    m_RootVme->AddChild(new_cube_vme);
  } // make local objects reference to be removed

  // save changed data
  m_Storage.Store();

  //
  // check if data files garbage collection works
  //
  mafDirectory dir;
  std::set<mafString> file_list;
  dir.Load(testMSFDir);
  for (i=0;i<dir.GetNumberOfFiles();i++)
  {
    file_list.insert(dir.GetFile(i));
  }

  for (i=0;i<100;i++)
  {
    // test if data files has been changed
    mafString new_fname=vcone->GetDataVector()->GetItemByIndex(i)->GetURL();
    CPPUNIT_ASSERT(new_fname!=cone_items_fname[i]);

    // test removal of old files
    CPPUNIT_ASSERT(file_list.find(cone_items_fname[i])==file_list.end());
    // commented line below because now the single file mode manage
    // by itself the compression, so old test is not valid anymore
    //CPPUNIT_ASSERT(file_list.find(new_fname)!=file_list.end());
  }

  // now reload the MSF file in a new tree
  mafVMEStorage reload_storage;
  reload_storage.SetURL("test_MSF/test_MSF.msf");
  mafVMERoot *reloaded_root=reload_storage.GetRoot();
  reload_storage.Restore();

  mafTimeStamp tb[2],rtb[2];
  m_StorageRoot->GetOutput()->GetTimeBounds(tb);
  reloaded_root->GetOutput()->GetTimeBounds(rtb);
  CPPUNIT_ASSERT(mafEquals(tb[0],rtb[0])&&mafEquals(tb[1],rtb[1]));

  // test if the new tree is equal to the old one
  CPPUNIT_ASSERT(m_StorageRoot->CompareTree(reloaded_root));

  // notice that the loaded_root tree is not aligned with the disk 
  // image, since it was loaded before editing the orginal tree
  // and saving it. Since the tree data has already been loaded into
  // memory MAF do not notice anything. BEWARE to never do this in real
  // code: two storages pointing to the same MSF file can be very dangerous!!!
  CPPUNIT_ASSERT(!m_StorageRoot->CompareTree(loaded_root));

  // display tree animationa again (for debug purposes)
  CPPUNIT_ASSERT(play_tree(reloaded_root)==MAF_OK);  
}
