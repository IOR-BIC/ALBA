// test.cpp - written and placed in the public domain by Wei Dai
#include "mafCrypt.h"
#ifdef MAF_USE_CRYPTO

#include "pch.h"

#include "default.h"
#include "md5.h"
#include "sha.h"
#include "ripemd.h"
#include "files.h"
#include "rng.h"
#include "hex.h"
#include "gzip.h"
#include "default.h"
#include "rsa.h"
#include "randpool.h"
#include "ida.h"
#include "socketft.h"

#include <iostream>

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)

const char * defaultPhrase="fattinonfostepervivercomebruti";


//----------------------------------------------------------------------------
void mafDefaultEncryptFile(const char *in, const char *out)
//----------------------------------------------------------------------------
{
	mafEncryptFile (in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafDefaultDecryptFile(const char *in, const char *out)
//----------------------------------------------------------------------------
{
	mafDecryptFile (in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafDefaultDecryptInMemory(const char *in, std::string &out)
//----------------------------------------------------------------------------
{
	mafDecryptInMemory (in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafDefaultEncryptFromMemory(const char *in, unsigned int len, const char *out)
//----------------------------------------------------------------------------
{
	mafEncryptFromMemory (in, len, out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafEncryptFile(const char *in, const char *out, const char *passPhrase)
//----------------------------------------------------------------------------
{
	FileSink *fsink=new FileSink(out);
	DefaultEncryptorWithMAC * mac =	new DefaultEncryptorWithMAC(passPhrase, fsink);
	
	FileSource f(in, true, mac);
}

//----------------------------------------------------------------------------
void mafDecryptFile(const char *in, const char *out, const char *passPhrase)
//----------------------------------------------------------------------------
{
	FileSink *fsink=new FileSink(out);
	DefaultDecryptorWithMAC  * mac =	new DefaultDecryptorWithMAC(passPhrase, fsink);

	FileSource f(in, true, mac);
}

//----------------------------------------------------------------------------
void mafDecryptInMemory(const char *in, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  StringSink *ssink = new StringSink(out);
	DefaultDecryptorWithMAC  * mac =	new DefaultDecryptorWithMAC(passPhrase, ssink);

	FileSource f(in, true, mac);
}

//----------------------------------------------------------------------------
void mafEncryptFromMemory(const char *in, unsigned int len, const char *out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  FileSink *fsink = new FileSink(out);
	DefaultEncryptorWithMAC  * mac =	new DefaultEncryptorWithMAC(passPhrase, fsink);

	StringSource s((const byte *)in, len, true, mac);
}
#endif