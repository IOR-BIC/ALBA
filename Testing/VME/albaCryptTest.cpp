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
 albaCalculateteChecksum(inputString.c_str(), inputString.length(), outputMD5CheckSum);

 CPPUNIT_ASSERT_EQUAL(inputStringMD5ToCheck, outputMD5CheckSum);

 assert(true);
}

void albaCryptTest::TestMafCalculateteChecksumFromFile()
{
 string inputString = "Hello I'm a string!";

 string fileName = GET_TEST_DATA_DIR() + "\\newfile.txt";
 fstream file( fileName.c_str(), ios::out | ios::binary);
 file << inputString;
 file.close();

 CPPUNIT_ASSERT(wxFileExists(fileName.c_str()));

 // MD5 from http://md5-hash-online.waraxe.us/ online md5 calculator :P
 string inputStringMD5ToCheck  = "4A8F7A67C7F3A921C707740421003562";

 string outputMD5CheckSum;
 albaCalculateteChecksum(fileName.c_str(), outputMD5CheckSum);

 CPPUNIT_ASSERT_EQUAL(inputStringMD5ToCheck, outputMD5CheckSum);

 CPPUNIT_ASSERT(wxRemoveFile(fileName.c_str()));

}

void albaCryptTest::TestMafEncryptMafDecryptFile()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = GET_TEST_DATA_DIR() + "\\inputFileToEncrypt.txt";
 fstream fileToEncrypt( fileToEncryptName.c_str(), ios::out);
 fileToEncrypt << stringToEncrypt;
 fileToEncrypt.close();

 CPPUNIT_ASSERT(wxFileExists(fileToEncryptName.c_str()));

 string encryptedFileName = GET_TEST_DATA_DIR() + "\\encryptedFile.txt";

 string passPhrase = "ImThePassPhrase";

 bool result = albaEncryptFile(fileToEncryptName.c_str(), encryptedFileName.c_str(), passPhrase.c_str() );

 CPPUNIT_ASSERT(result == true);

 string wrongPassPhrase = "wrongpassphrase";

 string decryptedFileName = GET_TEST_DATA_DIR() + "\\decryptedFile.txt";

 result = albaDecryptFile(encryptedFileName.c_str(),decryptedFileName.c_str(), wrongPassPhrase.c_str());

 CPPUNIT_ASSERT(result == false);

 result = albaDecryptFile(encryptedFileName.c_str(),decryptedFileName.c_str(), passPhrase.c_str());

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

 CPPUNIT_ASSERT(wxRemoveFile(fileToEncryptName.c_str()));
 CPPUNIT_ASSERT(wxRemoveFile(encryptedFileName.c_str()));
 CPPUNIT_ASSERT(wxRemoveFile(decryptedFileName.c_str()));
}


void albaCryptTest::TestMafDecryptFileInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string passPhrase = "ImThePassPhrase";

 string fileToEncryptName = GET_TEST_DATA_DIR() + "\\inputFileToEncrypt.txt";
 fstream file( fileToEncryptName.c_str(), ios::out);
 file << stringToEncrypt;
 file.close();

 CPPUNIT_ASSERT(wxFileExists(fileToEncryptName.c_str()));

 string encryptedFileName = GET_TEST_DATA_DIR() + "\\encryptedFile.txt";

 bool result = albaEncryptFile(fileToEncryptName.c_str(), encryptedFileName.c_str(), passPhrase.c_str() );

 CPPUNIT_ASSERT(result == true);

 string decryptedString;

 result = albaDecryptFileInMemory(encryptedFileName.c_str(), decryptedString, passPhrase.c_str());

 CPPUNIT_ASSERT_EQUAL(result, true);
 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);

 CPPUNIT_ASSERT(wxRemoveFile(fileToEncryptName.c_str()));
}

void albaCryptTest::TestMafEncryptFileFromMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedFileName = GET_TEST_DATA_DIR() + "\\encryptedFile.txt";
 string passPhrase = "ImThePassPhrase";

 albaEncryptFileFromMemory(stringToEncrypt.c_str(),stringToEncrypt.length(), encryptedFileName.c_str(), \
 passPhrase.c_str() );

 CPPUNIT_ASSERT(wxFileExists(encryptedFileName.c_str()));

 string decryptedString;
 albaDecryptFileInMemory(encryptedFileName.c_str(), decryptedString, passPhrase.c_str());

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt,decryptedString);

 CPPUNIT_ASSERT(wxRemoveFile(encryptedFileName.c_str()));
}

void albaCryptTest::TestMafDefaultEncryptFileMafDefaultDecryptFile()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = GET_TEST_DATA_DIR() + "\\inputFileToEncrypt.txt";
 fstream fileToEncrypt( fileToEncryptName.c_str(), ios::out);
 fileToEncrypt << stringToEncrypt;
 fileToEncrypt.close();

 CPPUNIT_ASSERT(wxFileExists(fileToEncryptName.c_str()));

 string encryptedFileName = GET_TEST_DATA_DIR() + "\\encryptedFile.txt";

 bool result = albaDefaultEncryptFile(fileToEncryptName.c_str(), encryptedFileName.c_str());

 CPPUNIT_ASSERT(result == true);

 string decryptedFileName = GET_TEST_DATA_DIR() + "\\decryptedFile.txt";

 result = albaDefaultDecryptFile(encryptedFileName.c_str(),decryptedFileName.c_str());

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

 CPPUNIT_ASSERT(wxRemoveFile(fileToEncryptName.c_str()));
 CPPUNIT_ASSERT(wxRemoveFile(encryptedFileName.c_str()));
 CPPUNIT_ASSERT(wxRemoveFile(decryptedFileName.c_str()));

}

void albaCryptTest::TestMafDefaultDecryptFileInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";

 string fileToEncryptName = GET_TEST_DATA_DIR() + "\\inputFileToEncrypt.txt";
 fstream file( fileToEncryptName.c_str(), ios::out);
 file << stringToEncrypt;
 file.close();

 CPPUNIT_ASSERT(wxFileExists(fileToEncryptName.c_str()));

 string encryptedFileName = GET_TEST_DATA_DIR() + "\\encryptedFile.txt";

 bool result = albaDefaultEncryptFile(fileToEncryptName.c_str(), encryptedFileName.c_str());

 CPPUNIT_ASSERT(result == true);

 string decryptedString;

 result = albaDefaultDecryptFileInMemory(encryptedFileName.c_str(), decryptedString);

 CPPUNIT_ASSERT_EQUAL(result, true);
 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);

 CPPUNIT_ASSERT(wxRemoveFile(fileToEncryptName.c_str()));
}

void albaCryptTest::TestMafDefaultEncryptFileFromMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedFileName = GET_TEST_DATA_DIR() + "\\encryptedFile.txt";

 albaDefaultEncryptFileFromMemory(stringToEncrypt.c_str(),stringToEncrypt.length(), encryptedFileName.c_str());

 CPPUNIT_ASSERT(wxFileExists(encryptedFileName.c_str()));

 string decryptedString;
 albaDefaultDecryptFileInMemory(encryptedFileName.c_str(), decryptedString);

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt,decryptedString);

 CPPUNIT_ASSERT(wxRemoveFile(encryptedFileName.c_str()));

}

void albaCryptTest::TestMafEncryptFromMemoryMafDecryptInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string passPhrase = "ImThePassPhrase";
 string encryptedString;
 string decryptedString;

 albaEncryptFromMemory(stringToEncrypt.c_str(), encryptedString, passPhrase.c_str());
 albaDecryptInMemory(encryptedString.c_str(), decryptedString, passPhrase.c_str());

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);
}

void albaCryptTest::TestMafDefaultEncryptFromMemoryMafDefaultDecryptInMemory()
{
 string stringToEncrypt = "Hello I'm the string to encrypt and decrypt!";
 string encryptedString;
 string decryptedString;

 albaDefaultEncryptFromMemory(stringToEncrypt.c_str(), encryptedString );
 albaDefaultDecryptInMemory(encryptedString.c_str(), decryptedString);

 CPPUNIT_ASSERT_EQUAL(stringToEncrypt, decryptedString);
}
