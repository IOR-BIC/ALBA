/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObjectFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-15 08:19:07 $
  Version:   $Revision: 1.3 $
  Authors:   Based on itkObjectFactory (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafObjectFactory.h"
#include "mafDynamicLoader.h"
#include "mafDirectory.h"
#include "mafVersion.h"
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>
#include <map>

/** Utility class to clean up factory memory.*/  
class CleanUpObjectFactory
{
public:
  inline void Use() 
  {
  }
  ~CleanUpObjectFactory()
  {
      mafObjectFactory::UnRegisterAllFactories();
  }  
};


// Static object used to clean up memory at program closing
static CleanUpObjectFactory CleanUpObjectFactoryGlobal;

//------------------------------------------------------------------------------
// Add this for the SGI compiler which does not seem
// to provide a default implementation as it should.
//
bool operator==(const mafObjectFactory::mafOverrideInformation& rhs, 
    const mafObjectFactory::mafOverrideInformation& lhs)
//------------------------------------------------------------------------------
{
  return (rhs.m_Description == lhs.m_Description
    && rhs.m_OverrideWithName == lhs.m_OverrideWithName);
}

//------------------------------------------------------------------------------
// Add this for the SGI compiler which does not seem
// to provide a default implementation as it should.
bool operator<(const mafObjectFactory::mafOverrideInformation& rhs, 
    const mafObjectFactory::mafOverrideInformation& lhs)
//------------------------------------------------------------------------------
{
  return (rhs.m_Description < lhs.m_Description
    && rhs.m_OverrideWithName < lhs.m_OverrideWithName);
}


/** mafStringOverMap - Internal implementation class for ObjectFactorBase.
  Create a sub class to shrink the size of the symbols
  Also, so a forward reference can be put in mafObjectFactory.h
  and a pointer member can be used.  This avoids other
  classes including <map> and getting long symbol warnings.*/
typedef std::multimap<std::string, mafObjectFactory::mafOverrideInformation> 
              mafStringOverMapType;

/** mafOverRideMap - Internal implementation class for ObjectFactorBase. */
class mafOverRideMap : public mafStringOverMapType
{
public:
};

/** Initialize static list of factories.*/
std::list<mafObjectFactory*>* 
  mafObjectFactory::m_RegisteredFactories = 0;

//------------------------------------------------------------------------------
// Create an instance of a named MAF object using the loaded factories
mafObject *mafObjectFactory::CreateInstance(const char* classname)
//------------------------------------------------------------------------------
{
  if ( !mafObjectFactory::m_RegisteredFactories )
  {
    mafObjectFactory::Initialize();
  }
  
  for ( std::list<mafObjectFactory*>::iterator 
      i = m_RegisteredFactories->begin();
      i != m_RegisteredFactories->end(); ++i )
  {
    mafObject *newobject = (*i)->CreateObject(classname);
    if(newobject)
    {
      return newobject;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
std::list<mafObject *> mafObjectFactory::CreateAllInstance(const char* classname)
//------------------------------------------------------------------------------
{
  if ( !mafObjectFactory::m_RegisteredFactories )
  {
    mafObjectFactory::Initialize();
  }
  std::list<mafObject *> created;
  for ( std::list<mafObjectFactory*>::iterator 
          i = m_RegisteredFactories->begin();
        i != m_RegisteredFactories->end(); ++i )
    {
    mafObject *newobject = (*i)->CreateObject(classname);
    if(newobject)
    {
      created.push_back(newobject);
    }
  }
  return created;
}


//------------------------------------------------------------------------------
// A one time initialization method.
void mafObjectFactory::Initialize()
//------------------------------------------------------------------------------
{
  CleanUpObjectFactoryGlobal.Use();
  /**
   * Don't do anything if we are already initialized
   */
  if ( mafObjectFactory::m_RegisteredFactories )
  {
    return;
  }
  
  mafObjectFactory::m_RegisteredFactories = new std::list<mafObjectFactory*>;
  mafObjectFactory::RegisterDefaults();
  mafObjectFactory::LoadDynamicFactories();
}


//------------------------------------------------------------------------------
// Register any factories that are always present in MAF like
// the OpenGL factory, currently this is not done.
void mafObjectFactory::RegisterDefaults()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
// Load all libraries in MAF_AUTOLOAD_PATH
void mafObjectFactory::LoadDynamicFactories()
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
  if (getenv("MAF_AUTOLOAD_PATH"))
  {
    LoadPath = getenv("MAF_AUTOLOAD_PATH");
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
    mafObjectFactory::LoadLibrariesInPath(CurrentPath.c_str());
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
typedef mafObjectFactory* (* MAF_LOAD_FUNCTION)();


//------------------------------------------------------------------------------
// A file scoped function to determine if a file has
// the shared library extension in its name, this converts name to lower
// case before the compare, mafDynamicLoader always uses
// lower case for LibExtension values.
inline bool NameIsSharedLibrary(const char* name)
//------------------------------------------------------------------------------
{
  std::string sname = name;
  if ( sname.find(mafDynamicLoader::LibExtension()) != std::string::npos )
    {
    return true;
    }
  return false;
}


//------------------------------------------------------------------------------
void mafObjectFactory::LoadLibrariesInPath(const char* path)
//------------------------------------------------------------------------------
{
  mafDirectory dir;
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
      LibHandle lib = mafDynamicLoader::OpenLibrary(fullpath.c_str());
      if ( lib )
      {
        /**
        * Look for the symbol mafLoad in the library
        */
        MAF_LOAD_FUNCTION loadfunction
          = (MAF_LOAD_FUNCTION)mafDynamicLoader::GetSymbolAddress(lib, "mafLoad");
        /**
        * if the symbol is found call it to create the factory
        * from the library
        */
        if ( loadfunction )
        {
          mafObjectFactory* newfactory = (*loadfunction)();
          /**
          * initialize class members if load worked
          */
          newfactory->m_LibraryHandle = (void*)lib;
          newfactory->m_LibraryPath = fullpath;
          newfactory->m_LibraryDate = 0; // unused for now...
          mafObjectFactory::RegisterFactory(newfactory);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
// Recheck the MAF_AUTOLOAD_PATH for new libraries
void mafObjectFactory::ReHash()
//------------------------------------------------------------------------------
{
  mafObjectFactory::UnRegisterAllFactories();
  mafObjectFactory::Initialize();
}


//------------------------------------------------------------------------------
// initialize class members
mafObjectFactory::mafObjectFactory()
//------------------------------------------------------------------------------
{
  m_LibraryHandle = 0;
  m_LibraryDate = 0;
  m_OverrideMap = new mafOverRideMap;
}


//------------------------------------------------------------------------------
// Unload the library and free the path string
mafObjectFactory::~mafObjectFactory()
//------------------------------------------------------------------------------
{
  if(m_LibraryHandle)
    {
    mafDynamicLoader::CloseLibrary((LibHandle)m_LibraryHandle);
    }
  m_OverrideMap->erase(m_OverrideMap->begin(), m_OverrideMap->end());
  delete m_OverrideMap;
}


//------------------------------------------------------------------------------
// Add a factory to the registered list
void mafObjectFactory::RegisterFactory(mafObjectFactory* factory)
//------------------------------------------------------------------------------
{
  if ( factory->m_LibraryHandle == 0 )
    {
    const char* nonDynamicName = "Non-Dynamicly loaded factory";
    factory->m_LibraryPath = nonDynamicName;
    }
  if ( strcmp(factory->GetMAFSourceVersion(), 
            mafVersion::GetMAFSourceVersion()) != 0 )
    {
    /*mafGenericOutputMacro(<< "Possible incompatible factory load:" 
    << "\nRunning maf version :\n" << Version::GetMAFSourceVersion() 
    << "\nLoaded Factory version:\n" << factory->GetMAFSourceVersion()
    << "\nLoading factory:\n" << factory->m_LibraryPath << "\n");
    }*/
  mafObjectFactory::Initialize();
  mafObjectFactory::m_RegisteredFactories->push_back(factory);
  factory->Register();
}

//------------------------------------------------------------------------------
void mafObjectFactory::PrintSelf(std::ostream& os, mafIndent indent) const
//------------------------------------------------------------------------------
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Factory DLL path: " << m_LibraryPath.c_str() << "\n";
  os << indent << "Factory description: " << this->GetDescription() << std::endl;

  int num = static_cast<int>( m_OverrideMap->size() );
  os << indent << "Factory overides " << num << " classes:" << std::endl;

  indent = indent.GetNextIndent();
  for(mafOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i)
    {
    os << indent << "Class : " <<  (*i).first.c_str() << "\n";
    os << indent << "Overriden with: " <<  (*i).second.m_OverrideWithName.c_str()
       << std::endl;
    os << indent << "Enable flag: " << (*i).second.m_EnabledFlag
       << std::endl;
    os << std::endl;
  }
}
*/

//------------------------------------------------------------------------------
void mafObjectFactory::UnRegisterFactory(mafObjectFactory* factory)
//------------------------------------------------------------------------------
{ 
  for ( std::list<mafObjectFactory*>::iterator i = 
        m_RegisteredFactories->begin();
      i != m_RegisteredFactories->end(); ++i )
    {
    if ( factory == *i )
      {
      m_RegisteredFactories->remove(factory);
      factory->UnRegister();
      return;
      }
    }
}

//------------------------------------------------------------------------------
// unregister all factories and delete the RegisteredFactories list
void mafObjectFactory::UnRegisterAllFactories()
//------------------------------------------------------------------------------
{
  if ( mafObjectFactory::m_RegisteredFactories )
  {
    for ( std::list<mafObjectFactory*>::iterator i 
            = m_RegisteredFactories->begin();
          i != m_RegisteredFactories->end(); ++i )
    {
      (*i)->UnRegister();
    }
    delete mafObjectFactory::m_RegisteredFactories;
    mafObjectFactory::m_RegisteredFactories = 0;
  }
}

//------------------------------------------------------------------------------
void mafObjectFactory::RegisterOverride(const char* classOverride,
                   const char* subclass,
                   const char* description,
                   bool enableFlag,
                   mafCreateObjectFunction*
                   createFunction)
//------------------------------------------------------------------------------
{
  mafObjectFactory::mafOverrideInformation info;
  info.m_Description = description;
  info.m_OverrideWithName = subclass;
  info.m_EnabledFlag = enableFlag;
  info.m_CreateObject = createFunction;
  m_OverrideMap->insert(mafOverRideMap::value_type(classOverride, info));
}

//------------------------------------------------------------------------------
mafObject *mafObjectFactory::CreateObject(const char* classname)
//------------------------------------------------------------------------------
{
  m_OverrideMap->find(classname);
  mafOverRideMap::iterator pos = m_OverrideMap->find(classname);
  if ( pos != m_OverrideMap->end() )
    {
    return (*pos).second.m_CreateObject->CreateObject();
    }
  return 0;
}


//------------------------------------------------------------------------------
void mafObjectFactory::SetEnableFlag(bool flag,
                const char* className,
                const char* subclassName)
//------------------------------------------------------------------------------
{
  mafOverRideMap::iterator start = m_OverrideMap->lower_bound(className);
  mafOverRideMap::iterator end = m_OverrideMap->upper_bound(className);
  for ( mafOverRideMap::iterator i = start; i != end; ++i )
    {
    if ( (*i).second.m_OverrideWithName == subclassName )
      {
      (*i).second.m_EnabledFlag = flag;
      }
    }
}


//------------------------------------------------------------------------------
bool mafObjectFactory::GetEnableFlag(const char* className, const char* subclassName)
//------------------------------------------------------------------------------
{
  mafOverRideMap::iterator start = m_OverrideMap->lower_bound(className);
  mafOverRideMap::iterator end = m_OverrideMap->upper_bound(className);
  for ( mafOverRideMap::iterator i = start; i != end; ++i )
  {
    if ( (*i).second.m_OverrideWithName == subclassName )
    {
      return (*i).second.m_EnabledFlag;
    }
  }
  return 0;
}


//------------------------------------------------------------------------------
void mafObjectFactory::Disable(const char* className)
//------------------------------------------------------------------------------
{
  mafOverRideMap::iterator start = m_OverrideMap->lower_bound(className);
  mafOverRideMap::iterator end = m_OverrideMap->upper_bound(className);
  for ( mafOverRideMap::iterator i = start; i != end; ++i )
    {
    (*i).second.m_EnabledFlag = 0;
    }
}


//------------------------------------------------------------------------------
std::list<mafObjectFactory*> mafObjectFactory::GetRegisteredFactories()
//------------------------------------------------------------------------------
{
  return *mafObjectFactory::m_RegisteredFactories;
}


//------------------------------------------------------------------------------
// Return a list of classes that this factory overrides.
std::list<std::string> mafObjectFactory::GetClassOverrideNames()
//------------------------------------------------------------------------------
{
  std::list<std::string> ret;
  for ( mafOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i )
  {
    ret.push_back((*i).first);
  }
  return ret;
}


//------------------------------------------------------------------------------
// Return a list of the names of classes that override classes.
std::list<std::string> mafObjectFactory::GetClassOverrideWithNames()
//------------------------------------------------------------------------------
{
  std::list<std::string> ret;
  for ( mafOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i )
  {
    ret.push_back((*i).second.m_OverrideWithName);
  }
  return ret;
}


//------------------------------------------------------------------------------
// Return a list of descriptions for class overrides
std::list<std::string> mafObjectFactory::GetClassOverrideDescriptions()
//------------------------------------------------------------------------------
{ 
  std::list<std::string> ret;
  for ( mafOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i )
  {
    ret.push_back((*i).second.m_Description);
  }
  return ret;
}


//------------------------------------------------------------------------------
// Return a list of enable flags
std::list<bool> mafObjectFactory::GetEnableFlags()
//------------------------------------------------------------------------------
{
  std::list<bool> ret;
  for( mafOverRideMap::iterator i = m_OverrideMap->begin();
      i != m_OverrideMap->end(); ++i)
  {
    ret.push_back((*i).second.m_EnabledFlag);
  }
  return ret;
}
