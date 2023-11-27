/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewManagerTest
 Authors: Alberto Losi
 
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
#include "albaViewManager.h"
#include "albaViewManagerTest.h"
#include "albaSmartPointer.h"
#include "albaVME.h"
#include "albaView.h"
#include "albaViewVTK.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"
#include "albaDeviceButtonsPadMouse.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

class albaViewDummy : public albaView // Dummy class for testing
{
public:

  albaTypeMacro(albaViewDummy, albaView);

  albaViewDummy(wxString label="viewDummy");
  ~albaViewDummy(){m_NodeVector.clear();};
  albaView *Copy(albaObserver *Listener,bool enableLightCopy = false);
  void VmeAdd(albaVME *vme){m_NodeVector.push_back(vme);};
  void VmeRemove(albaVME *vme);
  void VmeSelect(albaVME *vme, bool select);
  void VmeShow(albaVME *vme, bool show);
  void CameraReset(albaVME *node = NULL){m_CameraReset=true;};
  void CameraUpdate(){m_CameraUpdate=true;};
  void VmeUpdateProperty(albaVME *vme, bool fromTag = false){m_UpdatedVme=vme;};

  // The following methods are added for test albaViewManager automation
  albaVME *GetVME(albaID vmeID); // Not a albaView method!
	virtual void SetBackgroundColor(wxColor color);
	void SetMouse(albaDeviceButtonsPadMouse *mouse);
	albaVME *GetSelectedVME() { return m_SelectedVme; }; // Not a albaView method!
  albaVME *GetShownVME(){return m_ShownVme;}; // Not a albaView method!
  bool GetCameraReset(){return m_CameraReset;}; // Not a albaView method!
  bool GetCameraUpdate(){return m_CameraUpdate;}; // Not a albaView method!
  albaVME *GetUpdatedVME(){return m_UpdatedVme;}; // Not a albaView method!

protected:
  std::vector<albaVME*> m_NodeVector; // This vector contain the added vme
  albaVME *m_SelectedVme; // Selected VME
  albaVME *m_ShownVme;  // Shown VME
  albaVME *m_UpdatedVme; // Updated VME
  bool m_CameraReset; // true if has been performed a camera reset on this view
  bool m_CameraUpdate; // true if has been performed a camera update on this view
};

albaCxxTypeMacro(albaViewDummy);

albaViewDummy::albaViewDummy(wxString label):albaView(label)
{
  m_SelectedVme=NULL;
  m_ShownVme=NULL;
  m_CameraReset=false;
  m_CameraUpdate=false;
}
albaView *albaViewDummy::Copy(albaObserver *Listener, bool enableLightCopy)
{
  albaViewDummy *v = new albaViewDummy(m_Label);
  v->Create();
  return v;
}
void albaViewDummy::VmeRemove(albaVME *vme)
{
  std::vector<albaVME*>::iterator itVME;
  for(std::vector<albaVME*>::iterator it = m_NodeVector.begin(); it != m_NodeVector.end(); it++)
    if((*it)==vme)
    {
      itVME = it;
      break;
    }
    m_NodeVector.erase(itVME);
}
void albaViewDummy::VmeSelect(albaVME *vme, bool select) // Beware only test stuff! (It doesn't work like a normal view)
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
void albaViewDummy::VmeShow(albaVME *vme, bool show) // Beware only test stuff! (It doesn't work like a normal view)
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
albaVME *albaViewDummy::GetVME(albaID vmeID) // New method for test stuff
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
void albaViewDummy::SetBackgroundColor(wxColor color)
{
	m_BackgroundColor = color;
}
void albaViewDummy::SetMouse(albaDeviceButtonsPadMouse *mouse)
{
	
}

//----------------------------------------------------------------------------
void albaViewManagerTest::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
void albaViewManagerTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaViewManagerTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaViewManager();
  m_Manager->SetListener(this);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::AfterTest()
//----------------------------------------------------------------------------
{
  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaViewManager* manager = new albaViewManager();
  cppDEL(manager);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaViewManager manager;
}
//----------------------------------------------------------------------------
void albaViewManagerTest::ViewAddTest()
//----------------------------------------------------------------------------
{
  albaView* view1 = new albaViewVTK();
  m_Manager->ViewAdd(view1, false); // not visible to menu

//  CPPUNIT_ASSERT(m_Manager->m_TemplateNum == 1); // check if the number of views is 1

  albaView* view2 = new albaViewVTK();
  m_Manager->ViewAdd(view2, false); // not visible to menu

  //CPPUNIT_ASSERT(m_Manager->m_TemplateNum == 2); // check if the number of views is 2
  CPPUNIT_ASSERT((m_Manager->GetListTemplate())[0] == view1); // check if is the same as the first added view
  CPPUNIT_ASSERT((m_Manager->GetListTemplate())[1] == view2); // check if is the same as the second added view
}
//----------------------------------------------------------------------------
void albaViewManagerTest::ViewInsertTest()
//----------------------------------------------------------------------------
{
  albaView* view1 = new albaViewDummy();

  m_Manager->ViewInsert(view1);

  CPPUNIT_ASSERT(m_Manager->GetList() == view1); // check if is the same as the first added view

  albaView* view2 = new albaViewDummy();
  m_Manager->ViewInsert(view2); // not visible to menu

  CPPUNIT_ASSERT(m_Manager->GetList()->m_Next == view2); // check if is the same as the second added view
}
//----------------------------------------------------------------------------
void albaViewManagerTest::ViewCreateTest()
//----------------------------------------------------------------------------
{
  // Test create by ID
  // Create a view from the template array
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array
  int vID = tempView->m_Id; // this id is set by ViewAdd and is useful for test ViewCreate

  albaView *createdView1 = m_Manager->ViewCreate(vID);
  CPPUNIT_ASSERT(createdView1->IsA(tempView->GetTypeName())); // check if is of the same type of the template view
  CPPUNIT_ASSERT(m_Manager->GetList() == createdView1); // check if the created view is inside the view list
  CPPUNIT_ASSERT(m_EventResult == VIEW_CREATED); // check if the raised event is view creation


  // Test create by view type name
  // Create a view from the template array
  albaView *createdView2 = m_Manager->ViewCreate("viewDummy");
  CPPUNIT_ASSERT(createdView2->IsA(tempView->GetTypeName())); // check if is of the same type of the template view
  CPPUNIT_ASSERT(m_Manager->GetList()->m_Next == createdView2); // check if the created view is inside the view list
  CPPUNIT_ASSERT(m_EventResult == VIEW_CREATED); // check if the raised event is view creation

}
//----------------------------------------------------------------------------
void albaViewManagerTest::ViewDeleteTest()
//----------------------------------------------------------------------------
{
  albaView* view1 = new albaViewDummy();
  albaView* view2 = new albaViewDummy();

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
void albaViewManagerTest::GetViewTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  albaView* createdView1 = m_Manager->ViewCreate("viewDummy");
  albaView* createdView2 = m_Manager->ViewCreate("viewDummy");

  CPPUNIT_ASSERT(m_Manager->GetView(createdView1->m_Id,createdView1->m_Mult) == createdView1); // check that the first view is the same created in the fist position of the view matrix
}
//----------------------------------------------------------------------------
void albaViewManagerTest::GetListTest()
//----------------------------------------------------------------------------
{
  // insert some views in the manager views list
  albaView *view[6];
  
  for(int i = 0; i < 6; i++)
  {
    view[i] = new albaViewDummy();
    m_Manager->ViewInsert(view[i]);
  }

  albaView * curView = m_Manager->GetList();
  
  int v = 0;
  while(curView)
  {
    CPPUNIT_ASSERT(curView == view[v]); // check if views are listed correctly
    curView = curView->m_Next;
    v++;
  }
}
//----------------------------------------------------------------------------
void albaViewManagerTest::GetListTemplateTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView1 = new albaViewDummy();
  albaViewDummy *tempView2 = new albaViewDummy();
  
  m_Manager->ViewAdd(tempView1, false); // add this view to the template array
  m_Manager->ViewAdd(tempView2, false); // add this view to the template array

  albaView **temViewsList = m_Manager->GetListTemplate();
  CPPUNIT_ASSERT(temViewsList[0] == tempView1);
  CPPUNIT_ASSERT(temViewsList[1] == tempView2);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::ViewSelectedGetSelectedViewTest()
//----------------------------------------------------------------------------
{
  // insert some views in the manager views list
  albaView *view[6];

  for(int i = 0; i < 6; i++)
  {
    view[i] = new albaViewDummy();
    m_Manager->ViewInsert(view[i]);
  }

  // select view[3]
  m_Manager->ViewSelected(view[3]);
  CPPUNIT_ASSERT(m_Manager->GetSelectedView() == view[3]);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::VmeAddTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  albaViewDummy *view2 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));

  albaVMERoot *root;
  albaVMEGeneric *vme;

  albaNEW(root);
  albaNEW(vme);

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

  albaDEL(root);
  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::VmeRemoveTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  albaViewDummy *view2 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));

  albaVMERoot *root;
  albaVMEGeneric *vme;

  albaNEW(root);
  albaNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->VmeRemove(vme);

  // check if the vme was really deleted from all views
  CPPUNIT_ASSERT(view1->GetVME(1) == NULL);
  CPPUNIT_ASSERT(view2->GetVME(1) == NULL);

  albaDEL(root);
  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::VmeSelectTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  albaViewDummy *view2 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));

  albaVMERoot *root;
  albaVMEGeneric *vme;

  albaNEW(root);
  albaNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->VmeSelect(vme);

  // check if the vme was really selected in all views
  CPPUNIT_ASSERT(m_Manager->m_SelectedVme == vme);
  CPPUNIT_ASSERT(view1->GetSelectedVME() == vme);
  CPPUNIT_ASSERT(view2->GetSelectedVME() == vme);

  albaDEL(root);
  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::VmeShowTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  albaViewDummy *view2 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));

  albaVMERoot *root;
  albaVMEGeneric *vme;

  albaNEW(root);
  albaNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->ViewSelected(view1);

  m_Manager->VmeShow(vme,true);

  // check if the vme was really shown in the selected view
  CPPUNIT_ASSERT(view1->GetShownVME() == vme);
  CPPUNIT_ASSERT(view2->GetShownVME() == NULL); // only affect selected view!

  albaDEL(root);
  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::CameraResetTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  albaViewDummy *view2 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));

  albaVMERoot *root;
  albaVMEGeneric *vme;

  albaNEW(root);
  albaNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->ViewSelected(view1);

  m_Manager->CameraReset();

  // check if the camera was really resetted
  CPPUNIT_ASSERT(view1->GetCameraReset() == true);
  CPPUNIT_ASSERT(view2->GetCameraReset() == false); // only affect selected view!


  albaDEL(root);
  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::CameraUpdateTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  albaViewDummy *view2 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));

  albaVMERoot *root;
  albaVMEGeneric *vme;

  albaNEW(root);
  albaNEW(vme);

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

  albaDEL(root);
  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::PropertyUpdateTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  albaViewDummy *view2 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));

  albaVMERoot *root;
  albaVMEGeneric *vme;

  albaNEW(root);
  albaNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  m_Manager->VmeSelect(vme);

  m_Manager->PropertyUpdate(); // true to only affect selected view!

  // check if the update was 'sent' to the views
  CPPUNIT_ASSERT(view1->GetUpdatedVME() == vme);
  CPPUNIT_ASSERT(view2->GetUpdatedVME() == vme);

  albaDEL(root);
  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::GetCurrentRootTest()
//----------------------------------------------------------------------------
{
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  albaViewDummy *view2 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));

  albaVMERoot *root;
  albaVMEGeneric *vme;

  albaNEW(root);
  albaNEW(vme);

  // Add a root
  m_Manager->VmeAdd(root);
  // Add a vme
  m_Manager->VmeAdd(vme);

  CPPUNIT_ASSERT(m_Manager->GetCurrentRoot() == root);

  albaDEL(root);
  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaViewManagerTest::SetMouseTest()
//----------------------------------------------------------------------------
{
  albaDeviceButtonsPadMouse *mouse;
  albaNEW(mouse);
  albaViewDummy *tempView = new albaViewDummy();
  m_Manager->ViewAdd(tempView, false); // add this view to the template array

  // Create some views
  albaViewDummy *view1 = albaViewDummy::SafeDownCast(m_Manager->ViewCreate("viewDummy"));
  m_Manager->ViewSelected(view1);
  m_Manager->SetMouse(mouse);

  CPPUNIT_ASSERT(m_Manager->m_Mouse == mouse);

  albaDEL(mouse);
}

//----------------------------------------------------------------------------
void albaViewManagerTest::GetFromListTest()
//----------------------------------------------------------------------------
{
  // insert some views in the manager views list
	albaViewDummy *viewA, *viewB, *viewC;
	albaView *curView;

  viewA = new albaViewDummy("viewA");
  viewB = new albaViewDummy("viewB");
  viewC = new albaViewDummy("viewC");

  m_Manager->ViewInsert(viewA);
  m_Manager->ViewInsert(viewB);
  m_Manager->ViewInsert(viewC);

  curView = m_Manager->GetFromList("viewA");
  CPPUNIT_ASSERT(curView == viewA); 
  curView = m_Manager->GetFromList("viewB");
  CPPUNIT_ASSERT(curView == viewB); 
  curView = m_Manager->GetFromList("viewC");
  CPPUNIT_ASSERT(curView == viewC); 

}
