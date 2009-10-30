/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medQueryObject.h,v $
Language:  C++
Date:      $Date: 2009-10-30 07:24:41 $
Version:   $Revision: 1.1.2.2 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2008
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __medQueryObject_H__
#define __medQueryObject_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// typedefs :
//----------------------------------------------------------------------------

/**
 class name: medQueryObject
    class dor handling a query data type (stored as std::string), which can be converted to 
    int, long, float double or const char *
*/
class medQueryObject
{
public :
  /** constructor */
  medQueryObject(const char* data):m_QueryData(data){};
  /** set query data */
  void SetData(const char* data){m_QueryData = data;}
  
  /** retrieve data as const char pointer */
  const char* GetValueAsString()
  {
    return m_QueryData.c_str();
  }
  /** retrieve data as int */
  int GetValueAsInt()
  {
    return atoi(m_QueryData.c_str());
  }
  /** retrieve data as long */
  long GetValueAsLong()
  {
    return atol(m_QueryData.c_str());
  }
  /** retrieve data as float */
  float GetValueAsFloat()
  {
    return atof(m_QueryData.c_str());
  }
  /** retrieve data as double */
  double GetValueAsDouble()
  {
    return atof(m_QueryData.c_str());
  }

protected:
  std::string m_QueryData;
};

#endif //__medQueryObject_H__
