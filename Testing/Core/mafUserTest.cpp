/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafUserTest.cpp,v $
Language:  C++
Date:      $Date: 2008-01-09 13:12:05 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
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
#include "mafUserTest.h"

#include "mafUser.h"

// Helper class used to build a fake application needed by the wxConfig
// to store settings into the registry with the same name of the application
// ===============================================================================
class TestApp : public wxApp
  // ===============================================================================
{
public:
  bool OnInit();
  int  OnExit();
};
DECLARE_APP(TestApp)

IMPLEMENT_APP(TestApp)
//--------------------------------------------------------------------------------
bool TestApp::OnInit()
//--------------------------------------------------------------------------------
{
  return TRUE;
}
//--------------------------------------------------------------------------------
int TestApp::OnExit()
//--------------------------------------------------------------------------------
{
  wxApp::CleanUp();
  return 0;
}
// ===============================================================================

//----------------------------------------------------------------------------
void mafUserTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafUserTest::setUp()
//----------------------------------------------------------------------------
{
  m_App = new TestApp();  // Instantiate the application class
  m_App->argc = 0;        // set the number of input argument to 0
  m_App->argv = NULL;     // set to NULL the input argument's parameters
  wxTheApp->SetAppName("mafUserTest"); // Set the name for the application
}
//----------------------------------------------------------------------------
void mafUserTest::tearDown()
//----------------------------------------------------------------------------
{
  cppDEL(m_App);  // Destroy the application
}
//----------------------------------------------------------------------------
void mafUserTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafUser user;
}
//----------------------------------------------------------------------------
void mafUserTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafUser *user = new mafUser();
  cppDEL(user);
}
//----------------------------------------------------------------------------
void mafUserTest::TestSetCredentials()
//----------------------------------------------------------------------------
{
  mafString usr, pwd;
  int r_me;
  usr = "name";
  pwd = "pwd";
  r_me = 0;

  mafUser *user = new mafUser();
  CPPUNIT_ASSERT(user->SetCredentials(usr, pwd, r_me));
  
  bool name_ok = user->GetName().Equals("name");
  CPPUNIT_ASSERT(name_ok);

  bool pwd_ok = user->GetPwd().Equals("pwd");
  CPPUNIT_ASSERT(pwd_ok);

  bool remember = user->GetRememberUserCredentials() == 0;
  CPPUNIT_ASSERT(remember);

  // test storing user information.
  r_me = 1;
  CPPUNIT_ASSERT(user->SetCredentials(usr, pwd, r_me));
  cppDEL(user);

  user = new mafUser();
  // test again save username and password
  name_ok = user->GetName().Equals("name");
  CPPUNIT_ASSERT(name_ok);
  pwd_ok = user->GetPwd().Equals("pwd");
  CPPUNIT_ASSERT(pwd_ok);

  // remove user info file.
  r_me = 0;
  bool cred_ok = user->SetCredentials(usr, pwd, r_me);
  CPPUNIT_ASSERT(cred_ok);
  cppDEL(user);
}
//----------------------------------------------------------------------------
void mafUserTest::TestCheckUserCredentials()
//----------------------------------------------------------------------------
{
  mafString usr, pwd;
  int r_me;
  bool cred_ok;
  usr = "name";
  pwd = "pwd";
  r_me = 0;

  mafUser *user = new mafUser();
  cred_ok = user->SetCredentials(usr, pwd, r_me);
  CPPUNIT_ASSERT(cred_ok);

  // both username and password are given: basic credential check is ok.
  cred_ok = user->CheckUserCredentials();
  CPPUNIT_ASSERT(cred_ok);

  pwd = "";
  cred_ok = user->SetCredentials(usr, pwd, r_me);
  CPPUNIT_ASSERT(cred_ok);

  // empty password is admitted (anonymous authentication): basic credential check is ok.
  cred_ok = user->CheckUserCredentials();
  CPPUNIT_ASSERT(cred_ok);

  usr = "";
  pwd = "pwd";

  // empty username is NOT admitted (anonymous authentication): basic credential check is ok.
  cred_ok = user->SetCredentials(usr, pwd, r_me);
  CPPUNIT_ASSERT(!cred_ok);
  cppDEL(user);
}
