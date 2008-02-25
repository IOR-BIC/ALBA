#ifndef __CRYPTOPP_MYFUNCS_H__
#define __CRYPTOPP_MYFUNCS_H__

#include "mafConfigure.h"

#include <string>
#ifdef MAF_USE_CRYPTO
  extern const char *mafDefaultPassPhrase();

  // MD5 Checksum calculation for files and strings
  extern void mafCalculateteChecksum(const char *filename, std::string &checksum_result);
  extern void mafCalculateteChecksum(const char *input_string, int input_len, std::string &checksum_result);

  // files encryption/decryption
  extern bool mafEncryptFile(const char *filename_in, const char *filename_out, const char *passPhrase);
  extern bool mafDecryptFile(const char *filename_in, const char *filename_out, const char *passPhrase);
  extern bool mafDecryptFileInMemory(const char *filename_in, std::string &out, const char *passPhrase);
  extern bool mafEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out, const char *passPhrase);

  extern bool mafDefaultEncryptFile(const char *filename_in, const char *filename_out);
  extern bool mafDefaultDecryptFile(const char *filename_in, const char *filename_out);
  extern bool mafDefaultDecryptFileInMemory(const char *filename_in, std::string &out);
  extern bool mafDefaultEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out);

  // strings encryption/decryption
  extern bool mafDecryptInMemory(const char *in, std::string &out, const char *passPhrase);
  extern bool mafEncryptFromMemory(const char *in, unsigned int len, std::string &out, const char *passPhrase);

  extern bool mafDefaultDecryptInMemory(const char *in, std::string &out);
  extern bool mafDefaultEncryptFromMemory(const char *in, unsigned int len, std::string &out);
#endif
#endif
