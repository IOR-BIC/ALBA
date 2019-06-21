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


#ifndef __albaCurlUtility_H__
#define __albaCurlUtility_H__

struct msfTreeSearchReult {
  char *memory;
  size_t size;
};

/** initalize a given LUT with the idx.th preset */
ALBA_EXPORT extern size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

#endif
