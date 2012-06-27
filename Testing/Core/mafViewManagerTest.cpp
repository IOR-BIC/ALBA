/*=========================================================================

 Program: MAF2
 Module: mafViewManagerTest
 Authors: Alberto Losi
 
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
#include "mafViewManager.h"
#include "mafViewManagerTest.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafView.h"
#include "mafViewVTK.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"
#include "mafDeviceButtonsPadMouse.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

class mafViewDummy : public mafView // Dummy class for testing
{
public:

  mafTypeMacro(mafViewDummy, mafView);

  mafViewDummy(wxString m_Label="");
  ~mafViewDummy(){m_NodeVector.clear();};
  mafView *Copy(mafObserver *Listener,bool enableLightCopy = false);
  void VmeAdd(mafNode *vme){m_NodeVector.push_back(vme);};
  void VmeRemove(mafNode *vme);
  void VmeSelect(mafNode *vme, bool select);
  void VmeShow(mafNode *vme, bool show);
  void CameraReset(mafNode *node = NULL){m_CameraReset=true;};
  void CameraUpdate(){m_CameraUpdate=true;};
  void VmeUpdateProperty(mafNode *vme, bool fromTag = false){m_UpdatedVme=vme;};

  // The following methods are added for test mafViewManager automation
  mafNode *GetVME(mafID vmeID); // Not a mafView method!
  mafNode *GetSelectedVME(){return m_SelectedVme;}; // Not a mafView method!
  mafNode *GetShownVME(){return m_ShownVme;}; // Not a mafView method!
  bool GetCameraReset(){return m_CameraReset;}; // Not a mafView method!
  bool GetCameraUpdate(){return m_CameraUpdate;}; // Not a mafView method!
  mafNode *GetUpdatedVME(){return m_UpdatedVme;}; // Not a mafView method!

protected:
  std::vector<mafNode*> m_NodeVector; // This vector contain the added vme
  mafNode *m_SelectedVme; // Selected VME
  mafNode *m_ShownVme;  // Shown VME
  mafNode *m_UpdatedVme; // Updated VME
  bool m_CameraReset; // true if has been performed a camera reset on this view
  bool m_CameraUpdate; // true if has been performed a camera update on this view
};

mafCxxTypeMacro(mafViewDummy);

mafViewDummy::mafViewDummy(wxString m_Label)
{
  Superclass;
  m_SelectedVme=NULL;
  m_ShownVme=NULL;
  m_CameraReset=false;
  m_CameraUpdate=false;
}
mafView *mafViewDummy::Copy(mafObserver *Listener, bool enableLightCopy)
{
  mafViewDummy *v = new mafViewDummy(m_Label);
  v->Create();
  return v;
}
void mafViewDummy::VmeRemove(mafNode *vme)
{
  std::vector<mafNode*>::iterator itVME;
  for(std::vector<mafNode*>::iterator it = m_NodeVector.begin(); it != m_NodeVector.end(); it++)
    if((*it)==vme)
    {
      itVME = it;
      break;
    }
    m_NodeVector.erase(itVME);
}
void mafViewDummy::VmeSelect(mafNode *vme, bool select) // Beware only test stuff! (It doesn't work like a normal view)
{
  if(select)
  {
    m_SelectedVme = vme;
  }
  else
  {
    m_SelectedVme = NULL;
  }
}
void mafViewDummy::VmeShow(mafNode *vme, bool show) // Beware only test stuff! (It doesn't work like a normal view)
{
  if(show)
  {
    m_ShownVme = vme;
  }
  else
  {
    m_ShownVme = NULL;
  }
}
mafNode *mafViewDummy::GetVME(mafID vmeID) // New method for test stuff
{
  if(vmeID < m_NodeVector.size())
  {
    return (m_NodeVector.at(vmeID));
  }
  else
  {
    return NULL;
  }
}

//----------------------------------------------------------------------------
void mafViewManagerTest::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case VIEW_CREATED: // Logic addressed event
        {
          m_EventResult = VIEW_CREATED;
          e->GetView()->SetFrame(new wxFrame());
        }
        break;
      case VIEW_DELETE: // Sidebar addressed event
        {
          m_EventResult = VIEW_DELETE;
        }
        break;
    }
  }
}
//----------------------------------------------------------------------------
void mafViewManagerTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafViewManagerTest::setUp()
//----------------------------------------------------------------------------
{
  m_Manager = new mafViewManager();
  m_Manager->SetListener(this);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::tearDown()
//----------------------------------------------------------------------------
{
  cppDEL(m_Manager);
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafViewManager* manager = new mafViewManager();
  cppDEL(manager);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafViewManager manager;
}
//----------------------------------------------------------------------------
void mafViewManagerTest::ViewAddTest()
//----------------------------------------------------------------------------
{
  mafView* view1 = new mafView();
  m_Manager->ViewAdd(view1, false); // not visible to menu

//  CPPUNIT_ASSERT(m_Manager->m_TemplateNum == 1); // check if the number of views is 1

  mafView* view2 = new mafView();
  m_Manager->ViewAdd(view2, false); // not visible to menu

  //CPPUNIT_ASSERT(m_Manager->m_TemplateNum == 2); // check if the number of views is 2
  CPPUNIT_ASSERT((m_Manager->GetListTemplate())[0] == view1); // check if is the same as the first added view
  CPPUNIT_ASSERT((m_Manager->GetListTemplate())[1] == view2); // check if is the same as the second added view
}
//----------------------------------------------------------------------------
void mafViewManagerTest::ViewInsertTest()
//----------------------------------------------------------------------------
{
  mafView* view1 = new mafView();
  m_Manager->ViewInsert(view1);

  CPPUNIT_ASSERT(m_Manager->GetList() == view1); // check if is the same as the first added view

  mafView* view2 = new mafView();
  m_Manager->ViewInsert(view2); // not visible to menu

  CPPUNIT_ASSERT(m_Manager->GetList()->m_Next == view2); // check if is the same as the second added view
}
//----------------------------------------------------------------------------
void mafViewManagerTest::ViewCreateTest()
//----------------------------------------------------------------------------
{
  // Test create by ID
  // Create a view from the template array
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array
  int vID = tempView->m_Id; // this id is set by ViewAdd and is useful for test ViewCreate

  mafView *createdView1 = m_Manager->ViewCreate(vID);
  CPPUNIT_ASSERT(createdView1->IsA(tempView->GetTypeName())); // check if is of the same type of the template view
  CPPUNIT_ASSERT(m_Manager->GetList() == createdView1); // check if the created view is inside the view list
  CPPUNIT_ASSERT(m_EventResult == VIEW_CREATED); // check if the raised event is view creation

  // Must delete all to remove leaks
  wxFrame *frame = createdView1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;

  // Test create by view type name
  // Create a view from the template array
  mafView *createdView2 = m_Manager->ViewCreate("class mafViewDummy");
  CPPUNIT_ASSERT(createdView2->IsA(tempView->GetTypeName())); // check if is of the same type of the template view
  CPPUNIT_ASSERT(m_Manager->GetList()->m_Next == createdView2); // check if the created view is inside the view list
  CPPUNIT_ASSERT(m_EventResult == VIEW_CREATED); // check if the raised event is view creation

  // Must delete all to remove leaks
  frame = createdView2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
}
//----------------------------------------------------------------------------
void mafViewManagerTest::ViewDeleteTest()
//----------------------------------------------------------------------------
{
  mafView* view1 = new mafView();
  mafView* view2 = new mafView();

  // Insert a view
  m_Manager->ViewInsert(view1);
  m_Manager->ViewInsert(view2);

  view1->m_Id = VIEW_START; // workaround otherwise the view ID start from 0
  // And then delete it
  m_Manager->ViewDelete(view1);

  //CPPUNIT_ASSERT(view1 == NULL); // check if the view has been deleted  
  CPPUNIT_ASSERT(m_Manager->GetList() != view1); // check if the view has been removed from the view list
  CPPUNIT_ASSERT(m_EventResult == VIEW_DELETE); // check if the raised event is view delete
}
//----------------------------------------------------------------------------
void mafViewManagerTest::GetViewTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  mafView* createdView1 = m_Manager->ViewCreate("class mafViewDummy");
  mafView* createdView2 = m_Manager->ViewCreate("class mafViewDummy");

  CPPUNIT_ASSERT(m_Manager->GetView(createdView1->m_Id,createdView1->m_Mult) == createdView1); // check that the first view is the same created in the fist position of the view matrix

  // Must delete all to remove leaks
  wxFrame *frame = createdView1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = createdView2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
}
//----------------------------------------------------------------------------
void mafViewManagerTest::GetListTest()
//----------------------------------------------------------------------------
{
  // insert some views in the manager views list
  mafView *view[6];
  
  for(int i = 0; i < 6; i++)
  {
    view[i] = new mafView();
    m_Manager->ViewInsert(view[i]);
  }

  mafView * curView = m_Manager->GetList();
  
  int v = 0;
  while(curView)
  {
    CPPUNIT_ASSERT(curView == view[v]); // check if views are listed correctly
    curView = curView->m_Next;
    v++;
  }
}
//----------------------------------------------------------------------------
void mafViewManagerTest::GetListTemplateTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView1 = new mafViewDummy();
  mafViewDummy *tempView2 = new mafViewDummy();
  
  m_Manager->ViewAdd(tempView1, false); // add this view to the template array
  m_Manager->ViewAdd(tempView2, false); // add this view to the template array

  mafView **temViewsList = m_Manager->GetListTemplate();
  CPPUNIT_ASSERT(temViewsList[0] == tempView1);
  CPPUNIT_ASSERT(temViewsList[1] == tempView2);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::ViewSelectedGetSelectedViewTest()
//----------------------------------------------------------------------------
{
  // insert some views in the manager views list
  mafView *view[6];

  for(int i = 0; i < 6; i++)
  {
    view[i] = new mafView();
    m_Manager->ViewInsert(view[i]);
  }

  // select view[3]
  m_Manager->ViewSelected(view[3]);
  CPPUNIT_ASSERT(m_Manager->GetSelectedView() == view[3]);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::VmeAddTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  mafVMERoot *root;
  mafVMEGeneric *vme;

  mafNEW(root);
  mafNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);
  CPPUNIT_ASSERT(m_Manager->m_SelectedVme == root); // the selected vme must be the root

  // check if all views 'recieve' the VMEs
  CPPUNIT_ASSERT(view1->GetVME(0) == root);
  CPPUNIT_ASSERT(view1->GetVME(1) == vme);
  CPPUNIT_ASSERT(view2->GetVME(0) == root);
  CPPUNIT_ASSERT(view2->GetVME(1) == vme);

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;


  mafDEL(root);
  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::VmeRemoveTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  mafVMERoot *root;
  mafVMEGeneric *vme;

  mafNEW(root);
  mafNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->VmeRemove(vme);

  // check if the vme was really deleted from all views
  CPPUNIT_ASSERT(view1->GetVME(1) == NULL);
  CPPUNIT_ASSERT(view2->GetVME(1) == NULL);

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;

  mafDEL(root);
  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::VmeSelectTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  mafVMERoot *root;
  mafVMEGeneric *vme;

  mafNEW(root);
  mafNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->VmeSelect(vme);

  // check if the vme was really selected in all views
  CPPUNIT_ASSERT(m_Manager->m_SelectedVme == vme);
  CPPUNIT_ASSERT(view1->GetSelectedVME() == vme);
  CPPUNIT_ASSERT(view2->GetSelectedVME() == vme);

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;

  mafDEL(root);
  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::VmeShowTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  mafVMERoot *root;
  mafVMEGeneric *vme;

  mafNEW(root);
  mafNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->ViewSelected(view1);

  m_Manager->VmeShow(vme,true);

  // check if the vme was really shown in the selected view
  CPPUNIT_ASSERT(view1->GetShownVME() == vme);
  CPPUNIT_ASSERT(view2->GetShownVME() == NULL); // only affect selected view!

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;

  mafDEL(root);
  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::CameraResetTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  mafVMERoot *root;
  mafVMEGeneric *vme;

  mafNEW(root);
  mafNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->ViewSelected(view1);

  m_Manager->CameraReset();

  // check if the camera was really resetted
  CPPUNIT_ASSERT(view1->GetCameraReset() == true);
  CPPUNIT_ASSERT(view2->GetCameraReset() == false); // only affect selected view!

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;

  mafDEL(root);
  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::CameraUpdateTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  mafVMERoot *root;
  mafVMEGeneric *vme;

  mafNEW(root);
  mafNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->ViewSelected(view1);

  m_Manager->CameraUpdate(true); // true to only affect selected view!

  // check if the update was 'sent' to the views
  CPPUNIT_ASSERT(view1->GetCameraUpdate() == true);
  CPPUNIT_ASSERT(view2->GetCameraUpdate() == false); // only affect selected view!

  m_Manager->CameraUpdate(false);

  CPPUNIT_ASSERT(view1->GetCameraUpdate() == true);
  CPPUNIT_ASSERT(view2->GetCameraUpdate() == true);

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;

  mafDEL(root);
  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::PropertyUpdateTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  mafVMERoot *root;
  mafVMEGeneric *vme;

  mafNEW(root);
  mafNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->VmeSelect(vme);

  m_Manager->PropertyUpdate(); // true to only affect selected view!

  // check if the update was 'sent' to the views
  CPPUNIT_ASSERT(view1->GetUpdatedVME() == vme);
  CPPUNIT_ASSERT(view2->GetUpdatedVME() == vme);

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;

  mafDEL(root);
  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::GetCurrentRootTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  mafVMERoot *root;
  mafVMEGeneric *vme;

  mafNEW(root);
  mafNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);


  CPPUNIT_ASSERT(m_Manager->GetCurrentRoot() == root);

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;

  mafDEL(root);
  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::SetMouseTest()
//----------------------------------------------------------------------------
{
  mafDeviceButtonsPadMouse *mouse;
  mafNEW(mouse);
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  m_Manager->ViewSelected(view1);
  m_Manager->SetMouse(mouse);

  CPPUNIT_ASSERT(m_Manager->m_Mouse == mouse);

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  mafDEL(mouse);
}
//----------------------------------------------------------------------------
void mafViewManagerTest::CollaborateTest()
//----------------------------------------------------------------------------
{
  mafViewDummy *tempView = new mafViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  mafViewDummy *view1 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));
  mafViewDummy *view2 = mafViewDummy::SafeDownCast(m_Manager->ViewCreate("class mafViewDummy"));

  m_Manager->Collaborate(true);

  CPPUNIT_ASSERT(m_Manager->m_CollaborateStatus == true);

  // Must delete all to remove leaks
  wxFrame *frame = view1->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
  frame = view2->GetFrame();
  frame->Show(false);
  frame->Close(true);
  delete frame;
}