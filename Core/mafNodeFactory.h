/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeFactory.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 16:40:41 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafNodeFactory_h
#define __mafNodeFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObjectFactory.h"
#include "mafPics.h"

/** to be used internally for plugging default nodes --- calls a member function directly */
#define mafPlugNodeMacro(node_type,descr) \
  RegisterNewNode(node_type::GetStaticTypeName(), descr, node_type::NewObject); \
  mafPics.AddVmePic(node_type::GetStaticTypeName(),node_type::GetIcon());


//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  
class mafNode;

/** Object factory for Nodes.
  To make a new VME available in the MAF it must be plugged inside a factory, in particular
  this factory must be of type mafNodeFactory to be able to retrieve the list of nodes plugged
  in the factory. Also when using mafPlugNode<node_type> the node icon is plugged inside the
  the MAF picture factory. */
class MAF_EXPORT mafNodeFactory : public mafObjectFactory
{
public: 
  mafTypeMacro(mafNodeFactory,mafObjectFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  /** return the instance pointer of the factory. return NULL if not initialized yet */
  static mafNodeFactory *GetInstance() {if (!m_Instance) Initialize(); return m_Instance;}

  /** create an instance of the node give its type name */
  static mafNode *CreateNodeInstance(const char *type_name);
   
   /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewNode(const char* node_name, const char* description, mafCreateObjectFunction createFunction);

  /** return list of names for nodes plugged into this factory */
  const static std::vector<std::string> GetNodeNames() {return m_NodeNames;}

protected:
  mafNodeFactory();
  ~mafNodeFactory() { }

  static mafNodeFactory *m_Instance;
  static std::vector<std::string> m_NodeNames; 
  
private:
  mafNodeFactory(const mafNodeFactory&);  // Not implemented.
  void operator=(const mafNodeFactory&);  // Not implemented.
};

/** Plug  a node in the main MAF Node factory.*/
template <class T>
class MAF_EXPORT mafPlugNode
{
  public:
  mafPlugNode(const char *description);
  
};

//------------------------------------------------------------------------------
template <class T>
mafPlugNode<T>::mafPlugNode(const char *description)
//------------------------------------------------------------------------------
{ 
  mafNodeFactory *factory=mafNodeFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewNode(T::GetStaticTypeName(), description, T::NewObject);
    // here plug node's icon inside picture factory
    mafPics.AddVmePic(T::GetStaticTypeName(),T::GetIcon());
  }
}

#endif
