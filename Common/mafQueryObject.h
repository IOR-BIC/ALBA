/*=========================================================================

 Program: MAF2
 Module: mafQueryObject
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafQueryObject_H__
#define __mafQueryObject_H__

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
 class name: mafQueryObject
    class dor handling a query data type (stored as std::string), which can be converted to 
    int, long, float double or const char *
*/
class mafQueryObject
{
public :
  /** constructor */
  mafQueryObject(const char* data):m_QueryData(data){};
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

#endif //__mafQueryObject_H__
