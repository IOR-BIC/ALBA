/*=========================================================================

 Program: MAF2
 Module: mafCoreFactoryTest
 Authors: Stefano Perticoni
 
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
#include "mafCoreFactoryTest.h"

#include "mafObject.h"
#include "mafCoreFactory.h"
#include "mafVersion.h"
#include "mmaVolumeMaterial.h"
#include <iostream>

/** a factory. */
class mafDummyFactory: public mafCoreFactory
{
public:
	mafTypeMacro(mafDummyFactory,mafCoreFactory);
	mafDummyFactory() {}
	~mafDummyFactory() {}

	virtual const char* GetMAFSourceVersion() const {return MAF_SOURCE_VERSION;};
	virtual const char* GetDescription() const {return "Factory Dummy";};

	void Print(std::ostream &out) {out<<"Dummy";}
};

mafCxxTypeMacro(mafDummyFactory);

/** a mafObject to be placed in the factory. */
class mafFooObject: public mafObject
{
public:
  mafTypeMacro(mafFooObject,mafObject);
  mafFooObject() {}
  void Print(std::ostream &out) {out<<"Foo";}
};

mafCxxTypeMacro(mafFooObject);

/** a mafObject to be placed in the factory. */
class mafDummyObject: public mafObject
{
public:
  mafTypeMacro(mafDummyObject,mafObject);
  mafDummyObject() {}
  void Print(std::ostream &out) {out<<"Dummy";}
};

mafCxxTypeMacro(mafDummyObject);

//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestStaticAllocation()
//------------------------------------------------------------------------------
{
	mafDummyFactory factory;
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestDynamicAllocation()
//------------------------------------------------------------------------------
{
	mafDummyFactory *factory=new mafDummyFactory();
	delete factory;
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestCreateInstance()
//------------------------------------------------------------------------------
{
  // a couple of factories
  mafDummyFactory *first_factory = mafDummyFactory::New();
  mafDummyFactory *second_factory = mafDummyFactory::New();

  CPPUNIT_ASSERT(first_factory!=NULL);
  CPPUNIT_ASSERT(second_factory!=NULL);

  // register factories
  mafCoreFactory::RegisterFactory(first_factory);
  mafCoreFactory::RegisterFactory(second_factory);

  // register objects to the factories
  first_factory->RegisterOverride(mafFooObject::GetStaticTypeName(),mafFooObject::GetStaticTypeName(),"foo class",true,mafFooObject::NewObject);
  second_factory->RegisterOverride(mafDummyObject::GetStaticTypeName(),mafDummyObject::GetStaticTypeName(),"foo class",true,mafDummyObject::NewObject);

  // create objects from the factory
  mafFooObject *foo=mafFooObject::SafeDownCast(mafCoreFactory::CreateInstance("mafFooObject"));
  mafDummyObject *dummy=mafDummyObject::SafeDownCast(mafCoreFactory::CreateInstance("mafDummyObject"));

  CPPUNIT_ASSERT(foo!=NULL);
  CPPUNIT_ASSERT(dummy!=NULL);
  
  // test newly created objects types
  CPPUNIT_ASSERT(foo->IsA("mafFooObject"));
  CPPUNIT_ASSERT(foo->IsA(typeid(mafObject)));
  CPPUNIT_ASSERT(foo->IsA("mafObject"));
  CPPUNIT_ASSERT(foo->IsMAFType(mafObject));
  CPPUNIT_ASSERT(!foo->IsA(dummy->GetTypeId()));
  CPPUNIT_ASSERT(!foo->IsA(dummy->GetTypeName()));
  CPPUNIT_ASSERT(!dummy->IsMAFType(mafFooObject)); // test through static type id (equivalent to typeid())
  CPPUNIT_ASSERT(!dummy->IsA(mafFooObject::GetStaticTypeName())); // test through static string type 
  CPPUNIT_ASSERT(dummy->GetStaticTypeId()==dummy->GetTypeId()); 
  CPPUNIT_ASSERT(dummy->GetStaticTypeId()==dummy->GetTypeId());

  // cleanup factory products
  foo->Delete();
  dummy->Delete();
}

//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestGetSourceVersion()
//------------------------------------------------------------------------------
{
  mafDummyFactory *factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(factory);
  factory->RegisterNewObject(mafDummyObject::GetStaticTypeName(),"TestObject",mafDummyObject::NewObject);

  mafReferenceCounted *args=factory->GetArgs("Test");

  mafString version = factory->GetMAFSourceVersion();
  version.Erase(15);

  CPPUNIT_ASSERT(mafString::Compare(version,"maf version 2.2") == 0);
}


//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestGetDescription()
//------------------------------------------------------------------------------
{
  mafDummyFactory *factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(factory);
  factory->RegisterNewObject(mafDummyObject::GetStaticTypeName(),"TestObject",mafDummyObject::NewObject);

  mafReferenceCounted *args=factory->GetArgs("Test");

  mafString description = factory->GetDescription();

  CPPUNIT_ASSERT(mafString::Compare(description, "Factory Dummy") == 0);
}

//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestRegisterFactory()
//------------------------------------------------------------------------------
{
	// a couple of factories
	mafDummyFactory *first_factory = mafDummyFactory::New();
	mafDummyFactory *second_factory = mafDummyFactory::New();

	CPPUNIT_ASSERT(first_factory!=NULL);
	CPPUNIT_ASSERT(second_factory!=NULL);

	// register factories
	mafCoreFactory::RegisterFactory(first_factory);
	mafCoreFactory::RegisterFactory(second_factory);

	std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
	CPPUNIT_ASSERT(list.size()==2);
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestUnRegisterAllFactories()
//------------------------------------------------------------------------------
{
	// a couple of factories
	mafDummyFactory *first_factory = mafDummyFactory::New();
	mafDummyFactory *second_factory = mafDummyFactory::New();

	// register factories
	mafCoreFactory::RegisterFactory(first_factory);
	mafCoreFactory::RegisterFactory(second_factory);

	mafCoreFactory::UnRegisterAllFactories();

	mafDummyFactory *third_factory = mafDummyFactory::New();

	mafCoreFactory::RegisterFactory(third_factory);

	std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
	CPPUNIT_ASSERT(list.size()==1);

	for ( std::list<mafObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
	{
		CPPUNIT_ASSERT((*i)==third_factory);
	}
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestGetRegisteredFactories()
//------------------------------------------------------------------------------
{
	// a couple of factories
	mafDummyFactory *first_factory = mafDummyFactory::New();
	mafDummyFactory *second_factory = mafDummyFactory::New();

	// register factories
	mafCoreFactory::RegisterFactory(first_factory);
	mafCoreFactory::RegisterFactory(second_factory);

	std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
	CPPUNIT_ASSERT(list.size()==2);

	int j=0;
	for (std::list<mafObjectFactory*>::iterator i = list.begin();i != list.end(); ++i,j++ )
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
}
//------------------------------------------------------------------------------
mafObject* FunctionDummy(){return new mafDummyObject();};
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestRegisterOverride()
//------------------------------------------------------------------------------
{
  mafDummyFactory *factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(factory);
  factory->RegisterOverride("ClassOne","mafDummyObject","ClassTest",true,mafFooObject::NewObject);
  std::list<std::string> listClassOverrideWithName;
  listClassOverrideWithName=factory->GetClassOverrideWithNames();
  for ( std::list<std::string>::iterator i = listClassOverrideWithName.begin(); i != listClassOverrideWithName.end(); ++i )
  {
    std::string st=(*i);
    CPPUNIT_ASSERT(st.compare("mafDummyObject")==0);
  }

  std::list<std::string> listClassOverrideName;
  listClassOverrideName=factory->GetClassOverrideNames();
  for ( std::list<std::string>::iterator i = listClassOverrideName.begin(); i != listClassOverrideName.end(); ++i )
  {
    std::string st=(*i);
    CPPUNIT_ASSERT(st.compare("ClassOne")==0);
  }

  std::list<std::string> listClassOverrideDescription;
  listClassOverrideDescription=factory->GetClassOverrideDescriptions();
  for ( std::list<std::string>::iterator i = listClassOverrideDescription.begin(); i != listClassOverrideDescription.end(); ++i )
  {
    std::string st=(*i);
    CPPUNIT_ASSERT(st.compare("ClassTest")==0);
  }
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestRegisterNewObject()
//------------------------------------------------------------------------------
{
  mafDummyFactory *factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(factory);
  factory->RegisterNewObject(mafDummyObject::GetStaticTypeName(),"TestObject",mafDummyObject::NewObject);

  std::list<std::string> listClassOverrideName;
  listClassOverrideName=factory->GetClassOverrideNames();
  for ( std::list<std::string>::iterator i = listClassOverrideName.begin(); i != listClassOverrideName.end(); ++i )
  {
    std::string st=(*i);
    CPPUNIT_ASSERT(st.compare("mafDummyObject")==0);
  }
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestDisable()
//------------------------------------------------------------------------------
{
  mafDummyFactory *factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(factory);
  factory->RegisterNewObject(mafDummyObject::GetStaticTypeName(),"TestObject",mafDummyObject::NewObject);
  factory->RegisterNewObject(mafFooObject::GetStaticTypeName(),"TestObject",mafFooObject::NewObject);

  factory->Disable(mafDummyObject::GetStaticTypeName());
  factory->Disable(mafFooObject::GetStaticTypeName());

  std::list<bool> listFlag = factory->GetEnableFlags();
  for ( std::list<bool>::iterator i = listFlag.begin(); i != listFlag.end(); ++i )
  {
    bool val=(*i);
    CPPUNIT_ASSERT(val==FALSE);
  }
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestSetEnableFlag()
//------------------------------------------------------------------------------
{
  mafDummyFactory *factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(factory);
  factory->RegisterNewObject(mafDummyObject::GetStaticTypeName(),"TestObject",mafDummyObject::NewObject);

  factory->SetEnableFlag(false,mafDummyObject::GetStaticTypeName(),mafDummyObject::GetStaticTypeName());
  bool flag=factory->GetEnableFlag(mafDummyObject::GetStaticTypeName(),mafDummyObject::GetStaticTypeName());

  CPPUNIT_ASSERT(flag==false);

  factory->SetEnableFlag(true,mafDummyObject::GetStaticTypeName(),mafDummyObject::GetStaticTypeName());
  flag=factory->GetEnableFlag(mafDummyObject::GetStaticTypeName(),mafDummyObject::GetStaticTypeName());

  CPPUNIT_ASSERT(flag==true);
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestUnRegisterFactory()
//------------------------------------------------------------------------------
{
  // a couple of factories
  mafDummyFactory *first_factory = mafDummyFactory::New();
  mafDummyFactory *second_factory = mafDummyFactory::New();

  // register factories
  mafCoreFactory::RegisterFactory(first_factory);
  mafCoreFactory::RegisterFactory(second_factory);

  mafCoreFactory::UnRegisterFactory(first_factory);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);

  for ( std::list<mafObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
  {
    CPPUNIT_ASSERT((*i)==second_factory);
  }

}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestGetArgs()
//------------------------------------------------------------------------------
{
  mafDummyFactory *factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(factory);
  factory->RegisterNewObject(mafDummyObject::GetStaticTypeName(),"TestObject",mafDummyObject::NewObject);
  
  mafReferenceCounted *args=factory->GetArgs("Test");

  CPPUNIT_ASSERT(args==NULL);
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestReHash()
//------------------------------------------------------------------------------
{
  mafDummyFactory *first_factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(first_factory);
  
  mafCoreFactory::ReHash();

  mafDummyFactory *second_factory = mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(second_factory);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();

  CPPUNIT_ASSERT(list.size()==1);

  for ( std::list<mafObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
  {
    CPPUNIT_ASSERT((*i)==second_factory);
  }
}
//------------------------------------------------------------------------------
void mafCoreFactoryTest::TestCreateAllInstance()
//------------------------------------------------------------------------------
{
  mafDummyFactory *factory=mafDummyFactory::New();
  mafCoreFactory::RegisterFactory(factory);
  factory->RegisterNewObject(mafDummyObject::GetStaticTypeName(),"TestObject",mafDummyObject::NewObject);

  std::list<mafObject*>object_list=factory->CreateAllInstance(mafDummyObject::GetStaticTypeName());

  for ( std::list<mafObject*>::iterator i = object_list.begin();i != object_list.end(); ++i )
  {
    CPPUNIT_ASSERT(mafDummyObject::SafeDownCast(*i));
  }

  for ( std::list<mafObject*>::iterator i = object_list.begin();i != object_list.end(); ++i )
  {
    mafDummyObject::SafeDownCast(*i)->Delete();
  }
}

void mafCoreFactoryTest::AfterTest()
{
	 mafCoreFactory::UnRegisterAllFactories();
}
