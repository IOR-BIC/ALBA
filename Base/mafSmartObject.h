/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSmartObject.h,v $
  Language:  C++
  Date:      $Date: 2004-11-15 08:19:08 $
  Version:   $Revision: 1.1 $
  Authors:   based on vtkObjectBase (www.vtk.org), adapted Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafSmartObject_h
#define __mafSmartObject_h

//#include "mafIndent.h"
#include "mafObject.h"

/** mafSmartObject - abstract base class for MAF objects with reference counting.
  mafSmartObject is the base class for all reference counted classes
  in the MAF. mafSmartObjects are also mafObjects, thus implementing all RTTI APIs.
  mafSmartObject performs reference counting: objects that are
  reference counted exist as long as another object uses them. Once
  the last reference to a reference counted object is removed, the
  object will spontaneously destruct.
  
  Constructor and destructor of the subclasses of mafSmartObject
  should be protected, so that only New() and UnRegister() actually
  call them. Debug leaks can be used to see if there are any objects
  left with nonzero reference count.
 
  .SECTION Caveats
  Note: Objects of subclasses of mafSmartObject should always be
  created with the New() method and deleted with the Delete()
  method. They cannot be allocated off the stack (i.e., automatic
  objects) because the constructor is a protected method.
 
  @sa  mafObject */
class MAF_EXPORT mafSmartObject: public mafObject 
{
public:
  mafTypeMacro(mafSmartObject,mafObject);

  // Description:
  // Delete a MAF object.  This method should always be used to delete
  // an object when the New() method was used to create it. Using the
  // C++ delete method will not work with reference counting.
  virtual void Delete();

  // Description:
  // Create an object with Debug turned off, modified time initialized 
  // to zero, and reference counting on.
  static mafSmartObject *New() 
    {return new mafSmartObject;}
  
#ifdef _WIN32
  // avoid dll boundary problems
  void* operator new( size_t tSize );
  void operator delete( void* p );
#endif 
  
  // Description:
  // Print an object to an ostream. This is the method to call
  // when you wish to see print the internal state of an object.
  void Print(ostream& os);

  // Description:
  // Methods invoked by print to print information about the object
  // including superclasses. Typically not called by the user (use
  // Print() instead) but used in the hierarchical print process to
  // combine the output of several classes.
  virtual void PrintSelf(ostream& os, mafIndent indent);
  virtual void PrintHeader(ostream& os, mafIndent indent);
  virtual void PrintTrailer(ostream& os, mafIndent indent);

  // Description:
  // Increase the reference count (mark as used by another object).
  void Register(mafSmartObject* o);

  // Description:
  // Decrease the reference count (release by another object). This
  // has the same effect as invoking Delete() (i.e., it reduces the
  // reference count by 1).
  virtual void UnRegister(mafSmartObject* o);

  // Description:
  // Return the current reference count of this object.
  int  GetReferenceCount() 
    {return this->ReferenceCount;}

  // Description:
  // Sets the reference count. (This is very dangerous, use with care.)
  void SetReferenceCount(int);
  
  // Description:
  // Prints a list of the class .cxx file CVS revisions for all
  // classes in the object's inheritance chain.  The format of the
  // list is "mafSmartObject 1.4\n" with one class per line.  The list
  // always starts with the least-derived class (mafSmartObject), and
  // ends with the most-derived class.  This is useful for programs
  // wishing to do serialization of MAF objects.
  void PrintRevisions(ostream& os);
  
protected:
  mafSmartObject(); 
  virtual ~mafSmartObject(); 

  virtual void CollectRevisions(ostream& os);
  
  int ReferenceCount;      // Number of uses of this object by other objects

private:
  //BTX
  friend MAF_EXPORT ostream& operator<<(ostream& os, mafSmartObject& o);
  //ETX

protected:
//BTX
  mafSmartObject(const mafSmartObject&) {}
  void operator=(const mafSmartObject&) {}
//ETX
};

#endif

