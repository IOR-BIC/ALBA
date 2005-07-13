/******************************************************************************
//	File:	P5Bend.cpp
//	Use:	This file provides sample code on processing bend sensor data of the
//			P5 in order to generate clicks
//	
//	Authors:	Av Utukuri
//	
//	Revision History:
//
//	May 13/02: Created file for sample code
//
//
//	Copyright (c) 2001 Essential Reality LLC
******************************************************************************/

#include <math.h>
#include "p5dll.h"
#include "p5bend.h"

#define NULL 0

/***********************************************
//Global variables
***********************************************/

CP5DLL *myP5Devices = NULL;
int nP5Id;

//this array contains the edge of the click pulse. 1 represents clickedge, -1 unclickedge, 0 is neither
int nP5ClickEdge[5];

//This array contains the level of the click.  True is clicked, false is unclicked
bool bP5ClickLevel[5];

unsigned char nBendSensitivity[5] = {15, 15, 15, 15, 15};

/***********************************************
Function: P5Bend_Init()
Use: Call this function to initialize the file
Parameters: CP5DLL *device	- pointer to initialized P5 instance
			int id			- the id of the device you want to track
***********************************************/
void P5Bend_Init(CP5DLL *device, int id)
{
	myP5Devices = device;
	nP5Id = id;
}

/***********************************************
Function: P5Bend_SetClickSensitivity()
Use: Call this function to initialize set different click sensitivities
Parameters: int finger - the finger to change (P5_THUMB, P5INDEX, P5_MIDDLE, P5_RING, P5_PINKY)
			uchar value - Sensitivity value (0-30)
***********************************************/
void P5Bend_SetClickSensitivity(int finger, unsigned char value)
{
	nBendSensitivity[finger] = value;
}

/***********************************************
Function: P5Bend_Process()
Use: Call this function to process the bend sensor data frame to frame
Parameters: None
***********************************************/
int nDebounceCounter[5] = {0, 0, 0, 0, 0};
int nLastFingerValue[5];
unsigned char nClickSpot[5];

void P5Bend_Process()
{
	static int firsttime = 1;

	//Make sure we have a valid device handle
	if (myP5Devices != NULL)
	{
		if (firsttime==1)
		{
			//Initialize our vars the first time through

			for (int i=0; i<5; i++)
				nLastFingerValue[i] = myP5Devices->m_P5Devices[0].m_byBendSensor_Data[i];

			firsttime = 0;
		}
		else
		{
			for (int i=0; i<5; i++)
			{
				int delta = myP5Devices->m_P5Devices[0].m_byBendSensor_Data[i]-nLastFingerValue[i];

				nP5ClickEdge[i] = 0;
				if (bP5ClickLevel[i]==false)
				{
					//If the finger is not already pressed, check for the minimum rate of change of the finger
					if (delta>nBendSensitivity[i])
					{
						//generate a click
						bP5ClickLevel[i] = true;
						nP5ClickEdge[i] = 1;
						nClickSpot[i] = myP5Devices->m_P5Devices[0].m_byBendSensor_Data[i];
					}
				}
				else
				{
					//If finger is already pressed, see if it is unclicked
					if (myP5Devices->m_P5Devices[0].m_byBendSensor_Data[i]<nClickSpot[i])
					{
						//Finger returned to old position, so unclick it
						bP5ClickLevel[i] = false;
						nP5ClickEdge[i] = -1;
					}
				}
				nLastFingerValue[i] = myP5Devices->m_P5Devices[0].m_byBendSensor_Data[i];

			}
		}
	}
}
