

#ifndef __mafCoreFactory_h
#define __mafCoreFactory_h

#include "mafObjectFactory.h"

/** Object factory for objects in the MAF library */
class MAF_EXPORT mafCoreFactory : public mafObjectFactory
{
public: 
  mafTypeMacro(mafCoreFactory,mafObjectFactory);
  void PrintSelf(std::ostream& os, const int indent);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  /** return the instance pointer of the factory. return NULL if not iitialized yet */
  static mafCoreFactory *GetInstance() {return Instance;}

protected:
  mafCoreFactory();
  ~mafCoreFactory() { }

  static mafCoreFactory *Instance;
  
private:
  mafCoreFactory(const mafCoreFactory&);  // Not implemented.
  void operator=(const mafCoreFactory&);  // Not implemented.
};

/** Plug generic object into the MAF Core factory.*/
template <class T>
class MAF_EXPORT mafPlugObject
{
  public:
  mafPlugObject(const char *name) \
  { \
    mafCoreFactory *factory=mafCoreFactory::GetInstance(); \
    if (factory) \
      factory->RegisterNewObject(T::GetStaticTypeName(), name, T::NewObject); \
  }
};

#endif
