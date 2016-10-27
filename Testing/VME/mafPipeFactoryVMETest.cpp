/*=========================================================================

 Program: MAF2
 Module: mafPipeFactoryVMETest
 Authors: Daniele Giunchi
 
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
#include "mafPipeFactoryVMETest.h"

#include "mafPipe.h"
#include "mafPipeFactoryVME.h"

#include "mafVersion.h"
#include <iostream>

//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class mafPipeCustom : public mafPipe
	//-------------------------------------------------------------------------
{
public:
	mafTypeMacro(mafPipeCustom,mafPipe);

protected:
	mafPipeCustom();
	virtual ~mafPipeCustom();

private:
	mafPipeCustom(const mafPipeCustom&); // Not implemented
	void operator=(const mafPipeCustom&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeCustom);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafPipeCustom::mafPipeCustom()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
mafPipeCustom::~mafPipeCustom()
//-------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
// a custom plugger for the new factory
template <class T>
class mafPlugTestPipe
	//------------------------------------------------------------------------------
{
public:
	mafPlugTestPipe(const char *description);

};

//------------------------------------------------------------------------------
template <class T>
mafPlugTestPipe<T>::mafPlugTestPipe(const char *description)
	//------------------------------------------------------------------------------
{ 
	mafPipeFactoryVME *factory=mafPipeFactoryVME::GetInstance();
	if (factory)
	{
		factory->RegisterNewPipe(T::GetStaticTypeName(), description, T::NewObject);

	}
}

//-------------------------------------------------------------------------
void mafPipeFactoryVMETest::TestGetInstance_Initialize()
//-------------------------------------------------------------------------
{
  // a couple of factories
  mafPipeFactoryVME *pipe_factory = mafPipeFactoryVME::GetInstance();
  CPPUNIT_ASSERT(pipe_factory!=NULL);

  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(mafPipeFactoryVME::Initialize()==MAF_OK);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  
	bool found=false;
	for ( std::list<mafObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
	{
		if((*i)==pipe_factory)
			found = true;
	}
	CPPUNIT_ASSERT(found);

  mafPlugTestPipe<mafPipeCustom>("a custom pipe"); // plug a pipe in the main node factory

  mafPipe *pipe0 = mafPipeFactoryVME::CreatePipeInstance("mafPipeNotExisting");
  mafPipe *pipe1 = mafPipeFactoryVME::CreatePipeInstance("mafPipeCustom");

  CPPUNIT_ASSERT(pipe0 == NULL);
  CPPUNIT_ASSERT(pipe1!=NULL);

  CPPUNIT_ASSERT(pipe1->IsMAFType(mafPipeCustom));

  // test factory contents
  //poor proof to register again an already registered pipe
  mafPlugTestPipe<mafPipeCustom>("a custom pipe"); // plug a pipe in the main node factory

 
  const std::vector<std::string> pipes = pipe_factory->GetPipeNames();
  int s = pipes.size();
  CPPUNIT_ASSERT(s != 0);//must be a positive number (it sums all the maf pipes and the registered pipes in medical)
	  

	found=false;
	for (int i=0;i<pipes.size();i++)
	{
		std::string test;
		test = pipes[i];
		if (pipes[i]=="mafPipeCustom")
			found=true;
	}

	CPPUNIT_ASSERT(found);
	// cleanup factory products
	pipe1->Delete();

}
//-------------------------------------------------------------------------
void mafPipeFactoryVMETest::TestGetMAFSourceVersion()
//-------------------------------------------------------------------------
{
	mafPipeFactoryVME *pipe_factory = mafPipeFactoryVME::GetInstance();
	CPPUNIT_ASSERT(pipe_factory!=NULL);
	std::string value1 = pipe_factory->GetMAFSourceVersion();
	std::string value2 = std::string(MAF_SOURCE_VERSION);
	CPPUNIT_ASSERT( value1 == value2 );

}
//-------------------------------------------------------------------------
void mafPipeFactoryVMETest::TestGetDescription()
//-------------------------------------------------------------------------
{
	mafPipeFactoryVME *pipe_factory = mafPipeFactoryVME::GetInstance();
	CPPUNIT_ASSERT(pipe_factory!=NULL);
	std::string value1 = pipe_factory->GetDescription();
	std::string value2 = std::string("Factory for MAF Pipes of VME library");
	CPPUNIT_ASSERT( value1 == value2 );
}
  
