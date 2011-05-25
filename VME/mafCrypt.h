#ifndef __CRYPTOPP_MYFUNCS_H__
#define __CRYPTOPP_MYFUNCS_H__

#include "mafConfigure.h"

#include <string>
#ifdef MAF_USE_CRYPTO
  MAF_EXPORT extern const char *mafDefaultPassPhrase();

  // MD5 Checksum calculation for files and strings
  MAF_EXPORT extern void mafCalculateteChecksum(const char *filename, std::string &checksum_result);
  MAF_EXPORT extern void mafCalculateteChecksum(const char *input_string, int input_len, std::string &checksum_result);

  // files encryption/decryption
  MAF_EXPORT extern bool mafEncryptFile(const char *filename_in, const char *filename_out, const char *passPhrase);
  MAF_EXPORT extern bool mafDecryptFile(const char *filename_in, const char *filename_out, const char *passPhrase);
  MAF_EXPORT extern bool mafDecryptFileInMemory(const char *filename_in, std::string &out, const char *passPhrase);
  MAF_EXPORT extern bool mafEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out, const char *passPhrase);

  MAF_EXPORT extern bool mafDefaultEncryptFile(const char *filename_in, const char *filename_out);
  MAF_EXPORT extern bool mafDefaultDecryptFile(const char *filename_in, const char *filename_out);
  MAF_EXPORT extern bool mafDefaultDecryptFileInMemory(const char *filename_in, std::string &out);
  MAF_EXPORT extern bool mafDefaultEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out);

  // strings encryption/decryption
  MAF_EXPORT extern bool mafDecryptInMemory(const char *in, std::string &out, const char *passPhrase);
  MAF_EXPORT extern bool mafEncryptFromMemory(const char *in, std::string &out, const char *passPhrase);

  MAF_EXPORT extern bool mafDefaultDecryptInMemory(const char *in, std::string &out);
  MAF_EXPORT extern bool mafDefaultEncryptFromMemory(const char *in, std::string &out);
#endif
#endif
