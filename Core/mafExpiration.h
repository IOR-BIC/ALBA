/*=========================================================================

 Program: MAF2
 Module: mafExpiration
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafExpiration_H__
#define __mafExpiration_H__

#include "mafConfigure.h"

/**
  Class Name: mafExpiration.
  Abstract class that supplies API for generic expiration.
  Need to specify classes like mafExpiration*Modality* in which a modality of expiration
  is defined.
*/
class MAF_EXPORT mafExpiration  
{
public:
  /** Retrieve result of the expiration criteria */
	virtual bool HasExpired() = 0;
  /** Retrieve a textual information */
  virtual const char* GetInformation() = 0;

  /** constructor. */
	mafExpiration(){};
  /** destructor. */
	virtual ~mafExpiration(){};

private:

};
#endif // mafExpiration