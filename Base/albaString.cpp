/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaString
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaString.h"
#include "albaMatrix.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "wx/wx.h"
#include <wx/string.h>
#include <string>

#define STRING_BUFFER_DIMENSION 4096

static char glo_AlbaStringBuffer[STRING_BUFFER_DIMENSION];

//----------------------------------------------------------------------------
albaString::~albaString()
//----------------------------------------------------------------------------
{
  if (m_CStr)
  {
    if (m_Size>0)
      delete[] m_CStr;

    m_CStr=NULL;
    m_ConstCStr=NULL;
  }
}

//----------------------------------------------------------------------------
albaString::albaString():m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
albaString::albaString(const albaString& src):m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
  Copy(src.GetCStr());
}
//----------------------------------------------------------------------------
albaString::albaString(const char *src):m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
  Copy(src);
}
//----------------------------------------------------------------------------
albaString::albaString(const double &num):m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
  NPrintf(32,"%.16g",num); // only 16 digits are represented to avoid dirty bits
}
#ifdef ALBA_USE_WX
//----------------------------------------------------------------------------
albaString::albaString(const wxString& str):m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
  Copy(str.char_str());
}
#endif

//----------------------------------------------------------------------------
albaString &albaString::operator=(const albaString &src)
//----------------------------------------------------------------------------
{
  Copy(src.GetCStr());
  return *this;
}
//----------------------------------------------------------------------------
albaString &albaString::operator=(const char *src)
//----------------------------------------------------------------------------
{
  Copy(src);
  return *this;
}
//----------------------------------------------------------------------------
albaString &albaString::operator=(const double &num)
//----------------------------------------------------------------------------
{
  NPrintf(32,"%.16g",num);
  return *this;
}
#ifdef ALBA_USE_WX
//----------------------------------------------------------------------------
albaString &albaString::operator=(const wxString &src)
//----------------------------------------------------------------------------
{
  Copy(src.char_str());
  return *this;
}
#endif
/*
//----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const albaString& s)
//----------------------------------------------------------------------------
{
  const char *str=s.GetCStr();
  os << str;
  return os;
}

//----------------------------------------------------------------------------
void albaString::operator>>(std::istream &is)
//----------------------------------------------------------------------------
{
  std::string tmp;
  is >> tmp;
  *this=tmp.char_str();
}
*/

//TODO: verify portability of _snprintf ---- replace with NPrintf
//Marco. 1-4-2005: chnged _snprintf with NPrintf()
//----------------------------------------------------------------------------
albaString &albaString::operator<<( int d )
//----------------------------------------------------------------------------
{
  albaString tmp;
  tmp.NPrintf(100,"%d",d);
  Append(tmp);
  return *this;
}
//----------------------------------------------------------------------------
albaString &albaString::operator<<( long d )
//----------------------------------------------------------------------------
{
  albaString tmp;
  tmp.NPrintf(100,"%d",d);
  Append(tmp);
  return *this;
}
//----------------------------------------------------------------------------
albaString &albaString::operator<<( float d )
//----------------------------------------------------------------------------
{
  albaString tmp;
  tmp.NPrintf(100,"%.7g",d);
  Append(tmp);
  return *this;
}
//----------------------------------------------------------------------------
albaString &albaString::operator<<( double d )
//----------------------------------------------------------------------------
{
  albaString tmp;
  tmp.NPrintf(100,"%.16g",d);
  Append(tmp);
  return *this;
}
//----------------------------------------------------------------------------
albaString &albaString::operator<<( albaMatrix mat )
//----------------------------------------------------------------------------
{
	for(int i=0; i<4;i++)
	{
		for(int j=0; j<4;j++)
		{
			albaString tmp;
			tmp.NPrintf(100,"%.16g ",mat.GetElement(i,j));
			Append(tmp);
		}
	}
	return *this;
}
/*
//----------------------------------------------------------------------------
albaString &albaString::operator<<( std::string s )
//----------------------------------------------------------------------------
{
  Append(s);
  return *this;
}
*/
//----------------------------------------------------------------------------
albaString &albaString::operator<<( albaString *s )
//----------------------------------------------------------------------------
{
  Append( s->m_CStr );
  return *this;
}

//----------------------------------------------------------------------------
albaString& albaString::operator+=(const char *s)
//----------------------------------------------------------------------------
{
  Append(s);
  return *this;
}

//----------------------------------------------------------------------------
const char * albaString::GetCStr() const
//----------------------------------------------------------------------------
{
  if (m_Size>0)
  {
    return m_CStr;
  }
  else
  { 
    return m_ConstCStr;
  }
}

//----------------------------------------------------------------------------
int albaString::SetMaxLength(albaID len)
//----------------------------------------------------------------------------
{
  return (len>=m_Size)?SetSize(len+1):0;
}

//----------------------------------------------------------------------------
int albaString::SetSize(albaID size)
//----------------------------------------------------------------------------
{
  char *tmp;

  if (size==0)
  {
    // if size set to 0 simply free the memory
    tmp="";
  }
  else
  {
    // allocate new memory
    tmp=new char[size];
    if (!tmp)
    {
      //Cannot allocate memory
      return -1;
    }
    *tmp='\0';

    // copy old contents
    if (m_CStr)
    {
      if (size<m_Size)
      {
        strncpy(tmp,m_CStr,size-1);
      }
      else
      {
        strcpy(tmp,m_CStr);
      }
    }
    else if (m_ConstCStr)
    {
      if (size<m_Size)
      {
        strncpy(tmp,m_ConstCStr,size-1);
      }
      else
      {
        strcpy(tmp,m_ConstCStr);
      }
    }

  }

  // free old memory if present
  if (m_CStr&&m_Size>0)
  {
    delete [] m_CStr;
  }

  // store new memory pointer and new memory size
  if (size>0)
  {
    m_CStr=tmp;
    m_ConstCStr=NULL;
  }
  else
  {
    m_ConstCStr=tmp;
    m_CStr=NULL;
  }
  m_Size=size;

  return 0;
}

//----------------------------------------------------------------------------
void albaString::ForceDuplicate()
//----------------------------------------------------------------------------
{
  if (m_CStr==NULL)
  {
    unsigned long len=Length();
    SetMaxLength(m_Size>=len?m_Size:len);    
  }
}
//----------------------------------------------------------------------------
char *albaString::GetNonConstCStr()
//----------------------------------------------------------------------------
{
  ForceDuplicate();
  return m_CStr;
}
//----------------------------------------------------------------------------
const albaID albaString::Length() const
//----------------------------------------------------------------------------
{
  return Length(GetCStr());
}

//----------------------------------------------------------------------------
albaString &albaString::Set(const char *a, bool release)
//----------------------------------------------------------------------------
{
  if (a!=GetCStr())
  {
    SetSize(0); // force memory release
    m_ConstCStr = a;
    //m_Size = release?Length(a)+1:0;
  }
  return *this;
}

//----------------------------------------------------------------------------
void albaString::SetCStr(char *a, bool release)
//----------------------------------------------------------------------------
{
  if (a!=GetCStr())
  {
    SetSize(0); // force memory release
    m_CStr = a;
    m_Size = release?Length(a)+1:0; // notice: I could have a (char *) that must not be released
  }
}
//----------------------------------------------------------------------------
// This method returns the size of string. If the string is empty,
// it returns 0. It can handle null pointers.
albaID albaString::Length(const char* str)
//----------------------------------------------------------------------------
{
  if ( !str )
  {
    return 0;
  }
  return static_cast<albaID>(strlen(str));
}

  
//----------------------------------------------------------------------------
// Description:
// Copy string to the other string.
void albaString::Copy(char* dest, const char* src)
//----------------------------------------------------------------------------
{
  if ( !dest )
  {
    return;
  }
  if ( !src )
  {
    *dest = '\0';
    return;
  }
  strcpy(dest, src);
}

//----------------------------------------------------------------------------
void albaString::Copy(const char* src)
//----------------------------------------------------------------------------
{
  if (src)
  {
    // If the available memory is not sufficient, relocate!
    unsigned long len=Length(src);
    if (len>=m_Size)
    {
      SetMaxLength(len);
    }

    Copy(m_CStr,src);
  }
  else
  {
    SetMaxLength(0);
    m_ConstCStr="";
  }
}

//----------------------------------------------------------------------------
void albaString::NCopy(const char* src,int n)
//----------------------------------------------------------------------------
{
  if (src)
  {
    SetMaxLength(n);
    strncpy(m_CStr,src,n);
    m_CStr[n]='\0';
  }
  else
  {
    // release memory
    SetSize(0);
  }
}

//----------------------------------------------------------------------------
// Description:
// This method makes a duplicate of the string similar to
// C function strdup but it uses new to create new string, so
// you can use delete to remove it. It returns empty string 
// "" if the input is empty.
char* albaString::Duplicate(const char* str)
//----------------------------------------------------------------------------
{    
  if ( str )
  {
    char *newstr = new char [ Length(str) + 1 ];
    Copy(newstr, str);
    return newstr;
  }
  return 0;
}

//----------------------------------------------------------------------------
void albaString::Duplicate(char * &store,const char *src,bool release)
//----------------------------------------------------------------------------
{
  // Avoid to relocate memory if not necessary
  //if ( store && src && (!strcmp(store,src))) {return;}
   
  // release old memory
  if (store&&release) { delete [] store; }

  // allocate new memory and copy the second string's content
  if (src)
  { 
    store = new char[strlen(src)+1];
    strcpy(store,src);
  }
  else 
  { 
    // in case of NULL string reset the output string pointer...
    store = NULL;
  }
}

//----------------------------------------------------------------------------
char* albaString::Duplicate() const
//---------------------------------------------------------------------------- 
{
  return Duplicate(m_Size>0?m_CStr:m_ConstCStr);
}


//----------------------------------------------------------------------------
// This method compare two strings. It is similar to strcmp,
// but it can handle null pointers.
int albaString::Compare(const char* str1, const char* str2)
//----------------------------------------------------------------------------
{
  if (!str1&&!str2)
    return 0;

  if ( !str1&&str2 )
  {
    return -1;
  }

  if ( !str2&&str1 )
  {
    return 1;
  }

  return strcmp(str1, str2);
}

//----------------------------------------------------------------------------
int albaString::Compare(const char* str) const
//----------------------------------------------------------------------------
{
  return Compare(m_Size>0?m_CStr:m_ConstCStr, str);
}

//----------------------------------------------------------------------------
bool albaString::Equals(const char* str) const
//----------------------------------------------------------------------------
{
  return Equals(m_Size>0?m_CStr:m_ConstCStr, str);
}

//----------------------------------------------------------------------------
// Check if the first string starts with the second one.
bool albaString::StartsWith(const char* str1, const char* str2)
//----------------------------------------------------------------------------
{
  if ( !str1 || !str2 || strlen(str1) < strlen(str2) )
  {
    return false;
  }
  return !strncmp(str1, str2, strlen(str2));  
}

//----------------------------------------------------------------------------
bool albaString::StartsWith(const char* str) const
//----------------------------------------------------------------------------
{ 
  return StartsWith(m_Size>0?m_CStr:m_ConstCStr, str);
}

//----------------------------------------------------------------------------
// Check if the first string starts with the second one.
bool albaString::EndsWith(const char* str1, const char* str2)
//----------------------------------------------------------------------------
{
  if ( !str1 || !str2 || strlen(str1) < strlen(str2) )
  {
    return false;
  }
  return !strncmp(str1 + (strlen(str1)-strlen(str2)), str2, strlen(str2));
}

//----------------------------------------------------------------------------
bool albaString::EndsWith(const char* str) const
//----------------------------------------------------------------------------
{
  return EndsWith(m_Size>0?m_CStr:m_ConstCStr, str);
}

//----------------------------------------------------------------------------
const char *albaString::BaseName() const
//----------------------------------------------------------------------------
{
  return BaseName(m_Size>0?m_CStr:m_ConstCStr);
}

//----------------------------------------------------------------------------
char* albaString::Append(const char* str1, const char* str2)
//----------------------------------------------------------------------------
{
  if ( !str1 && !str2 )
  {
    return NULL;
  }

  char *newstr = new char[ Length(str1) + Length(str2)+1];
  if ( !newstr )
  {
    return NULL;
  }

  newstr[0] = '\0';
  if ( str1 )
  {
    strcat(newstr, str1);
  }

  if ( str2 )
  {
    strcat(newstr, str2);
  }

  return newstr;
}

//----------------------------------------------------------------------------
albaString &albaString::Append(const char* str)
//----------------------------------------------------------------------------
{
  if ( !str )
  {
    return *this;
  }

  unsigned long newsize = Length(GetCStr()) + Length(str)+1;

  if (SetMaxLength(newsize))
  {
    assert(1);
  }

  if ( str )
  {
    strcat(m_CStr, str);
  }

  return *this;
}

//----------------------------------------------------------------------------
int albaString::FindFirst(const char *str) const
//----------------------------------------------------------------------------
{
  int len=Length();
  int len2=Length(str);

  for (int i=0;i<=len-len2;i++)
  {
    if (strncmp(&(GetCStr()[i]),str,len2)==0)
      return i;
  }
  return -1;
}

//----------------------------------------------------------------------------
int albaString::FindLast(const char *str) const  
//----------------------------------------------------------------------------
{
  int len=Length();
  int len2=Length(str);

  for (int i=0;i<=len-len2;i++)
  {
    if (strncmp(&(GetCStr()[len-len2-i-1]),str,len2)==0)
      return len-len2-i-1;
  }
  return -1;  
}

//----------------------------------------------------------------------------
void albaString::Erase(int start,int end)
//----------------------------------------------------------------------------
{
  ForceDuplicate(); // force allocating memory in case of m_Size=0
  int len=Length();
  if (end==-1||end>=len)
    end=len-1;

  if (start>end||end<0)
    return;

  // copy shift back the tail
  strcpy(&m_CStr[start],&m_CStr[end+1]);

}

//----------------------------------------------------------------------------
char *albaString::ParsePathName(char *str)
//----------------------------------------------------------------------------
{
  if (albaString::IsEmpty(str))
    return str;

 // for Windows platforms parse the string to substitute "/" and "\\" with the right one.
#ifdef _WIN32
  for (unsigned int i=0;i<Length(str);i++)
  {
    if (str[i]=='\\')
      str[i]='/';
  }
#endif

  return str;
}

//----------------------------------------------------------------------------
char *albaString::ParsePathName(albaString *str)
//----------------------------------------------------------------------------
{
  str->ForceDuplicate();
  return ParsePathName(str->m_CStr);
}

//----------------------------------------------------------------------------
char *albaString::ParsePathName()
//----------------------------------------------------------------------------
{
  return albaString::ParsePathName(this);
}

//----------------------------------------------------------------------------
void albaString::SetPathName(const char *str)
//----------------------------------------------------------------------------
{
  if (albaString::IsEmpty(str))
    return;

  Copy(str);

  ParsePathName(m_CStr);
}

//----------------------------------------------------------------------------
void albaString::SetPathName(albaString *str)
//----------------------------------------------------------------------------
{
  SetPathName(str->GetCStr());
}

//----------------------------------------------------------------------------
const char *albaString::BaseName(const char *filename)
//----------------------------------------------------------------------------
{
  if (filename==NULL)
    return NULL;

  //char *ptr = strrchr(filename,'/');
  const char *ptr = strrchr(filename,'/');
#ifdef _WIN32
  if (ptr == NULL)
  {
    ptr = strrchr(filename,'\\');
  }
#endif
  return (ptr == NULL ? filename : ptr + 1);
}

//----------------------------------------------------------------------------
void albaString::ExtractPathName()
//----------------------------------------------------------------------------
{
  *this = wxPathOnly(GetCStr());
}

//----------------------------------------------------------------------------
const bool albaString::operator==(const char *src) const
//----------------------------------------------------------------------------
{
  return Equals(GetCStr(),src);
}

//----------------------------------------------------------------------------
const bool albaString::operator!=(const char *src) const
//----------------------------------------------------------------------------
{
  return !Equals(GetCStr(),src);
}

//----------------------------------------------------------------------------
const bool albaString::operator<(const char *a) const
//----------------------------------------------------------------------------
{
  return Compare(a)<0;
}
//----------------------------------------------------------------------------
const bool albaString::operator>(const char *a) const
//----------------------------------------------------------------------------
{
  return Compare(a)>0;
}
//----------------------------------------------------------------------------
const bool albaString::operator<=(const char *a) const
//----------------------------------------------------------------------------
{
  return Compare(a)<=0;
}
//----------------------------------------------------------------------------
const bool albaString::operator>=(const char *a) const
//----------------------------------------------------------------------------
{
  return Compare(a)>=0;
}

//------------------------------------------------------------------------------
char & albaString::operator [] (const int i)
//------------------------------------------------------------------------------
{
  // check if the string is referencing a "const char *"
  if (!m_CStr)
    ForceDuplicate(); // force memory copying
    
  return m_CStr[i];
}

//------------------------------------------------------------------------------
const char albaString::operator [] (const int i) const
//------------------------------------------------------------------------------
{
  return GetCStr()[i];
}

//----------------------------------------------------------------------------
int albaString::FindChr(const int c) const
//----------------------------------------------------------------------------
{
  if (IsEmpty())
  {
    return -1;
  }
  char *pdest;
  pdest = strchr((char *)GetCStr(), c);
  int result = (int)(pdest - GetCStr());
  return result < 0 ? -1 : result;
//  return (IsEmpty()?-1:(strchr(GetCStr(),c)-GetCStr()));
}

//----------------------------------------------------------------------------
void albaString::Replace(char from, char to)
{
	char *str=(char *)GetCStr();
	int count=0;

	for (int i=0; str[i]!= '\0'; i++)
	{
		if (str[i] == from)
		{
			str[i]=to;
			count++;
		}
	}
}

//----------------------------------------------------------------------------
int albaString::FindLastChr(const int c) const
//----------------------------------------------------------------------------
{
  if (IsEmpty())
  {
    return -1;
  }
  char *pdest;
  pdest = strrchr((char *)GetCStr(), c);
  int result = (int)(pdest - GetCStr());
  return result < 0 ? -1 : result;
//  return (IsEmpty()?-1:(strrchr(GetCStr(),c)-GetCStr()));
}

//----------------------------------------------------------------------------
void albaString::Printf(const char *format, ...)
//----------------------------------------------------------------------------
{
  SetSize(0); // release old memory
  SetSize(2048); // Preallocate space. Sorry, maximum output string size is 2048...
  ALBA_PRINT_MACRO(format,m_CStr,2048);
  SetCStr(Duplicate(),true); // release extra memory
}

//----------------------------------------------------------------------------
wxString albaString::Format(const char *format, ...)
{
	ALBA_PRINT_MACRO(format,glo_AlbaStringBuffer,ALBA_STRING_BUFFER_SIZE);
	return wxString(glo_AlbaStringBuffer);
}

//----------------------------------------------------------------------------
void albaString::NPrintf(unsigned long size, const char *format, ...)
//----------------------------------------------------------------------------
{
  SetSize(0); // release old memory
  SetSize(size); // Pre-allocate space.
  ALBA_PRINT_MACRO(format,m_CStr,size);
}
