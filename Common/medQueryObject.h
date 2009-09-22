/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medQueryObject.h,v $
Language:  C++
Date:      $Date: 2009-09-22 07:33:45 $
Version:   $Revision: 1.1.2.1 $
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

class medQueryObject
{
public :
  medQueryObject(const char* data):m_QueryData(data){};
  void SetData(const char* data){m_QueryData = data;}
  
  const char* GetValueAsString()
  {
    return m_QueryData.c_str();
  }
  int GetValueAsInt()
  {
    return atoi(m_QueryData.c_str());
  }
  long GetValueAsLong()
  {
    return atol(m_QueryData.c_str());
  }
  float GetValueAsFloat()
  {
    return atof(m_QueryData.c_str());
  }
  double GetValueAsDouble()
  {
    return atof(m_QueryData.c_str());
  }

protected:
  std::string m_QueryData;
};

#endif //__medQueryObject_H__
