#ifndef __CRYPTOPP_MYFUNCS_H__
#define __CRYPTOPP_MYFUNCS_H__

#include "mafConfigure.h"

#include <string>
#ifdef MAF_USE_CRYPTO
  extern const char *mafDefaultPassPhrase();

  // files encryption/decryption
  extern void mafEncryptFile(const char *filename_in, const char *filename_out, const char *passPhrase);
  extern void mafDecryptFile(const char *filename_in, const char *filename_out, const char *passPhrase);
  extern void mafDecryptFileInMemory(const char *filename_in, std::string &out, const char *passPhrase);
  extern void mafEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out, const char *passPhrase);

  extern void mafDefaultEncryptFile(const char *filename_in, const char *filename_out);
  extern void mafDefaultDecryptFile(const char *filename_in, const char *filename_out);
  extern void mafDefaultDecryptFileInMemory(const char *filename_in, std::string &out);
  extern void mafDefaultEncryptFileFromMemory(const char *in, unsigned int len, const char *filename_out);

  // strings encryption/decryption
  extern void mafDecryptInMemory(const char *in, std::string &out, const char *passPhrase);
  extern void mafEncryptFromMemory(const char *in, unsigned int len, std::string &out, const char *passPhrase);

  extern void mafDefaultDecryptInMemory(const char *in, std::string &out);
  extern void mafDefaultEncryptFromMemory(const char *in, unsigned int len, std::string &out);
#endif
#endif
