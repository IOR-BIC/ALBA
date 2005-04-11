#include "mafVME.h"
#include "mafTagArray.h"
#include "mafTransform.h"
#include "mafNodeFactory.h"
#include "mafVMEOutputNULL.h"
#include "mafVersion.h"
#include <iostream>

//-------------------------------------------------------------------------
class mafTestNode: public mafNode
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafTestNode,mafNode);
  mafTestNode():Flag(NULL),m_Value(-1) {}
  ~mafTestNode() {if (Flag) *Flag=false;}
  bool *Flag;
  int m_Value;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafTestNode)
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class mafVMETest : public mafVME
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafVMETest,mafVME);

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
  mafVMETest();
  virtual ~mafVMETest();

  mafTransform *m_Transform;
private:
  mafVMETest(const mafVMETest&); // Not implemented
  void operator=(const mafVMETest&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETest);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool mafVMETest::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    mafVMETest *vme_test=mafVMETest::SafeDownCast(vme);
    assert(vme);
    
    return m_Transform->GetMatrix()==vme_test->m_Transform->GetMatrix();
  }

  return false;
}

//-------------------------------------------------------------------------
mafVMETest::mafVMETest()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  m_Output = mafVMEOutputNULL::New();
  m_Output->SetVME(this);
  m_Output->SetTransform(m_Transform);
}
//-------------------------------------------------------------------------
mafVMETest::~mafVMETest()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
}

//-------------------------------------------------------------------------
/** example of CUSTOM node factory */
class MAF_EXPORT mafTestNodeFactory : public mafNodeFactory
//-------------------------------------------------------------------------
{
public: 
  mafTypeMacro(mafTestNodeFactory,mafNodeFactory);
  virtual const char* GetMAFSourceVersion() const {return MAF_SOURCE_VERSION;}
  virtual const char* GetDescription() const {return "My own Node factory";}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
  static mafTestNodeFactory *GetInstance() {if (!m_Instance) Initialize(); return m_Instance; }

protected:
  mafTestNodeFactory();
  ~mafTestNodeFactory() {}

  static mafTestNodeFactory *m_Instance; // stores pointer to this kind of factory
  
private:
  mafTestNodeFactory(const mafTestNodeFactory&);  // Not implemented.
  void operator=(const mafTestNodeFactory&);  // Not implemented.
};

//----------------------------------------------------------------------------
mafTestNodeFactory *mafTestNodeFactory::m_Instance=NULL;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafTestNodeFactory)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafTestNodeFactory::mafTestNodeFactory()
//----------------------------------------------------------------------------
{
  m_Instance = NULL;
  
// a VME plugged by default in this factory
  mafPlugNodeMacro(mafVMETest,"a test VME");
  mafPlugObjectMacro(mafTagArray,"the TagArray attribute");
}

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafTestNodeFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    if (m_Instance=mafTestNodeFactory::New())
    {
      m_Instance->RegisterFactory(m_Instance);
      return MAF_OK;  
    }
    
    return MAF_ERROR;
  }

  return MAF_OK;
}

//------------------------------------------------------------------------------
// a custom plugger for the new factory
template <class T>
class mafPlugTestNode
//------------------------------------------------------------------------------
{
  public:
  mafPlugTestNode(const char *description);
  
};

//------------------------------------------------------------------------------
template <class T>
mafPlugTestNode<T>::mafPlugTestNode(const char *description)
//------------------------------------------------------------------------------
{ 
  mafTestNodeFactory *factory=mafTestNodeFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewNode(T::GetStaticTypeName(), description, T::NewObject);
    // here plug node's icon inside picture factory
    mafPics.AddVmePic(T::GetStaticTypeName(),T::GetIcon());
  }
}

//-------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------
{
  // a couple of factories
  mafNodeFactory *node_factory = mafNodeFactory::GetInstance();
  MAF_TEST(node_factory!=NULL);

  // this custom factory needs only to be initialized
  MAF_TEST(mafTestNodeFactory::Initialize()==MAF_OK);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  MAF_TEST(list.size()==2);

  mafPlugTestNode<mafTestNode>("a test node"); // plug a node in the main node factory

  mafNode *node1=mafNodeFactory::CreateNodeInstance("mafTestNode");
  mafNode *node2=mafNodeFactory::CreateNodeInstance("mafVMETest");

  MAF_TEST(node1!=NULL);
  MAF_TEST(node2!=NULL);

  MAF_TEST(node1->IsMAFType(mafTestNode));
  MAF_TEST(node2->IsMAFType(mafVMETest));

  // test factory contents
  const std::vector<std::string> &nodes=node_factory->GetNodeNames();
  MAF_TEST(nodes.size()==2); // one node is plugged by default, the other is plugged in the main
  
  bool found1=false;
  bool found2=false;
  for (int i=0;i<nodes.size();i++)
  {
    if (nodes[i]=="mafTestNode")
      found1=true;
    if (nodes[i]=="mafVMETest")
      found2=true;
  }
  
  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
