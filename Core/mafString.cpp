/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafString.cpp,v $
  Language:  C++
  Date:      $Date: 2004-10-25 09:41:16 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafString.h"
#include <stdio.h>

//----------------------------------------------------------------------------
mafString::~mafString()
//----------------------------------------------------------------------------
{
  if (this->CStr)
  {
    delete this->CStr;
    this->CStr=NULL;
  }
}

//----------------------------------------------------------------------------
mafString::mafString(const mafString& src)
//----------------------------------------------------------------------------
{
  this->Initialize();
  this->Copy(src.CStr);
}
//----------------------------------------------------------------------------
mafString::mafString(const char *src)
//----------------------------------------------------------------------------
{
  this->Initialize();
  this->Copy(src);
}
//----------------------------------------------------------------------------
mafString::mafString(double num)
//----------------------------------------------------------------------------
{
  this->Initialize();
  char tmp[64];
  sprintf(tmp,"%.16g",num);
  this->Copy(tmp);
}

//----------------------------------------------------------------------------
// Description:
// This method returns the size of string. If the string is empty,
// it returns 0. It can handle null pointers.
mafID mafString::Length(const char* str)
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
{
  if ( !dest )
    {
    return;
    }
  if ( !src )
    {
    *dest = 0;
    return;
    }
  strcpy(dest, src);
}

//----------------------------------------------------------------------------
// Description:
// This method makes a duplicate of the string similar to
// C function strdup but it uses new to create new string, so
// you can use delete to remove it. It returns empty string 
// "" if the input is empty.
char* mafString::Duplicate(const char* str)
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
// Description:
// This method compare two strings. It is similar to strcmp,
// but it can handle null pointers.
int mafString::Compare(const char* str1, const char* str2)
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
// Description:
// Check if the first string starts with the second one.
int mafString::StartsWith(const char* str1, const char* str2)
{
  if ( !str1 || !str2 || strlen(str1) < strlen(str2) )
    {
    return 0;
    }
  return !strncmp(str1, str2, strlen(str2));  
}

//----------------------------------------------------------------------------
// Description:
// Check if the first string starts with the second one.
int mafString::EndsWith(const char* str1, const char* str2)
{
  if ( !str1 || !str2 || strlen(str1) < strlen(str2) )
    {
    return 0;
    }
  return !strncmp(str1 + (strlen(str1)-strlen(str2)), str2, strlen(str2));
}

//----------------------------------------------------------------------------
char* mafString::Append(const char* str1, const char* str2)
{
  if ( !str1 && !str2 )
    {
    return 0;
    }
  char *newstr = 
    new char[ Length(str1) + Length(str2)+1];
  if ( !newstr )
    {
    return 0;
    }
  newstr[0] = 0;
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
void mafString::Duplicate(char * &store,const char *src)
//----------------------------------------------------------------------------
{
  // Avoid to relocate memory if not necessary
  if ( store && src && (!strcmp(store,src))) {return;}
   
  // release old memory
  if (store) { delete [] store; }

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
void mafString::Copy(const char* src)
//----------------------------------------------------------------------------
{
  if (src)
  {
    // If the available memory is not sufficient, relocate!
    if (Length(src)>=this->Size)
    {
      this->Duplicate(this->CStr,src);
    }
    else
    {
      Copy(this->CStr,src);
    }
  }
}

//----------------------------------------------------------------------------
void mafString::NCopy(const char* src,int n)
//----------------------------------------------------------------------------
{
  if (src)
  {
    this->SetMaxLength(n);
    strncpy(this->CStr,src,n);
    this->CStr[n]='\0';
  }
  else
  {
    this->SetSize(0);
  }
}

//----------------------------------------------------------------------------
int mafString::SetSize(mafID size)
//----------------------------------------------------------------------------
{
  char *tmp;

  if (size==0)
  {
    // if size set to 0 simply free the memory
    tmp=NULL;
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

    // copy old contents
    if (this->CStr)
    {
      if (size<this->Size)
      {
        strncpy(tmp,this->CStr,size-1);
      }
      else
      {
        Copy(tmp,this->CStr);
      }
    }
  }

  // free old memory if present
  if (this->CStr)
  {
    delete [] this->CStr;
  }

  // store new memory pointer and new memory size
  this->CStr=tmp;
  this->Size=size;

  return 0;
}

//----------------------------------------------------------------------------
void mafString::Append(const char* str)
//----------------------------------------------------------------------------
{

  const char *newstr=Append(this->CStr,str);
  if (newstr)
  {
    if (this->CStr)
    {
      delete [] this->CStr;
    }
    this->CStr=(char *)newstr;
  
  }
}

//----------------------------------------------------------------------------
int mafString::FindFirst(mafString *str)
//----------------------------------------------------------------------------
{
  int len=this->Length();
  int len2=str->Length();

  for (int i=0;i<=len-len2;i++)
  {
    if (strcmp(&this->CStr[i],str->CStr)==0)
      return i;
  }
  return -1;
}

//----------------------------------------------------------------------------
int mafString::FindLast(mafString *str)
//----------------------------------------------------------------------------
{
  int len=this->Length();
  int len2=str->Length();

  for (int i=0;i<=len-len2;i++)
  {
    if (strcmp(&this->CStr[len-len2-i-1],str->CStr)==0)
      return i;
  }
  return -1;  
}

//----------------------------------------------------------------------------
void mafString::Erase(int start,int end)
//----------------------------------------------------------------------------
{
  int len=this->Length();

  if (end==-1||end>=len)
    end=len-1;

  if (start>end||end<0)
    return;

  // copy shift back the tail
  strcpy(&this->CStr[start],&this->CStr[end+1]);

}

//----------------------------------------------------------------------------
char *mafString::ParsePathName(char *str)
//----------------------------------------------------------------------------
{
  if (mafString::IsEmpty(str))
  {
    return str;
  }

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
void mafString::SetPathName(const char *str)
//----------------------------------------------------------------------------
{
  if (mafString::IsEmpty(str))
  {
    return;
  }

  this->Set(str);

  mafString::ParsePathName(this->CStr);
}

//----------------------------------------------------------------------------
void mafString::AppendPath(const char *str)
//----------------------------------------------------------------------------
{
  if (mafString::IsEmpty(str))
  {
    return;
  }

#ifdef _WIN32
  if (!this->IsEmpty()&&!this->EndsWith("\\"))
  {
    this->Append("\\");
  }
  
  this->Append(str);

#else
  if (!this->IsEmpty()&&!this->EndsWith("/"))
  {
    this->Append("/");
  }

  this->Append(str);

#endif

  mafString::ParsePathName(this->CStr);
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
  int idx=this->FindLastChr('\\');
#else
  int idx=this->FindLastChr('/');
#endif

  if (idx>=0)
  { 
    this->Erase(idx+1,-1);
  }
  else
  {
    this->Set("");
  }

}
