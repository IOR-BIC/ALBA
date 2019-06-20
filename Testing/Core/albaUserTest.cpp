/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaUserTest
 Authors: Paolo Quadrani
 
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
#include "albaUserTest.h"
#include "albaCoreTests.h"

#include "albaUser.h"

//----------------------------------------------------------------------------
void albaUserTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaUserTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaUser user;
}
//----------------------------------------------------------------------------
void albaUserTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaUser *user = new albaUser();
  cppDEL(user);
}
//----------------------------------------------------------------------------
void albaUserTest::TestSetCredentials()
//----------------------------------------------------------------------------
{
  albaString usr, pwd, proxyHost, proxyPort;
  int r_me, proxyFlag;
  usr = "name";
  pwd = "pwd";
  proxyHost = "cineca";
  proxyPort = 1111;
  r_me = 0;
  proxyFlag = 0;

  albaUser *user = new albaUser();
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

  user = new albaUser();
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
void albaUserTest::TestCheckUserCredentials()
//----------------------------------------------------------------------------
{
  albaString usr, pwd, proxyHost, proxyPort;
  int r_me, proxyFlag;
  usr = "name";
  pwd = "pwd";
  proxyHost = "cineca";
  proxyPort = 1111;
  r_me = 0;
  proxyFlag = 0;
  bool cred_ok;


  albaUser *user = new albaUser();
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
