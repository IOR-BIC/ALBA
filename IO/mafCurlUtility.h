
/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCurlUtility.h,v $
  Language:  C++
  Date:      $Date: 2006-11-02 11:26:01 $
  Version:   $Revision: 1.2 $
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
extern size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

#endif
