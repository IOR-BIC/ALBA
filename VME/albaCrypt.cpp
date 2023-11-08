// test.cpp - written and placed in the public domain by Wei Dai
#include "albaCrypt.h"
#ifdef ALBA_USE_CRYPTO

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
#include "albaString.h"
#include <base64.h>

#include <iostream>
#include <wx/msgdlg.h>
#include <wx/intl.h>

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)

const char * defaultPhrase="fattinonfostepervivercomebruti";

//----------------------------------------------------------------------------
const char *albaDefaultPassPhrase()
//----------------------------------------------------------------------------
{
  return defaultPhrase;
}
//----------------------------------------------------------------------------
bool albaDefaultEncryptFile(const char *filename_in, const char *filename_out)
//----------------------------------------------------------------------------
{
	return albaEncryptFile(filename_in, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool albaDefaultDecryptFile(const char *filename_in, const char *filename_out)
//----------------------------------------------------------------------------
{
	return albaDecryptFile(filename_in, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool albaDefaultDecryptFileInMemory(const char *filename_in, std::string &out)
//----------------------------------------------------------------------------
{
	return albaDecryptFileInMemory(filename_in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool albaDefaultEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out)
//----------------------------------------------------------------------------
{
	return albaEncryptFileFromMemory(in, len, filename_out, defaultPhrase);
}

//----------------------------------------------------------------------------
ALBA_EXPORT extern void albaCalculateteSHA256(const char *filename, std::string &checksum_result)
{
	try
	{
		SHA256 hashSHA256;
		HashFilter filterMD5(hashSHA256);

		std::auto_ptr<ChannelSwitch>
			channelSwitch(new ChannelSwitch);

		channelSwitch->AddDefaultRoute(filterMD5);

		FileSource f(filename, true, channelSwitch.release());

		HexEncoder encoder(new StringSink(checksum_result), true);
		filterMD5.TransferTo(encoder);
	}
	catch (...)
	{
		wxString msg = albaString::Format(_("Problem on calculating SHA256 on file %s !!"), filename);
		wxMessageBox(msg, _("Error"));
	}

}

//----------------------------------------------------------------------------
ALBA_EXPORT extern void albaCalculateteSHA256(const char *input_string, int input_len, std::string &checksum_result)
{
	try
	{
		SHA256 hashSHA256;
		HashFilter filterMD5(hashSHA256);

		std::auto_ptr<ChannelSwitch>
			channelSwitch(new ChannelSwitch);

		channelSwitch->AddDefaultRoute(filterMD5);

		StringSource s((const byte *)input_string, input_len, true, channelSwitch.release());

		HexEncoder encoder(new StringSink(checksum_result), true);
		filterMD5.TransferTo(encoder);
	}
	catch (...)
	{
		wxMessageBox(_("Problem on calculating Checksum on string!!"), _("Error"));
	}
}

//----------------------------------------------------------------------------
bool albaEncryptFile(const char *filename_in, const char *filename_out, const char *passPhrase)
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
bool albaDecryptFile(const char *filename_in, const char *filename_out, const char *passPhrase)
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
bool albaDecryptFileInMemory(const char *filename_in, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result = true;
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
bool albaEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result = true;
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
bool albaDefaultDecryptInMemory(const char *in, std::string &out)
//----------------------------------------------------------------------------
{
  return albaDecryptInMemory(in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool albaDefaultEncryptFromMemory(const char *in, std::string &out)
//----------------------------------------------------------------------------
{
  return albaEncryptFromMemory(in, out, defaultPhrase);
}

//----------------------------------------------------------------------------
bool albaDecryptInMemory(const char *in, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result = true;
  try
  {
		Base64Decoder decryptor(new DefaultDecryptorWithMAC(passPhrase, new StringSink(out)));
		decryptor.Put((byte *)in, strlen(in));
		decryptor.MessageEnd();
  }
  catch (...)
  {
    result = false;
  }
  return result;
}

//----------------------------------------------------------------------------
bool albaEncryptFromMemory(const char *in, std::string &out, const char *passPhrase)
//----------------------------------------------------------------------------
{
  bool result = true;
  try
  {
		DefaultEncryptorWithMAC encryptor(passPhrase, new Base64Encoder(new StringSink(out)));
		encryptor.Put((byte *)in, strlen(in));
		encryptor.MessageEnd();
  }
  catch (...)
  {
    result = false;
  }
  return result;
}
//----------------------------------------------------------------------------
void albaCalculateteChecksum(const char *filename, std::string &checksum_result)
//----------------------------------------------------------------------------
{
  try
  {
		MD5 hashMD5;
		HashFilter filterMD5(hashMD5);

    std::auto_ptr<ChannelSwitch>
      channelSwitch(new ChannelSwitch);

    channelSwitch->AddDefaultRoute(filterMD5);

    FileSource f(filename,true, channelSwitch.release());

    HexEncoder encoder(new StringSink( checksum_result ), true);
    filterMD5.TransferTo( encoder );
  }
  catch (...)
  {
    wxString msg = albaString::Format(_("Problem on calculating Checksum on file %s !!"), filename);
    wxMessageBox(msg, _("Error"));
  }
}
//----------------------------------------------------------------------------
void albaCalculateteChecksum(const char *input_string, int input_len, std::string &checksum_result)
//----------------------------------------------------------------------------
{
  try
  {
    MD5 hashMD5;
    HashFilter filterMD5(hashMD5);

    std::auto_ptr<ChannelSwitch>
      channelSwitch(new ChannelSwitch);

    channelSwitch->AddDefaultRoute(filterMD5);

    StringSource s((const byte *)input_string, input_len, true, channelSwitch.release());

    HexEncoder encoder(new StringSink( checksum_result ), true);
    filterMD5.TransferTo( encoder );
  }
  catch (...)
  {
    wxMessageBox(_("Problem on calculating Checksum on string!!"), _("Error"));
  }
}
#endif
