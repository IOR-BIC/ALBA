/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERawMotionData.h,v $
  Language:  C++
  Date:      $Date: 2007-03-16 13:03:52 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni - porting Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMERawMotionData_h
#define __mafVMERawMotionData_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGroup.h"
#include <fstream>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;

/** mafVMERawMotionData - 
*/
class  mafVMERawMotionData : public mafVMEGroup  
{
public:

  mafTypeMacro(mafVMERawMotionData,mafVMEGroup);
  
  /** Set RAW motion data  Dictionary file name*/
  void SetDictionaryFileName(const char *name);
  
  /** Get RAW motion data  Dictionary file name*/
  const char *GetDictionaryName() {return this->m_DictionaryFileName.GetCStr();}

  /** Set RAW motion data file name*/
  void mafVMERawMotionData::SetFileName(const char *name);
  
  /** Get RAW motion data file name*/
  const char *GetFileName() {return this->m_FileName.GetCStr(); }	
  
  /** Parse RAW motion data file and fill VME Tree*/
  int Read();

	/**
	Use DictionaryOn() to use a dictionary otherwise
	only one segment will be created and all the landmarks 
	will be appended to it.*/
	void SetDictionary(int Dict){m_Dictionary = Dict;}
	int GetDictionary(){return m_Dictionary;}

	void DictionaryOn () { this->SetDictionary((int)1);}
	void DictionaryOff () { this->SetDictionary((int)0);}

protected:

  mafVMERawMotionData();
  virtual ~mafVMERawMotionData();

  // By default, UpdateInformation calls this method. Subclasses should fill
  // the output structure information.
  //virtual int ExecuteInformation();

  //virtual int ExecuteData(mafVMEItem *item); //virtual int ExecuteData(mflVMEItem *item);

  mafString m_DictionaryFileName; 
  mafString m_FileName; 
  int m_Dictionary;

private:
  mafVMERawMotionData(const mafVMERawMotionData&);  // Not implemented.
  void operator=(const mafVMERawMotionData&);  // Not implemented.

  
};

#endif


