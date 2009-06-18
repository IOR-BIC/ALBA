/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafExpirationDate.h,v $
  Language:  C++
  Date:      $Date: 2009-06-18 08:43:22 $
  Version:   $Revision: 1.1.2.2 $
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

  
  wxDateTime *m_CurrentDateTime;
  wxDateTime *m_LastDateTime;
  wxDateTime *m_FirstAccessDateTime;

  int m_TrialNumberOfDays;
  wxDateTime *m_ExpirationDate;

  std::string m_ControlFileName;
  std::string m_Information;

  std::string m_PermanentExpirationFileName;
	
};
#endif // mafExpirationDate