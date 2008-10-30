/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipeFactoryVMETest.cpp,v $
Language:  C++
Date:      $Date: 2008-10-30 16:44:38 $
Version:   $Revision: 1.1.2.1 $
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
#include "medPipeFactoryVMETest.h"

#include "mafPipe.h"
#include "medPipeFactoryVME.h"

#include "mafVersion.h"
#include <iostream>

//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class medPipeCustom : public mafPipe
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(medPipeCustom,mafPipe);

protected:
  medPipeCustom();
  virtual ~medPipeCustom();

private:
  medPipeCustom(const medPipeCustom&); // Not implemented
  void operator=(const medPipeCustom&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(medPipeCustom);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medPipeCustom::medPipeCustom()
//-------------------------------------------------------------------------
{
  
}
//-------------------------------------------------------------------------
medPipeCustom::~medPipeCustom()
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
  medPipeFactoryVME *factory=medPipeFactoryVME::GetInstance();
  if (factory)
  {
    factory->RegisterNewPipe(T::GetStaticTypeName(), description, T::NewObject);

  }
}

//-------------------------------------------------------------------------
void medPipeFactoryVMETest::TestGetInstance_Initialize_CreatePipeInstance_RegisterNewPipe_GetPipeNames()
//-------------------------------------------------------------------------
{
  // a couple of factories
  medPipeFactoryVME *pipe_factory = medPipeFactoryVME::GetInstance();
  CPPUNIT_ASSERT(pipe_factory!=NULL);

  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(medPipeFactoryVME::Initialize()==MAF_OK);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);

  mafPlugTestPipe<medPipeCustom>("a custom pipe"); // plug a pipe in the main node factory

	
  mafPipe *pipe0 = medPipeFactoryVME::CreatePipeInstance("mafPipeNotExisting");
  mafPipe *pipe1 = medPipeFactoryVME::CreatePipeInstance("medPipeCustom");

  CPPUNIT_ASSERT(pipe0 == NULL);
  CPPUNIT_ASSERT(pipe1!=NULL);

  CPPUNIT_ASSERT(pipe1->IsMAFType(medPipeCustom));

  // test factory contents
	//poor proof to register again an already registered pipe
	mafPlugTestPipe<medPipeCustom>("a custom pipe"); // plug a pipe in the main node factory

  const std::vector<std::string> pipes = pipe_factory->GetPipeNames();
  int s = pipes.size();
  CPPUNIT_ASSERT(s != 0);//must be a positive number (it sums all the maf pipes and the registered pipes in medical)
                        
  bool found=false;
  for (int i=0;i<pipes.size();i++)
  {
		std::string test;
		test = pipes[i];
    if (pipes[i]=="medPipeCustom")
      found=true;
  }
  
	CPPUNIT_ASSERT(found);
  // cleanup factory products
  pipe1->Delete();
}
//-------------------------------------------------------------------------
void medPipeFactoryVMETest::TestGetDescription()
//-------------------------------------------------------------------------
{
	medPipeFactoryVME *pipe_factory = medPipeFactoryVME::GetInstance();
	CPPUNIT_ASSERT(pipe_factory!=NULL);
	std::string value1 = pipe_factory->GetDescription();
	std::string value2 = std::string("Factory for medical Pipes of medVME library");
	CPPUNIT_ASSERT( value1 == value2 );
}
  
