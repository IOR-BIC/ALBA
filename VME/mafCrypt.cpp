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
#include <wx/msgdlg.h>
#include <wx/intl.h>

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)

const char * defaultPhrase="fattinonfostepervivercomebruti";

//----------------------------------------------------------------------------
const char *mafDefaultPassPhrase()
//----------------------------------------------------------------------------
{
  return defaultPhrase;
}
//----------------------------------------------------------------------------
bool mafDefaultEncryptFile(const char *filename_in, const char *filename_out)
//----------------------------------------------------------------------------
{
	return mafEncryptFile(filename_in, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool mafDefaultDecryptFile(const char *filename_in, const char *filename_out)
//----------------------------------------------------------------------------
{
	return mafDecryptFile(filename_in, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool mafDefaultDecryptFileInMemory(const char *filename_in, std::string &out)
//----------------------------------------------------------------------------
{
	return mafDecryptFileInMemory(filename_in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool mafDefaultEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out)
//----------------------------------------------------------------------------
{
	return mafEncryptFileFromMemory(in, len, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool mafEncryptFile(const char *filename_in, const char *filename_out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result = true;
  try
  {
    FileSink *fsink = new FileSink(filename_out);
    DefaultEncryptorWithMAC * mac =	new DefaultEncryptorWithMAC(passPhrase, fsink);

    FileSource f(filename_in, true, mac);
  }
  catch (...)
  {
    result = false;
  }
  return result;
}

//----------------------------------------------------------------------------
bool mafDecryptFile(const char *filename_in, const char *filename_out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result = true;
  try
  {
    FileSink *fsink=new FileSink(filename_out);
    DefaultDecryptorWithMAC  * mac =	new DefaultDecryptorWithMAC(passPhrase, fsink);

    FileSource f(filename_in, true, mac);
  }
  catch (...)
  {
    result = false;
  }
  return result;
}

//----------------------------------------------------------------------------
bool mafDecryptFileInMemory(const char *filename_in, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result;
  try
  {
    StringSink *ssink = new StringSink(out);
    DefaultDecryptorWithMAC  * mac =	new DefaultDecryptorWithMAC(passPhrase, ssink);

    FileSource f(filename_in, true, mac);
  }
  catch (...)
  {
    result = false;
  }
  return result;
}

//----------------------------------------------------------------------------
bool mafEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result;
  try
  {
    FileSink *fsink = new FileSink(filename_out);
    DefaultEncryptorWithMAC  * mac =	new DefaultEncryptorWithMAC(passPhrase, fsink);

    StringSource s((const byte *)in, len, true, mac);
  }
  catch (...)
  {
    result = false;
  }
  return result;
}

//----------------------------------------------------------------------------
bool mafDefaultDecryptInMemory(const char *in, std::string &out)
//----------------------------------------------------------------------------
{
  return mafDecryptInMemory(in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool mafDefaultEncryptFromMemory(const char *in, unsigned int len, std::string &out)
//----------------------------------------------------------------------------
{
  return mafEncryptFromMemory(in, len, out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool mafDecryptInMemory(const char *in, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result;
  try
  {
    StringSink *ssink = new StringSink(out);
    DefaultDecryptorWithMAC  *mac =	new DefaultDecryptorWithMAC(passPhrase, ssink);

    StringSource s(in, true, mac);
  }
  catch (...)
  {
    result = false;
  }
  return result;
}

//----------------------------------------------------------------------------
bool mafEncryptFromMemory(const char *in, unsigned int len, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result;
  try
  {
    StringSink *ssink = new StringSink(out);
    DefaultEncryptorWithMAC  * mac =	new DefaultEncryptorWithMAC(passPhrase, ssink);

    StringSource s((const byte *)in, len, true, mac);
  }
  catch (...)
  {
    result = false;
  }
  return result;
}
#endif
