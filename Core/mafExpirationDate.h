/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafExpirationDate.h,v $
  Language:  C++
  Date:      $Date: 2009-10-23 16:33:39 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafExpirationDate_H__
#define __mafExpirationDate_H__

//----------------------------------------------------------------------
// Forward references:
//----------------------------------------------------------------------
class wxDateTime;

//----------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------
#include "mafExpiration.h"
#include <string>
#include <vector>

/**
Class Name: mafExpirationDate.
  Class that supplies API for handling expiration based on simple control of local date
  in the machine in which the application is installed.
*/
class mafExpirationDate : public mafExpiration
{
public:
  /** Control trial period is over */
	/*virtual*/ bool HasExpired();
  /** Retrieve information */
  /*virtual*/ const char* GetInformation(){return m_Information.c_str();};

  /** constructor, with numerOfDays after the first access of free trial period */
	mafExpirationDate(int trialNumberOfDays);
  /** constructor, with expiration date after the first access of free trial period */
  mafExpirationDate(int year, unsigned short month, unsigned short day);
  /** constructor, that accept a string formatted as YYYY-MM-DD */ 
  mafExpirationDate(const char *dateString);

  /** set version of application*/
  void SetApplicationVersion(const char* version){m_ApplicationVersion = version;}

  /** set demo version flag*/
  void SetDemoVersion(bool enable){m_DemoVersionFlag = enable;}

  /** destructor. */
	virtual ~mafExpirationDate();

private:
  /** Create filename absolute path of the file to check. */
  void InitializePathFileName();
  /** check if file exists and update with last information*/
  void CheckFile();
  /** check if locally the expiration date is reached. */
  bool CheckLocalTimeExpiration();
  /** check if permanent expiration is reached. */
  bool CheckPermanentExpiration();
  /** activate permanent expiration. */
  void ActivatePermanentExpiration();
	/** Simple Obfuscator for filename*/
  void Obfuscate(std::string &toObfuscate);
  /** Check version application, if different remove all time check  files */
  void CheckApplicationVersion();
  /** Check if application is a demo */
  bool CheckDemoVersion();
  /** Giving a string as parameter and a vector of strings, it clean the vector and fill it with splitted substring controlling the separator.
  It returns the number of elements*/
  static int SplitString(std::string input, const char separator, std::vector<std::string> &outputSubStrings);

  
  wxDateTime *m_CurrentDateTime;
  wxDateTime *m_LastDateTime;
  wxDateTime *m_FirstAccessDateTime;

  int m_TrialNumberOfDays;
  wxDateTime *m_ExpirationDate;

  std::string m_ControlFileName;
  std::string m_Information;

  std::string m_ApplicationVersion;
  std::string m_ApplicationVersionFileName;

  std::string m_PermanentExpirationFileName;

  bool m_DemoVersionFlag;
	
};
#endif // mafExpirationDate