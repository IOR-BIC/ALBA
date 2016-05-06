/*=========================================================================

 Program: MAF2
 Module: mafVMEFactoryTest
 Authors: Stefano Perticoni, Marco Petrone
 
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
#include "mafTagArray.h"
#include "mafTransform.h"
#include "mafVMEFactory.h"
#include "mafVMEOutputNULL.h"
#include "mafVersion.h"
#include <iostream>


//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class mafVMECustom : public mafVME
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafVMECustom,mafVME);

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  virtual bool Equals(mafVME *vme);
 
  /**
    Set the Pose matrix of the VME.*/
  void SetMatrix(const mafMatrix &mat) {m_Transform->SetMatrix(mat);Modified();}

  /**
    Return the unique timestamp */
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) {kframes.clear();kframes.push_back(m_Transform->GetTimeStamp());}

protected:
  mafVMECustom();
  virtual ~mafVMECustom();

  mafTransform *m_Transform;
private:
  mafVMECustom(const mafVMECustom&); // Not implemented
  void operator=(const mafVMECustom&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMECustom);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool mafVMECustom::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    mafVMECustom *vme_test=mafVMECustom::SafeDownCast(vme);
    assert(vme);
    
    return m_Transform->GetMatrix()==vme_test->m_Transform->GetMatrix();
  }

  return false;
}

//-------------------------------------------------------------------------
mafVMECustom::mafVMECustom()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  m_Output = mafVMEOutputNULL::New();
  m_Output->SetVME(this);
  m_Output->SetTransform(m_Transform);
}
//-------------------------------------------------------------------------
mafVMECustom::~mafVMECustom()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
}

//-------------------------------------------------------------------------
/** example of CUSTOM node factory */
class mafVMEFactoryCustom : public mafVMEFactory
//-------------------------------------------------------------------------
{
public: 
  mafTypeMacro(mafVMEFactoryCustom,mafVMEFactory);
  virtual const char* GetMAFSourceVersion() const {return MAF_SOURCE_VERSION;}
  virtual const char* GetDescription() const {return "My own Node factory";}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
  static mafVMEFactoryCustom *GetInstance();

protected:
  mafVMEFactoryCustom();
  ~mafVMEFactoryCustom() {}

  static bool m_Initialized;
  
private:
  mafVMEFactoryCustom(const mafVMEFactoryCustom&);  // Not implemented.
  void operator=(const mafVMEFactoryCustom&);  // Not implemented.
};

//----------------------------------------------------------------------------
bool mafVMEFactoryCustom::m_Initialized=false;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEFactoryCustom)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafVMEFactoryCustom::mafVMEFactoryCustom()
//----------------------------------------------------------------------------
{
  // m_Instance = NULL;
  
// a VME plugged by default in this factory
  mafPlugVMEMacro(mafVMECustom,"a test VME");
  mafPlugObjectMacro(mafTagArray,"the TagArray attribute");
}
//----------------------------------------------------------------------------
mafVMEFactoryCustom* mafVMEFactoryCustom::GetInstance()
//----------------------------------------------------------------------------
{
  static mafVMEFactoryCustom &istance = *(mafVMEFactoryCustom::New());;
  mafVMEFactoryCustom::Initialize();
  return &istance;
}
//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafVMEFactoryCustom::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    // m_Instance=mafVMEFactory::New();

    m_Initialized = true;
    if (mafVMEFactoryCustom::GetInstance())
    {
      mafVMEFactoryCustom::GetInstance()->RegisterFactory(mafVMEFactoryCustom::GetInstance());
      return MAF_OK;  
    }
    else
    {
      m_Initialized = false;
      return MAF_ERROR;
    }
  }

  return MAF_OK;
}

//-------------------------------------------------------------------------
void mafVMEFactoryTest::TestCreateNodeInstance()
//-------------------------------------------------------------------------
{
  // a couple of factories
  mafVMEFactory *node_factory = mafVMEFactory::GetInstance();
  CPPUNIT_ASSERT(node_factory!=NULL);

  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(mafVMEFactoryCustom::Initialize()==MAF_OK);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
	bool found=false;
	for ( std::list<mafObjectFactory*>::iterator i = list.begin();i != list.end(); ++i )
	{
		if((*i)==node_factory)
			found = true;
	}
	CPPUNIT_ASSERT(found);

  mafPlugVME<mafVMECustom>("a test node"); // plug a node in the main node factory

  mafVME *node0 = node_factory->CreateVMEInstance("mafVMENotExisting");
  mafVME *node1 = node_factory->CreateVMEInstance("mafVMECustom");
  mafVME *node2 = node_factory->CreateVMEInstance("mafVMECustom");

  CPPUNIT_ASSERT(node0 == NULL);
  CPPUNIT_ASSERT(node1!=NULL);
  CPPUNIT_ASSERT(node2!=NULL);

  CPPUNIT_ASSERT(node1->IsMAFType(mafVMECustom));
  CPPUNIT_ASSERT(node2->IsMAFType(mafVMECustom));

  // test factory contents
  const std::vector<std::string> &nodes = node_factory->GetNodeNames();

  
  bool found1=false;
  bool found2=false;
  for (int i=0;i<nodes.size();i++)
  {
    if (nodes[i]=="mafTestNode")
      found1=true;
    if (nodes[i]=="mafVMETest")
      found2=true;
  }

	assert(node1,node2);
  
  // cleanup factory products
  node1->Delete();
  node2->Delete();
}

  
