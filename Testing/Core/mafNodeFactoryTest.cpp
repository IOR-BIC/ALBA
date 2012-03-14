/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafNodeFactoryTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.3.2.1 $
Authors:   Stefano Perticoni, Marco Petrone
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
#include "mafNodeFactoryTest.h"

#include "mafVME.h"
#include "mafTagArray.h"
#include "mafTransform.h"
#include "mafNodeFactory.h"
#include "mafVMEOutputNULL.h"
#include "mafVersion.h"
#include <iostream>

//-------------------------------------------------------------------------
class mafNodeCustom: public mafNode
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafNodeCustom,mafNode);
  mafNodeCustom():Flag(NULL),m_Value(-1) {}
  ~mafNodeCustom() {if (Flag) *Flag=false;}
  bool *Flag;
  int m_Value;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeCustom)
//-------------------------------------------------------------------------


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
class mafNodeFactoryCustom : public mafNodeFactory
//-------------------------------------------------------------------------
{
public: 
  mafTypeMacro(mafNodeFactoryCustom,mafNodeFactory);
  virtual const char* GetMAFSourceVersion() const {return MAF_SOURCE_VERSION;}
  virtual const char* GetDescription() const {return "My own Node factory";}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
  static mafNodeFactoryCustom *GetInstance();

protected:
  mafNodeFactoryCustom();
  ~mafNodeFactoryCustom() {}

  static bool m_Initialized;
  
private:
  mafNodeFactoryCustom(const mafNodeFactoryCustom&);  // Not implemented.
  void operator=(const mafNodeFactoryCustom&);  // Not implemented.
};

//----------------------------------------------------------------------------
bool mafNodeFactoryCustom::m_Initialized=false;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeFactoryCustom)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafNodeFactoryCustom::mafNodeFactoryCustom()
//----------------------------------------------------------------------------
{
  // m_Instance = NULL;
  
// a VME plugged by default in this factory
  mafPlugNodeMacro(mafVMECustom,"a test VME");
  mafPlugObjectMacro(mafTagArray,"the TagArray attribute");
}
//----------------------------------------------------------------------------
mafNodeFactoryCustom* mafNodeFactoryCustom::GetInstance()
//----------------------------------------------------------------------------
{
  static mafNodeFactoryCustom &istance = *(mafNodeFactoryCustom::New());;
  mafNodeFactoryCustom::Initialize();
  return &istance;
}
//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafNodeFactoryCustom::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    // m_Instance=mafNodeFactory::New();

    m_Initialized = true;
    if (mafNodeFactoryCustom::GetInstance())
    {
      mafNodeFactoryCustom::GetInstance()->RegisterFactory(mafNodeFactoryCustom::GetInstance());
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
void mafNodeFactoryTest::TestCreateNodeInstance()
//-------------------------------------------------------------------------
{
  // a couple of factories
  mafNodeFactory *node_factory = mafNodeFactory::GetInstance();
  CPPUNIT_ASSERT(node_factory!=NULL);

  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(mafNodeFactoryCustom::Initialize()==MAF_OK);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==2);

  mafPlugNode<mafNodeCustom>("a test node"); // plug a node in the main node factory

  mafNode *node0 = mafNodeFactory::CreateNodeInstance("mafNodeNotExisting");
  mafNode *node1 = mafNodeFactory::CreateNodeInstance("mafNodeCustom");
  mafNode *node2 = mafNodeFactory::CreateNodeInstance("mafVMECustom");

  CPPUNIT_ASSERT(node0 == NULL);
  CPPUNIT_ASSERT(node1!=NULL);
  CPPUNIT_ASSERT(node2!=NULL);

  CPPUNIT_ASSERT(node1->IsMAFType(mafNodeCustom));
  CPPUNIT_ASSERT(node2->IsMAFType(mafVMECustom));

  // test factory contents
  const std::vector<std::string> &nodes = node_factory->GetNodeNames();
  int s = nodes.size();
  CPPUNIT_ASSERT(s==3); // one node is plugged by default (mafNodeLayout), 
                        // the others are plugged in the constructor and in this method
  
  bool found1=false;
  bool found2=false;
  for (int i=0;i<nodes.size();i++)
  {
    if (nodes[i]=="mafTestNode")
      found1=true;
    if (nodes[i]=="mafVMETest")
      found2=true;
  }
  
  // cleanup factory products
  node1->Delete();
  node2->Delete();
}

  
