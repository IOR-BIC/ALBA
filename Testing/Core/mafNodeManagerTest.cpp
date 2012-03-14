/*==============================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeManagerTest.cpp,v $
  Language:  C++
  Date:      $Date: 2009-11-17 09:53:55 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Alberto Losi
================================================================================
  Copyright (c) 2007 Cineca, UK (www.cineca.it)
  All rights reserved.
===============================================================================*/

#include "mafDefines.h" 
//------------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//------------------------------------------------------------------------------

#include "mafNodeManager.h"
#include "mafNodeManagerTest.h"
#include "mafVMESurfaceParametric.h"

//------------------------------------------------------------------------------
void mafNodeManagerTest::setUp()
//------------------------------------------------------------------------------
{
  m_Node = NULL;
  m_EventID = -1;

  m_Config = new wxFileConfig("mafNodeManagerTest");
  wxConfigBase::Set(m_Config);
}
//------------------------------------------------------------------------------
void mafNodeManagerTest::tearDown()
//------------------------------------------------------------------------------
{
  //cppDEL( m_Config ); Already deleted by mafNodeManager
}
//------------------------------------------------------------------------------
void mafNodeManagerTest::OnEvent(mafEventBase *maf_event)
//------------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    m_EventID = e->GetId();
    m_Node = e->GetVme();
  }
}
//------------------------------------------------------------------------------
void mafNodeManagerTest::MSFNewTest()
//------------------------------------------------------------------------------
{
  mafNodeManager manager;
  manager.SetListener(this);
  manager.MSFNew(true); // notify root creation
  
  CPPUNIT_ASSERT(m_EventID == VME_SELECTED);
  CPPUNIT_ASSERT(manager.GetRoot() == m_Node);
}
//------------------------------------------------------------------------------
void mafNodeManagerTest::VmeAddRemoveTest()
//------------------------------------------------------------------------------
{
  mafNodeManager manager;
  manager.SetListener(this);
  manager.MSFNew(false); // don't notify root creation

  mafVMESurfaceParametric *n; // Create a vme
  mafNEW(n);
  n->SetGeometryType(0);

  manager.VmeAdd(n);
  CPPUNIT_ASSERT(m_EventID == VME_ADDED);

  CPPUNIT_ASSERT(m_Node == n);

  CPPUNIT_ASSERT(n->GetParent() == manager.GetRoot());

  manager.VmeRemove(n); // This should destroy the node
  
  // but
  if(n != NULL)
  {
    mafDEL(n);
  }

  CPPUNIT_ASSERT(m_EventID == VME_REMOVING);
}
//------------------------------------------------------------------------------
void mafNodeManagerTest::TimeGetBoundsTest()
//------------------------------------------------------------------------------
{
  mafNodeManager manager;
  double min, max;
  manager.TimeGetBounds(&min, &max);

  CPPUNIT_ASSERT(min == 0 && max == 0);
}
//------------------------------------------------------------------------------
void mafNodeManagerTest::SetGetFileNameTest()
//------------------------------------------------------------------------------
{
  mafNodeManager manager;

  manager.SetFileName(wxString("test.msf"));
  wxString fileName = manager.GetFileName();
  CPPUNIT_ASSERT(fileName.Cmp("test.msf") == 0);
}
//------------------------------------------------------------------------------
void mafNodeManagerTest::GetRootTest()
//------------------------------------------------------------------------------
{
  mafNodeManager manager;
  manager.SetListener(this);
  manager.MSFNew(false); // don't notify root creation
  CPPUNIT_ASSERT(manager.GetRoot()->IsA("mafVMERoot"));
}
