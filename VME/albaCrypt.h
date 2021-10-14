#ifndef __CRYPTOPP_MYFUNCS_H__
#define __CRYPTOPP_MYFUNCS_H__

#include "albaConfigure.h"

#include <string>
#ifdef ALBA_USE_CRYPTO
  ALBA_EXPORT extern const char *albaDefaultPassPhrase();

  // MD5 Checksum calculation for files and strings
  ALBA_EXPORT extern void albaCalculateteChecksum(const char *filename, std::string &checksum_result);
	ALBA_EXPORT extern void albaCalculateteChecksum(const char *input_string, int input_len, std::string &checksum_result);

	// SHA256 Hash calculation for files and strings
	ALBA_EXPORT extern void albaCalculateteSHA256(const char *filename, std::string &checksum_result);
	ALBA_EXPORT extern void albaCalculateteSHA256(const char *input_string, int input_len, std::string &checksum_result);

  // files encryption/decryption
  ALBA_EXPORT extern bool albaEncryptFile(const char *filename_in, const char *filename_out, const char *passPhrase);
  ALBA_EXPORT extern bool albaDecryptFile(const char *filename_in, const char *filename_out, const char *passPhrase);
  ALBA_EXPORT extern bool albaDecryptFileInMemory(const char *filename_in, std::string &out, const char *passPhrase);
  ALBA_EXPORT extern bool albaEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out, const char *passPhrase);

  ALBA_EXPORT extern bool albaDefaultEncryptFile(const char *filename_in, const char *filename_out);
  ALBA_EXPORT extern bool albaDefaultDecryptFile(const char *filename_in, const char *filename_out);
  ALBA_EXPORT extern bool albaDefaultDecryptFileInMemory(const char *filename_in, std::string &out);
  ALBA_EXPORT extern bool albaDefaultEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out);

  // strings encryption/decryption
  ALBA_EXPORT extern bool albaDecryptInMemory(const char *in, std::string &out, const char *passPhrase);
  ALBA_EXPORT extern bool albaEncryptFromMemory(const char *in, std::string &out, const char *passPhrase);

  ALBA_EXPORT extern bool albaDefaultDecryptInMemory(const char *in, std::string &out);
  ALBA_EXPORT extern bool albaDefaultEncryptFromMemory(const char *in, std::string &out);
#endif
#endif
