/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObjectFactoryTest
 Authors: Stefano Perticoni, Marco Petrone, Matteo Giacomoni
 
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
#include "albaBaseTests.h"
#include "albaObjectFactoryTest.h"

#include "albaObject.h"
#include "albaObjectFactory.h"
#include "albaVersion.h"
#include "mmaVolumeMaterial.h"
#include <iostream>

/** a factory. */
class albaDummyFactory: public albaObjectFactory
{
public:
	albaTypeMacro(albaDummyFactory,albaObjectFactory);
	albaDummyFactory() {}
	~albaDummyFactory() {}

	virtual const char* GetALBASourceVersion() const {return ALBA_SOURCE_VERSION;};
	virtual const char* GetDescription() const {return "Factory Dummy";};

	void Print(std::ostream &out) {out<<"Dummy";}
};

albaCxxTypeMacro(albaDummyFactory);


/** a albaObject to be placed in the factory. */
class albaDummyObject: public albaObject
{
public:
  albaTypeMacro(albaDummyObject,albaObject);
  albaDummyObject() {}
  void Print(std::ostream &out) {out<<"Dummy";}
};

albaCxxTypeMacro(albaDummyObject);

//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestStaticAllocation()
//------------------------------------------------------------------------------
{
	albaDummyFactory factory;
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestDynamicAllocation()
//------------------------------------------------------------------------------
{
	albaDummyFactory *factory=new albaDummyFactory();
	delete factory;
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestCreateInstance()
//------------------------------------------------------------------------------
{
  // a couple of factories
  albaDummyFactory *first_factory = albaDummyFactory::New();
  albaDummyFactory *second_factory = albaDummyFactory::New();

  CPPUNIT_ASSERT(first_factory!=NULL);
  CPPUNIT_ASSERT(second_factory!=NULL);

  // register factories
  albaObjectFactory::RegisterFactory(first_factory);
  albaObjectFactory::RegisterFactory(second_factory);

  // register objects to the factories
  first_factory->RegisterOverride(albaFooObject::GetStaticTypeName(),albaFooObject::GetStaticTypeName(),"foo class",true,albaFooObject::NewObject);
  second_factory->RegisterOverride(albaDummyObject::GetStaticTypeName(),albaDummyObject::GetStaticTypeName(),"foo class",true,albaDummyObject::NewObject);

  // create objects from the factory
  albaFooObject *foo=albaFooObject::SafeDownCast(albaObjectFactory::CreateInstance("albaFooObject"));
  albaDummyObject *dummy=albaDummyObject::SafeDownCast(albaObjectFactory::CreateInstance("albaDummyObject"));

  CPPUNIT_ASSERT(foo!=NULL);
  CPPUNIT_ASSERT(dummy!=NULL);
  
  // test newly created objects types
  CPPUNIT_ASSERT(foo->IsA("albaFooObject"));
  CPPUNIT_ASSERT(foo->IsA(typeid(albaObject)));
  CPPUNIT_ASSERT(foo->IsA("albaObject"));
  CPPUNIT_ASSERT(foo->IsALBAType(albaObject));
  CPPUNIT_ASSERT(!foo->IsA(dummy->GetTypeId()));
  CPPUNIT_ASSERT(!foo->IsA(dummy->GetTypeName()));
  CPPUNIT_ASSERT(!dummy->IsALBAType(albaFooObject)); // test through static type id (equivalent to typeid())
  CPPUNIT_ASSERT(!dummy->IsA(albaFooObject::GetStaticTypeName())); // test through static string type 
  CPPUNIT_ASSERT(dummy->GetStaticTypeId()==dummy->GetTypeId()); 
  CPPUNIT_ASSERT(dummy->GetStaticTypeId()==dummy->GetTypeId());

  // cleanup factory products
  foo->Delete();
  dummy->Delete();

	albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestRegisterFactory()
//------------------------------------------------------------------------------
{
	// a couple of factories
	albaDummyFactory *first_factory = albaDummyFactory::New();
	albaDummyFactory *second_factory = albaDummyFactory::New();

	CPPUNIT_ASSERT(first_factory!=NULL);
	CPPUNIT_ASSERT(second_factory!=NULL);

	// register factories
	albaObjectFactory::RegisterFactory(first_factory);
	albaObjectFactory::RegisterFactory(second_factory);

	std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();
	CPPUNIT_ASSERT(list.size()==2);

	albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestUnRegisterAllFactories()
//------------------------------------------------------------------------------
{
	// a couple of factories
	albaDummyFactory *first_factory = albaDummyFactory::New();
	albaDummyFactory *second_factory = albaDummyFactory::New();

	// register factories
	albaObjectFactory::RegisterFactory(first_factory);
	albaObjectFactory::RegisterFactory(second_factory);

	albaObjectFactory::UnRegisterAllFactories();

	albaDummyFactory *third_factory = albaDummyFactory::New();

	albaObjectFactory::RegisterFactory(third_factory);

	std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();
	CPPUNIT_ASSERT(list.size()==1);

	for ( std::list<albaObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
	{
		CPPUNIT_ASSERT((*i)==third_factory);
	}

	albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestGetRegisteredFactories()
//------------------------------------------------------------------------------
{
	// a couple of factories
	albaDummyFactory *first_factory = albaDummyFactory::New();
	albaDummyFactory *second_factory = albaDummyFactory::New();

	// register factories
	albaObjectFactory::RegisterFactory(first_factory);
	albaObjectFactory::RegisterFactory(second_factory);

	std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();
	CPPUNIT_ASSERT(list.size()==2);

	int j=0;
	for (std::list<albaObjectFactory*>::iterator i = list.begin();i != list.end(); ++i,j++ )
	{
		switch(j)
		{
		case 0:
			CPPUNIT_ASSERT((*i)==first_factory);
			break;
		case 1:
			CPPUNIT_ASSERT((*i)==second_factory);
			break;
		}
	}

	albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
albaObject* FunctionDummy(){return new albaDummyObject();};
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestRegisterOverride()
//------------------------------------------------------------------------------
{
  albaDummyFactory *factory=albaDummyFactory::New();
  albaObjectFactory::RegisterFactory(factory);
  factory->RegisterOverride("ClassOne","albaDummyObject","ClassTest",true,albaFooObject::NewObject);
  std::list<std::string> listClassOverrideWithName;
  listClassOverrideWithName=factory->GetClassOverrideWithNames();
  for ( std::list<std::string>::iterator i = listClassOverrideWithName.begin(); i != listClassOverrideWithName.end(); ++i )
  {
    std::string st=(*i);
    CPPUNIT_ASSERT(st.compare("albaDummyObject")==0);
  }

  std::list<std::string> listClassOverrideName;
  listClassOverrideName=factory->GetClassOverrideNames();
  for ( std::list<std::string>::iterator i = listClassOverrideName.begin(); i != listClassOverrideName.end(); ++i )
  {
    std::string st=(*i);
    CPPUNIT_ASSERT(st.compare("ClassOne")==0);
  }

  std::list<std::string> listClassOverrideDescription;
  listClassOverrideDescription=factory->GetClassOverrideTypeNames();
  for ( std::list<std::string>::iterator i = listClassOverrideDescription.begin(); i != listClassOverrideDescription.end(); ++i )
  {
    std::string st=(*i);
    CPPUNIT_ASSERT(st.compare("ClassTest")==0);
  }

  albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestRegisterNewObject()
//------------------------------------------------------------------------------
{
  albaDummyFactory *factory=albaDummyFactory::New();
  albaObjectFactory::RegisterFactory(factory);
  factory->RegisterNewObject(albaDummyObject::GetStaticTypeName(),"TestObject",albaDummyObject::NewObject);

  std::list<std::string> listClassOverrideName;
  listClassOverrideName=factory->GetClassOverrideNames();
  for ( std::list<std::string>::iterator i = listClassOverrideName.begin(); i != listClassOverrideName.end(); ++i )
  {
    std::string st=(*i);
    CPPUNIT_ASSERT(st.compare("albaDummyObject")==0);
  }

  albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestDisable()
//------------------------------------------------------------------------------
{
  albaDummyFactory *factory=albaDummyFactory::New();
  albaObjectFactory::RegisterFactory(factory);
  factory->RegisterNewObject(albaDummyObject::GetStaticTypeName(),"TestObject",albaDummyObject::NewObject);
  factory->RegisterNewObject(albaFooObject::GetStaticTypeName(),"TestObject",albaFooObject::NewObject);

  factory->Disable(albaDummyObject::GetStaticTypeName());
  factory->Disable(albaFooObject::GetStaticTypeName());

  std::list<bool> listFlag = factory->GetEnableFlags();
  for ( std::list<bool>::iterator i = listFlag.begin(); i != listFlag.end(); ++i )
  {
    bool val=(*i);
    CPPUNIT_ASSERT(val==false);
  }

  albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestSetEnableFlag()
//------------------------------------------------------------------------------
{
  albaDummyFactory *factory=albaDummyFactory::New();
  albaObjectFactory::RegisterFactory(factory);
  factory->RegisterNewObject(albaDummyObject::GetStaticTypeName(),"TestObject",albaDummyObject::NewObject);

  factory->SetEnableFlag(false,albaDummyObject::GetStaticTypeName(),albaDummyObject::GetStaticTypeName());
  bool flag=factory->GetEnableFlag(albaDummyObject::GetStaticTypeName(),albaDummyObject::GetStaticTypeName());

  CPPUNIT_ASSERT(flag==false);

  factory->SetEnableFlag(true,albaDummyObject::GetStaticTypeName(),albaDummyObject::GetStaticTypeName());
  flag=factory->GetEnableFlag(albaDummyObject::GetStaticTypeName(),albaDummyObject::GetStaticTypeName());

  CPPUNIT_ASSERT(flag==true);

  albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestUnRegisterFactory()
//------------------------------------------------------------------------------
{
  // a couple of factories
  albaDummyFactory *first_factory = albaDummyFactory::New();
  albaDummyFactory *second_factory = albaDummyFactory::New();

  // register factories
  albaObjectFactory::RegisterFactory(first_factory);
  albaObjectFactory::RegisterFactory(second_factory);

  albaObjectFactory::UnRegisterFactory(first_factory);

  std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);

  for ( std::list<albaObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
  {
    CPPUNIT_ASSERT((*i)==second_factory);
  }

  albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestGetArgs()
//------------------------------------------------------------------------------
{
  albaDummyFactory *factory=albaDummyFactory::New();
  albaObjectFactory::RegisterFactory(factory);
  factory->RegisterNewObject(albaDummyObject::GetStaticTypeName(),"TestObject",albaDummyObject::NewObject);
  
  albaReferenceCounted *args=factory->GetArgs("Test");

  CPPUNIT_ASSERT(args==NULL);

  albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestReHash()
//------------------------------------------------------------------------------
{
  albaDummyFactory *first_factory=albaDummyFactory::New();
  albaObjectFactory::RegisterFactory(first_factory);
  
  albaObjectFactory::ReHash();

  albaDummyFactory *second_factory = albaDummyFactory::New();
  albaObjectFactory::RegisterFactory(second_factory);

  std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();

  CPPUNIT_ASSERT(list.size()==1);

  for ( std::list<albaObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
  {
    CPPUNIT_ASSERT((*i)==second_factory);
  }

  albaObjectFactory::UnRegisterAllFactories();
}
//------------------------------------------------------------------------------
void albaObjectFactoryTest::TestCreateAllInstance()
//------------------------------------------------------------------------------
{
  albaDummyFactory *factory=albaDummyFactory::New();
  albaObjectFactory::RegisterFactory(factory);
  factory->RegisterNewObject(albaDummyObject::GetStaticTypeName(),"TestObject",albaDummyObject::NewObject);

  std::list<albaObject*>object_list=factory->CreateAllInstance(albaDummyObject::GetStaticTypeName());

  for ( std::list<albaObject*>::iterator i = object_list.begin();i != object_list.end(); ++i )
  {
    CPPUNIT_ASSERT(albaDummyObject::SafeDownCast(*i));
  }

  for ( std::list<albaObject*>::iterator i = object_list.begin();i != object_list.end(); ++i )
  {
    albaDummyObject::SafeDownCast(*i)->Delete();
  }

  albaObjectFactory::UnRegisterAllFactories();
}