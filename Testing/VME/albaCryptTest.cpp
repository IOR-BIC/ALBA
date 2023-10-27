/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCryptTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "albaCryptTest.h"

#include <wx/dir.h>
#include <iostream>

#include "albaCrypt.h"
using namespace std;



void albaCryptTest::TestFixture()
{

}

void albaCryptTest::TestMafCalculateteChecksumFromString()
{
 string inputString = "Hello I'm a string!";

 // MD5 from http://md5-hash-online.waraxe.us/ online md5 calculator :P
 string inputStringMD5ToCheck  = "4A8F7A67C7F3A921C707740421003562";

 string outputMD5CheckSum;
 albaCalculateteChecksum(inputString.char_str(), inputString.length(), outputMD5CheckSum);

 CPPUNIT_ASSERT_EQUAL(inputStringMD5ToCheck, outputMD5CheckSum);

 assert(true);
}

void albaCryptTest::TestMafCalculateteChecksumFromFile()
{
 string inputString = "Hello I'm a string!";

 string fileName = "newfile.txt";
 fstream file( fileName.char_str(), ios::out | ios::binary);
 file << inputString;
 file.close();

 assert(wxFileExists(fileName.char_str()));

 // MD5 from http://md5-hash-online.waraxe.us/ online md5 calculator :P
 string inputStringMD5ToCheck  = "4A8F7A67C7F3A921C707740421003562";

 string outputMD5CheckSum;
 albaCalculateteChecksum(fileName.char_str(), outputMD5CheckSum);

 CPPUNIT_ASSERT_EQUAL(inputStringMD5ToCheck, outputMD5CheckSum);

 assert(wxRemoveFile(fileName.char_str()));

}

void albaCryptTest::TestMafEncryptMafDecryptFile()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = "inputFileToEncrypt.txt";
 fstream fileToEncrypt( fileToEncryptName.char_str(), ios::out);
 fileToEncrypt << stringToEncrypt;
 fileToEncrypt.close();

 assert(wxFileExists(fileToEncryptName.char_str()));

 string encryptedFileName = "encryptedFile.txt";

 string passPhrase = "ImThePassPhrase";

 bool result = albaEncryptFile(fileToEncryptName.char_str(), encryptedFileName.char_str(), passPhrase.char_str() );

 CPPUNIT_ASSERT(result == true);

 string wrongPassPhrase = "wrongpassphrase";

 string decryptedFileName = "decryptedFile.txt";

 result = albaDecryptFile(encryptedFileName.char_str(),decryptedFileName.char_str(), wrongPassPhrase.char_str());

 CPPUNIT_ASSERT(result == false);

 result = albaDecryptFile(encryptedFileName.char_str(),decryptedFileName.char_str(), passPhrase.char_str());

 CPPUNIT_ASSERT(result == true);

 fstream decryptedFile( decryptedFileName.char_str(), ios::in);

 string tmp;
 string decryptedFileString;

 while (decryptedFile >> tmp)
 {
   decryptedFileString = decryptedFileString + tmp + " ";
 }

 wxString outWX = wxString(decryptedFileString.char_str());
 outWX.Trim();

 decryptedFileString.clear();
 decryptedFileString = outWX.char_str();

 decryptedFile.close();

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedFileString);

 assert(wxRemoveFile(fileToEncryptName.char_str()));
 assert(wxRemoveFile(encryptedFileName.char_str()));
 assert(wxRemoveFile(decryptedFileName.char_str()));
}


void albaCryptTest::TestMafDecryptFileInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string passPhrase = "ImThePassPhrase";

 string fileToEncryptName = "inputFileToEncrypt.txt";
 fstream file( fileToEncryptName.char_str(), ios::out);
 file << stringToEncrypt;
 file.close();

 assert(wxFileExists(fileToEncryptName.char_str()));

 string encryptedFileName = "encryptedFile.txt";

 bool result = albaEncryptFile(fileToEncryptName.char_str(), encryptedFileName.char_str(), passPhrase.char_str() );

 CPPUNIT_ASSERT(result == true);

 string decryptedString;

 result = albaDecryptFileInMemory(encryptedFileName.char_str(), decryptedString, passPhrase.char_str());

 CPPUNIT_ASSERT_EQUAL(result, true);
 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);

 assert(wxRemoveFile(fileToEncryptName.char_str()));
}

void albaCryptTest::TestMafEncryptFileFromMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedFileName = "encryptedFile.txt";
 string passPhrase = "ImThePassPhrase";

 albaEncryptFileFromMemory(stringToEncrypt.char_str(),stringToEncrypt.length(), encryptedFileName.char_str(), \
 passPhrase.char_str() );

 assert(wxFileExists(encryptedFileName.char_str()));

 string decryptedString;
 albaDecryptFileInMemory(encryptedFileName.char_str(), decryptedString, passPhrase.char_str());

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt,decryptedString);

 assert(wxRemoveFile(encryptedFileName.char_str()));
}

void albaCryptTest::TestMafDefaultEncryptFileMafDefaultDecryptFile()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = "inputFileToEncrypt.txt";
 fstream fileToEncrypt( fileToEncryptName.char_str(), ios::out);
 fileToEncrypt << stringToEncrypt;
 fileToEncrypt.close();

 assert(wxFileExists(fileToEncryptName.char_str()));

 string encryptedFileName = "encryptedFile.txt";

 bool result = albaDefaultEncryptFile(fileToEncryptName.char_str(), encryptedFileName.char_str());

 CPPUNIT_ASSERT(result == true);

 string decryptedFileName = "decryptedFile.txt";

 result = albaDefaultDecryptFile(encryptedFileName.char_str(),decryptedFileName.char_str());

 CPPUNIT_ASSERT(result == true);

 fstream decryptedFile( decryptedFileName.char_str(), ios::in);

 string tmp;
 string decryptedFileString;

 while (decryptedFile >> tmp)
 {
   decryptedFileString = decryptedFileString + tmp + " ";
 }

 wxString outWX = wxString(decryptedFileString.char_str());
 outWX.Trim();

 decryptedFileString.clear();
 decryptedFileString = outWX.char_str();

 decryptedFile.close();

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedFileString);

 assert(wxRemoveFile(fileToEncryptName.char_str()));
 assert(wxRemoveFile(encryptedFileName.char_str()));
 assert(wxRemoveFile(decryptedFileName.char_str()));

}

void albaCryptTest::TestMafDefaultDecryptFileInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = "inputFileToEncrypt.txt";
 fstream file( fileToEncryptName.char_str(), ios::out);
 file << stringToEncrypt;
 file.close();

 assert(wxFileExists(fileToEncryptName.char_str()));

 string encryptedFileName = "encryptedFile.txt";

 bool result = albaDefaultEncryptFile(fileToEncryptName.char_str(), encryptedFileName.char_str());

 CPPUNIT_ASSERT(result == true);

 string decryptedString;

 result = albaDefaultDecryptFileInMemory(encryptedFileName.char_str(), decryptedString);

 CPPUNIT_ASSERT_EQUAL(result, true);
 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);

 assert(wxRemoveFile(fileToEncryptName.char_str()));
}

void albaCryptTest::TestMafDefaultEncryptFileFromMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedFileName = "encryptedFile.txt";

 albaDefaultEncryptFileFromMemory(stringToEncrypt.char_str(),stringToEncrypt.length(), encryptedFileName.char_str());

 assert(wxFileExists(encryptedFileName.char_str()));

 string decryptedString;
 albaDefaultDecryptFileInMemory(encryptedFileName.char_str(), decryptedString);

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt,decryptedString);

 assert(wxRemoveFile(encryptedFileName.char_str()));

}

void albaCryptTest::TestMafEncryptFromMemoryMafDecryptInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string passPhrase = "ImThePassPhrase";
 string encryptedString;
 string decryptedString;

 albaEncryptFromMemory(stringToEncrypt.char_str(), encryptedString, passPhrase.char_str());
 albaDecryptInMemory(encryptedString.char_str(), decryptedString, passPhrase.char_str());

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);
}

void albaCryptTest::TestMafDefaultEncryptFromMemoryMafDefaultDecryptInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedString;
 string decryptedString;

 albaDefaultEncryptFromMemory(stringToEncrypt.char_str(), encryptedString );
 albaDefaultDecryptInMemory(encryptedString.char_str(), decryptedString);

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);
}
