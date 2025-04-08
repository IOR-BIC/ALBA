/***********************************************************************
*
*    File Name:      isense.c
*    Description:    wrappers for isdriver routines
*    Created:        12/4/98
*    Author:         Yury Altshuler
*
*    Copyright:      InterSense 1999 - All rights Reserved.
*
*                    
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "isense.h"

#if !defined UNIX
#include <windows.h>
#include <winbase.h>
#endif

#include "isdriver.h"
#include "timer.h"

InterSenseTrackerType ISD_tracker[ISD_MAX_TRACKERS];


/*****************************************************************************
*
*   functionName:   ISD_OpenTracker
*   Description:
*   Created:        12/3/98
*
*   Comments:       Returns -1 on failure. To detect tracker automatically 
*                   specify 0 for commPort    
*
******************************************************************************/
ISD_TRACKER_HANDLE ISD_OpenTracker( DWORD commPort, BOOL infoScreen, BOOL verbose )
{
    ISD_TRACKER_HANDLE i;

    /* locate a free tracker */
    for(i=0; i<ISD_MAX_TRACKERS; i++)
    {
        /* tracker structure is available */
        if(ISD_tracker[i].hardware == ISD_NONE)
        {
            /* detect tracker and initialize it to defaults */
            if(ISD_openTracker( &ISD_tracker[i], commPort, infoScreen, verbose ))
            {
                ISD_tracker[i].ID = i+1;

                /* configure tracker for best performance */
                ISD_configureTracker( &ISD_tracker[i], verbose );

                return i+1;
            }
            else
            {
                return -1;
            }
        }
    }
    return -1;
}


/*****************************************************************************
*
*   functionName:   ISD_CloseTracker
*   Description:
*   Created:        12/3/98
*
*   Comments:       
*
******************************************************************************/
BOOL  ISD_CloseTracker( ISD_TRACKER_HANDLE handle )
{
    int i;

    if( handle > 0 && handle <= ISD_MAX_TRACKERS )
    {
        return ISD_closeTracker( &ISD_tracker[ handle - 1 ] ); 
    }
    else if( handle == 0 )   /* close all */
    {
        for( i=0; i<ISD_MAX_TRACKERS; i++ )
        {
            ISD_closeTracker( &ISD_tracker[ i ] ); 
        }
        return true;
    }
    return false;
}


/*****************************************************************************
*
*   functionName:   ISD_NumOpenTrackers
*   Description:
*   Created:        12/3/98
*
*   Comments:       Returns the number of currently opened trackers.   
*
******************************************************************************/
BOOL  ISD_NumOpenTrackers( WORD *num )
{
    int i;

    *num = 0;

    for(i = 0; i < ISD_MAX_TRACKERS; i++)
    {
        /* tracker structure is in use */
        if(ISD_tracker[i].hardware != ISD_NONE)
        {
            *num += 1;
        }
    }
    return true;
}


/*****************************************************************************
*
*   functionName:   ISD_GetCommInfo
*   Description:
*   Created:        12/3/98
*
*   Comments:       Get RecordsPerSec and KBitsPerSec without requesting 
*                   genlock settings from the tracker. Use this instead of 
*                   ISD_GetTrackerConfig to prevent your program from stalling while
*                   waiting for the tracker response   
*
******************************************************************************/
BOOL ISD_GetCommInfo( ISD_TRACKER_HANDLE handle, ISD_TRACKER_INFO_TYPE *Tracker )
{
    if( handle > 0 && handle <= ISD_MAX_TRACKERS)
    {
        return ISD_getCommInfo( &ISD_tracker[ handle - 1 ], Tracker );
    }
    return false;
}


/*****************************************************************************
*
*   functionName:   ISD_GetTrackerConfig
*   Description:
*   Created:        12/3/98

*
*   Comments:          
*
******************************************************************************/
BOOL ISD_GetTrackerConfig( ISD_TRACKER_HANDLE handle, 
                           ISD_TRACKER_INFO_TYPE *Tracker, BOOL verbose )
{
    if( handle > 0 && handle <= ISD_MAX_TRACKERS)
    {
        return ISD_getTrackerConfig( &ISD_tracker[ handle - 1 ], Tracker, verbose );
    }
    return false;
}


/*****************************************************************************
*
*   functionName:   ISD_SetTrackerConfig
*   Description:
*   Created:        12/3/98
*
*   Comments:        
*
******************************************************************************/
BOOL ISD_SetTrackerConfig( ISD_TRACKER_HANDLE handle, 
                           ISD_TRACKER_INFO_TYPE *Tracker, BOOL verbose )
{
    if( handle > 0 && handle <= ISD_MAX_TRACKERS)
    {
        return ISD_setTrackerConfig( &ISD_tracker[ handle - 1 ], Tracker, verbose );
    }
    return false;
}


/*****************************************************************************
*
*   functionName:   ISD_SetStationConfig
*   Description:
*   Created:        12/3/98
*
*   Comments:          
*
******************************************************************************/
BOOL ISD_SetStationConfig( ISD_TRACKER_HANDLE handle, 
                           ISD_STATION_INFO_TYPE *Station, 
                           WORD stationNum, BOOL verbose )
{
    if( handle > 0 && handle <= ISD_MAX_TRACKERS)
    {
        return ISD_setStationConfig( &ISD_tracker[ handle - 1 ], Station, 
                                     stationNum, verbose, true );
    }
    return false;
}


/*****************************************************************************
*
*   functionName:   ISD_GetStationConfig
*   Description:
*   Created:        12/3/98
*
*   Comments:          
*
******************************************************************************/
BOOL ISD_GetStationConfig( ISD_TRACKER_HANDLE handle, 
                           ISD_STATION_INFO_TYPE *Station,
                           WORD stationNum, BOOL verbose )
{
    if( handle > 0 && handle <= ISD_MAX_TRACKERS )
    {
        return ISD_getStationConfig( &ISD_tracker[ handle - 1 ], Station, 
                                      stationNum, verbose );
    }
    return false;
}


/*****************************************************************************
*
*   functionName:   ISD_GetTrackerData
*   Description:
*   Created:        12/3/98
*
*   Comments:       get data from all configured stations    
*
******************************************************************************/
BOOL ISD_GetTrackerData( ISD_TRACKER_HANDLE handle, ISD_DATA_TYPE *Data )
{
    InterSenseTrackerType *tracker;
    int i;

    if( handle > 0 && handle <= ISD_MAX_TRACKERS )
    {
        tracker = &ISD_tracker[ handle - 1 ];

        ISD_getTrackerData( tracker );
        
        /* copy tracker data from internal structures to the user data */
        for(i=0; i<ISD_MAX_STATIONS; i++)
        {
            Data->Station[i].Orientation[0] = tracker->station[i].Orientation[0];
            Data->Station[i].Orientation[1] = tracker->station[i].Orientation[1];
            Data->Station[i].Orientation[2] = tracker->station[i].Orientation[2];
            Data->Station[i].Orientation[3] = tracker->station[i].Orientation[3];

            Data->Station[i].Position[0] = tracker->station[i].Position[0];
            Data->Station[i].Position[1] = tracker->station[i].Position[1];
            Data->Station[i].Position[2] = tracker->station[i].Position[2];

            Data->Station[i].NewData   = tracker->station[i].NewData;
            Data->Station[i].TimeStamp = tracker->station[i].TimeStamp;

            memcpy((void *)Data->Station[i].ButtonState, (void *)tracker->station[i].ButtonState, 
                sizeof(Data->Station[i].ButtonState));

            memcpy((void *)Data->Station[i].AnalogData, (void *)tracker->station[i].AnalogData, 
                sizeof(Data->Station[i].AnalogData));

            tracker->station[i].NewData = false;
        }
        return true;
    }
    return false;
}


/*****************************************************************************
*
*   functionName:   ISD_GetCameraData
*   Description:
*   Created:        12/3/98

*
*   Comments:       get data from all configured stations    
*
******************************************************************************/
BOOL ISD_GetCameraData( ISD_TRACKER_HANDLE handle, ISD_CAMERA_DATA_TYPE *Data )
{
    InterSenseTrackerType *tracker;
    int i;

    if( handle > 0 && handle <= ISD_MAX_TRACKERS )
    {
        tracker = &ISD_tracker[ handle - 1 ];

        /* copy tracker data from internal structures to the user data */
        for(i=0; i<ISD_MAX_STATIONS; i++)
        {
            Data->Camera[i].NewData             = tracker->station[i].NewCameraData;
            Data->Camera[i].Status              = tracker->station[i].Status;
            Data->Camera[i].Timecode            = tracker->station[i].Timecode;
            Data->Camera[i].ApetureEncoder      = tracker->station[i].ApetureEncoder;
            Data->Camera[i].FocusEncoder        = tracker->station[i].FocusEncoder;
            Data->Camera[i].ZoomEncoder         = tracker->station[i].ZoomEncoder;
            Data->Camera[i].TimeCodeUserBits    = tracker->station[i].TimeCodeUserBits;
            Data->Camera[i].Apeture             = tracker->station[i].Apeture;
            Data->Camera[i].Focus               = tracker->station[i].Focus;
            Data->Camera[i].FOV                 = tracker->station[i].FOV;
            Data->Camera[i].NodalPoint          = tracker->station[i].NodalPoint;

            tracker->station[i].NewCameraData = false;
        }
        return true;
    }
    return false;
}


/*****************************************************************************
*
*   functionName:   ISD_SendScript
*   Description:
*   Created:        12/3/98

*
*   Comments:       Send a configuration script to the tracker. Script must 
*                   consist of valid commands as described in the interface 
*                   protocol. Commands in the script should be terminated by
*                   the New Line character '\n'. Line Feed character '\r' is 
*                   added by the function and is not required.   
*
******************************************************************************/
BOOL ISD_SendScript( ISD_TRACKER_HANDLE handle, char *script )
{
    char *buf;
    char command[120];
    InterSenseTrackerType *tracker;
    unsigned int i, len;
    int stringReady = false;

    if( handle > 0 && handle <= ISD_MAX_TRACKERS )
    {
        tracker = &ISD_tracker[ handle - 1 ];
        buf = command;
        len = strlen(script);

        for(i=0; i<len; i++)
        {
            /* time to send the command */
            if(script[i] == '\n' || script[i] == '\r' || script[i] == '\0')
            {
                stringReady = true;
            }
            /* skip the white space */
            else if(script[i] != ' ' && script[i] != '\t')
            {
                *buf++ = script[i];
            }
            if((stringReady && buf != command) || i==len-1)
            {
                *buf = '\0';
                
                /* do not allow user to change some of the settings */
                if(ISD_allowUserCommand( &command[0] ))
                {
                    /* give the tracker time to process previous command */
                    tdelay(0.05f);
                    
                    /* CR LF pair is added in ISD_sendCommand */
                    ISD_sendCommand( tracker, command );
                }
                buf = command;
                stringReady = false;
            }
        }
        return true;
    }
    return false;
}



