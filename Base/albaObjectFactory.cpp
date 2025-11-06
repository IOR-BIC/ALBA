/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObjectFactory
 Authors: Based on itkObjectFactory (www.itk.org), adapted by Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDynamicLoader.h"
#include "albaObjectFactory.h"
#include "albaDirectory.h"
#include "albaIndent.h"
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>

//------------------------------------------------------------------------------
/** Utility class to clean up factory memory.*/  
class CleanUpObjectFactory
{
public:
  inline void Use() 
  {
  }
  ~CleanUpObjectFactory()
  {
      albaObjectFactory::UnRegisterAllFactories();
  }  
};

//------------------------------------------------------------------------------
// Static object used to clean up memory at program closing
static CleanUpObjectFactory CleanUpObjectFactoryGlobal;
//------------------------------------------------------------------------------

albaCxxAbstractTypeMacro(albaObjectFactory);

//------------------------------------------------------------------------------
// Add this for the SGI compiler which does not seem
// to provide a default implementation as it should.
//
bool operator==(const albaObjectFactory::mmuOverrideInformation& rhs, 
    const albaObjectFactory::mmuOverrideInformation& lhs)
//------------------------------------------------------------------------------
{
  return (rhs.m_TypeName == lhs.m_TypeName
    && rhs.m_OverrideWithName == lhs.m_OverrideWithName);
}

//------------------------------------------------------------------------------
// Add this for the SGI compiler which does not seem
// to provide a default implementation as it should.
bool operator<(const albaObjectFactory::mmuOverrideInformation& rhs, 
    const albaObjectFactory::mmuOverrideInformation& lhs)
//------------------------------------------------------------------------------
{
  return (rhs.m_TypeName < lhs.m_TypeName
    && rhs.m_OverrideWithName < lhs.m_OverrideWithName);
}

//------------------------------------------------------------------------------
/** Initialize static list of factories.*/
std::list<albaObjectFactory*>* 
  albaObjectFactory::m_RegisteredFactories = 0;

//------------------------------------------------------------------------------
// Create an instance of a named ALBA object using the loaded factories
albaObject *albaObjectFactory::CreateInstance(const char* classname)
//------------------------------------------------------------------------------
{
  if ( !albaObjectFactory::m_RegisteredFactories )
  {
    albaObjectFactory::Initialize();
  }
  
  for ( std::list<albaObjectFactory*>::iterator 
      i = m_RegisteredFactories->begin();
      i != m_RegisteredFactories->end(); ++i )
  {
    albaObject *newobject = (*i)->CreateObject(classname);
    if(newobject)
    {
      return newobject;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
std::list<albaObject *> albaObjectFactory::CreateAllInstance(const char* classname)
//------------------------------------------------------------------------------
{
  if ( !albaObjectFactory::m_RegisteredFactories )
  {
    albaObjectFactory::Initialize();
  }
  std::list<albaObject *> created;
  for ( std::list<albaObjectFactory*>::iterator 
          i = m_RegisteredFactories->begin();
        i != m_RegisteredFactories->end(); ++i )
    {
    albaObject *newobject = (*i)->CreateObject(classname);
    if(newobject)
    {
      created.push_back(newobject);
    }
  }
  return created;
}


//------------------------------------------------------------------------------
// A one time initialization method.
void albaObjectFactory::Initialize()
//------------------------------------------------------------------------------
{
  CleanUpObjectFactoryGlobal.Use();
  /**
   * Don't do anything if we are already initialized
   */
  if ( albaObjectFactory::m_RegisteredFactories )
  {
    return;
  }
  
  albaObjectFactory::m_RegisteredFactories = new std::list<albaObjectFactory*>;
  albaObjectFactory::RegisterDefaults();
  albaObjectFactory::LoadDynamicFactories();
}


//------------------------------------------------------------------------------
// Register any factories that are always present in ALBA like
// the OpenGL factory, currently this is not done.
void albaObjectFactory::RegisterDefaults()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
// Load all libraries in ALBA_AUTOLOAD_PATH
void albaObjectFactory::LoadDynamicFactories()
//------------------------------------------------------------------------------
{
   //
   // follow PATH conventions
   //
#ifdef _WIN32
  char PathSeparator = ';';
#else
  char PathSeparator = ':';
#endif
  
  std::string LoadPath;
  if (getenv("ALBA_AUTOLOAD_PATH"))
  {
    LoadPath = getenv("ALBA_AUTOLOAD_PATH");
  }
  else
  {
    return;
  }

  if(LoadPath.size() == 0)
  {
    return;
  }
  std::string::size_type EndSeparatorPosition = 0;
  std::string::size_type StartSeparatorPosition = 0;
  while ( StartSeparatorPosition != std::string::npos )
  {
    StartSeparatorPosition = EndSeparatorPosition;
    //
    // find PathSeparator in LoadPath
    //
    EndSeparatorPosition = LoadPath.find(PathSeparator, 
                                         StartSeparatorPosition);
    if(EndSeparatorPosition == std::string::npos)
    {
      EndSeparatorPosition = LoadPath.size();
    }
    std::string CurrentPath = 
      LoadPath.substr(StartSeparatorPosition, EndSeparatorPosition);
    albaObjectFactory::LoadLibrariesInPath(CurrentPath.c_str());
    //
    // move past separator
    //
    if(EndSeparatorPosition == LoadPath.size())
    {
      StartSeparatorPosition = std::string::npos;
    }
    else
    {
      EndSeparatorPosition++;
    }
  }
}

//------------------------------------------------------------------------------
// A file scope helper function to concat path and file into
// a full path
static std::string CreateFullPath(const char* path, const char* file)
//------------------------------------------------------------------------------
{
  std::string ret;
#ifdef _WIN32
  const char sep = '\\';
#else
  const char sep = '/';
#endif
  //
  // make sure the end of path is a separator
  //
  ret = path;
  if ( ret[ret.size()-1] != sep )
  {
    ret.append(1, sep);
  }
  ret.append(file);
  return ret;
}


/** A file scope typedef to make the cast code to the load
  function cleaner to read. */
typedef albaObjectFactory* (* ALBA_LOAD_FUNCTION)();


//------------------------------------------------------------------------------
// A file scoped function to determine if a file has
// the shared library extension in its name, this converts name to lower
// case before the compare, albaDynamicLoader always uses
// lower case for LibExtension values.
inline bool NameIsSharedLibrary(const char* name)
//------------------------------------------------------------------------------
{
  std::string sname = name;
  if ( sname.find(albaDynamicLoader::LibExtension()) != std::string::npos )
    {
    return true;
    }
  return false;
}


//------------------------------------------------------------------------------
void albaObjectFactory::LoadLibrariesInPath(const char* path)
//------------------------------------------------------------------------------
{
  albaDirectory dir;
  if ( !dir.Load(path) )
  {
    return;
  }
  
  /**
   * Attempt to load each file in the directory as a shared library
   */
  for ( unsigned int i = 0; i < dir.GetNumberOfFiles(); i++ )
  {
    const char* file = dir.GetFile(i);
    /**
     * try to make sure the file has at least the extension
     * for a shared library in it.
     */
    if ( NameIsSharedLibrary(file) )
    {
      std::string fullpath = CreateFullPath(path, file);
      LibHandle lib = albaDynamicLoader::OpenLibrary(fullpath.c_str());
      if ( lib )
      {
        /**
        * Look for the symbol albaLoad in the library
        */
        ALBA_LOAD_FUNCTION loadfunction
          = (ALBA_LOAD_FUNCTION)albaDynamicLoader::GetSymbolAddress(lib, "albaLoad");
        /**
        * if the symbol is found call it to create the factory
        * from the library
        */
        if ( loadfunction )
        {
          albaObjectFactory* newfactory = (*loadfunction)();
          /**
          * initialize class members if load worked
          */
          newfactory->m_LibraryHandle = (void*)lib;
          newfactory->m_LibraryPath = fullpath;
          newfactory->m_LibraryDate = 0; // unused for now...
          albaObjectFactory::RegisterFactory(newfactory);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
// Recheck the ALBA_AUTOLOAD_PATH for new libraries
void albaObjectFactory::ReHash()
//------------------------------------------------------------------------------
{
  albaObjectFactory::UnRegisterAllFactories();
  albaObjectFactory::Initialize();
}


//------------------------------------------------------------------------------
// initialize class members
albaObjectFactory::albaObjectFactory()
//------------------------------------------------------------------------------
{
  m_LibraryHandle = 0;
  m_LibraryDate = 0;
  m_OverrideMap = new mmuOverRideMap;
}


//------------------------------------------------------------------------------
// Unload the library and free the path string
albaObjectFactory::~albaObjectFactory()
//------------------------------------------------------------------------------
{
  if(m_LibraryHandle)
    {
    albaDynamicLoader::CloseLibrary((LibHandle)m_LibraryHandle);
    }
  m_OverrideMap->erase(m_OverrideMap->begin(), m_OverrideMap->end());
  delete m_OverrideMap;
}


//------------------------------------------------------------------------------
// Add a factory to the registered list
void albaObjectFactory::RegisterFactory(albaObjectFactory* factory)
//------------------------------------------------------------------------------
{
	if (factory->m_LibraryHandle == 0)
	{
		const char* nonDynamicName = "Non-Dynamicly loaded factory";
		factory->m_LibraryPath = nonDynamicName;
	}
  
  albaObjectFactory::Initialize();
  albaObjectFactory::m_RegisteredFactories->push_back(factory);
  factory->Register(0);
}

//------------------------------------------------------------------------------
void albaObjectFactory::Print(std::ostream& os, const int indent) const
//------------------------------------------------------------------------------
{
  Superclass::Print(os, indent);

  os << indent << "Factory DLL path: " << m_LibraryPath.c_str() << "\n";
  os << indent << "Factory description: " << this->GetDescription() << std::endl;

  int num = static_cast<int>( m_OverrideMap->size() );
  os << indent << "Factory overrides " << num << " classes:" << std::endl;

  albaIndent next_indent = albaIndent(indent).GetNextIndent();
  for(mmuOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i)
    {
    os << next_indent << "Class : " <<  (*i).first.c_str() << "\n";
    os << next_indent << "Overridden with: " <<  (*i).second.m_OverrideWithName.c_str()
       << std::endl;
    os << next_indent << "Enable flag: " << (*i).second.m_EnabledFlag
       << std::endl;
    os << std::endl;
  }
}

//------------------------------------------------------------------------------
void albaObjectFactory::UnRegisterFactory(albaObjectFactory* factory)
//------------------------------------------------------------------------------
{ 
  for ( std::list<albaObjectFactory*>::iterator i = 
        m_RegisteredFactories->begin();
      i != m_RegisteredFactories->end(); ++i )
  {
    if ( factory == *i )
    {
      m_RegisteredFactories->remove(factory);
      factory->UnRegister(0);
      return;
    }
  }
}

//------------------------------------------------------------------------------
// unregister all factories and delete the RegisteredFactories list
void albaObjectFactory::UnRegisterAllFactories()
//------------------------------------------------------------------------------
{
  if ( albaObjectFactory::m_RegisteredFactories )
  {
    for ( std::list<albaObjectFactory*>::iterator i 
            = m_RegisteredFactories->begin();
          i != m_RegisteredFactories->end(); ++i )
    {
      (*i)->UnRegister(0);
    }
    delete albaObjectFactory::m_RegisteredFactories;
    albaObjectFactory::m_RegisteredFactories = 0;
  }
}

//------------------------------------------------------------------------------
void albaObjectFactory::RegisterOverride(const char* classOverride,
                   const char* subclass,
                   const char* typeName,
                   bool enableFlag,
                   albaCreateObjectFunction createFunction,
                   albaReferenceCounted *args)
//------------------------------------------------------------------------------
{
  albaObjectFactory::mmuOverrideInformation info;
  info.m_TypeName = typeName;
  info.m_OverrideWithName = subclass;
  info.m_EnabledFlag = enableFlag;
  info.m_CreateObject = createFunction;
  info.m_Args = args;
  m_OverrideMap->insert(mmuOverRideMap::value_type(classOverride, info));
}

//------------------------------------------------------------------------------
void albaObjectFactory::RegisterNewObject(const char* ObjectName, const char* typeName, albaCreateObjectFunction createFunction,albaReferenceCounted *args)
//------------------------------------------------------------------------------
{
  this->RegisterOverride(ObjectName,ObjectName, typeName,true,createFunction,args);
}

//------------------------------------------------------------------------------
albaObject *albaObjectFactory::CreateObject(const char* classname)
//------------------------------------------------------------------------------
{
  mmuOverRideMap::iterator pos = m_OverrideMap->find(classname);
  if ( pos != m_OverrideMap->end() )
  {
    return (*pos).second.m_CreateObject();
  }
  return NULL;
}


//------------------------------------------------------------------------------
std::string albaObjectFactory::GetObjectTypeName(const char* classname)
{
	mmuOverRideMap::iterator pos = m_OverrideMap->find(classname);
	if (pos != m_OverrideMap->end())
	{
		return (*pos).second.m_TypeName;
	}
	return NULL;
}


//------------------------------------------------------------------------------
void albaObjectFactory::SetEnableFlag(bool flag,
                const char* className,
                const char* subclassName)
//------------------------------------------------------------------------------
{
  mmuOverRideMap::iterator start = m_OverrideMap->lower_bound(className);
  mmuOverRideMap::iterator end = m_OverrideMap->upper_bound(className);
  for ( mmuOverRideMap::iterator i = start; i != end; ++i )
  {
    if ( (*i).second.m_OverrideWithName == subclassName )
    {
      (*i).second.m_EnabledFlag = flag;
    }
  }
}


//------------------------------------------------------------------------------
bool albaObjectFactory::GetEnableFlag(const char* className, const char* subclassName)
//------------------------------------------------------------------------------
{
  mmuOverRideMap::iterator start = m_OverrideMap->lower_bound(className);
  mmuOverRideMap::iterator end = m_OverrideMap->upper_bound(className);
  for ( mmuOverRideMap::iterator i = start; i != end; ++i )
  {
    if ( (*i).second.m_OverrideWithName == subclassName )
    {
      return (*i).second.m_EnabledFlag;
    }
  }
  return 0;
}


//------------------------------------------------------------------------------
void albaObjectFactory::Disable(const char* className)
//------------------------------------------------------------------------------
{
  mmuOverRideMap::iterator start = m_OverrideMap->lower_bound(className);
  mmuOverRideMap::iterator end = m_OverrideMap->upper_bound(className);
  for ( mmuOverRideMap::iterator i = start; i != end; ++i )
    {
    (*i).second.m_EnabledFlag = 0;
    }
}


//------------------------------------------------------------------------------
std::list<albaObjectFactory*> albaObjectFactory::GetRegisteredFactories()
//------------------------------------------------------------------------------
{
  return *albaObjectFactory::m_RegisteredFactories;
}


//------------------------------------------------------------------------------
// Return a list of classes that this factory overrides.
std::list<std::string> albaObjectFactory::GetClassOverrideNames()
//------------------------------------------------------------------------------
{
  std::list<std::string> ret;
  for ( mmuOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i )
  {
    ret.push_back((*i).first);
  }
  return ret;
}


//------------------------------------------------------------------------------
// Return a list of the names of classes that override classes.
std::list<std::string> albaObjectFactory::GetClassOverrideWithNames()
//------------------------------------------------------------------------------
{
  std::list<std::string> ret;
  for ( mmuOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i )
  {
    ret.push_back((*i).second.m_OverrideWithName);
  }
  return ret;
}


//------------------------------------------------------------------------------
// Return a list of typeNames for class overrides
std::list<std::string> albaObjectFactory::GetClassOverrideTypeNames()
//------------------------------------------------------------------------------
{ 
  std::list<std::string> ret;
  for ( mmuOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i )
  {
    ret.push_back((*i).second.m_TypeName);
  }
  return ret;
}


//------------------------------------------------------------------------------
// Return a list of enable flags
std::list<bool> albaObjectFactory::GetEnableFlags()
//------------------------------------------------------------------------------
{
  std::list<bool> ret;
  for( mmuOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i)
  {
    ret.push_back((*i).second.m_EnabledFlag);
  }
  return ret;
}

//------------------------------------------------------------------------------
albaReferenceCounted *albaObjectFactory::GetFactoryArgs(const char *type_name)
//------------------------------------------------------------------------------
{
  mmuOverRideMap::iterator i=m_OverrideMap->find(type_name);
  if (i!=m_OverrideMap->end())
  {
    return i->second.m_Args;
  }

  return NULL;
}

//------------------------------------------------------------------------------
albaReferenceCounted *albaObjectFactory::GetArgs(const char *type_name)
//------------------------------------------------------------------------------
{
  if ( !albaObjectFactory::m_RegisteredFactories )
  {
    albaObjectFactory::Initialize();
  }
  
  for ( std::list<albaObjectFactory*>::iterator 
      i = m_RegisteredFactories->begin();
      i != m_RegisteredFactories->end(); ++i )
  {
    albaReferenceCounted *args = (*i)->GetFactoryArgs(type_name);
    if(args)
    {
      return args;
    }
  }
  return NULL;
}
