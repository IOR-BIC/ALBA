/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafString.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-04 11:50:08 $
  Version:   $Revision: 1.12 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafString.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "wx/wx.h"
#include <wx/string.h>
#include <string>


//----------------------------------------------------------------------------
mafString::~mafString()
//----------------------------------------------------------------------------
{
  if (m_CStr)
  {
    if (m_Size>0)
      delete m_CStr;

    m_CStr=NULL;
    m_ConstCStr=NULL;
  }
}

//----------------------------------------------------------------------------
mafString::mafString():m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafString::mafString(const mafString& src):m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
  Copy(src.GetCStr());
}
//----------------------------------------------------------------------------
mafString::mafString(const char *src):m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
  Copy(src);
}
//----------------------------------------------------------------------------
mafString::mafString(const double &num):m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
  NPrintf(32,"%.16g",num);
}
#ifdef MAF_USE_WX
//----------------------------------------------------------------------------
mafString::mafString(const wxString& str):m_CStr(NULL),m_ConstCStr(""),m_Size(0)
//----------------------------------------------------------------------------
{
  Copy(str.c_str());
}
#endif

//----------------------------------------------------------------------------
mafString &mafString::operator=(const mafString &src)
//----------------------------------------------------------------------------
{
  Copy(src.GetCStr());
  return *this;
}
//----------------------------------------------------------------------------
mafString &mafString::operator=(const char *src)
//----------------------------------------------------------------------------
{
  Copy(src);
  return *this;
}
//----------------------------------------------------------------------------
mafString &mafString::operator=(const double &num)
//----------------------------------------------------------------------------
{
  NPrintf(32,"%.16g",num);
  return *this;
}
#ifdef MAF_USE_WX
//----------------------------------------------------------------------------
mafString &mafString::operator=(const wxString &src)
//----------------------------------------------------------------------------
{
  Copy(src.c_str());
  return *this;
}
#endif
/*
//----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const mafString& s)
//----------------------------------------------------------------------------
{
  const char *str=s.GetCStr();
  os << str;
  return os;
}

//----------------------------------------------------------------------------
void mafString::operator>>(std::istream &is)
//----------------------------------------------------------------------------
{
  std::string tmp;
  is >> tmp;
  *this=tmp.c_str();
}
*/
//SIL. 16-3-2005: begin
//TODO: verify portability of _snprintf ---- replace with NPrintf
//Marco. 1-4-2005: chnged _snprintf with NPrintf()
//----------------------------------------------------------------------------
mafString &mafString::operator<<( int d )
//----------------------------------------------------------------------------
{
  NPrintf(100,"%d",d);
  return *this;
}
//----------------------------------------------------------------------------
mafString &mafString::operator<<( long d )
//----------------------------------------------------------------------------
{
  NPrintf(100,"%d",d);
  return *this;
}
//----------------------------------------------------------------------------
mafString &mafString::operator<<( float d )
//----------------------------------------------------------------------------
{
  NPrintf(100,"%g",d);
  return *this;
}
//----------------------------------------------------------------------------
mafString &mafString::operator<<( double d )
//----------------------------------------------------------------------------
{
  NPrintf(100,"%g",d);
  return *this;
}
/*
//----------------------------------------------------------------------------
mafString &mafString::operator<<( std::string s )
//----------------------------------------------------------------------------
{
  Append(s);
  return *this;
}
*/
//----------------------------------------------------------------------------
mafString &mafString::operator<<( mafString *s )
//----------------------------------------------------------------------------
{
  Append( s->m_CStr );
  return *this;
}

//SIL. 16-3-2005: end

//----------------------------------------------------------------------------
const char * mafString::GetCStr() const
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
int mafString::SetMaxLength(mafID len)
//----------------------------------------------------------------------------
{
  return (len>=m_Size)?SetSize(len+1):0;
}

//----------------------------------------------------------------------------
int mafString::SetSize(mafID size)
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
    *tmp='\0';
    if (!tmp)
    {
      //Cannot allocate memory
      return -1;
    }

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
void mafString::ForceDuplicate()
//----------------------------------------------------------------------------
{
  if (m_CStr==NULL)
  {
    unsigned long len=Length();
    SetMaxLength(m_Size>=len?m_Size:len);    
  }
}
//----------------------------------------------------------------------------
char *mafString::GetNonConstCStr()
//----------------------------------------------------------------------------
{
  ForceDuplicate();
  return m_CStr;
}
//----------------------------------------------------------------------------
const mafID mafString::Length() const
//----------------------------------------------------------------------------
{
  return Length(GetCStr());
}

//----------------------------------------------------------------------------
mafString &mafString::Set(const char *a, bool release)
//----------------------------------------------------------------------------
{
  if (a!=GetCStr())
  {
    SetSize(0); // force memory release
    m_ConstCStr = a;
    m_Size = release?Length(a)+1:0;
  }
  return *this;
}

//----------------------------------------------------------------------------
void mafString::SetCStr(char *a, bool release)
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
mafID mafString::Length(const char* str)
//----------------------------------------------------------------------------
{
  if ( !str )
  {
    return 0;
  }
  return static_cast<mafID>(strlen(str));
}

  
//----------------------------------------------------------------------------
// Description:
// Copy string to the other string.
void mafString::Copy(char* dest, const char* src)
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
void mafString::Copy(const char* src)
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
void mafString::NCopy(const char* src,int n)
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
char* mafString::Duplicate(const char* str)
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
void mafString::Duplicate(char * &store,const char *src,bool release)
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
char* mafString::Duplicate() const
//---------------------------------------------------------------------------- 
{
  return Duplicate(m_Size>0?m_CStr:m_ConstCStr);
}


//----------------------------------------------------------------------------
// This method compare two strings. It is similar to strcmp,
// but it can handle null pointers.
int mafString::Compare(const char* str1, const char* str2)
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
int mafString::Compare(const char* str) const
//----------------------------------------------------------------------------
{
  return Compare(m_Size>0?m_CStr:m_ConstCStr, str);
}

//----------------------------------------------------------------------------
bool mafString::Equals(const char* str) const
//----------------------------------------------------------------------------
{
  return Equals(m_Size>0?m_CStr:m_ConstCStr, str);
}

//----------------------------------------------------------------------------
// Check if the first string starts with the second one.
bool mafString::StartsWith(const char* str1, const char* str2)
//----------------------------------------------------------------------------
{
  if ( !str1 || !str2 || strlen(str1) < strlen(str2) )
  {
    return false;
  }
  return !strncmp(str1, str2, strlen(str2));  
}

//----------------------------------------------------------------------------
bool mafString::StartsWith(const char* str) const
//----------------------------------------------------------------------------
{ 
  return StartsWith(m_Size>0?m_CStr:m_ConstCStr, str);
}

//----------------------------------------------------------------------------
// Check if the first string starts with the second one.
bool mafString::EndsWith(const char* str1, const char* str2)
//----------------------------------------------------------------------------
{
  if ( !str1 || !str2 || strlen(str1) < strlen(str2) )
  {
    return false;
  }
  return !strncmp(str1 + (strlen(str1)-strlen(str2)), str2, strlen(str2));
}

//----------------------------------------------------------------------------
bool mafString::EndsWith(const char* str) const
//----------------------------------------------------------------------------
{
  return EndsWith(m_Size>0?m_CStr:m_ConstCStr, str);
}

//----------------------------------------------------------------------------
const char *mafString::BaseName() const
//----------------------------------------------------------------------------
{
  return BaseName(m_Size>0?m_CStr:m_ConstCStr);
}

//----------------------------------------------------------------------------
char* mafString::Append(const char* str1, const char* str2)
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
void mafString::AppendPath(mafString *str)
//----------------------------------------------------------------------------
{
  AppendPath(str->m_Size>0?m_CStr:m_ConstCStr);
}

//----------------------------------------------------------------------------
mafString &mafString::Append(const char* str)
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
int mafString::FindFirst(const char *str) const
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
int mafString::FindLast(const char *str) const  
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
void mafString::Erase(int start,int end)
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
char *mafString::ParsePathName(char *str)
//----------------------------------------------------------------------------
{
  if (mafString::IsEmpty(str))
    return str;

  // parse the appended string to substitute "/" and "\\" with the right one.
  for (unsigned int i=0;i<Length(str);i++)
  {
#ifdef _WIN32
    if (str[i]=='/')
      str[i]='\\';
#else
    if (str[i]=='\\')
      str[i]='/';
#endif
  }

  return str;
}

//----------------------------------------------------------------------------
char *mafString::ParsePathName(mafString *str)
//----------------------------------------------------------------------------
{
  str->ForceDuplicate();
  return ParsePathName(str->m_CStr);
}

//----------------------------------------------------------------------------
char *mafString::ParsePathName()
//----------------------------------------------------------------------------
{
  return mafString::ParsePathName(this);
}

//----------------------------------------------------------------------------
void mafString::SetPathName(const char *str)
//----------------------------------------------------------------------------
{
  if (mafString::IsEmpty(str))
    return;

  Copy(str);

  ParsePathName(m_CStr);
}

//----------------------------------------------------------------------------
void mafString::SetPathName(mafString *str)
//----------------------------------------------------------------------------
{
  SetPathName(str->GetCStr());
}

//----------------------------------------------------------------------------
void mafString::AppendPath(const char *str)
//----------------------------------------------------------------------------
{
  if (mafString::IsEmpty(str))
    return;

#ifdef _WIN32
  if (!IsEmpty()&&!EndsWith("\\"))
  {
    Append("\\");
  }
  
  Append(str);

#else
  if (!IsEmpty()&&!EndsWith("/"))
  {
    Append("/");
  }

  Append(str);

#endif

  mafString::ParsePathName(m_CStr);
}

//----------------------------------------------------------------------------
const char *mafString::BaseName(const char *filename)
//----------------------------------------------------------------------------
{
if (filename==NULL)
  return NULL;

#ifdef _WIN32
  char *ptr=strrchr(filename,'\\');
#else
  char *ptr=strrchr(filename,'/');
#endif
  return (ptr==NULL?filename:ptr+1);
}

//----------------------------------------------------------------------------
void mafString::ExtractPathName()
//----------------------------------------------------------------------------
{
#ifdef _WIN32
  int idx=FindLastChr('\\');
#else
  int idx=FindLastChr('/');
#endif

  if (idx>=0)
  { 
    Erase(idx+1,-1);
  }
  else
  {
    Set("");
  }
}

//----------------------------------------------------------------------------
const bool mafString::operator==(const char *src) const
//----------------------------------------------------------------------------
{
  return Equals(GetCStr(),src);
}

//----------------------------------------------------------------------------
const bool mafString::operator!=(const char *src) const
//----------------------------------------------------------------------------
{
  return !Equals(GetCStr(),src);
}

//----------------------------------------------------------------------------
const bool mafString::operator<(const char *a) const
//----------------------------------------------------------------------------
{
  return Compare(a)<0;
}
//----------------------------------------------------------------------------
const bool mafString::operator>(const char *a) const
//----------------------------------------------------------------------------
{
  return Compare(a)>0;
}
//----------------------------------------------------------------------------
const bool mafString::operator<=(const char *a) const
//----------------------------------------------------------------------------
{
  return Compare(a)<=0;
}
//----------------------------------------------------------------------------
const bool mafString::operator>=(const char *a) const
//----------------------------------------------------------------------------
{
  return Compare(a)>=0;
}

//------------------------------------------------------------------------------
char & mafString::operator [] (const int i)
//------------------------------------------------------------------------------
{
  // check if the string is referencing a "const char *"
  if (!m_CStr)
    ForceDuplicate(); // force memory copying
    
  return m_CStr[i];
}

//------------------------------------------------------------------------------
const char mafString::operator [] (const int i) const
//------------------------------------------------------------------------------
{
  return GetCStr()[i];
}

//----------------------------------------------------------------------------
int mafString::FindChr(const int c) const
//----------------------------------------------------------------------------
{
  return (IsEmpty()?-1:(strchr(GetCStr(),c)-GetCStr())); // difference between pointers
}
//----------------------------------------------------------------------------
int mafString::FindLastChr(const int c) const
//----------------------------------------------------------------------------
{
  return (IsEmpty()?-1:(strrchr(GetCStr(),c)-GetCStr()));
}

//----------------------------------------------------------------------------
void mafString::Printf(const char *format, ...)
//----------------------------------------------------------------------------
{
  SetSize(0); // release old memory
  SetSize(2048); // Preallocate space. Sorry, maximum output string size is 2048...
  MAF_PRINT_MACRO(format,m_CStr,2048);
  Set(Duplicate(),true); // release extra memory
}

//----------------------------------------------------------------------------
void mafString::NPrintf(unsigned long size, const char *format, ...)
//----------------------------------------------------------------------------
{
  SetSize(0); // release old memory
  SetSize(size); // Pre-allocate space.
  MAF_PRINT_MACRO(format,m_CStr,size);
}

