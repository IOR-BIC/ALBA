/*=========================================================================

 Program: MAF2
 Module: mafVMEFactoryTest
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
#include "mafVMEFactoryTest.h"

#include "mafVME.h"
#include "mafVMEFactory.h"

#include "mafVersion.h"
#include <iostream>
#include <algorithm>


//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class mafVMECustom : public mafVME
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafVMECustom,mafVME);
  /*virtual*/ void SetMatrix(const mafMatrix &mat){};
  /*virtual*/ void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};

protected:
  mafVMECustom();
  virtual ~mafVMECustom();

private:
  mafVMECustom(const mafVMECustom&); // Not implemented
  void operator=(const mafVMECustom&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMECustom);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMECustom::mafVMECustom()
//-------------------------------------------------------------------------
{
  
}
//-------------------------------------------------------------------------
mafVMECustom::~mafVMECustom()
//-------------------------------------------------------------------------
{
  
}
//-------------------------------------------------------------------------
void mafVMEFactoryTest::TestGetInstance()
//-------------------------------------------------------------------------
{
  mafVMEFactory *vme_factory = NULL;
  vme_factory = (mafVMEFactory*) mafVMEFactory::GetInstance();
  CPPUNIT_ASSERT(vme_factory != NULL);
}
//-------------------------------------------------------------------------
void mafVMEFactoryTest::Initialize_CreateVMEInstance()
//-------------------------------------------------------------------------
{
  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(mafVMEFactory::Initialize()==MAF_OK);
  mafNodeFactory *vmeFactory=mafVMEFactory::GetInstance();

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();

	bool found=false;
	for ( std::list<mafObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
	{
		if((*i)==vmeFactory)
			found = true;
	}
	CPPUNIT_ASSERT(found);

  mafVMEFactory *vme_factory = NULL;
  vme_factory = (mafVMEFactory*) mafVMEFactory::GetInstance();

  mafPlugNode<mafVMECustom>("a custom vme"); // plug a vme in the main node factory

  mafVME *vme0 = vme_factory->CreateVMEInstance("mafVMENotExisting");
  mafVME *vme1 = vme_factory->CreateVMEInstance("mafVMECustom");

  CPPUNIT_ASSERT(vme0 == NULL);
  CPPUNIT_ASSERT(vme1!=NULL);

  CPPUNIT_ASSERT(vme1->IsMAFType(mafVMECustom));

  std::vector<std::string> vmes = vme_factory->GetNodeNames();
  int s1 = vmes.size();
  CPPUNIT_ASSERT(s1 == 36); //24 VMES are registered in constructor actually, 
                            //please modify also the test when (un)plugged inside maf and update this number
           
  // test factory contents
  //poor proof to register again an already registered vme
  mafPlugNode<mafVMECustom>("a custom vme"); // plug a vme in the main node factory again
  vmes = vme_factory->GetNodeNames();
  s1 = vmes.size();
  CPPUNIT_ASSERT(s1 == 36);

  found=false;
  for (int i=0;i<vmes.size();i++)
  {
		std::string test;
		test = vmes[i];
    if (vmes[i]=="mafVMECustom")
      found=true;
  }
  
	CPPUNIT_ASSERT(found);
  // cleanup factory products
  vme1->Delete();
}
//-------------------------------------------------------------------------
void mafVMEFactoryTest::TestGetMAFSourceVersion()
//-------------------------------------------------------------------------
{
  CPPUNIT_ASSERT(mafVMEFactory::Initialize()==MAF_OK);

  mafVMEFactory *vme_factory = NULL;
  vme_factory = (mafVMEFactory*)mafVMEFactory::GetInstance();
	CPPUNIT_ASSERT(vme_factory!=NULL);
	std::string value1 = vme_factory->GetMAFSourceVersion();
	std::string value2 = std::string(MAF_SOURCE_VERSION);
	CPPUNIT_ASSERT( value1 == value2 );

}
//-------------------------------------------------------------------------
void mafVMEFactoryTest::TestGetDescription()
//-------------------------------------------------------------------------
{
  CPPUNIT_ASSERT(mafVMEFactory::Initialize()==MAF_OK);

  mafVMEFactory *vme_factory = NULL;
  vme_factory = (mafVMEFactory*)mafVMEFactory::GetInstance();
	CPPUNIT_ASSERT(vme_factory!=NULL);
	std::string value1 = vme_factory->GetDescription();
	std::string value2 = std::string("Factory for MAF VMEs");
	CPPUNIT_ASSERT( value1 == value2 );
}
  
