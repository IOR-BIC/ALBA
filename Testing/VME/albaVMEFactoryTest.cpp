/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEFactoryTest
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
#include "albaVMEFactoryTest.h"

#include "albaVME.h"
#include "albaVMEFactory.h"

#include <iostream>
#include <algorithm>


//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class albaVMECustom : public albaVME
{
public:
  albaTypeMacro(albaVMECustom,albaVME);
  /*virtual*/ void SetMatrix(const albaMatrix &mat){};
  /*virtual*/ void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes){};

protected:
  albaVMECustom();
  virtual ~albaVMECustom();

private:
  albaVMECustom(const albaVMECustom&); // Not implemented
  void operator=(const albaVMECustom&); // Not implemented
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMECustom);

//-------------------------------------------------------------------------
albaVMECustom::albaVMECustom()
{
  
}
//-------------------------------------------------------------------------
albaVMECustom::~albaVMECustom()
{
  
}
//-------------------------------------------------------------------------
void albaVMEFactoryTest::TestGetInstance()
{
  albaVMEFactory *vme_factory = NULL;
  vme_factory = (albaVMEFactory*) albaVMEFactory::GetInstance();
  CPPUNIT_ASSERT(vme_factory != NULL);
}
//-------------------------------------------------------------------------
void albaVMEFactoryTest::Initialize_CreateVMEInstance()
{
  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(albaVMEFactory::Initialize()==ALBA_OK);
  albaVMEFactory *vmeFactory=albaVMEFactory::GetInstance();

  std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();

	bool found=false;
	for ( std::list<albaObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
	{
		if((*i)==vmeFactory)
			found = true;
	}
	CPPUNIT_ASSERT(found);
  
	int vmeNum1 = vmeFactory->GetNodeNames().size();
  
  albaPlugVME<albaVMECustom>("a custom vme"); // plug a vme in the main node factory
	int vmeNum2 = vmeFactory->GetNodeNames().size();
	CPPUNIT_ASSERT(vmeNum2 == vmeNum1 + 1);


  albaVME *vme0 = vmeFactory->CreateVMEInstance("albaVMENotExisting");
  albaVME *vme1 = vmeFactory->CreateVMEInstance("albaVMECustom");

  CPPUNIT_ASSERT(vme0 == NULL);
  CPPUNIT_ASSERT(vme1!=NULL);

  CPPUNIT_ASSERT(vme1->IsALBAType(albaVMECustom));

                            
           
  // test factory contents
  //poor proof to register again an already registered vme
  albaPlugVME<albaVMECustom>("a custom vme"); // plug a vme in the main node factory again

  int vmeNum3 = vmeFactory->GetNodeNames().size();;
  CPPUNIT_ASSERT(vmeNum3 == vmeNum2);

	std::vector<std::string> vmes = vmeFactory->GetNodeNames();
  found=false;
  for (int i=0;i<vmes.size();i++)
    if (vmes[i]=="albaVMECustom")
      found=true;

  CPPUNIT_ASSERT(found);
  // cleanup factory products
  vme1->Delete();
}
//-------------------------------------------------------------------------
void albaVMEFactoryTest::TestGetDescription()
{
  CPPUNIT_ASSERT(albaVMEFactory::Initialize()==ALBA_OK);

  albaVMEFactory *vme_factory = NULL;
  vme_factory = (albaVMEFactory*)albaVMEFactory::GetInstance();
	CPPUNIT_ASSERT(vme_factory!=NULL);
	std::string value1 = vme_factory->GetDescription();
	std::string value2 = std::string("Factory for ALBA VMEs");
	CPPUNIT_ASSERT( value1 == value2 );
}
  
