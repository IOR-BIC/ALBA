/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEFactoryTest
 Authors: Stefano Perticoni, Marco Petrone
 
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
#include "albaTagArray.h"
#include "albaTransform.h"
#include "albaVMEFactory.h"
#include "albaVMEOutputNULL.h"
#include <iostream>


//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class albaVMECustom : public albaVME
//-------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaVMECustom,albaVME);

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  virtual bool Equals(albaVME *vme);
 
  /**
    Set the Pose matrix of the VME.*/
  void SetMatrix(const albaMatrix &mat) {m_Transform->SetMatrix(mat);Modified();}

  /**
    Return the unique timestamp */
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes) {kframes.clear();kframes.push_back(m_Transform->GetTimeStamp());}

protected:
  albaVMECustom();
  virtual ~albaVMECustom();

  albaTransform *m_Transform;
private:
  albaVMECustom(const albaVMECustom&); // Not implemented
  void operator=(const albaVMECustom&); // Not implemented
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMECustom);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool albaVMECustom::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    albaVMECustom *vme_test=albaVMECustom::SafeDownCast(vme);
    assert(vme);
    
    return m_Transform->GetMatrix()==vme_test->m_Transform->GetMatrix();
  }

  return false;
}

//-------------------------------------------------------------------------
albaVMECustom::albaVMECustom()
//-------------------------------------------------------------------------
{
  albaNEW(m_Transform);
  m_Output = albaVMEOutputNULL::New();
  m_Output->SetVME(this);
  m_Output->SetTransform(m_Transform);
}
//-------------------------------------------------------------------------
albaVMECustom::~albaVMECustom()
//-------------------------------------------------------------------------
{
  albaDEL(m_Transform);
}

//-------------------------------------------------------------------------
/** example of CUSTOM node factory */
class albaVMEFactoryCustom : public albaVMEFactory
//-------------------------------------------------------------------------
{
public: 
  albaTypeMacro(albaVMEFactoryCustom,albaVMEFactory);
  virtual const char* GetDescription() const {return "My own Node factory";}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
  static albaVMEFactoryCustom *GetInstance();

protected:
  albaVMEFactoryCustom();
  ~albaVMEFactoryCustom() {}

  static bool m_Initialized;
  
private:
  albaVMEFactoryCustom(const albaVMEFactoryCustom&);  // Not implemented.
  void operator=(const albaVMEFactoryCustom&);  // Not implemented.
};

//----------------------------------------------------------------------------
bool albaVMEFactoryCustom::m_Initialized=false;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEFactoryCustom)

//----------------------------------------------------------------------------
albaVMEFactoryCustom::albaVMEFactoryCustom()
{
  // m_Instance = NULL;
  
// a VME plugged by default in this factory
  albaPlugVMEMacro(albaVMECustom,"a test VME");
  albaPlugObjectMacro(albaTagArray,"the TagArray attribute");
}
//----------------------------------------------------------------------------
albaVMEFactoryCustom* albaVMEFactoryCustom::GetInstance()
{
  static albaVMEFactoryCustom &istance = *(albaVMEFactoryCustom::New());;
  albaVMEFactoryCustom::Initialize();
  return &istance;
}
//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int albaVMEFactoryCustom::Initialize()
{
  if (!m_Initialized)
  {
    // m_Instance=albaVMEFactory::New();

    m_Initialized = true;
    if (albaVMEFactoryCustom::GetInstance())
    {
      albaVMEFactoryCustom::GetInstance()->RegisterFactory(albaVMEFactoryCustom::GetInstance());
      return ALBA_OK;  
    }
    else
    {
      m_Initialized = false;
      return ALBA_ERROR;
    }
  }

  return ALBA_OK;
}

//-------------------------------------------------------------------------
void albaVMEFactoryTest::TestCreateNodeInstance()
{
  // a couple of factories
  albaVMEFactory *node_factory = albaVMEFactory::GetInstance();
  CPPUNIT_ASSERT(node_factory!=NULL);

  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(albaVMEFactoryCustom::Initialize()==ALBA_OK);

  std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();
	bool found=false;
	for ( std::list<albaObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
	{
		if((*i)==node_factory)
			found = true;
	}
	CPPUNIT_ASSERT(found);

  albaPlugVME<albaVMECustom>("a test node"); // plug a node in the main node factory

  albaVME *node0 = node_factory->CreateVMEInstance("albaVMENotExisting");
  albaVME *node1 = node_factory->CreateVMEInstance("albaVMECustom");
  albaVME *node2 = node_factory->CreateVMEInstance("albaVMECustom");

  CPPUNIT_ASSERT(node0 == NULL);
  CPPUNIT_ASSERT(node1!=NULL);
  CPPUNIT_ASSERT(node2!=NULL);

  CPPUNIT_ASSERT(node1->IsALBAType(albaVMECustom));
  CPPUNIT_ASSERT(node2->IsALBAType(albaVMECustom));

  // test factory contents
  const std::vector<std::string> &nodes = node_factory->GetNodeNames();

  
  bool found1=false;
  bool found2=false;
  for (int i=0;i<nodes.size();i++)
  {
    if (nodes[i]=="albaTestNode")
      found1=true;
    if (nodes[i]=="albaVMETest")
      found2=true;
  }

	assert(node1,node2);
  
  // cleanup factory products
  node1->Delete();
  node2->Delete();
}
