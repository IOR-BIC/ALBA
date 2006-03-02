#ifndef __CRYPTOPP_MYFUNCS_H__
#define __CRYPTOPP_MYFUNCS_H__

#include "mafConfigure.h"

#include <string>
#ifdef MAF_USE_CRYPTO
  extern void mafEncryptFile(const char *in, const char *out, const char *passPhrase);
  extern void mafDecryptFile(const char *in, const char *out, const char *passPhrase);
  extern void mafDecryptInMemory(const char *in, std::string &out, const char *passPhrase);
  extern void mafEncryptFromMemory(const char *in, unsigned int len, const char *out, const char *passPhrase);

  extern void mafDefaultEncryptFile(const char *in, const char *out);
  extern void mafDefaultDecryptFile(const char *in, const char *out);
  extern void mafDefaultDecryptInMemory(const char *in, std::string &out);
  extern void mafDefaultEncryptFromMemory(const char *in, unsigned int len, const char *out);
#endif
#endif
