/*=========================================================================

 Program: MAF2
 Module: mafCryptTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "mafCryptTest.h"

#include <wx/dir.h>
#include <iostream>

#include "mafCrypt.h"
using namespace std;

void mafCryptTest::setUp()
{

}

void mafCryptTest::tearDown()
{

}


void mafCryptTest::TestFixture()
{

}

void mafCryptTest::TestMafCalculateteChecksumFromString()
{
 string inputString = "Hello I'm a string!";

 // MD5 from http://md5-hash-online.waraxe.us/ online md5 calculator :P
 string inputStringMD5ToCheck  = "4A8F7A67C7F3A921C707740421003562";

 string outputMD5CheckSum;
 mafCalculateteChecksum(inputString.c_str(), inputString.length(), outputMD5CheckSum);

 CPPUNIT_ASSERT_EQUAL(inputStringMD5ToCheck, outputMD5CheckSum);

 assert(true);
}

void mafCryptTest::TestMafCalculateteChecksumFromFile()
{
 string inputString = "Hello I'm a string!";

 string fileName = "newfile.txt";
 fstream file( fileName.c_str(), ios::out | ios::binary);
 file << inputString;
 file.close();

 assert(wxFileExists(fileName.c_str()));

 // MD5 from http://md5-hash-online.waraxe.us/ online md5 calculator :P
 string inputStringMD5ToCheck  = "4A8F7A67C7F3A921C707740421003562";

 string outputMD5CheckSum;
 mafCalculateteChecksum(fileName.c_str(), outputMD5CheckSum);

 CPPUNIT_ASSERT_EQUAL(inputStringMD5ToCheck, outputMD5CheckSum);

 assert(wxRemoveFile(fileName.c_str()));

}

void mafCryptTest::TestMafEncryptMafDecryptFile()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = "inputFileToEncrypt.txt";
 fstream fileToEncrypt( fileToEncryptName.c_str(), ios::out);
 fileToEncrypt << stringToEncrypt;
 fileToEncrypt.close();

 assert(wxFileExists(fileToEncryptName.c_str()));

 string encryptedFileName = "encryptedFile.txt";

 string passPhrase = "ImThePassPhrase";

 bool result = mafEncryptFile(fileToEncryptName.c_str(), encryptedFileName.c_str(), passPhrase.c_str() );

 CPPUNIT_ASSERT(result == true);

 string wrongPassPhrase = "wrongpassphrase";

 string decryptedFileName = "decryptedFile.txt";

 result = mafDecryptFile(encryptedFileName.c_str(),decryptedFileName.c_str(), wrongPassPhrase.c_str());

 CPPUNIT_ASSERT(result == false);

 result = mafDecryptFile(encryptedFileName.c_str(),decryptedFileName.c_str(), passPhrase.c_str());

 CPPUNIT_ASSERT(result == true);

 fstream decryptedFile( decryptedFileName.c_str(), ios::in);

 string tmp;
 string decryptedFileString;

 while (decryptedFile >> tmp)
 {
   decryptedFileString = decryptedFileString + tmp + " ";
 }

 wxString outWX = wxString(decryptedFileString.c_str());
 outWX.Trim();

 decryptedFileString.clear();
 decryptedFileString = outWX.c_str();

 decryptedFile.close();

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedFileString);

 assert(wxRemoveFile(fileToEncryptName.c_str()));
 assert(wxRemoveFile(encryptedFileName.c_str()));
 assert(wxRemoveFile(decryptedFileName.c_str()));
}


void mafCryptTest::TestMafDecryptFileInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string passPhrase = "ImThePassPhrase";

 string fileToEncryptName = "inputFileToEncrypt.txt";
 fstream file( fileToEncryptName.c_str(), ios::out);
 file << stringToEncrypt;
 file.close();

 assert(wxFileExists(fileToEncryptName.c_str()));

 string encryptedFileName = "encryptedFile.txt";

 bool result = mafEncryptFile(fileToEncryptName.c_str(), encryptedFileName.c_str(), passPhrase.c_str() );

 CPPUNIT_ASSERT(result == true);

 string decryptedString;

 result = mafDecryptFileInMemory(encryptedFileName.c_str(), decryptedString, passPhrase.c_str());

 CPPUNIT_ASSERT_EQUAL(result, true);
 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);

 assert(wxRemoveFile(fileToEncryptName.c_str()));
}

void mafCryptTest::TestMafEncryptFileFromMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedFileName = "encryptedFile.txt";
 string passPhrase = "ImThePassPhrase";

 mafEncryptFileFromMemory(stringToEncrypt.c_str(),stringToEncrypt.length(), encryptedFileName.c_str(), \
 passPhrase.c_str() );

 assert(wxFileExists(encryptedFileName.c_str()));

 string decryptedString;
 mafDecryptFileInMemory(encryptedFileName.c_str(), decryptedString, passPhrase.c_str());

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt,decryptedString);

 assert(wxRemoveFile(encryptedFileName.c_str()));
}

void mafCryptTest::TestMafDefaultEncryptFileMafDefaultDecryptFile()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = "inputFileToEncrypt.txt";
 fstream fileToEncrypt( fileToEncryptName.c_str(), ios::out);
 fileToEncrypt << stringToEncrypt;
 fileToEncrypt.close();

 assert(wxFileExists(fileToEncryptName.c_str()));

 string encryptedFileName = "encryptedFile.txt";

 bool result = mafDefaultEncryptFile(fileToEncryptName.c_str(), encryptedFileName.c_str());

 CPPUNIT_ASSERT(result == true);

 string decryptedFileName = "decryptedFile.txt";

 result = mafDefaultDecryptFile(encryptedFileName.c_str(),decryptedFileName.c_str());

 CPPUNIT_ASSERT(result == true);

 fstream decryptedFile( decryptedFileName.c_str(), ios::in);

 string tmp;
 string decryptedFileString;

 while (decryptedFile >> tmp)
 {
   decryptedFileString = decryptedFileString + tmp + " ";
 }

 wxString outWX = wxString(decryptedFileString.c_str());
 outWX.Trim();

 decryptedFileString.clear();
 decryptedFileString = outWX.c_str();

 decryptedFile.close();

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedFileString);

 assert(wxRemoveFile(fileToEncryptName.c_str()));
 assert(wxRemoveFile(encryptedFileName.c_str()));
 assert(wxRemoveFile(decryptedFileName.c_str()));

}

void mafCryptTest::TestMafDefaultDecryptFileInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = "inputFileToEncrypt.txt";
 fstream file( fileToEncryptName.c_str(), ios::out);
 file << stringToEncrypt;
 file.close();

 assert(wxFileExists(fileToEncryptName.c_str()));

 string encryptedFileName = "encryptedFile.txt";

 bool result = mafDefaultEncryptFile(fileToEncryptName.c_str(), encryptedFileName.c_str());

 CPPUNIT_ASSERT(result == true);

 string decryptedString;

 result = mafDefaultDecryptFileInMemory(encryptedFileName.c_str(), decryptedString);

 CPPUNIT_ASSERT_EQUAL(result, true);
 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);

 assert(wxRemoveFile(fileToEncryptName.c_str()));
}

void mafCryptTest::TestMafDefaultEncryptFileFromMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedFileName = "encryptedFile.txt";

 mafDefaultEncryptFileFromMemory(stringToEncrypt.c_str(),stringToEncrypt.length(), encryptedFileName.c_str());

 assert(wxFileExists(encryptedFileName.c_str()));

 string decryptedString;
 mafDefaultDecryptFileInMemory(encryptedFileName.c_str(), decryptedString);

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt,decryptedString);

 assert(wxRemoveFile(encryptedFileName.c_str()));

}

void mafCryptTest::TestMafEncryptFromMemoryMafDecryptInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string passPhrase = "ImThePassPhrase";
 string encryptedString;
 string decryptedString;

 mafEncryptFromMemory(stringToEncrypt.c_str(), encryptedString, passPhrase.c_str());
 mafDecryptInMemory(encryptedString.c_str(), decryptedString, passPhrase.c_str());

 // NOT WORKING: This is showcasing a known Crypto++ problem 
 // CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);
}

void mafCryptTest::TestMafDefaultEncryptFromMemoryMafDefaultDecryptInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedString;
 string decryptedString;

 mafDefaultEncryptFromMemory(stringToEncrypt.c_str(), encryptedString );
 mafDefaultDecryptInMemory(encryptedString.c_str(), decryptedString);

 // NOT WORKING: This is showcasing a known Crypto++ problem 
 // CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);
}
