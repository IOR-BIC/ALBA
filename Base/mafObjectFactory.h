/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObjectFactory.h,v $
  Language:  C++
  Date:      $Date: 2006-07-24 08:52:50 $
  Version:   $Revision: 1.15 $
  Authors:   Based on itkObjectFactory (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafObjectFactory_h
#define __mafObjectFactory_h

#include "mafReferenceCounted.h"
#include "mafSmartPointer.h"
#include "mafString.h"

#include <vector>
#include <list>
#include <map>
#include <string>

/** to be used internally for plugging default objects --- calls a member function directly */
#define mafPlugObjectMacro(node_type,descr) \
  RegisterNewObject(node_type::GetStaticTypeName(), descr, node_type::NewObject);

typedef mafObject* (*mafCreateObjectFunction)();

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafIndent;

/** Create instances of classes using an object factory.
  mafObjectFactory is used to create mafObject's. The base class
  mafObjectFactory contains a static method CreateInstance() that is
  used to create maf objects from the list of registered mafObjectFactory
  sub-classes.  The first time CreateInstance() is called, all dll's or
  shared libraries in the environment variable MAF_AUTOLOAD_PATH are loaded
  into the current process.  The C function mafLoad is called on each dll.
  mafLoad should return an instance of the factory sub-class implemented in
  the shared library. MAF_AUTOLOAD_PATH is an environment variable
  containing a colon separated (semi-colon on win32) list of paths.
  @sa mafObject 
*/
class MAF_EXPORT mafObjectFactory : public mafReferenceCounted
{
public:    
  /** Run-time type information (and related methods). */
  mafAbstractTypeMacro(mafObjectFactory, mafReferenceCounted);

  /** Create and return an instance of the named object.
   * Each loaded mafObjectFactory will be asked in the order
   * the factory was in the MAF_AUTOLOAD_PATH.  After the
   * first factory returns the object no other factories are asked. */
  static mafObject *CreateInstance(const char* classname);

  /** Create and return all possible instances of the named object.
   * Each loaded mafObjectFactory will be asked in the order
   * the factory was in the MAF_AUTOLOAD_PATH.  All created objects
   * will be returned in the list. */
  static std::list<mafObject *>
  CreateAllInstance(const char* classname);
  
  /** Re-check the MAF_AUTOLOAD_PATH for new factory libraries.
   * This calls UnRegisterAll before re-loading. */
  static void ReHash(); 

  /** Register a factory so it can be used to create maf objects. */
  static void RegisterFactory(mafObjectFactory* );

  /** Remove a factory from the list of registered factories. */
  static void UnRegisterFactory(mafObjectFactory*);

  /** Unregister all factories. */
  static void UnRegisterAllFactories();
  
  /** Return the list of all registered factories.  This is NOT a copy,
   * do not remove items from this list! */
  static std::list<mafObjectFactory*> GetRegisteredFactories();

  /** All sub-classes of mafObjectFactory should must return the version of 
   * MAF they were built with.  This should be implemented with the macro
   * MAF_SOURCE_VERSION and NOT a call to Version::GetMAFSourceVersion.
   * As the version needs to be compiled into the file as a string constant.
   * This is critical to determine possible incompatible dynamic factory loads. */
  virtual const char* GetMAFSourceVersion(void) const = 0;

  /** Return a descriptive string describing the factory. */
  virtual const char* GetDescription(void) const = 0;

  /** Return a list of classes that this factory overrides. */
  virtual std::list<std::string> GetClassOverrideNames();

  /** Return a list of the names of classes that override classes. */
  virtual std::list<std::string> GetClassOverrideWithNames();

  /** Return a list of descriptions for class overrides. */
  virtual std::list<std::string> GetClassOverrideDescriptions();

  /** Return a list of enable flags. */
  virtual std::list<bool> GetEnableFlags();

  /** Set the Enable flag for the specific override of className. */
  virtual void SetEnableFlag(bool flag,
           const char* className,
           const char* subclassName);

  /** Get the Enable flag for the specific override of className. */
  virtual bool GetEnableFlag(const char* className,
                             const char* subclassName);

  /** 
    Set all enable flags for the given class to 0.  This will
    mean that the factory will stop producing class with the given
    name. */
  virtual void Disable(const char* className);
  
  /** This returns the path to a dynamically loaded factory. */
  const char* GetLibraryPath();

  /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewObject(const char* ObjectName, const char* description, mafCreateObjectFunction createFunction,mafReferenceCounted *args=NULL);

  /** Register object creation information with the factory. */
  void RegisterOverride(const char* classOverride,
      const char* overrideClassName,
      const char* description,
      bool enableFlag,
      mafCreateObjectFunction createFunction,
      mafReferenceCounted *args=NULL);

  /** return argument data assigned to object inthe factory */
  mafReferenceCounted *GetArgs(const char *type_name);

  /** mmuOverrideInformation utility class.
    Internal implementation class for mafObjectFactory. */
  struct mmuOverrideInformation
  {
    std::string m_Description;
    std::string m_OverrideWithName;
    bool m_EnabledFlag;
    mafCreateObjectFunction m_CreateObject;
    mafAutoPointer<mafReferenceCounted> m_Args;
  };
  
  /** dump the object */
  virtual void Print(std::ostream& os, const int indent=0) const;

  mafObjectFactory();
  virtual ~mafObjectFactory();


protected:
    
  /** This method is provided by sub-classes of mafObjectFactory.
   * It should create the named maf object or return 0 if that object
   * is not supported by the factory implementation. */
  virtual mafObject *CreateObject(const char* classname );

  /** internally used to retrieve args for this factory */
  mafReferenceCounted *GetFactoryArgs(const char *type_name);

  //------------------------------------------------------------------------------
  /** mafStringOverMap - Internal implementation class for ObjectFactorBase.
    Create a sub class to shrink the size of the symbols
    Also, so a forward reference can be put in mafObjectFactory.h
    and a pointer member can be used.  This avoids other
    classes including <map> and getting long symbol warnings.*/
  typedef std::multimap<std::string, mmuOverrideInformation> mmuOverRideMap;

  mmuOverRideMap* m_OverrideMap;

private:
  mafObjectFactory(const mafObjectFactory&); //purposely not implemented
  void operator=(const mafObjectFactory&); //purposely not implemented

  /** Initialize the static members of mafObjectFactory.   RegisterDefaults
   * is called here. */
  static void Initialize();

  /** Register default factories which are not loaded at run time. */
  static void RegisterDefaults();

  /** Load dynamic factories from the MAF_AUTOLOAD_PATH */
  static void LoadDynamicFactories();

  /** Load all dynamic libraries in the given path */
  static void LoadLibrariesInPath( const char*);
  
  /** list of registered factories */
  static std::list<mafObjectFactory*>* m_RegisteredFactories; 
  
  /** Member variables for a factory set by the base class
   * at load or register time */
  void* m_LibraryHandle;
  unsigned long m_LibraryDate;
  std::string m_LibraryPath;
};

#endif

