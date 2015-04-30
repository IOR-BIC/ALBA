/*=========================================================================

 Program: MAF2
 Module: mafUserTest
 Authors: Paolo Quadrani
 
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
#include "mafUserTest.h"
#include "mafCoreTests.h"

#include "mafUser.h"

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
	wxAppConsole::SetInstance(NULL);
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
  mafString usr, pwd, proxyHost, proxyPort;
  int r_me, proxyFlag;
  usr = "name";
  pwd = "pwd";
  proxyHost = "cineca";
  proxyPort = 1111;
  r_me = 0;
  proxyFlag = 0;

  mafUser *user = new mafUser();
  CPPUNIT_ASSERT(user->SetCredentials(usr, pwd, proxyFlag, proxyHost, proxyPort, r_me));
  
  bool name_ok = user->GetName().Equals("name");
  CPPUNIT_ASSERT(name_ok);

  bool pwd_ok = user->GetPwd().Equals("pwd");
  CPPUNIT_ASSERT(pwd_ok);

  bool proxyHost_ok = user->GetProxyHost().Equals("cineca");
  CPPUNIT_ASSERT(proxyHost_ok);

  bool proxyPort_ok = user->GetProxyPort() == 1111;
  CPPUNIT_ASSERT(proxyPort_ok);

  bool proxyFlag_ok = user->GetProxyFlag() == 0;
  CPPUNIT_ASSERT(proxyFlag_ok);

  bool remember = user->GetRememberUserCredentials() == 0;
  CPPUNIT_ASSERT(remember);

  // test storing user information.
  r_me = 1;
  CPPUNIT_ASSERT(user->SetCredentials(usr, pwd, proxyFlag, proxyHost,proxyPort, r_me));
  cppDEL(user);

  user = new mafUser();
  // test again save username and password
  name_ok = user->GetName().Equals("name");
  CPPUNIT_ASSERT(name_ok);
  pwd_ok = user->GetPwd().Equals("pwd");
  CPPUNIT_ASSERT(pwd_ok);
   proxyHost_ok = user->GetProxyHost().Equals("cineca");
  CPPUNIT_ASSERT(proxyHost_ok);
  proxyPort_ok = user->GetProxyPort() == 1111;
  CPPUNIT_ASSERT(proxyPort_ok);
  proxyFlag_ok = user->GetProxyFlag() == 0;
  CPPUNIT_ASSERT(proxyFlag_ok);

  // remove user info file.
  r_me = 0;
  bool cred_ok = user->SetCredentials(usr, pwd, proxyFlag, proxyHost,proxyPort, r_me);
  CPPUNIT_ASSERT(cred_ok);
  cppDEL(user);
}
//----------------------------------------------------------------------------
void mafUserTest::TestCheckUserCredentials()
//----------------------------------------------------------------------------
{
  mafString usr, pwd, proxyHost, proxyPort;
  int r_me, proxyFlag;
  usr = "name";
  pwd = "pwd";
  proxyHost = "cineca";
  proxyPort = 1111;
  r_me = 0;
  proxyFlag = 0;
  bool cred_ok;


  mafUser *user = new mafUser();
  cred_ok = user->SetCredentials(usr, pwd, proxyFlag, proxyHost,proxyPort, r_me);
  CPPUNIT_ASSERT(cred_ok);

  // both username and password are given: basic credential check is ok.
  cred_ok = user->CheckUserCredentials();
  CPPUNIT_ASSERT(cred_ok);

  pwd = "";
  cred_ok = user->SetCredentials(usr, pwd, proxyFlag, proxyHost,proxyPort, r_me);
  CPPUNIT_ASSERT(cred_ok);

  // empty password is admitted (anonymous authentication): basic credential check is ok.
  cred_ok = user->CheckUserCredentials();
  CPPUNIT_ASSERT(cred_ok);

  usr = "";
  pwd = "pwd";

  // empty username is NOT admitted (anonymous authentication): basic credential check is ok.
  cred_ok = user->SetCredentials(usr, pwd, proxyFlag, proxyHost,proxyPort, r_me);
  CPPUNIT_ASSERT(!cred_ok);
  cppDEL(user);
}
