/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafString.h,v $
  Language:  C++
  Date:      $Date: 2004-11-25 11:29:36 $
  Version:   $Revision: 1.8 $
  Authors:   originally based on vtkString (www.vtk.org), rewritten Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafString_h
#define __mafString_h

#include "mafDefines.h"
#include <string.h>

/** mafString - performs common string operations on c-strings.
  mafString is an implementation of string which operates on a traditional
  c-string internally stored. At each moment this string can be retrieved with
  GetCStr(). The memory of this m_CStr is automatically garbaged. Originally based
  on vtkString. BEWARE, when mafString is assigned to a "const char *" this is simply
  referenced and not copied, this implies very high performance but can cause invalid
  memory access: e.g. in case the "const char *" is a function argument. In this case
  you should explicitly make a copy with Copy() or explicitelly converting with mafString(). 
  */
class MAF_EXPORT mafString
{
public:
  
  /**
    This static method returns the size of c-string. If the string is empty,
    it returns 0. It can handle null pointers.*/
  static mafID Length(const char* str);
   
  /** This method returns the size of this string. */
  const mafID Length();

  /** static method to copy c-string to the another c-string.*/
  static void Copy(char* dest, const char* src);
  
  /** Copy a c-string to this string.*/
  void Copy(const char* src);

  /** Copy N characters of another string to this string.*/
  void NCopy(const char* src,int n);

  /**  Erase characters from start position to end position. If end
    is not specified erase to the end of the string.*/
  void Erase(int start,int end=-1);

  /**
    This method makes a duplicate of a c-string similar to C function
    strdup but it uses new to create new string, so you can use
    delete to remove it. It returns 0 if the input is empty. This function
    automatically release old pointed data (if not specified differently)*/
  static char* Duplicate(const char* str);

  /**
    This method makes a duplicate of the string similar to C function
    strdup but it uses new to create new string, so you can use
    delete to remove it. It returns 0 if the input is empty. The new
    string pointer is copied in the destination pointer. If this was
    already != NULL, the corresponding memory is released. This is useful
    to automatically manage garbage collection but beware to not provide 
    an uninitialized pointer variable.*/
  static void Duplicate(char * &store,const char *src,bool release=true);
  
  /**
    Duplicate the string stored inside this object.*/
  char* Duplicate() { return Duplicate(m_CStr);};
 
  /** 
    This static method compare two strings. It is similar to strcmp, but it
    can handle null pointers. return 0 if str1 == str2, -1 if str1<str2,
    1 if str1>str2*/
  static int Compare(const char* str1, const char* str2); 

  /** 
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Return 0 if str equal this,
    -1 if str > this, 1 if str < this*/
  int Compare(const char* str) { return Compare(m_CStr, str);};
  
  /**
    This static method compare two strings. It is similar to strcmp, but it
    can handle null pointers. Also it only returns C style true or
    false versus compare which returns also which one is greater.*/
  static bool Equals(const char* str1, const char* str2){ return Compare(str1, str2) == 0;}
  
  /**
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Also it only
    returns C style true or false versus compare which returns also which
    one is greater.*/
  bool Equals(const char* str) { return Equals(m_CStr, str); };
  
  /** Static method to check if the first string starts with the second one.*/
  static bool StartsWith(const char* str1, const char* str2);
  /** Check if this string starts with the given one.*/
  bool StartsWith(const char* str) { return StartsWith(m_CStr, str);}

  /** Static method to check if the first string ends with the second one.*/
  static bool EndsWith(const char* str1, const char* str2);
  /** Check if this string ends with the given one.*/
  bool EndsWith(const char* str) { return EndsWith(m_CStr, str);}

  /**
    Append two strings and produce a new one.  The consumer must delete
    the resulting string. The method returns 0 if inputs are empty or
    if there was an error.*/
  static char* Append(const char* str1, const char* str2);
  /** Append a new string to this string. */
  mafString &Append(const char* str);

  /** Scan the string for the first occurrence of the character */
  int FindChr(const int c);

  /** Scan the string for the first occurrence of the character */
  int FindLastChr(const int c);

  /** Find first occurrence of a substring */
  int FindFirst(const char *str);

  /** Find last occurrence of a substring */
  int FindLast(const char *str);

  /** Extract the base name of a filename string */
  static const char *BaseName(const char *filename);
  const char *BaseName() {return BaseName(m_CStr);}

  /** Extract the pathname from a filename string */
  void ExtractPathName();

  /**
    Concatenate a string prepending a "/" or "\", depending on the 
    local OS, if needed also parse the given string to substitute each (back)slash
    character with the right pathname separator.*/
  void AppendPath(const char *str);
  void AppendPath(mafString *str)
    { AppendPath(str->m_CStr);}

  /**
    parse the given string to substitute each (back)slash
    character with the right pathname separator.*/
  void SetPathName(const char *str);
  void SetPathName(mafString *str)
    { SetPathName(str->m_CStr);}

  /**
    parse the given string to substitute each (back)slash
    character with the right pathname separator.*/
  void ParsePathName() {mafString::ParsePathName(this);}
  static char *ParsePathName(char *str);
  static char *ParsePathName(mafString *str)
    { return mafString::ParsePathName(str->m_CStr);}

  //void SPrintf(const char *format,...);

  /** Return the pointer to the internal c-string */
  char * GetCStr() {return m_CStr;};

  /** return the real memory size allocated for the internal c-string */
  int GetSize() {return m_Size;};

  /**
    Pre-Allocate space for the internal c-string. The memory size is
    is given in terms of string length, that is one more character
    for the trailing '\0' is allocated. Previous data is retained.
    Memory is reallocated only if requested size is > of existing one.
    All the mafString methods resize this memory when necessary, but to
    improve performance it's possible to preallocate an enough large
    memory to store the data preventing reallocation of memory.
    Return 0 if OK, -1 in case of relocation problems */
  int SetMaxLength(mafID len);

  /**  return true if empty*/
  static bool IsEmpty(const char *str) { return (str?str[0]=='\0':true);};
  /**  return true if empty*/
  bool IsEmpty() { return IsEmpty(m_CStr);};

  /** 
    Set the internal pointer to a give pointer. Second parameter allow 
    to force the release of the memory */
  void Set(const char *a, bool release=false);

  /** Format given arguments according to format string. Format string format is
      that of vsprintf function */
  void Printf(const char *format, ...);

  /** like Printf but faster (you can specify output string size) */ 
  void NPrintf(unsigned long size, const char *format, ...);

  /** this allows to convert a mafString to const char *. */
  operator const char*() const {return m_CStr;}  

  const bool operator==(const char *src);
  const bool operator<(const char *a);
  const bool operator>(const char *a);
  const bool operator<=(const char *a);
  const bool operator>=(const char *a);

  mafString &operator<<(const char *a) {return Append(a);};

  mafString(const mafString &src);
  mafString(const char *src);
  mafString(double num);

  mafString();
  ~mafString();
protected:

  /** Allocate space for the internal c-string. */
  int SetSize(mafID size);

  void Initialize() {m_CStr=NULL;m_Size=0;};

  char *m_CStr;
  mafID m_Size;
};

#endif
