/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaQueryObject
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaQueryObject_H__
#define __albaQueryObject_H__

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
 class name: albaQueryObject
    class dor handling a query data type (stored as std::string), which can be converted to 
    int, long, float double or const char *
*/
class albaQueryObject
{
public :
  /** constructor */
  albaQueryObject(const char* data):m_QueryData(data){};
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

#endif //__albaQueryObject_H__
