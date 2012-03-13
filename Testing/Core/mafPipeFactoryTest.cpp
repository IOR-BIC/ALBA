/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeFactoryTest.cpp,v $
Language:  C++
Date:      $Date: 2008-05-27 11:16:30 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
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
#include "mafPipeFactoryTest.h"

#include "mafPipe.h"
#include "mafPipeFactory.h"

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
  mafPipeFactory *factory=mafPipeFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewPipe(T::GetStaticTypeName(), description, T::NewObject);

  }
}

//-------------------------------------------------------------------------
void mafPipeFactoryTest::TestGetInstance_Initialize_CreatePipeInstance_RegisterNewPipe_GetPipeNames()
//-------------------------------------------------------------------------
{
  // a couple of factories
  mafPipeFactory *pipe_factory = mafPipeFactory::GetInstance();
  CPPUNIT_ASSERT(pipe_factory!=NULL);

  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(mafPipeFactory::Initialize()==MAF_OK);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);

  mafPlugTestPipe<mafPipeCustom>("a custom pipe"); // plug a pipe in the main node factory

	
  mafPipe *pipe0 = mafPipeFactory::CreatePipeInstance("mafPipeNotExisting");
  mafPipe *pipe1 = mafPipeFactory::CreatePipeInstance("mafPipeCustom");

  CPPUNIT_ASSERT(pipe0 == NULL);
  CPPUNIT_ASSERT(pipe1!=NULL);

  CPPUNIT_ASSERT(pipe1->IsMAFType(mafPipeCustom));

  // test factory contents
	//poor proof to register again an already registered pipe
	mafPlugTestPipe<mafPipeCustom>("a custom pipe"); // plug a pipe in the main node factory

  const std::vector<std::string> pipes = pipe_factory->GetPipeNames();
  int s = pipes.size();
  CPPUNIT_ASSERT(s == 2); // one pipe is plugged by default (mafPipeBox), 
                        
  bool found=false;
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
void mafPipeFactoryTest::TestGetMAFSourceVersion()
//-------------------------------------------------------------------------
{
	mafPipeFactory *pipe_factory = mafPipeFactory::GetInstance();
	CPPUNIT_ASSERT(pipe_factory!=NULL);
	std::string value1 = pipe_factory->GetMAFSourceVersion();
	std::string value2 = std::string(MAF_SOURCE_VERSION);
	CPPUNIT_ASSERT( value1 == value2 );

}
//-------------------------------------------------------------------------
void mafPipeFactoryTest::TestGetDescription()
//-------------------------------------------------------------------------
{
	mafPipeFactory *pipe_factory = mafPipeFactory::GetInstance();
	CPPUNIT_ASSERT(pipe_factory!=NULL);
	std::string value1 = pipe_factory->GetDescription();
	std::string value2 = std::string("Factory for MAF Pipes");
	CPPUNIT_ASSERT( value1 == value2 );
}
  
