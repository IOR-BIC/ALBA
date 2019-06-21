/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCurlUtility
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaCurlUtility.h"

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
