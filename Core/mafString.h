/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafString.h,v $
  Language:  C++
  Date:      $Date: 2004-11-09 06:43:10 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafString_h
#define __mafString_h

#include "mafDefines.h"
#include <string.h>


/** mafString - performs common string operations.
  mafString is an implementation of string which operates on a traditional
  c-string internally stored. At each moment this string can be retrived with
  GetCStr(). The memory of this m_CStr is automatically garbaged. Originally based
  on vtkString. */
class mafCore_EXPORT mafString
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
    delete to remove it. It returns 0 if the input is empty.*/
  static char* Duplicate(const char* str);

  /**
    This method makes a duplicate of the string similar to C function
    strdup but it uses new to create new string, so you can use
    delete to remove it. It returns 0 if the input is empty. The new
    string pointer is copied in the destination pointer. If this was
    already != NULL, the corresponding memory is released. This is useful
    to automatically manage garbage collection but beware to not provide 
    an uninitialized pointer variable.*/
  static void Duplicate(char * &store,const char *src);
  
  /**
    Duplicate the string stored inside this object.*/
  char* Duplicate() { return Duplicate(m_CStr);};
 
  /** 
    This static method compare two strings. It is similar to strcmp, but it
    can handle null pointers.*/
  static int Compare(const char* str1, const char* str2); 

  /** 
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers.*/
  int Compare(const char* str) { return Compare(m_CStr, str);};
  
  /**
    This static method compare two strings. It is similar to strcmp, but it
    can handle null pointers. Also it only returns C style true or
    false versus compare which returns also which one is greater.*/
  static int Equals(const char* str1, const char* str2){ return Compare(str1, str2) == 0;}
  
  /**
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Also it only
    returns C style true or false versus compare which returns also which
    one is greater.*/
  int Equals(const char* str) { return Equals(m_CStr, str); };
  
  /** Static method to check if the first string starts with the second one.*/
  static int StartsWith(const char* str1, const char* str2);
  /** Check if this string starts with the given one.*/
  int StartsWith(const char* str) { return StartsWith(m_CStr, str);}

  /** Static method to check if the first string ends with the second one.*/
  static int EndsWith(const char* str1, const char* str2);
  /** Check if this string ends with the given one.*/
  int EndsWith(const char* str) { return EndsWith(m_CStr, str);}

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
  int GetSize() {return Size;};

  /**
    Pre-Allocate space for the internal c-string. The memory size is
    is given in terms of string length, that is one more character
    for the trailing '\0' is allocated.
    Previous data is retained.
    All the methods resize this memory when necessary, but to
    improve performance it's possible to preallocate an enough large
    memory to store the data preventing reallocation of memory.*/
  int SetMaxLength(int len) {return SetSize(len+1);};

  /**  return true if empty*/
  static bool IsEmpty(const char *str) { return (str?str[0]=='\0':true);};
  /**  return true if empty*/
  bool IsEmpty() { return IsEmpty(m_CStr);};

  /** Set the string to the specified value*/
  void Set(const char *a) {*this=a;};
  void Set(double a) {*this=a;};

  /** this allows to convert a mafString to const char *. */
  operator const char*() const {return m_CStr;}  

  const bool operator==(const char *src);

  mafString &operator<<(const char *a) {return Append(a);};

  mafString(const mafString &src);
  mafString(const char *src);
  mafString(double num);

  mafString();
  ~mafString();
protected:

  /** Pre-Allocate space for the internal c-string. */
  int SetSize(mafID size);

  void Initialize() {m_CStr=NULL;Size=0;};

  char *m_CStr;
  mafID Size;
};

#endif
