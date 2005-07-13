/***********************************************************************
*
*       File Name:      vicon.h
*       Description:    Header File for Vicon Driver
*                     
************************************************************************/


#pragma once

#include <winsock2.h>

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>	
#include <functional>
#include <limits>

#include <math.h>

#define VICON_MAX_BODY		   8
/* orientation format */
#define ISD_EULER              1
#define ISD_QUATERNION         2

#define PROTOCOL "tcp"
#define BUFFER_SIZE 2040


//-----------------------------------------------------------------------------
//	ClientCodes
//-----------------------------------------------------------------------------
class ClientCodes
{
public:
	enum EType		
	{
		ERequest, 
		EReply
	};

	enum EPacket	
	{
		EClose, 
		EInfo, 
		EData, 
		EStreamOn, 
		EStreamOff
	};

	static const std::vector< std::string > MarkerTokens;
	static const std::vector< std::string > BodyTokens;

	static std::vector< std::string > MakeMarkerTokens()
	{
		std::vector< std::string > v;
		v.push_back("<P-X>");
		v.push_back("<P-Y>");
		v.push_back("<P-Z>");
		v.push_back("<P-O>");
		return v;
	}

	static std::vector< std::string > MakeBodyTokens()
	{
		std::vector< std::string > v;
		v.push_back("<A-X>");
		v.push_back("<A-Y>");
		v.push_back("<A-Z>");
		v.push_back("<T-X>");
		v.push_back("<T-Y>");
		v.push_back("<T-Z>");
    v.push_back("<r-H>");
		return v;
	}

	struct CompareNames : std::binary_function<std::string, std::string, bool>
	{
		bool operator()(const std::string & a_S1, const std::string & a_S2) const
		{
			std::string::const_iterator iS1 = a_S1.begin();
			std::string::const_iterator iS2 = a_S2.begin();

			while(iS1 != a_S1.end() && iS2 != a_S2.end())
				if(toupper(*(iS1++)) != toupper(*(iS2++))) return false;

			return a_S1.size() == a_S2.size();
		}
	};



};

class MarkerChannel
{
public:
	std::string Name;

	int X;
	int Y;
	int Z;
	int O;

	MarkerChannel(std::string & a_rName) : X(-1), Y(-1), Z(-1), O(-1), Name(a_rName) {}

	int & operator[](int i)
	{
		switch(i)
		{
		case 0:		return X;
		case 1:		return Y;
		case 2:		return Z;
		case 3:		return O;
		default:	assert(false); return O;
		}
	}

	int operator[](int i) const
	{
		switch(i)
		{
		case 0:		return X;
		case 1:		return Y;
		case 2:		return Z;
		case 3:		return O;
		default:	assert(false); return -1;
		}
	}


	bool operator==(const std::string & a_rName) 
	{
		ClientCodes::CompareNames comparitor;
		return comparitor(Name, a_rName);
	}

};


class MarkerData
{
public:
	double	X;
	double	Y;
	double	Z;
	bool	Visible;
};

class BodyChannel
{
public:
	std::string Name;

	int TX;
	int TY;
	int TZ;
	int RX;
	int RY;
	int RZ;
  int RH;
  /*int idName;*/

	BodyChannel(std::string & a_rName/*, int id*/) : RX(-1), RY(-1), RZ(-1), TX(-1), TY(-1), TZ(-1), RH(-1), Name(a_rName)/*, idName(id)*/ {}

	int & operator[](int i)
	{
		switch(i)
		{
		case 0:		return RX;
		case 1:		return RY;
		case 2:		return RZ;
		case 3:		return TX;
		case 4:		return TY;
		case 5:		return TZ;
    case 6:   return RH;
		default:	assert(false); return TZ;
		}
	}

	int operator[](int i) const
	{
		switch(i)
		{
		case 0:		return RX;
		case 1:		return RY;
		case 2:		return RZ;
		case 3:		return TX;
		case 4:		return TY;
		case 5:		return TZ;
    case 6:   return RH;
		default:	assert(false); return -1;
		}
	}

	bool operator==(const std::string & a_rName) 
	{
		ClientCodes::CompareNames comparitor;
		return comparitor(Name, a_rName);
	}
};

class BodyData
{
public:
  std::string Name;
	// Representation of body position
	double	TX;
	double	TY;
	double	TZ;

	// Representation of body rotation
	// Quaternion
	double	QX;
	double	QY;
	double	QZ;
	double	QW;
	// Global rotation matrix
	double GlobalRotation[3][3];

	double EulerX;
	double EulerY;
	double EulerZ;

  // one degree rotation
  double RH;
  int    RHPresent;
	
};


//

class ViconData{
	protected:
		bool recieve(SOCKET Socket, char * pBuffer, int BufferSize);
		//	helpers to make the code a little less ugly.
		bool recieve(SOCKET Socket, long int & Val)
			{ return recieve(Socket, (char*) & Val, sizeof(Val));}
		bool recieve(SOCKET Socket, unsigned long int & Val)
			{ return recieve(Socket, (char*) & Val, sizeof(Val));}
		bool recieve(SOCKET Socket, double & Val)
			{ return recieve(Socket, (char*) & Val, sizeof(Val));}

		std::vector< std::string > info;

		char buff[BUFFER_SIZE];
		char * pBuff;
		std::vector< MarkerChannel >	MarkerChannels;
		std::vector< BodyChannel >		BodyChannels;
		int	FrameChannel;
		//variabili relative alla socket

		SOCKET	SocketHandle;
		struct protoent*	pProtocolInfoEntry;
		int					type;
    
	public:

		ViconData();
		~ViconData();
		int vicon_start(const char *hostname);
		int vicon_getData();
		int vicon_stop();
    
		std::vector< MarkerData > markerPositions;
		std::vector< BodyData > bodyPositions;
		double timestamp;
		int numBodies, numMarkers;
};