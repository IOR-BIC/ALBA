/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: testMSF.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-12 19:26:27 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone m.petrone@cineca.it
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2001 
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .SECTION Description
// Test for VME related classes. 
// .SECTION See Also
// mflVME mflVMEStorage
// .SECTION ToDo
// - Improve Test for Copy of VME
// - Implement Test for SaveAs
// - Implement Test for Load


#include <string>

#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"
#include "vtkTextSource.h"
#include "vtkCylinderSource.h"
#include "vtkAxes.h"
#include <vtkImageCanvasSource2D.h> 
#include <vtkPointSource.h> 
#include <vtkFileOutputWindow.h> 

#include "vtkDataSetMapper.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkPivotTransform.h"

#include "vtkDicomUnPacker.h"
#include "mflDefines.h"

#include "mflVMEGeneric.h"
#include "mflVMELandmark.h"
#include "mflVMEGroup.h"
#include "mflVMELandmarkCloud.h"
#include "mflVMEPointSet.h"
#include "mflVMERoot.h"
#include "mflVMESurface.h"
#include "mflVMEVolume.h"

#include "mflVMEIterator.h"
#include "mflVMEStorage.h"

#include "mflVMEItemArray.h"
#include "mflItemArray.txx"
#include "mflMatrixVector.h"

#include <vector>

// test includes
#include "TestUnit.h"

static const char *TestNameStr = "MSFTest";

class MSFTestCase : public TestCase {
  protected:
    vtkRenderWindow         *renWin;
    vtkRenderer             *renderer;
    vtkRenderWindowInteractor *iren;

    mflVMEStorage *storage;

    vtkFileOutputWindow *log;
 
  public:
    MSFTestCase(std::string name);
    virtual ~MSFTestCase();
    
	  static Test		*suite();

    void setUp();
    void tearDown();

    // tests
    void MSFWriteTest();
    void MSFReadTest();
    void MSFImportTest();
    void MSFSaveAsTest();

  
};


//----------------------------------------------------------------------------
//                          MSFTestCase methods
//----------------------------------------------------------------------------
Test *MSFTestCase::suite() {
	TestSuite *testSuite = new TestSuite (TestNameStr);
  testSuite->addTest (new TestCaller <MSFTestCase> ("Test for mflVMEStorage Write",  &MSFTestCase::MSFWriteTest));
  testSuite->addTest (new TestCaller <MSFTestCase> ("Test for mflVMEStorage Read",  &MSFTestCase::MSFReadTest));
  testSuite->addTest (new TestCaller <MSFTestCase> ("Test for mflVMEStorage SaveAs",  &MSFTestCase::MSFSaveAsTest));
  testSuite->addTest (new TestCaller <MSFTestCase> ("Test for mflVMEStorage Import",  &MSFTestCase::MSFImportTest));
  
	return testSuite;
}

MSFTestCase::MSFTestCase(std::string name) : TestCase(name) { 

  
}

MSFTestCase::~MSFTestCase()
{

}

void MSFTestCase::setUp()
{
  log=vtkFileOutputWindow::New();
  log->SetInstance(log);
  log->SetFileName("test.log");
  log->AppendOn();
  storage=mflVMEStorage::New();

  // create windows
  this->renderer = vtkRenderer::New();
  this->renWin = vtkRenderWindow::New();
  this->renWin->AddRenderer(this->renderer);
  this->iren = vtkRenderWindowInteractor::New();
  this->iren->SetRenderWindow(renWin);


  this->renderer->SetBackground(0.1, 0.1, 0.1);
  this->renWin->SetSize(640, 480);
  this->renWin->SetPosition(400,0);
}

void MSFTestCase::tearDown()
{
  if (storage)
  {
    storage->Delete();
  }
  log->Delete();

  this->renderer->Delete();
  this->renWin->Delete();
  this->iren->Delete();
}


//--------------------------------------------------------------------------
//          Read test for MSF file
//--------------------------------------------------------------------------
void MSFTestCase::MSFReadTest()
{
  //storage->SetFileName(mflString::ParsePathName("../../../../mmData/MSF/Example1/prova.msf"));
  storage->SetFileName("msfTest.msf");

  mflVMERoot *root=storage->GetOutput();

  storage->Read();

  mflVMEIterator *iter=root->NewIterator();

  //root->AutoUpdateCurrentDataOn();
  root->SetCurrentTime(0);

  // connect VME to assemblies and put root assembly into the renderer
  for (mflVME *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
  {
    
    if (vme->IsA("mflVMERoot"))
    {
      ASSERT(vme->GetReferenceCount()==2);
      vtkAssembly *vmeasm=vtkAssembly::New();
      vmeasm->SetUserMatrix(vme->GetMatrix());
      vme->SetClientData(vmeasm);
      
      this->renderer->AddActor(vmeasm);
    }
    else
    {
      ASSERT(vme->GetReferenceCount()==1);
      mflVMEItem *item=vme->GetItem(0);

      ASSERT(item!=NULL);

      vtkDataSet *itemdata=item->GetData();
      ASSERT(itemdata!=NULL);

      vtkDataSetMapper *mapper=vtkDataSetMapper::New();;

      // Test on the type of stored data
      if (vme->IsA("mflVMEVolume"))
      {
        ASSERT(itemdata->IsA("vtkImageData")!=0);
      }
      else
      {
        ASSERT(itemdata->IsA("vtkPolyData")!=0);
      }

      mapper->SetInput((vtkPolyData *)itemdata);
  
      vtkActor *vmeact=vtkActor::New();
      vmeact->SetMapper(mapper);
      mapper->Delete();

      vtkAssembly *vmeasm=vtkAssembly::New();
      vmeasm->AddPart(vmeact);

      vmeact->Delete();

      vmeasm->SetUserMatrix(vme->GetMatrix());

      vme->SetClientData(vmeasm);

      ASSERT(vme->GetClientData()==vmeasm);

      vtkAssembly *pvmeasm=(vtkAssembly *)vme->GetParent()->GetClientData();
      ASSERT(pvmeasm!=NULL);
      ASSERT(pvmeasm->IsA("vtkAssembly")!=0);
      pvmeasm->AddPart(vmeasm);
    }
    printf(".");
  }

  for (int t = 0; t < 10; t++) 
  {
  
    // Update time into all the tree
    root->SetTreeTime(t);

    this->renderer->ResetCameraClippingRange();
    this->renWin->Render();
    
  }

  for (vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
  {
    vtkObject * obj=(vtkObject *)vme->GetClientData();
    if (obj)
      obj->Delete();
    vme->SetClientData(NULL);
  }

  iter->Delete();
}

//--------------------------------------------------------------------------
//          Import three Dicom Images into a VME tree
//--------------------------------------------------------------------------
void MSFTestCase::MSFWriteTest() {

  cerr<<"Testing mflVMEStorage Write() and Load()\n";

  ASSERT(storage!=NULL);

  mflVMERoot *root=storage->GetOutput();
  ASSERT(root!=NULL);

  root->SetName("Root");

  vtkDicomUnPacker *dicom_reader = vtkDicomUnPacker::New();
  ASSERT(dicom_reader!=NULL);

	dicom_reader->SetDictionaryFileName(mflString::ParsePathName("../../../../mmData/Dictionaries/dicom3.dic"));
	dicom_reader->SetFileName(mflString::ParsePathName("../../../../mmData/CT/1.2.840.113619.2.30.1.1762288646.2052.1034313897.736"));
	dicom_reader->Update();

  // if dictionay has been found
  ASSERT(dicom_reader->GetDictionarySize()>0);

  
  mflVMEVolume *vdicom=mflVMEVolume::New();
  mflVMESurface *vstem=mflVMESurface::New();
  
  ASSERT(vdicom != NULL);
  ASSERT(vstem != NULL);
  
  // VME type are multiple of 2 since each type can be natural or synthetic
  enum datatypes {CT_SLICE=2,PROSTHESIS=4};

  vdicom->SetName("CT Slice1");
  vstem->SetName("stem");
  
  vtkPivotTransform *trans=vtkPivotTransform::New();

  mflVMEItem *idicom=mflVMEItem::New();
  ASSERT(idicom != NULL);

  idicom->SetData(dicom_reader->GetOutput());
  ASSERT(idicom->IsDataModified() != 0);

  idicom->SetTimeStamp(0);
  ASSERT(idicom->GetTimeStamp() == 0);

  dicom_reader->Delete();

  ASSERT(idicom->GetData() != NULL);
  ASSERT(dicom_reader->GetReferenceCount() == 1);
  
  root->AddChild(vdicom);
  
  vdicom->Delete();  
  ASSERT(root->GetNumberOfChildren() == 1);
  
  vdicom->AddItem(idicom);

  idicom->Delete();

  // Ref count == 2 since idicom is referenced
  // both by DataArray in VME and the interpolator
  // in the DataArray (as CurrentItem)
  ASSERT(idicom->GetReferenceCount() == 2);
  ASSERT(vdicom->GetNumberOfItems() == 1);

  vdicom->AddChild(vstem);

  vstem->Delete();

  ASSERT(vstem->GetReferenceCount()==1);
  ASSERT(vdicom->GetNumberOfChildren() == 1);
  ASSERT(vdicom->GetReferenceCount() == 1);

  // Import DICOM Tags
  vtkTagArray *tags=idicom->GetTagArray();

  for (int i=0;i<dicom_reader->GetNumberOfTags();i++)
  {
	  char tmp[256];
	  const char *keyword;

    const vtkDicomUnPacker::DICOM *tag=dicom_reader->GetTag(i);

	  if (!strcmp(tag->Keyword,""))
	  {
      ASSERT(tag->intoDictionary==vtkDicomUnPacker::no);
		  
			int group=tag->Group;
			int element=tag->Element;
			cout<<"Tag not in Dictionary Group="<<group<<" Element="<<element<<"\n";
			sprintf(tmp,"G%d_E%d",group,element);
			keyword=tmp;
	  }
	  else
	  {
		  keyword=(char *)tag->Keyword;
      ASSERT(tag->intoDictionary==vtkDicomUnPacker::yes);
	  }

    int mult=dicom_reader->GetTagElement(i)->mult;

	  const vtkDicomUnPacker::VALUE *tagElement=dicom_reader->GetTagElement(i);
    
    if (tagElement->type==vtkDicomUnPacker::string)
    {
      // String TAG
      int idx=tags->AddTag(vtkTagItem(keyword,""));

      // filter this hurting Tag
      if (vtkString::Equals(keyword,"OtherPatientID"))
      {
        tags->GetTag(idx)->SetNumberOfComponents(0);
        continue;
      }
      if (mult>=0)
      {
        // Fill the Element components
        tags->GetTag(idx)->SetNumberOfComponents(mult+1);

        for (int j=0;j<=mult;j++)
        {
          const char *value=tagElement->stringa[j];
          tags->GetTag(idx)->SetValue(value,j);
        }
      }
      else
      {
        tags->GetTag(idx)->SetNumberOfComponents(0);
        //cerr << "empty string...\n";
      }

      //tags->GetTag(idx)->PrintSelf(cout,vtkIndent());
    }
    else
    {
      // NUMERIC TAG

	    double *value=(double *)tagElement->num;
      
      if (mult>0)
      {
        tags->AddTag(vtkTagItem(keyword,value,mult+1));
      }
	    else
	    {
        // This is a tag with an empty value...
        vtkTagItem empty(keyword,"",vtkTagItem::MFL_MISSING_TAG);
        empty.SetNumberOfComponents(0);
        tags->AddTag(empty);
	    }
 
    }
  }

  //*****

  // Set stem position
  trans->Identity();
  float bounds[6];
  idicom->GetData()->GetBounds(bounds);
  trans->Translate((bounds[0]+bounds[1])/2,(bounds[2]+bounds[3])/2,(bounds[4]+bounds[5])/2);
  vstem->SetMatrix(trans->GetMatrix());

  // The stem
  vtkPolyDataReader *stem = vtkPolyDataReader::New();
  ASSERT(stem != NULL);
  mflString stemfname;
  stemfname.AppendPath("../../../../mmData/vtk_comp/stelo.vtk");
  stem->SetFileName(stemfname.GetCStr());

  mflVMEItem *istem=mflVMEItem::New();
  stem->Update();
  istem->SetData(stem->GetOutput());
  stem->Delete();

  vstem->AddItem(istem);
  istem->Delete();

  trans->Identity();
  trans->SetOrigin((bounds[0]+bounds[1])/2,(bounds[2]+bounds[3])/2,(bounds[4]+bounds[5])/2);
  // Set the root change of orientation along time

  int t;

  for (t = 0; t < 100; t++) 
  {
    root->SetMatrix(trans->GetMatrix(),t);
    trans->RotateY(3.6);
    trans->RotateZ(1.8);
  }

  // ...............
  mflVMELandmarkCloud *vlmCloud=mflVMELandmarkCloud::New();
  vlmCloud->SetName("landmarks");
  vlmCloud->AppendLandmark("paperino");
  vlmCloud->AppendLandmark("pluto");
  vlmCloud->AppendLandmark("topolino");
  vlmCloud->AppendLandmark("qui");
  vlmCloud->AppendLandmark("quo");
  vlmCloud->AppendLandmark("qua");

  ASSERT(vlmCloud->GetNumberOfLandmarks()==6);
  ASSERT(vtkString::Equals(vlmCloud->GetLandmarkName(3),"qui")!=0);
  ASSERT(vtkString::Equals(vlmCloud->GetLandmarkName(5),"qua")!=0);
  ASSERT(vtkString::Equals(vlmCloud->GetLandmarkName(0),"paperino")!=0);
  ASSERT(vlmCloud->FindLandmarkIndex("topolino")==2);

  for (i=0;i<20;i++)
  {
    ASSERT(vlmCloud->SetLandmark("pluto",.1*i,5+.5*i,1+i*.1,i)==VTK_OK);
    ASSERT(vlmCloud->SetLandmark("qui",-.1*i,-5+.5*i,i,i)==VTK_OK);
    ASSERT(vlmCloud->SetLandmark("quo",i,.5*i,-.1*i,i)==VTK_OK);
    ASSERT(vlmCloud->SetLandmark("topolino",i,.5*i,-.1*i,i)==VTK_OK);
    ASSERT(vlmCloud->SetLandmark("pluto",2+i*.1,i-10,-.1*i,i)==VTK_OK);
    ASSERT(vlmCloud->SetLandmark("paperino",-2+i*.3,.3*i-10,.7*i,i)==VTK_OK);
    ASSERT(vlmCloud->SetLandmark("qua",-i,-.5*i,.1*i,i)==VTK_OK);
  }

  double x,y,z;
  for (i=0;i<20;i++)
  {
    vlmCloud->GetLandmark("paperino",x,y,z,i);
    ASSERT(x==-2+i*.3); ASSERT(y==.3*i-10); ASSERT(z==.7*i);
    vlmCloud->GetLandmark("pluto",x,y,z,i);
    ASSERT(x==2+i*.1); ASSERT(y==i-10); ASSERT(z==-.1*i);
    vlmCloud->GetLandmark("qui",x,y,z,i);
    ASSERT(x==-.1*i); ASSERT(y==-5+.5*i); ASSERT(z==i);
  }

  root->AddChild(vlmCloud);
  vlmCloud->Delete();

  trans->Delete();

  // Test display of generic VME tree of surfaces

  mflVMEIterator *iter=root->NewIterator();

  //root->AutoUpdateCurrentDataOn();
  root->SetCurrentTime(0);

  

  // connect VME to assemblies and put root assembly into the renderer
  for (mflVME *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
  {
    if (vme->IsA("mflVMERoot"))
    {
      vtkAssembly *vmeasm=vtkAssembly::New();
      vmeasm->SetUserMatrix(vme->GetMatrix());
      vme->SetClientData(vmeasm);
      
      this->renderer->AddActor(vmeasm);
    }
    else
    {
      mflVMEItem *item=vme->GetItem(0);

      ASSERT(item!=NULL);

      vtkDataSet *itemdata=item->GetData();
      ASSERT(itemdata!=NULL);

      vtkDataSetMapper *mapper=vtkDataSetMapper::New();;

      // Test on the type of stored data
      if (vme->IsA("mflVMEVolume"))
      {
        ASSERT(itemdata->IsA("vtkImageData")!=0);
      }
      else
      {
        ASSERT(itemdata->IsA("vtkPolyData")!=0);
      }

      mapper->SetInput(itemdata);
  
      vtkActor *vmeact=vtkActor::New();
      vmeact->SetMapper(mapper);
      mapper->Delete();

      vtkAssembly *vmeasm=vtkAssembly::New();
      vmeasm->AddPart(vmeact);

      vmeact->Delete();

      vmeasm->SetUserMatrix(vme->GetMatrix());

      vme->SetClientData(vmeasm);

      ASSERT(vme->GetClientData()==vmeasm);

      vtkAssembly *pvmeasm=(vtkAssembly *)vme->GetParent()->GetClientData();
      ASSERT(pvmeasm!=NULL);
      ASSERT(pvmeasm->IsA("vtkAssembly")!=0);
      pvmeasm->AddPart(vmeasm);
    }
    printf(".");
  }

  for (t = 0; t < 1; t++) 
  {
  
    // Update time into all the tree
    root->SetTreeTime(t);

    this->renderer->ResetCameraClippingRange();
    this->renWin->Render();
    
  }

  for (vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
  {
    vtkObject * obj=(vtkObject *)vme->GetClientData();
    if (obj)
      obj->Delete();
    vme->SetClientData(NULL);
  }

  iter->Delete();

  storage->SetFileName("msfTest.msf");
  storage->Write();

  //----------------------------------------------------------------
  // Try reload the same file to verify writing and load correctness
  //----------------------------------------------------------------

  mflVMEStorage *newstorage=mflVMEStorage::New();
  newstorage->SetFileName(storage->GetFileName());
  newstorage->Load();

  mflVMEItem::GlobalCompareDataFlagOn();

  // compare the two Trees
  ASSERT(storage->GetOutput()->CompareTree(newstorage->GetOutput()));

  ASSERT(newstorage->GetOutput()->CompareTree(storage->GetOutput()));
  

  newstorage->Delete();

  // test correctness reference counting
  storage->Delete();
  SHOULDFAIL(storage->GetReferenceCount()>0);

  storage=NULL;
  
}

void MSFTestCase::MSFSaveAsTest() 
{
  cerr << "Testing mflVMEStorage SaveAs()\n";

  ASSERT(storage!=NULL);

  mflVMERoot *root=storage->GetOutput();
  ASSERT(root!=NULL);

  storage->SetFileName(mflString::ParsePathName("../../../../mmData/MSF/Example1/prova.msf"));

  storage->Load();

  mflString newfilename;
  newfilename.AppendPath("saveas.msf");
  storage->SetFileName(newfilename);
  storage->Write();

  storage->SetFileName(mflString::ParsePathName("../../../../mmData/MSF/Example1/prova.msf"));
  storage->Load();
  vtkDataSet *poly=storage->GetOutput()->GetFirstChild()->GetCurrentData();
  ASSERT(vtkString::Equals(poly->GetClassName(),"vtkPolyData")!=0);

  // reload the tree to verify it has written all the data
  mflVMEStorage *newstorage=mflVMEStorage::New();
  newstorage->SetFileName(newfilename);
  newstorage->Load();

  mflVMEItem::GlobalCompareDataFlagOn();

  // compare the two Trees
  ASSERT(storage->GetOutput()->CompareTree(newstorage->GetOutput()));

  ASSERT(newstorage->GetOutput()->CompareTree(storage->GetOutput()));
  

  newstorage->Delete();

}

void MSFTestCase::MSFImportTest()
{
  cerr << "Testing mflVMEStorage Import()\n";

  ASSERT(storage!=NULL);

  mflVMERoot *root=storage->GetOutput();
  ASSERT(root!=NULL);

  storage->SetFileName(mflString::ParsePathName("../../../../mmData/MSF/Example1/prova.msf"));

  storage->Load();

  ASSERT(storage->GetOutput()!=NULL);

  mflVME *copy=mflVME::CopyTree(storage->GetOutput());
  ASSERT(copy!=NULL);
  ASSERT(copy->GetFirstChild()!=NULL);

  /*vtkVector<mflVME *> *children=vtkVector<mflVME *>::NewCompressor();
  for (int i=0;i<storage->GetOutput()->GetNumberOfChildren();i++)
  {
    children->AppendItem(storage->GetOutput()->GetChild(i));
  }
*/

  mflString newfilename;
  //newfilename.AppendPath("imported.msf");
  //newfilename.AppendPath("d:/tmp/msf1/prova.msf");
  newfilename.AppendPath("../../../../mmData/MSF/Example2/FloatTime.msf");

  // reload the tree to verify it has written all the data
  mflVMEStorage *newstorage=mflVMEStorage::New();
  newstorage->SetFileName(newfilename);
  newstorage->Read();

  newstorage->Import(storage->GetOutput());

  ASSERT(newstorage->GetOutput()->GetNumberOfChildren()==2);

  mflVMEItem::GlobalCompareDataFlagOn();

  newstorage->SetFileName("merged.msf");

  newstorage->Write();

  // compare the two Trees
  ASSERT(copy->GetFirstChild()->CompareTree(newstorage->GetOutput()->GetLastChild()));
  newstorage->Delete();

  copy->Delete();

  cerr <<"\n";
  
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

  runner.addTest(TestNameStr, MSFTestCase::suite());  

  bool result=runner.run(argc, argv, s);

  return (result?0:-1);
}
