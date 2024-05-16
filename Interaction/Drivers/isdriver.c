/***********************************************************************
*
*    File Name:      isdriver.c
*    Description:    contains main functionality of the library
*    Created:        11/15/97
*    Author:         Yury Altshuler
*
*    Copyright:      InterSense 1999 - All rights Reserved.
*
*                    
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>

#include "isense.h"

#if !defined UNIX 
#include <windows.h>
#include <winbase.h>
#include <windowsx.h>
#endif

#include "isdriver.h"
#include "itcom.h"
#include "timer.h"
#include "serial.h"

#define ISD_INTERTRAX_SAMPLE_MODE  IT_COM_SYS_CONTINUOUS

static BOOL ISD_detectTracker(InterSenseTrackerType *tracker, DWORD commPort);
static BOOL ISD_resetInterTrax(InterSenseTrackerType *tracker);
static BOOL ISD_openInterTrax(InterSenseTrackerType *tracker);

extern InterSenseTrackerType ISD_tracker[ISD_MAX_TRACKERS];

/*****************************************************************************
*
*   functionName:   ISD_detectTracker
*   Description:
*   Created:        12/3/98
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_detectTracker(InterSenseTrackerType *tracker, DWORD commPort)
{
    char   *systemName[MAX_HARDWARE_VERSIONS] =
              {"IS-300 Series", "IS-600 Series", "IS-900 Series", "InterTrax", "Unknown"};
    char   *message[4] = {"first", "second", "third", "fourth"};
    DWORD  rates[MAX_BAUD_RATES] = {115200L, 57600L, 38400L, 19200L, 9600L};
    DWORD  port, baudRate;
    WORD   numTracker;
    float  startTime;

    ISD_NumOpenTrackers( &numTracker );
    ISD_printf(tracker, "\nLooking for %s InterSense tracking device. \n",
        message[numTracker] );

    port = commPort;

    /* ports greater that 1000 are assumed to be network ports */
    if(commPort >= 1000 || commPort == 0)
    {
        if(commPort == 0) /* try the default */
            port = 5001;

        if(wsockBroadcastClientInit(&tracker->wsock, (unsigned short) port))
        {
            startTime = timeNow();
        
            while(timeNow() - startTime < 0.4)
            {
                if(wsockReceiveData(tracker) > 0)
                {
                    tracker->CommPort.portNumber = (unsigned short) port;

                    ISD_printf(tracker, "\n%s device detected on network port %d\n",
                        systemName[toIndex(tracker->state.hardwareVersion)], port);
                
                    return PASS;
                }
            }
            wsockCloseAll(&tracker->wsock);
        }
    }    
    
    port = commPort;
    if(commPort == 0)
    {
        port = 1;
    }

#if !defined UNIX
    ISD_printf(tracker, "Please wait ");
#endif

    for(; port <= MAX_COM_PORTS; port++)
    {
        /* first try to detect InterTrax */
        if(rs232InitCommunications(&(tracker->CommPort), port, 38400L))
        { 
            /* port is available. Try to detect the tracker */
            if(ISD_openInterTrax(tracker))
            {
                tracker->hardware = ISD_INTERTRAX_SERIES;
                ISD_printf(tracker, "\nInterTrax detected on port %d\n", port);
                return PASS;
            }
        }

        for(baudRate = 0; baudRate < MAX_BAUD_RATES; baudRate++)
        {
            if(rates[baudRate] > MAX_BAUD_RATE) continue;

#if !defined UNIX
            ISD_printf(tracker, ".");
#endif

            if(rs232InitCommunications(&(tracker->CommPort), port, rates[baudRate]))
            {
                itSendCommand(tracker, "c");
                tdelay(0.1f);
                rs232RxFlush(&(tracker->CommPort), 0);
                itSendCommand(tracker, "S"); /* just to make sure */

                if(itComUpdateIntrackSystemStat(tracker, 0.15f) == PASS)
                {
                    /* change baud rate to the maximum supported, defined in isense.h */
                    if(rates[baudRate] != MAX_BAUD_RATE)
                    {
                        itSendCommand(tracker, "o%d,N,8,0\r\n", MAX_BAUD_RATE/100);
                        tdelay(0.1f);
                        rs232InitCommunications(&(tracker->CommPort), port, MAX_BAUD_RATE);
                    }

                    ISD_printf(tracker, "\n%s device detected on port %d\n",
                        systemName[toIndex(tracker->state.hardwareVersion)], port);

                    ISD_printf(tracker, "Firmware Version: ");

                    if(itComUpdateStatusRecord(tracker, 2.0f) == PASS)
                    {
                        /* bug fix for Intrack versions 2.0.5b and below. Version string
                           starts with a blank */

                        if(tracker->state.ver[0] == ' ')  ISD_printf(tracker, "%s\n",  tracker->state.ver);
                        else                              ISD_printf(tracker, " %s\n", tracker->state.ver);
                    }
                    else
                    {
                        ISD_printf(tracker, " unknown\n");
                        return FAIL;
                    }
    
                    tracker->hardware = ISD_PRECISION_SERIES;
                    return PASS;
                }
            }
        }

        /* do not try to detect on other ports */
        if(commPort != 0)
        {
            rs232DeinitCommunications(&(tracker->CommPort));
            ISD_printf(tracker, "\nInterSense tracker not present\n");
            return FAIL;
        }
    }
    rs232DeinitCommunications(&(tracker->CommPort));
    ISD_printf(tracker, "\nInterSense tracker not present\n");
    return FAIL;
}


/*****************************************************************************
*
*   functionName:   ISD_resetInterTrax
*   Description:
*   Created:
*
*   Comments:       
*
******************************************************************************/
static BOOL ISD_resetInterTrax(InterSenseTrackerType *tracker)
{
    char byte;
    int  status, pass, num;
    COMM_PORT *port;

    port = &(tracker->CommPort);

#if defined USE_RTS_LINE

    rs232SetSpeed(port, 1200L);
    for(pass = 0; pass < 1; pass++)
    {
        /* Toggle RTS line */
        rs232SetRTSState(port, false);
        tdelay(0.15f);
        rs232SetRTSState(port, true);
        tdelay(0.1f);

        rs232PutChar(port,  'c');
        status = waitForChar(port,  &byte);
        if(status && byte == 'o') return PASS;
    }
    return FAIL;

#else

    /* If your operating system does not support direct control of RTS line use this code.
	   Here we have to jump through a few hoops to connect to the tracker */

    /* First, check if tracker is already running */
    tracker->state.transmitMode = ISD_INTERTRAX_SAMPLE_MODE;
    rs232SetSpeed(port, 38400L);
    num = tracker->numSamplesReceived;
    tdelay(0.1);

    for(pass = 0; pass < 2; pass++)
    {
        ISD_INTERTRAX_serviceSerialPort(tracker);
        if(tracker->numSamplesReceived != num) return 2;
    }
    
    for(pass = 0; pass < 2; pass++)
    {
        /* try 38400 command mode */
        rs232SetSpeed(port, 38400L);
        rs232PutChar(port,  'c');
        status = waitForChar(port,  &byte);
        if(status && byte == 'o') return PASS;
        if(status && byte == '!')
        {
            waitForChar(port,  &byte);
            return PASS;    
        }

        /* try 1200 command mode */
        rs232SetSpeed(port, 1200L);
        rs232PutChar(port,  'c');
        status = waitForChar(port,  &byte);
        if(status && byte == 'o') return PASS;
        if(status && byte == '!')
        {
            waitForChar(port,  &byte);
            return PASS;
        }

        /* try 38400 reset */
        rs232SetSpeed(port, 38400L);
        rs232SendString(port, "*reset");
        tdelay(0.1f);

        rs232SetSpeed(port, 38400L);
        rs232PutChar(port,  'c');
        if(status && byte == 'o') return PASS;
        if(status && byte == '!')
        {
            waitForChar(port,  &byte);
            return PASS;
        }

        /* try 1200 reset */
        rs232SetSpeed(port, 1200L);
        rs232SendString(port, "*reset");
        tdelay(0.1f);

        rs232SetSpeed(port, 38400L);
        rs232PutChar(port,  'c');
        if(status && byte == 'o') return PASS;
        if(status && byte == '!')
        {
            waitForChar(port,  &byte);
            return PASS;
        }
    } 
#endif

    return FAIL;
}


/*****************************************************************************
*
*   functionName:   ISD_openInterTrax
*   Description:
*   Created:
*
*   Comments:       
*
******************************************************************************/
static BOOL ISD_openInterTrax(InterSenseTrackerType *tracker)
{
    char byte;
    int  status;
    COMM_PORT *port;

    port = &(tracker->CommPort);

    if((status = ISD_resetInterTrax(tracker)) == FAIL) return FAIL;
    if(status ==  2) return PASS;

    /* set output mode to continuous */
    rs232PutChar(port, 't');
    rs232PutChar(port, 2);

    status = waitForChar(port,  &byte);
    if(!status || byte != 't')
    {
        return FAIL;
    }

    /* set baud rate */
    rs232PutChar(port, 'b');
    rs232PutChar(port, 4);

    status = waitForChar(port,  &byte);
    if(!status || byte != 'b')
    {
        return FAIL;
    }

    /* Finished setting up tracker, so switch to data mode */

    rs232PutChar(port,  'd');
    status = waitForChar(port,  &byte);
    if(!status || byte != 'd')
    {
        return FAIL;
    }

    /* Set baud rate to specified baud rate */
    rs232SetSpeed(port, 38400L);

    return PASS;
}


/*****************************************************************************
*
*   functionName:   ISD_sendCommand
*   Description:    Sends command string to the tracker
*   Created:        12/4/98
*
*   Comments:       User is not allowed to send commands directly to the tracker. 
*                   Uses itSendCommand to make sure that applyPSEConfigPending is
*                   set correctly.    
*
******************************************************************************/
BOOL ISD_sendCommand(InterSenseTrackerType *tracker, char *command)
{
    if(wsockIsClient(tracker)) return FAIL;

    if(tracker->hardware == ISD_PRECISION_SERIES)
    {
        /* add CR LF pair to command */
        if(strlen(command) > 1)
        {
            itSendCommand(tracker, "%s\r\n", command);
        }
        else
        {
            /* single character commands can be safely sent out without
               checking if they change PSE configuration */
            rs232PutChar(&(tracker->CommPort), command[0]);
        }
        return PASS;
    }
    return FAIL;
}


/*****************************************************************************
*
*   functionName:   ISD_requestTrackerUpdate
*   Description:    
*   Created:        12/4/98
*
*   Comments:       RecvUpdateFlag is set to True when update is received.
*                   If it is set then send command requesting another update.
*                   If it's False then check if timed out and send a request
*                   again. This prevents procedure from sending too many commands
*                   when communication is slow or failed 
*
******************************************************************************/
void ISD_requestTrackerUpdate(InterSenseTrackerType *tracker)
{
    if(wsockIsClient(tracker)) return;

    switch(tracker->hardware)
    {
        case ISD_PRECISION_SERIES:

            if(tracker->state.transmitMode == IT_COM_SYS_POLLED)
            {
                if(tracker->RecvUpdateFlag >= tracker->state.numActive ||
                     (timeNow() - tracker->LastReqTime) > 0.1)
                {
                    tracker->LastReqTime = timeNow();
                    rs232PutChar(&tracker->CommPort, 'P');
                    tracker->RecvUpdateFlag = 0;
                }
            }
            break;

        case ISD_INTERTRAX_SERIES:

            if(tracker->state.transmitMode == IT_COM_SYS_POLLED)
            {
                if(tracker->RecvUpdateFlag > 0 ||
                    (timeNow() - tracker->LastReqTime) > 0.1)
                {
                    tracker->LastReqTime = timeNow();
                    tracker->RecvUpdateFlag = 0;

                    /* Flush any characters in the serial buffer */
                    rs232RxFlush(&tracker->CommPort, 0);

                    /* Request a 3DOF data packet from the tracker */
                    rs232PutChar(&tracker->CommPort, '3'); 
                }
            }
            break;

        default:
            break;
    }
}


/*****************************************************************************
*
*   functionName:   ISD_getTrackerData
*   Description:
*   Created:        12/4/98
*
*   Comments:      
*
******************************************************************************/
void ISD_getTrackerData(InterSenseTrackerType *tracker)
{
    float currentTime;

    ISD_requestTrackerUpdate(tracker);

    currentTime = timeNow();
    
    if(currentTime - tracker->statsTime > 1.0)
    {
        tracker->RecordsPerSec = (WORD) (tracker->numSamplesReceived/(currentTime - tracker->statsTime));
        tracker->KBitsPerSec = (float) (tracker->numBytesReceived/100.0f)/(currentTime - tracker->statsTime);

        tracker->numBytesReceived = 0;
        tracker->numSamplesReceived = 0;

        tracker->statsTime = currentTime;
    }
    serviceSerialPort(tracker);
    wsockReceiveData(tracker);
}

/*****************************************************************************
*
*   functionName:   ISD_initTracker
*   Description:
*   Created:
*
*   Comments:       
*
******************************************************************************/
void ISD_initTracker(InterSenseTrackerType *tracker)
{
    memset((void *)tracker, 0, sizeof(InterSenseTrackerType));

    wsockInit( &tracker->wsock );

    /* init variables */
    tracker->hardware = ISD_NONE;
    tracker->applyPSEConfigPending = false;

    itComLogClear( tracker );
}


/*****************************************************************************
*
*   functionName:   ISD_openTracker
*   Description:
*   Created:
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_openTracker(InterSenseTrackerType *tracker, 
                     DWORD commPort, BOOL infoScreen, BOOL verbose)
{
    WORD i;
    BOOL status;
    ISD_STATION_INFO_TYPE Station;
    static int init = false;

    if(!init)
    {
        ISD_printf(tracker, "InterSense Driver Ver. %.2f\n", ISLIB_VERSION);
        ISD_printf(tracker, "Copyright 1998-2000 InterSense Inc.\n");
        init = true;
        initTimer();
    }

    ISD_initTracker(tracker);

    if(!ISD_detectTracker(tracker, commPort))
    {
        /* failed to detect an InterSense tracking device */
        return FAIL;
    }

    tracker->statsTime = timeNow();

    /* found a upd broadcast server */
    if(wsockIsClient(tracker))
    {
        return PASS;
    }

    if(tracker->hardware == ISD_INTERTRAX_SERIES)
    {
        tracker->state.transmitMode = ISD_INTERTRAX_SAMPLE_MODE;
        return PASS;
    }

    /* switch to polled mode during the configuration session */
    itSendCommand(tracker, "c");

    /* set default station configuration */
    for (i = 1, status = true; i <= ISD_MAX_STATIONS && status;  i++)
    {
        if(tracker->state.hardwareVersion == IS900)
        {
            Station.GetAnalogData = true;
            Station.GetButtons = true;
        }
        else
        {
            Station.GetAnalogData = false;
            Station.GetButtons = false;
        }

        /* no time stamp unless requested by application */
        Station.TimeStamped = false;

        Station.AngleFormat = ISD_EULER;

        /* no camera tracker encoder data */
        Station.GetCameraDataRaw = false;        
        Station.GetCameraDataComputed = false;        

        /* set frame to default */
        Station.CoordFrame = ISD_DEFAULT_FRAME;

        ISD_setOutputRecordList(tracker, &Station, i);  

        status = itComUpdateOutputList(tracker, i);
    }
    return PASS;
}

/*****************************************************************************
*
*   functionName:   ISD_closeTracker
*   Description:
*   Created:        12/3/98
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_closeTracker(InterSenseTrackerType *tracker)
{
    if(!wsockIsClient(tracker))
    {
        /* switch to polled mode */
        itSendCommand(tracker, "c");
        tdelay(0.1f);
        
        rs232DeinitCommunications(&(tracker->CommPort));
    }
    
    tracker->hardware = ISD_NONE;
    return PASS;
}


/*****************************************************************************
*
*   functionName:   ISD_getCommInfo
*   Description:
*   Created:        12/4/98
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_getCommInfo( InterSenseTrackerType *tracker, 
                      ISD_TRACKER_INFO_TYPE *Tracker )
{
    WORD systemModel[MAX_HARDWARE_VERSIONS] =
              {ISD_IS300, ISD_IS600, ISD_IS900, ISD_INTERTRAX, ISD_UNKNOWN};
    BOOL status = PASS;

    Tracker->TrackerType = tracker->hardware;  
    
    if(tracker->hardware == ISD_PRECISION_SERIES)
    {
        Tracker->TrackerModel = systemModel[toIndex(tracker->state.hardwareVersion)];   
    }
    else
    {
        Tracker->TrackerModel = ISD_INTERTRAX;   
    }

    Tracker->CommPort      = tracker->CommPort.portNumber;
    Tracker->RecordsPerSec = tracker->RecordsPerSec;
    Tracker->KBitsPerSec   = tracker->KBitsPerSec;

    return PASS;
}


/*****************************************************************************
*
*   functionName:   ISD_getTrackerConfig
*   Description:
*   Created:        12/4/98
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_getTrackerConfig( InterSenseTrackerType *tracker, 
                           ISD_TRACKER_INFO_TYPE *Tracker, BOOL verbose )
{
    WORD systemModel[MAX_HARDWARE_VERSIONS] =
              {ISD_IS300, ISD_IS600, ISD_IS900, ISD_INTERTRAX, ISD_UNKNOWN};
    BOOL status = PASS;

    Tracker->TrackerType = tracker->hardware;  
    
    if(tracker->hardware == ISD_PRECISION_SERIES)
    {
        Tracker->TrackerModel = systemModel[toIndex(tracker->state.hardwareVersion)];   
    }
    else
    {
        Tracker->TrackerModel = ISD_INTERTRAX;   
    }

    Tracker->CommPort      = tracker->CommPort.portNumber;
    Tracker->RecordsPerSec = tracker->RecordsPerSec;
    Tracker->KBitsPerSec   = tracker->KBitsPerSec;
  
    if(!wsockIsClient(tracker) && tracker->hardware == ISD_PRECISION_SERIES)
    {
        if(verbose)
            ISD_printf(tracker, "Reading tracker system information ... ");

        if((status = itComUpdateStatusRecord(tracker, IT_COM_STATUS_REC_TIMEOUT)) == PASS)
        {
            /* genlock feature is available with firmware versions 3.016 and above */
            if(tracker->state.firmwareVersion >= 3.0160f)
            {
                /* get sync state */
                status = itComUpdateSyncState(tracker);

                Tracker->SyncState = tracker->SyncState;  
                Tracker->SyncRate  = tracker->SyncRate;     
                Tracker->SyncPhase = tracker->SyncPhase; 
            }
        }

        if(verbose)
            ISD_printf(tracker, "%s\n", status == PASS ? "done" : "failed");
    }

    return status;
}


/*****************************************************************************
*
*   functionName:   ISD_setTrackerConfig
*   Description:
*   Created:        12/4/98
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_setTrackerConfig( InterSenseTrackerType *tracker, 
                           ISD_TRACKER_INFO_TYPE *Tracker, BOOL verbose )
{
    BOOL status = FAIL;

    if(!wsockIsClient(tracker) && tracker->hardware == ISD_PRECISION_SERIES)
    {
        /* genlock is supported by firmware versions 3.0160 and higher */
        if(tracker->state.firmwareVersion >= 3.0160f)
        {
            /* send sync command */
            itSendCommand(tracker, "MG%d,%.2f\r\n", (int) Tracker->SyncState, (float) Tracker->SyncRate);
            itSendCommand(tracker, "MGP%d\r\n", (int) Tracker->SyncPhase);
        }
    }

    return ISD_getTrackerConfig(tracker, Tracker, verbose);
}


/*****************************************************************************
*
*   functionName:   ISD_getStationState
*   Description:
*   Created:        12/4/98
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_getStationConfig( InterSenseTrackerType *tracker, 
                           ISD_STATION_INFO_TYPE *Station,
                           WORD stationNum, BOOL verbose )
{
    BOOL status = true;
    int i;

    if(wsockIsClient(tracker))
    {
        return wsockGetStationConfig(tracker, Station, stationNum, verbose);
    }

    /* fill in the defaults */
    memset((void *) Station, 0, sizeof(ISD_STATION_INFO_TYPE));

    Station->ID                     = stationNum;
    Station->GetButtons             = false;
    Station->GetAnalogData          = false;
    Station->TimeStamped            = false;
    Station->AngleFormat            = ISD_EULER;
    Station->GetCameraDataRaw       = false;
    Station->GetCameraDataComputed  = false;
    Station->CoordFrame             = ISD_DEFAULT_FRAME;
    Station->InertiaCube            = 1;
    Station->Sensitivity            = 3;
    Station->Enhancement            = 2;
    Station->Compass                = 2;
    Station->Prediction             = 0;

    /* this function should only be used with IS series devices  */
    if(tracker->hardware != ISD_PRECISION_SERIES)
    {
        return FAIL;
    }

    if(verbose)
        ISD_printf(tracker, "Reading configuration for station %d ... ", (int) stationNum);

    status = itComUpdateOutputList(tracker, stationNum);
    if(status)
    {
        /* look for the time stamp item in the output record list */
        for(i=0; i<MAX_OUTPUT_LIST_ITEMS && tracker->station[stationNum-1].outputList[i] != -1; i++)
        {
            switch(tracker->station[stationNum-1].outputList[i])
            {
                case 38:
                case 39:
                    Station->CoordFrame = ISD_VSET_FRAME;
                    break;
                case 21:
                    Station->TimeStamped = true;
                    break;
                case 22:
                    Station->GetButtons = true;
                    break;
                case 4:
                    Station->AngleFormat = ISD_EULER;
                    break;
                case 11:
                    Station->AngleFormat = ISD_QUATERNION;
                    break;
                case 23:
                    Station->GetAnalogData = true;
                    break;
                case 43:
                case 44:
                case 45:
                    Station->GetCameraDataRaw = true;
                    break;
                case 41:
                case 42:
                case 46:
                case 47:
                case 48:
                case 49:
                    Station->GetCameraDataComputed = true;
                    break;
            }
        }

        status = itComUpdateStationStat(tracker, stationNum);
        if(status)
        {
            Station->State = tracker->station[stationNum-1].state;

            status = itComUpdateSensorStat(tracker, stationNum);
            if(status)
            {
                Station->Enhancement = tracker->station[stationNum-1].enhancement;
                Station->Compass     = tracker->station[stationNum-1].compass;

                status = itComUpdatePredictionString(tracker, stationNum);
                if(status)
                {
                    Station->Prediction = tracker->station[stationNum-1].prediction;

                    /* InertiaCube assignment is a feature of firmware 3.0 and above */
                    if(tracker->state.firmwareVersion >= 3.0f)
                    {
                        status = itComUpdateImu(tracker, stationNum);
                        if(status)
                        {
                            Station->InertiaCube = tracker->station[stationNum-1].InertiaCube;

                            status = itComUpdateSensitivityVal(tracker, stationNum);
                            if(status)
                            {
                                Station->Sensitivity = tracker->station[stationNum-1].sensitivity;
                            }
                        }
                    }
                    else
                    {
                        Station->InertiaCube = 1;
                        Station->Sensitivity = 1;
                    }
                }
            }
        }
    }
    
    if(verbose)
        ISD_printf(tracker, "%s\n", status == PASS ? "done" : "failed");

    return status;
}


/*****************************************************************************
*
*   functionName:   ISD_setOutputRecordList
*   Description:    set output record list for a station
*   Created:        12/8/98
*
*   Comments:       Output record list is determined by the tracker model,
*                   firmware version, and time stamp request status.
*
******************************************************************************/
BOOL ISD_setOutputRecordList ( InterSenseTrackerType *tracker, 
                               ISD_STATION_INFO_TYPE *Station,
                               WORD stationNum )
{
    char command[100];
    int pos = 0;

    /* this function should only be used with IS series devices  */
    if(tracker->hardware != ISD_PRECISION_SERIES)
    {
        return FAIL;
    }

    /* only use time stamps if firmware version is greater than 3.0 */
    if(tracker->state.firmwareVersion >= 3.0f)
    {
        pos += sprintf(&command[pos], "O%c,", stationToChar(stationNum));

        switch(Station->CoordFrame)
        {
            case ISD_VSET_FRAME:
                if(tracker->state.hardwareVersion == IS600 || tracker->state.hardwareVersion == IS900)
                    pos += sprintf(&command[pos], "38,");

                pos += sprintf(&command[pos], "39");
                break;

            default:
                if(tracker->state.hardwareVersion == IS600 || tracker->state.hardwareVersion == IS900)
                    pos += sprintf(&command[pos], "2,");

                pos += sprintf(&command[pos], "%d", Station->AngleFormat == ISD_QUATERNION ? 11 : 4);
                break;
        }

        if(Station->TimeStamped) pos += sprintf(&command[pos], ",21");

        if(tracker->state.hardwareVersion & IS900)
        {
            /* always get tracking status, just one byte */
            pos += sprintf(&command[pos], ",40");

            if(Station->GetButtons)     pos += sprintf(&command[pos], ",22");
            if(Station->GetAnalogData)  pos += sprintf(&command[pos], ",23");
            if(Station->GetCameraDataRaw)       pos += sprintf(&command[pos], ",43,44,45");
            if(Station->GetCameraDataComputed)  pos += sprintf(&command[pos], ",41,46,47,48,49");
        }

        pos += sprintf(&command[pos], ",1\r\n");
        
        itSendCommand(tracker, command);
    }

    /* firmware versions prior to 3.0 did not have hardware identifier, so it is not
       possible to determine if the tracker is 3 or 6 DOF. Also, not all versions 
       supported time stamp */
    else
    {
        itSendCommand(tracker, "O%c,2,%d,1\r\n", stationToChar(stationNum),
            Station->AngleFormat == ISD_QUATERNION ? 11 : 4);
    }

    return true;
}


/*****************************************************************************
*
*   functionName:   ISD_setStationConfig
*   Description:
*   Created:        12/4/98
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_setStationConfig ( InterSenseTrackerType *tracker, 
                            ISD_STATION_INFO_TYPE *Station,
                            WORD stationNum, BOOL verbose, BOOL apply )
{
    ISD_STATION_INFO_TYPE PrevStationState;

    if(wsockIsClient(tracker))
    {
        return wsockSetStationConfig(tracker, Station, stationNum, verbose);
    }

    /* this function should only be used with IS series devices  */
    if(tracker->hardware != ISD_PRECISION_SERIES)
    {
        return FAIL;
    }

    /* start by obtaining current state of the station */
    if(ISD_getStationConfig(tracker, &PrevStationState, stationNum, false))
    {
        /* check if any changes are requested */
        if(memcmp((void *)&PrevStationState, 
                   (void *)&Station, sizeof(ISD_STATION_INFO_TYPE)) != 0)
        {
            if(verbose)
                ISD_printf(tracker, "Sending configuration for station %d ... ", (int) stationNum);

            /* set output record list */
            ISD_setOutputRecordList(tracker, Station, stationNum);

            itSendCommand(tracker, "Mp%c,%d\r\n", stationToChar(stationNum), (short)Station->Prediction);
            itSendCommand(tracker, "MQ%c,%d\r\n", stationToChar(stationNum), (short)Station->Sensitivity);
            itSendCommand(tracker, "MF%c,%d\r\n", stationToChar(stationNum), (short)Station->Enhancement);

            if(Station->State)   itSendCommand(tracker, "l%c,1\r\n", stationToChar(stationNum));
            else                 itSendCommand(tracker, "l%c,0\r\n", stationToChar(stationNum));

            if(tracker->state.firmwareVersion < 3.0160f)
            {    
                if(Station->Compass != 0) itSendCommand(tracker, "MH%c\r\n", stationToChar(stationNum));
                else                      itSendCommand(tracker, "Mh%c\r\n", stationToChar(stationNum));
            }
            else
            {
                itSendCommand(tracker, "MH%c,%d\r\n", stationToChar(stationNum), (int) Station->Compass);
            }

            /* only send InertiaCube command if different from current configuration */
            if(PrevStationState.InertiaCube != Station->InertiaCube)
            {
                /* disassociate InertiaCube from the station */
                if(Station->InertiaCube == -1 && PrevStationState.InertiaCube > 0)
                {
                    itSendCommand(tracker, "MCi%c,%d\r\n", stationToChar(stationNum), 
                        PrevStationState.InertiaCube);

                    if(apply)
                    {
                        ISD_applyConfiguration(tracker, verbose);
                    }
                    else
                    {
                        tracker->applyPSEConfigPending = true;
                    }
                }
                if(Station->InertiaCube > 0)
                {
                    itSendCommand(tracker, "MCI%c,%d\r\n", stationToChar(stationNum), 
                        Station->InertiaCube);
                    
                    if(apply)
                    {
                        ISD_applyConfiguration(tracker, verbose);
                    }
                    else
                    {
                        tracker->applyPSEConfigPending = true;
                    }
                }
            }

            if(verbose)
                ISD_printf(tracker, "done\n");

            /* confirm new configuration */
            return(ISD_getStationConfig(tracker, Station, stationNum, verbose));
        }
    }
    return FAIL;
}


/*****************************************************************************
*
*   functionName:   ISD_configureTracker
*   Description:    Configure tracker for best performance
*   Created:        12/30/98
*
*   Comments:       
*
******************************************************************************/
BOOL ISD_configureTracker( InterSenseTrackerType *tracker, BOOL verbose )
{
    BOOL status;
    ISD_TRACKER_INFO_TYPE Tracker;
    char c;

    if(wsockIsClient(tracker))
        return FAIL;

    /* this function should only be used with IS series devices  */
    if(tracker->hardware != ISD_PRECISION_SERIES)
    {
        return FAIL;
    }

    /* output record list for stations is first set in ISD_openTracker, then
       in ISD_configureFromFile. In both cases settings are confirmed and there is
       no need to repeat that here. */
    
    if(verbose)
       ISD_printf(tracker, "Finalizing tracker configuration ... ");

    /* binary mode */
    itSendCommand(tracker, "f");

    /* centimeters */
    itSendCommand(tracker, "u");

#ifdef USE_POLLED_MODE
    /* polled mode */
    itSendCommand(tracker, "c");
#else
    /* continuous mode */
    itSendCommand(tracker, "C");
#endif

    /* make sure output is not suspended */
    c = 'Q' - 'A' + 1; /* RESUME command */
    itSendCommand(tracker, &c);

    /* get active station list, particularly important in polled mode */
    status = itComUpdateStationStat(tracker, 1);

    /* Get system status parameters so that data records can be
       correctly interpreted */

    status = ISD_getTrackerConfig(tracker, &Tracker, false);

    if(verbose)
        ISD_printf(tracker, "%s\n", status == PASS ? "done" : "failed");

    return status;
}


/*****************************************************************************
*
*   functionName:   ISD_applyConfiguration
*   Description:
*   Created:        12/23/98
*
*   Comments:       apply PSE configuration   
*
******************************************************************************/
BOOL ISD_applyConfiguration( InterSenseTrackerType *tracker, BOOL verbose )
{
    float startTime;
    BOOL status = PASS;

    if(wsockIsClient(tracker))
        return FAIL;

    if(tracker->applyPSEConfigPending)
    {
        if(verbose)
        {
            ISD_printf(tracker, "Tracker configration changed, sending MCe command to apply ... ");
        }

        itSendCommand(tracker, "MCe\r\n");

        /* wait no more that 7 seconds for tracker to finish configuration */
        for(startTime = timeNow(); timeNow() - startTime < 7.0;)
        {
            if((status = itComUpdateIntrackSystemStat(tracker, 0.5f)) == PASS)
            {
                break;
            }
        }
        if(verbose)
        {
            if(status)  ISD_printf(tracker, "done\n");
            else        ISD_printf(tracker, "failed\n");
        }            
    }
    return status;
}


/*****************************************************************************
*
*   functionName:   ISD_allowUserCommand
*   Description:
*   Created:        12/23/98
*
*   Comments:       Check if this configuration file or script command is allowed   
*
******************************************************************************/
BOOL ISD_allowUserCommand ( char *cmd )
{
    switch(*cmd)
    {
        /* following configuration parameters are determined by the DLL and should 
           not be changed by the user */

        case 'O':   /* output record list */
        case 'C':   /* continuous mode */
        case 'c':   /* polled mode */
        case 'F':   /* ASCII mode */
        case 'f':   /* binary mode */
        case 'o':   /* communication parameters */
        case 'U':   /* units inches */
        case 'u':   /* units centimeters */
            return false;

        default:
            return true;
    }
}


/***************************************************************************/
void ISD_printf ( InterSenseTrackerType *tracker, char *fs,... )
{
    char sbuf[256];
    va_list argptr;

    va_start(argptr,fs);
    vsprintf(sbuf, fs,argptr);
    va_end(argptr);

    printf("%s", sbuf);
}


/******************************************************************************/
int itComLogCount( InterSenseTrackerType *tracker )
{
    if( tracker->log.head >= tracker->log.tail )
    {
        return tracker->log.head - tracker->log.tail;
    }
    else
    {
        return MAX_LOG - tracker->log.tail + tracker->log.head;
    }
}


/*******************************************************************************/
int ISD_GetRecordLog( ISD_TRACKER_HANDLE handle, char *records[] )
{
    InterSenseTrackerType *tracker;
    int i, count = 0, index;

    if( handle > 0 && handle <= ISD_MAX_TRACKERS )
    {
        tracker = &ISD_tracker[ handle - 1 ];
        serviceSerialPort(tracker);

        count = itComLogCount( tracker );
        index = tracker->log.head;
        
        for( i = 0; i < count; i++ )
        {
            if( --index < 0 ) index = MAX_LOG - 1;
            records[i] = tracker->log.records[index];
        }
    }
    return count;
}
