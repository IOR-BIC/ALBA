/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafExpirationDate.cpp,v $
  Language:  C++
  Date:      $Date: 2009-08-31 17:24:44 $
  Version:   $Revision: 1.1.2.6 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafExpirationDate.h"
#include "mafCrypt.h"

#include <sstream>
#include <fstream>


#include <wx/datetime.h>
#include "wx/filefn.h"
#include "wx/stdpaths.h"
#include <wx/app.h>

#define ZERO 0x30 
#define NINE 0x39

#define AUP 0x41 
#define ZUP 0x5A

#define ADOWN 0x61 
#define ZDOWN 0x7A

//----------------------------------------------------------------------------
mafExpirationDate::mafExpirationDate(int trialNumberOfDays)
:m_TrialNumberOfDays(trialNumberOfDays),
m_CurrentDateTime(NULL),
m_LastDateTime(NULL),
m_FirstAccessDateTime(NULL),
m_ExpirationDate(NULL)
//----------------------------------------------------------------------------
{	
 
}
//----------------------------------------------------------------------------
mafExpirationDate::mafExpirationDate(int year, unsigned short month, unsigned short day)
:m_TrialNumberOfDays(-1),
m_CurrentDateTime(NULL),
m_LastDateTime(NULL),
m_FirstAccessDateTime(NULL),
m_ExpirationDate(NULL)
//----------------------------------------------------------------------------
{	
  m_ExpirationDate = new wxDateTime(wxDateTime::UNow());
  m_ExpirationDate->SetDay(day);
  m_ExpirationDate->SetMonth((wxDateTime::Month)(month-1)); //need to handle the correct array range of wxDateTime (0-11)
  m_ExpirationDate->SetYear(year);
  m_ExpirationDate->SetHour(0);
  m_ExpirationDate->SetMinute(0);
  m_ExpirationDate->SetSecond(0);
}
//----------------------------------------------------------------------------
mafExpirationDate::mafExpirationDate(const char *dateString)
:m_TrialNumberOfDays(-1),
m_CurrentDateTime(NULL),
m_LastDateTime(NULL),
m_FirstAccessDateTime(NULL),
m_ExpirationDate(NULL)
//----------------------------------------------------------------------------
{
  std::string check = dateString;
  short indexOfSeparator = -1;
  //search for the separator
  short j=0, sizeC = check.length();
  for(; j<sizeC; j++)
  {
    if(((char)check[j]) < ZERO || ((char)check[j]) > NINE) //is not a digit
    {
      indexOfSeparator = j;
      break;
    }
  }
  
  if( indexOfSeparator != -1 ) //find expiration date
  {
    size_t foundFirst;
    foundFirst = check.find(check[indexOfSeparator]);
    size_t foundLast;
    foundLast = check.rfind(check[indexOfSeparator]);

    std::string year, month, day;
    year = check.substr(0,foundFirst);
    month = check.substr(foundFirst+1,foundLast-foundFirst-1);
    day = check.substr(foundLast+1);

    std::stringstream ss;
    int dayInt,monthEnum,yearInt;
    ss << day;
    ss >> dayInt;
    ss.clear();
    ss << month;
    ss >> monthEnum;
    ss.clear();
    ss << year;
    ss >> yearInt;
    
    m_ExpirationDate = new wxDateTime(wxDateTime::UNow());
    m_ExpirationDate->SetDay(dayInt);
    m_ExpirationDate->SetMonth((wxDateTime::Month)(monthEnum-1));//need to handle the correct array range of wxDateTime (0-11)
    m_ExpirationDate->SetYear(yearInt);
    m_ExpirationDate->SetHour(0);
    m_ExpirationDate->SetMinute(0);
    m_ExpirationDate->SetSecond(0);
  }
  else //find number of days
  {
    m_TrialNumberOfDays = atoi(check.c_str());
  }
}
//----------------------------------------------------------------------------
mafExpirationDate::~mafExpirationDate()
//----------------------------------------------------------------------------
{
  if(m_CurrentDateTime)delete m_CurrentDateTime;
  if(m_LastDateTime)delete m_LastDateTime;
  if(m_FirstAccessDateTime)delete m_FirstAccessDateTime;
  if(m_ExpirationDate)delete m_ExpirationDate;
}
//----------------------------------------------------------------------------
void mafExpirationDate::InitializePathFileName()
//----------------------------------------------------------------------------
{
  if(wxApp::GetInstance() != NULL)
  {
    std::string dir;
    wxStandardPaths std_paths;
    dir = std_paths.GetUserLocalDataDir().c_str();
    if (!wxDirExists(dir.c_str()))
    {
      wxMkdir(dir.c_str());
    } 
    m_ControlFileName = dir.c_str();
    m_ControlFileName.append("/../");
		std::string dirAppname = wxApp::GetInstance()->GetAppName();
		Obfuscate(dirAppname);
    m_ControlFileName.append(dirAppname);
    m_ControlFileName.append("/");
		if (!wxDirExists(m_ControlFileName.c_str()))
		{
			wxMkdir(m_ControlFileName.c_str());
		} 
		std::string fileAppname = wxApp::GetInstance()->GetAppName();
		Obfuscate(fileAppname);
		m_ControlFileName.append(fileAppname);
    m_PermanentExpirationFileName = m_ControlFileName;
    m_PermanentExpirationFileName.append("f");
    m_ApplicationVersionFileName = m_ControlFileName;
    m_ApplicationVersionFileName.append("v");
  }
  else
  {
    m_ControlFileName = wxGetCwd();
		m_ControlFileName.append("/../");
    std::string dirConsole = "console";
		Obfuscate(dirConsole);
		m_ControlFileName.append(dirConsole);
		m_ControlFileName.append("/");
		if (!wxDirExists(m_ControlFileName.c_str()))
		{
			wxMkdir(m_ControlFileName.c_str());
		} 
		std::string fileConsole = "console";
		Obfuscate(fileConsole);
    m_ControlFileName.append(fileConsole); 
		
		m_PermanentExpirationFileName = m_ControlFileName;
		m_PermanentExpirationFileName.append("f");

    m_ApplicationVersionFileName = m_ControlFileName;
    m_ApplicationVersionFileName.append("v");
  }
}
//----------------------------------------------------------------------------
bool mafExpirationDate::HasExpired()
//----------------------------------------------------------------------------
{
  InitializePathFileName();
  //fill variables
  CheckApplicationVersion();
	CheckFile();

  bool result = CheckLocalTimeExpiration();
  return result;
}
//----------------------------------------------------------------------------
void mafExpirationDate::CheckFile()
//----------------------------------------------------------------------------
{
  std::ofstream os;

  m_CurrentDateTime = new wxDateTime(wxDateTime::UNow());
  m_CurrentDateTime->SetHour(0);
  m_CurrentDateTime->SetMinute(0);
  m_CurrentDateTime->SetSecond(0);

  std::string tempCurrentDateTime;
  char temp[50];
  sprintf(temp,"%ld",m_CurrentDateTime->GetAsDOS());
  //m_CurrentDateTime->SetFromDOS(m_CurrentDateTime->GetAsDOS()); //little trick to avoid round off problem first time creation
  //m_CurrentDateTime->SetHour(0);
  //m_CurrentDateTime->SetMinute(0);
  //m_CurrentDateTime->SetSecond(0);
  tempCurrentDateTime = std::string(temp);

  if(!::wxFileExists(m_ControlFileName.c_str()))
  {
    //create it
    std::string toEncrypt;
    toEncrypt.append(tempCurrentDateTime);
    toEncrypt.append("\n");
    toEncrypt.append(tempCurrentDateTime);
    
    mafEncryptFileFromMemory(toEncrypt.c_str(),toEncrypt.length(),m_ControlFileName.c_str(), "fattinonfostepervivercomebruti");
  }

  //read it
  std::string toDecrypt;
  mafDecryptFileInMemory(m_ControlFileName.c_str(), toDecrypt, "fattinonfostepervivercomebruti");

  std::stringstream ss;
  ss << toDecrypt;

  // set all variables in order to control expiration
  long dosTimeAccess;
  ss >> dosTimeAccess;
  m_FirstAccessDateTime = new wxDateTime(wxDateTime::UNow());
  m_FirstAccessDateTime->SetFromDOS(dosTimeAccess);
  //long testAccess = m_FirstAccessDateTime.GetAsDOS();
  long dosTimeLast;
  ss >> dosTimeLast;
  m_LastDateTime = new wxDateTime(wxDateTime::UNow());
  m_LastDateTime->SetFromDOS(dosTimeLast);
  //long testLast = m_LastDateTime.GetAsDOS();

  std::string toEncrypt;
  char tmp[50];
  sprintf(tmp,"%ld",dosTimeAccess);
  toEncrypt.append(tmp);
  toEncrypt.append("\n");
  toEncrypt.append(tempCurrentDateTime);

  mafEncryptFileFromMemory(toEncrypt.c_str(),toEncrypt.length(),m_ControlFileName.c_str(), "fattinonfostepervivercomebruti");
  

  
  //debug file
  /*std::ofstream os2;
  os2.open("expirationFileCheck.txt");

  os2 << m_FirstAccessDateTime->GetYear() 
    << "-" << (m_FirstAccessDateTime->GetMonth()+1)
    << "-" << m_FirstAccessDateTime->GetDay() 
    << " "
    << m_FirstAccessDateTime->GetHour() 
    << ":" << m_FirstAccessDateTime->GetMinute() 
    << ":" << m_FirstAccessDateTime->GetSecond() << "\n";

  os2 << m_LastDateTime->GetYear() 
    << "-" << (m_LastDateTime->GetMonth()+1)
    << "-" << m_LastDateTime->GetDay() 
    << " "
    << m_LastDateTime->GetHour() 
    << ":" << m_LastDateTime->GetMinute() 
    << ":" << m_LastDateTime->GetSecond() << "\n";

  os2 << m_CurrentDateTime->GetYear() 
    << "-" << (m_CurrentDateTime->GetMonth()+1)
    << "-" << m_CurrentDateTime->GetDay() 
    << " "
    << m_CurrentDateTime->GetHour() 
    << ":" << m_CurrentDateTime->GetMinute() 
    << ":" << m_CurrentDateTime->GetSecond() << "\n";

  os2.close();*/
}
//----------------------------------------------------------------------------
void mafExpirationDate::CheckApplicationVersion()
//----------------------------------------------------------------------------
{
  if(!::wxFileExists(m_ApplicationVersionFileName.c_str()))
  {
    //create it
    std::string toEncrypt;
    toEncrypt.append(m_ApplicationVersion);

    mafEncryptFileFromMemory(toEncrypt.c_str(),toEncrypt.length(),m_ApplicationVersionFileName.c_str(), "fattinonfostepervivercomebruti");
  }

  //read it
  std::string toDecrypt;
  mafDecryptFileInMemory(m_ApplicationVersionFileName.c_str(), toDecrypt, "fattinonfostepervivercomebruti");

  std::stringstream ss;
  ss << toDecrypt;

  std::string oldApplicationVersion;
  ss >> oldApplicationVersion;
  
  // the string for version is ${APP_MAJOR_VERSION}_${APP_MINOR_VERSION}_TS_${APP_BUILD_TIMESTAMP}_BUILD_${APP_BUILD_NUMBER}
  // generated during cmake script
  
  enum
  {
    APP_MAJOR_VERSION = 0,
    APP_MINOR_VERSION,
    APP_TS,
    APP_BUILD_TIMESTAMP,
    APP_BUILD,
    APP_BUILD_NUMBER,
    APP_FIELDS_NUMBER,
  };

  std::vector<std::string> substringsOld;
  SplitString(oldApplicationVersion, '_', substringsOld);
  std::vector<std::string> substringsCurrent;
  SplitString(m_ApplicationVersion, '_', substringsCurrent);

  //build revision format not supported
  if(substringsCurrent.size() < APP_FIELDS_NUMBER)
  {
    return;
  }

  //build revision format bind to parabuild and cmake
  if(substringsOld[APP_BUILD_TIMESTAMP].compare(substringsCurrent[APP_BUILD_TIMESTAMP]) < 0)
  {
    //remove all files of directory
    ::wxRemoveFile(m_ControlFileName.c_str());
    ::wxRemoveFile(m_PermanentExpirationFileName.c_str());
    ::wxRemoveFile(m_ApplicationVersionFileName.c_str());

    if(!::wxFileExists(m_ApplicationVersionFileName.c_str()))
    {
      //create it
      std::string toEncrypt;
      toEncrypt.append(m_ApplicationVersion);

      mafEncryptFileFromMemory(toEncrypt.c_str(),toEncrypt.length(),m_ApplicationVersionFileName.c_str(), "fattinonfostepervivercomebruti");
    }
  }
  else
  {
    //do nothing, the version is current or older.
  }
}
//----------------------------------------------------------------------------
int mafExpirationDate::SplitString(std::string input, const char separator, std::vector<std::string> &outputSubStrings)
//----------------------------------------------------------------------------
{
  outputSubStrings.clear();
  std::string::size_type prev_pos = 0, pos = 0;
  while( (pos = input.find(separator, pos)) != std::string::npos )
  {
    std::string substring( input.substr(prev_pos, pos-prev_pos) );
    outputSubStrings.push_back(substring);
    prev_pos = ++pos;
  }
  std::string substring( input.substr(prev_pos, pos-prev_pos) ); // Last word
  outputSubStrings.push_back(substring);

  return outputSubStrings.size();
}
//----------------------------------------------------------------------------
bool mafExpirationDate::CheckLocalTimeExpiration()
//----------------------------------------------------------------------------
{
  m_Information.clear();

  //permanent expiration
  if(CheckPermanentExpiration() == true)
  {
    m_Information = "Trial Period is over";
    return true;
  }

  long difference = 0;
  //check if date is inconsistent
  if(m_CurrentDateTime->GetAsDOS() - m_LastDateTime->GetAsDOS() < 0)
  {
    m_Information = "Incoherent time check.";
    ActivatePermanentExpiration();
    return true;
  }
  else if (m_CurrentDateTime->GetAsDOS() - m_FirstAccessDateTime->GetAsDOS() < 0)
  {
    m_Information = "Incoherent time check.";
    ActivatePermanentExpiration();
    return true;
  }

  //real check
  if(m_TrialNumberOfDays > 0)
  {
    wxDateTime endTrial = *m_FirstAccessDateTime;
    endTrial = endTrial.Add(wxDateSpan(0,0,0,m_TrialNumberOfDays));

    long long curr = m_CurrentDateTime->GetAsDOS();
    long long end = endTrial.GetAsDOS();

    long cTicks = m_CurrentDateTime->GetTicks();
    long eTicks = endTrial.GetTicks();

    difference = cTicks  - eTicks;
    if(curr - end > 0)
    {
      m_Information = "Trial Period is over";
      ActivatePermanentExpiration();
      return true;
    }
  }
  else
  {
    long long curr = m_CurrentDateTime->GetAsDOS();
    long long end = m_ExpirationDate->GetAsDOS();
    
    long cTicks = m_CurrentDateTime->GetTicks();
    long eTicks = m_ExpirationDate->GetTicks();
    difference = cTicks  - eTicks;

    if(curr - end > 0)
    {
      m_Information = "Trial Period is over";
      ActivatePermanentExpiration();
      return true;
    }
  }

  //fill information string
  int days = abs(difference / (24 * 3600));
  if(days == 0)
  {
    m_Information = "Last Trail day";
  }
  else if (days == 1)
  {
    m_Information = "1 day remaining to the end of the trial period";
  }
  else
  {
    char temp[5];
    sprintf(temp,"%d",days);

    m_Information.append(temp);
    m_Information.append(" days remaining to the end of the trial period");
  }

  return false;
}
//----------------------------------------------------------------------------
bool mafExpirationDate::CheckPermanentExpiration()
//----------------------------------------------------------------------------
{
  if(::wxFileExists(m_PermanentExpirationFileName.c_str()))
  {
    return true;
  }
  else
  {
    return false;
  }
}
//----------------------------------------------------------------------------
void mafExpirationDate::ActivatePermanentExpiration()
//----------------------------------------------------------------------------
{
  std::ofstream os;
  os.open(m_PermanentExpirationFileName.c_str());
  os.close();
}
//----------------------------------------------------------------------------
void mafExpirationDate::Obfuscate(std::string &toObfuscate) 
//----------------------------------------------------------------------------
{
  int i=0, size = toObfuscate.size();
	int number = 0;
  for( ; i < size; ++i )
  {
		number += (int) toObfuscate[i];
    if(toObfuscate[i] >= ZERO && toObfuscate[i] <= NINE)
		{
			toObfuscate[i] = (toObfuscate[i]-ZERO)>(NINE-toObfuscate[i])?(NINE-(toObfuscate[i]-ZERO)):(ZERO+(NINE-toObfuscate[i]));
		}
		else if (toObfuscate[i] >= AUP && toObfuscate[i] <= ZUP)
    {
      toObfuscate[i] = (toObfuscate[i]-AUP)>(ZUP-toObfuscate[i])?(ZUP-(toObfuscate[i]-AUP)):(AUP+(ZUP-toObfuscate[i]));
    }
		else if((toObfuscate[i] >= ADOWN && toObfuscate[i] <= ZDOWN))
		{
			toObfuscate[i] = (toObfuscate[i]-ADOWN)>(ZDOWN-toObfuscate[i])?(ZDOWN-(toObfuscate[i]-ADOWN)):(ADOWN+(ZDOWN-toObfuscate[i]));
			toObfuscate[i] -= 0x20; //upper case
		}
		else
		{
      toObfuscate[i] = ZERO;
		}
  }
	char num[10];
	sprintf(num,"%d",number);
	toObfuscate.append(num);
}