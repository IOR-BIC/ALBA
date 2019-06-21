/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaDataChecksum.h,v $ 
  Language: C++ 
  Date: $Date: 2009-10-30 07:24:41 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  Computes checksum of general data that can be used to detect,
  whether something important has changed (something has changed
  as timestamp is different). For instance, a VTK filter may
  compute something internally from a scalar field of input data
  and reuse it as long as this field does not change, no matter
  whether any other field has changed (which would mean that the 
  input has a different timestamp).

*/

#ifndef albaDataChecksum_h__
#define albaDataChecksum_h__
/**
class name: albaDataChecksum
  Can compute Adler checksum for a given data, and combine two adler checksums in order to
  obtain a third checksum
*/
class albaDataChecksum
{
public:
  /** Computes Adler32 checksum for the given data. */
  inline static unsigned long Adler32Checksum(unsigned char* data, int len);

  /** Combines two checksums computed by Adler32Checksum to give a new one */
  inline static unsigned long CombineAdler32Checksums(unsigned long chcksum1, unsigned long chcksum2); 
};

#pragma region Inlines
//----------------------------------------------------------------------------------
//Computes Adler32 checksum for the given data
inline /*static*/ unsigned long albaDataChecksum::Adler32Checksum(unsigned char* data, int len)
//----------------------------------------------------------------------------------
{
  unsigned long a = 1, b = 0; 
  while (len != 0)
  {
    a = (a + *data++) % 65521;
    b = (b + a) % 65521;

    len--;
  }

  return (b << 16) | a;
}

//----------------------------------------------------------------------------------
//Combines two checksums computed by Adler32Checksum to give a new one */
inline /*static*/unsigned long albaDataChecksum::CombineAdler32Checksums(
  unsigned long chcksum1, unsigned long chcksum2)
  //----------------------------------------------------------------------------------
{
  unsigned long a = ((chcksum1 & 0xFFFF) + (chcksum2 & 0xFFFF)) % 65521;
  unsigned long b = ((chcksum1 >> 16) + (chcksum2 >> 16)) % 65521;

  return (b << 16) | a;
}
#pragma endregion Inlines



#endif // albaDataChecksum_h__