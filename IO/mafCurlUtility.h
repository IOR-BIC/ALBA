
/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCurlUtility.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 09:57:52 $
  Version:   $Revision: 1.2.6.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafCurlUtility_H__
#define __mafCurlUtility_H__

struct msfTreeSearchReult {
  char *memory;
  size_t size;
};

/** initalize a given LUT with the idx.th preset */
MAF_EXPORT extern size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

#endif
