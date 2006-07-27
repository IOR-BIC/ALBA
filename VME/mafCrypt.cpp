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
void mafDefaultEncryptFile(const char *filename_in, const char *filename_out)
//----------------------------------------------------------------------------
{
	mafEncryptFile (filename_in, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafDefaultDecryptFile(const char *filename_in, const char *filename_out)
//----------------------------------------------------------------------------
{
	mafDecryptFile (filename_in, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafDefaultDecryptFileInMemory(const char *filename_in, std::string &out)
//----------------------------------------------------------------------------
{
	mafDecryptFileInMemory (filename_in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafDefaultEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out)
//----------------------------------------------------------------------------
{
	mafEncryptFileFromMemory (in, len, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafEncryptFile(const char *filename_in, const char *filename_out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  try
  {
    FileSink *fsink = new FileSink(filename_out);
    DefaultEncryptorWithMAC * mac =	new DefaultEncryptorWithMAC(passPhrase, fsink);

    FileSource f(filename_in, true, mac);
  }
  catch (...)
  {
  	wxMessageBox(_("Error occourred during the file Encryption!"),_("Error"));
  }
}

//----------------------------------------------------------------------------
void mafDecryptFile(const char *filename_in, const char *filename_out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  try
  {
    FileSink *fsink=new FileSink(filename_out);
    DefaultDecryptorWithMAC  * mac =	new DefaultDecryptorWithMAC(passPhrase, fsink);

    FileSource f(filename_in, true, mac);
  }
  catch (...)
  {
    wxMessageBox(_("Error occourred during the file Decryption!"),_("Error"));
  }
}

//----------------------------------------------------------------------------
void mafDecryptFileInMemory(const char *filename_in, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  try
  {
    StringSink *ssink = new StringSink(out);
    DefaultDecryptorWithMAC  * mac =	new DefaultDecryptorWithMAC(passPhrase, ssink);

    FileSource f(filename_in, true, mac);
  }
  catch (...)
  {
    wxMessageBox(_("Error occourred during the string Decryption!"),_("Error"));
  }
}

//----------------------------------------------------------------------------
void mafEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  try
  {
    FileSink *fsink = new FileSink(filename_out);
    DefaultEncryptorWithMAC  * mac =	new DefaultEncryptorWithMAC(passPhrase, fsink);

    StringSource s((const byte *)in, len, true, mac);
  }
  catch (...)
  {
    wxMessageBox(_("Error occourred during the string Encryption!"),_("Error"));
  }
}

//----------------------------------------------------------------------------
void mafDefaultDecryptInMemory(const char *in, std::string &out)
//----------------------------------------------------------------------------
{
  mafDecryptInMemory(in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafDefaultEncryptFromMemory(const char *in, unsigned int len, std::string &out)
//----------------------------------------------------------------------------
{
  mafEncryptFromMemory(in, len, out, defaultPhrase);
}

//----------------------------------------------------------------------------
void mafDecryptInMemory(const char *in, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  try
  {
    StringSink *ssink = new StringSink(out);
    DefaultDecryptorWithMAC  *mac =	new DefaultDecryptorWithMAC(passPhrase, ssink);

    StringSource s(in, true, mac);
  }
  catch (...)
  {
    wxMessageBox(_("Error occourred during the string Decryption!"),_("Error"));
  }
}

//----------------------------------------------------------------------------
void mafEncryptFromMemory(const char *in, unsigned int len, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  try
  {
    StringSink *ssink = new StringSink(out);
    DefaultEncryptorWithMAC  * mac =	new DefaultEncryptorWithMAC(passPhrase, ssink);

    StringSource s((const byte *)in, len, true, mac);
  }
  catch (...)
  {
    wxMessageBox(_("Error occourred during the string Encryption!"),_("Error"));
  }
}
#endif
