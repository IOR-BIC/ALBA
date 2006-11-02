/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCurlUtility.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-02 11:26:01 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafCurlUtility.h"

void *safeRealloc(void *ptr, size_t size)
{
  // There might be a realloc() out there that doesn't like reallocing
  // NULL pointers, so we take care of it here
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

//----------------------------------------------------------------------------
size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
//----------------------------------------------------------------------------
{
  size_t realsize = size * nmemb;
  struct msfTreeSearchReult *mem = (struct msfTreeSearchReult *)data;

  mem->memory = (char *)safeRealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) 
  {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}
