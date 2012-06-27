/*=========================================================================

 Program: MAF2
 Module: mafVMEFactory
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEFactory_h
#define __mafVMEFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafNodeFactory.h"
#include "mafVME.h"

// /** to be used internally for plugging default nodes --- calls a member function directly */
// #define mafPlugNodeMacro(node_type,descr) \
//   RegisterNewNode(node_type::GetStaticTypeName(), descr, node_type::NewObject); \
//   if (mafPictureFactory::GetPicsInitialized()) \
//   mafPictureFactory::GetPictureFactory()->AddVmePic(node_type::GetStaticTypeName(),node_type::GetIcon());

/** Object factory for VMEs.
  This is a simple specialization of mafVMEFactory. Notice that mafVMEFactory is also
  a mafVMEFactory this last does not need to be crated: you should create one or the other but
  not both since the Instance would be overwritten and thus the Plug*<> methods would always
  plug inside one of the two. */
class MAF_EXPORT mafVMEFactory : public mafNodeFactory
{
public: 
  mafTypeMacro(mafVMEFactory,mafNodeFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();

  /** return the instance pointer of the factory. return NULL if not initialized yet */
  static mafVMEFactory *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}

  mafVME *CreateVMEInstance(const char *type_name);

//   /**
//   This function can be used by Application code to register new Objects's to the mflCoreFactory */
//   void RegisterNewNode(const char* node_name, const char* description, mafCreateObjectFunction createFunction);
// 
//   /** return list of names for nodes plugged into this factory */
//   static std::vector<std::string> &GetNodeNames();// {return m_NodeNames;}
  
protected:
  mafVMEFactory();
  ~mafVMEFactory() { }

  static bool m_Initialized;
  
private:
  mafVMEFactory(const mafVMEFactory&);  // Not implemented.
  void operator=(const mafVMEFactory&);  // Not implemented.
};
// /** Plug  a node in the main MAF Node factory.*/
// template <class T>
// class mafPlugNode
// {
// public:
//   mafPlugNode(const char *description);
// 
// };
// 
// //------------------------------------------------------------------------------
// /** Plug a new Node class into the Node factory.*/
// template <class T>
// mafPlugNode<T>::mafPlugNode(const char *description)
// //------------------------------------------------------------------------------
// { 
//   mafVMEFactory *factory=mafVMEFactory::GetInstance();
//   if (factory)
//   {
//     factory->RegisterNewNode(T::GetStaticTypeName(), description, T::NewObject);
//     // here plug node's icon inside picture factory
//     if (mafPictureFactory::GetPicsInitialized())
//       mafPictureFactory::GetPictureFactory()->AddVmePic(T::GetStaticTypeName(),T::GetIcon());
//   }
// }
// 
// /** Plug an attribute class into the Node factory.*/
// template <class T>
// class mafPlugAttribute
// {
// public:
//   mafPlugAttribute(const char *description) \
//   { \
//   mafVMEFactory *factory=mafVMEFactory::GetInstance(); \
//   if (factory) \
//     factory->RegisterNewObject(T::GetStaticTypeName(), description, T::NewObject); \
//   }
// };

#endif
