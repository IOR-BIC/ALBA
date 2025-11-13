/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeFactoryTest
 Authors: Daniele Giunchi
 
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
#include "albaPipeFactoryTest.h"
#include "albaPipe.h"
#include "albaPipeFactory.h"
#include <iostream>

//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class albaPipeCustom : public albaPipe
//-------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaPipeCustom,albaPipe);

protected:
  albaPipeCustom();
  virtual ~albaPipeCustom();

private:
  albaPipeCustom(const albaPipeCustom&); // Not implemented
  void operator=(const albaPipeCustom&); // Not implemented
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeCustom);

//-------------------------------------------------------------------------
albaPipeCustom::albaPipeCustom()
{
  
}
//-------------------------------------------------------------------------
albaPipeCustom::~albaPipeCustom()
{
  
}

//------------------------------------------------------------------------------
// a custom plugger for the new factory
template <class T>
class albaPlugTestPipe
{
  public:
  albaPlugTestPipe(const char *description);
  
};

//------------------------------------------------------------------------------
template <class T>
albaPlugTestPipe<T>::albaPlugTestPipe(const char *description)
{ 
  albaPipeFactory *factory=albaPipeFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewPipe(T::GetStaticTypeName(), description, T::NewObject);

  }
}

//-------------------------------------------------------------------------
void albaPipeFactoryTest::TestGetInstance_Initialize_CreatePipeInstance_RegisterNewPipe_GetPipeNames()
{
  // a couple of factories
  albaPipeFactory *pipe_factory = albaPipeFactory::GetInstance();
  CPPUNIT_ASSERT(pipe_factory!=NULL);

  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(albaPipeFactory::Initialize()==ALBA_OK);

  std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();

	bool found=false;
	for ( std::list<albaObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
	{
		if((*i)==pipe_factory)
			found = true;
	}
	CPPUNIT_ASSERT(found);

  albaPlugTestPipe<albaPipeCustom>("a custom pipe"); // plug a pipe in the main node factory

	
  albaPipe *pipe0 = albaPipeFactory::CreatePipeInstance("albaPipeNotExisting");
  albaPipe *pipe1 = albaPipeFactory::CreatePipeInstance("albaPipeCustom");

  CPPUNIT_ASSERT(pipe0 == NULL);
  CPPUNIT_ASSERT(pipe1!=NULL);

  CPPUNIT_ASSERT(pipe1->IsALBAType(albaPipeCustom));

  // test factory contents
	//poor proof to register again an already registered pipe
	albaPlugTestPipe<albaPipeCustom>("a custom pipe"); // plug a pipe in the main node factory

  const std::vector<std::string> pipes = pipe_factory->GetPipeNames();
  int s = pipes.size();
  CPPUNIT_ASSERT(s == 2); // one pipe is plugged by default (albaPipeBox), 
                        
  found=false;
  for (int i=0;i<pipes.size();i++)
  {
		std::string test;
		test = pipes[i];
    if (pipes[i]=="albaPipeCustom")
      found=true;
  }
  
	CPPUNIT_ASSERT(found);
  // cleanup factory products
  pipe1->Delete();
}
//-------------------------------------------------------------------------
void albaPipeFactoryTest::TestGetDescription()
{
	albaPipeFactory *pipe_factory = albaPipeFactory::GetInstance();
	CPPUNIT_ASSERT(pipe_factory!=NULL);
	std::string value1 = pipe_factory->GetDescription();
	std::string value2 = std::string("Factory for ALBA Pipes");
	CPPUNIT_ASSERT( value1 == value2 );
}
  
