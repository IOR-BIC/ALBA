/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaString
 Authors: originally based on vtkString (www.vtk.org), rewritten Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaString_h
#define __albaString_h

#include "albaDefines.h"
#include "albaBase.h" 
#include <string.h>
#include "wx/string.h"

//----------------------------------------------------------
// forward references:
//----------------------------------------------------------
class albaMatrix;

/** albaString - performs common string operations on c-strings.
  albaString is an implementation of string which operates on a traditional
  c-string internally stored. At each moment this string can be retrieved with
  GetCStr(). The memory of this m_CStr is automatically garbaged. Originally based
  on vtkString. BEWARE, when albaString is assigned with Set() to a "const char *" this
  is simply referenced and not copied, this implies very high performance but can cause
  invalid  memory access: e.g. in case the "const char *" is a function argument.
  When a albaString storing a reference to a const char * is modified the string is 
  automatically copied into a new memory.
  This string can be passed as argument wherever a const char * is expected.
  @sa albaCString
  */
class ALBA_EXPORT albaString : public albaBase
{
public:
  
  /**
    This static method returns the size of c-string. If the string is empty,
    it returns 0. It can handle null pointers.*/
  static albaID Length(const char* str);
   
  /** This method returns the size of this string. */
  const albaID Length() const;

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
    an uninitialized pointer variable. Beware memory releasing
    must be performed by consumer*/
  static void Duplicate(char * &store,const char *src,bool release=true);
  
  /**
    Duplicate the string stored inside this object. Beware memory releasing
    must be performed by consumer */
  char* Duplicate() const;
 
  /** 
    This static method compare two strings. It is similar to strcmp, but it
    can handle null pointers. return 0 if str1 == str2, -1 if str1<str2,
    1 if str1>str2*/
  static int Compare(const char* str1, const char* str2); 

  /** 
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Return 0 if str equal this,
    -1 if str > this, 1 if str < this*/
  int Compare(const char* str) const;
  
  /**
    This static method compare two strings. It is similar to strcmp, but it
    can handle null pointers. Also it only returns C style true or
    false versus compare which returns also which one is greater.*/
  static bool Equals(const char* str1, const char* str2) { return Compare(str1, str2) == 0;}
  
  /**
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Also it only
    returns C style true or false versus compare which returns also which
    one is greater.*/
   bool Equals(const char* str) const;
  
  /** Static method to check if the first string starts with the second one.*/
  static bool StartsWith(const char* str1, const char* str2);
  /** Check if this string starts with the given one.*/
  bool StartsWith(const char* str) const;

  /** Static method to check if the first string ends with the second one.*/
  static bool EndsWith(const char* str1, const char* str2); 
  /** Check if this string ends with the given one.*/
  bool EndsWith(const char* str) const;

  /**
    Append two strings and produce a new one.  The consumer must delete
    the resulting string. The method returns 0 if inputs are empty or
    if there was an error.*/
  static char* Append(const char* str1, const char* str2);
  /** Append a new string to this string. */
  albaString &Append(const char* str);

  /** Scan the string for the first occurrence of the character */
  int FindChr(const int c) const;

	/** Replace each caracter from with to */
	void Replace(char from, char to);

	/** Scan the string for the first occurrence of the character */
  int FindLastChr(const int c) const;

  /** Find first occurrence of a substring */
  int FindFirst(const char *str) const;

  /** Find last occurrence of a substring */
  int FindLast(const char *str) const;

  /** Extract the base name of a filename string */
  static const char *BaseName(const char *filename);
  const char *BaseName() const;

  /** Extract the pathname from a filename string. Result is written inplace. */
  void ExtractPathName();

  /**
    parse the given string to substitute each (back)slash
    character with the right pathname separator.*/
  void SetPathName(const char *str);
  void SetPathName(albaString *str);

  /**
    parse the given string to substitute each (back)slash
    character with the right pathname separator.*/
  char *ParsePathName();
  static char *ParsePathName(char *str);
  char *ParsePathName(albaString *str);

  /** Force the string to create an internal duplication in place of reference to const char */
  void ForceDuplicate();
  
  /** Return the pointer to the internal c-string */
  const char * GetCStr() const;
  
  /** 
    Return the pointer to the internal c-string, but first force string
    the internal copy @sa ForceDulicate () */
  char *GetNonConstCStr();

  /** return the real memory size allocated for the internal c-string */
  int GetSize() const {return m_Size;};

  /**
    Pre-Allocate space for the internal c-string. The memory size is
    is given in terms of string length, that is one more character
    for the trailing '\0' is allocated. Previous data is retained.
    Memory is reallocated only if requested size is > of existing one.
    All the albaString methods resize this memory when necessary, but to
    improve performance it's possible to preallocate an enough large
    memory to store the data preventing reallocation of memory.
    Return 0 if OK, -1 in case of relocation problems */
  int SetMaxLength(albaID len);

  /**  return true if empty*/
  static bool IsEmpty(const char *str) { return (str?str[0]=='\0':true);};
  /**  return true if empty*/
  bool IsEmpty() const { return IsEmpty(GetCStr());};

  /** 
    Set the internal pointer to a give pointer. Second parameter allow 
    to force the release of the memory */
  albaString &Set(const char *a, bool release=false);
  
  /** this can be used only with non constant c-string */
  void SetCStr(char *a, bool release=false);

  /** Format given arguments according to format string. Format string format is
      that of vsprintf function */
  void Printf(const char *format, ...);

	static wxString Format(const char *format, ...);

  /** like Printf but faster (you can specify output string size) */ 
  void NPrintf(unsigned long size, const char *format, ...);

  /** this allows to convert a albaString to const char *. */
  operator const char*() const {return GetCStr();}

  /** 
    Direct access to single string elements for writing. This operator
    forces memory copy in case of internal const char reference. */
  char & operator [] (const int i);

  /** direct access to string single elements for reading */
  const char operator [] (const int i) const;

  const bool operator==(const char *src) const;
  const bool operator!=(const char *src) const;
  const bool operator<(const char *a) const;
  const bool operator>(const char *a) const;
  const bool operator<=(const char *a) const;
  const bool operator>=(const char *a) const;

  //friend ALBA_EXPORT std::ostream& operator<<(std::ostream& os, const albaString& s);
  //friend ALBA_EXPORT std::istream& operator>>(std::istream& is, albaString& s);

  albaString &operator<<(const char *a) {return Append(a);};

  void operator<<(std::ostream &os);
  void operator>>(std::istream &os);

  albaString& operator<<( int        d);
  albaString& operator<<( long       d);
  albaString& operator<<( float      d);
  albaString& operator<<( double     d);
	/** Put inside string a albaMatrix in row order 
	example:
  1 0 0 0
	0 1 0 0
	0 0 1 0
	0 0 0 1
	-> "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1"
	*/
	albaString& operator<<( albaMatrix     d);
  albaString& operator<<( albaString  *s);
 //void operator<<( std::string     s);

  albaString& operator+=(const char *s);

  albaString(const albaString &src);
  albaString(const char *src);
  albaString(const double &num);
#ifdef ALBA_USE_WX
  albaString(const wxString &str);
#endif

  albaString &operator=(const albaString &mat);
  albaString &operator=(const char *src);
  albaString &operator=(const double &num);
#ifdef ALBA_USE_WX
  albaString &operator=(const wxString &str);
#endif
  albaString();
  ~albaString();
protected:

  /** Allocate space for the internal c-string. */
  int SetSize(albaID size);

  void Initialize() {m_CStr=NULL;m_ConstCStr="";m_Size=0;};

  char *m_CStr;
  const char *m_ConstCStr;
  albaID m_Size;
};

/**
     Class Name : albaCString.
     this string class is thought to simply wrap a c-string: no memory copy.
*/
class albaCString : public albaString
{
public:
  /** constructor. */
  albaCString(const char *str) {Set(str);}
};

#endif

