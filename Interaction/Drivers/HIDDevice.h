// Wiim API ©2006 Eric B.
// http://digitalretrograde.com/projects/wiim/

// May be used and modified freely as long as this message is left intact

#include "defines.h"

#include <wtypes.h>
#include <initguid.h>
#include <process.h>

#include <string>
#include <vector>

using namespace std;

#include "Console.h"

#define MAX_LOADSTRING 256

extern "C" {

// This file is in the Windows DDK available from Microsoft.
#include "hidsdi.h"

#include <setupapi.h>
#include <dbt.h>
}

#pragma once

static const int WIIMOTE_VENDOR_ID = 0x057E;
static const int WIIMOTE_PRODUCT_ID = 0x0306;

class HIDDevice
{
protected:
	int				m_controller_id;

	HIDP_CAPS		m_capabilities;
	HANDLE			m_device_handle, m_read_handle, m_write_handle;
	HANDLE			m_event_object;
	OVERLAPPED		m_hid_overlapped;
	LPOVERLAPPED	m_lp_overlap;
	string			m_device_path_name;

public:
	HIDDevice(void);
	~HIDDevice(void);

	//static RegisterForDeviceNotifications(Wiimote & device);
	static void PrepareForOverlappedTransfer(HIDDevice & device, PSP_DEVICE_INTERFACE_DETAIL_DATA & detailData);
	static void GetDeviceCapabilities(HIDDevice & device);
	
	void WriteOutputReport(char out_bytes[]);

	/* This function is templated in order to support instantiation of subclassed versions of Wiimote */
	template <class T>
	static vector<T> ConnectToHIDDevices()
	{
		vector<T> devices;

		HANDLE WriteHandle = 0, DeviceHandle = 0;

		HANDLE hDevInfo;

		HIDD_ATTRIBUTES						Attributes;
		SP_DEVICE_INTERFACE_DATA			devInfoData;
		int									MemberIndex = 0;
		LONG								Result;	
		GUID								HidGuid;
		PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData;

		ULONG Required = 0;
		ULONG Length = 0;
		detailData = NULL;
		DeviceHandle = NULL;

		HidD_GetHidGuid(&HidGuid);	
		
		hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
			
		devInfoData.cbSize = sizeof(devInfoData);
		MemberIndex = 0;

		do
		{
			// Got any more devices?
			Result = SetupDiEnumDeviceInterfaces (hDevInfo, 0, &HidGuid, MemberIndex,	&devInfoData);

			if (Result == 0)
				break;

			// Call once to get the needed buffer length
			Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);
			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// After allocating, call again to get data
			Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length, 
				&Required, NULL);

			DeviceHandle = CreateFile(detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, 
				(LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,	0, NULL);

			Attributes.Size = sizeof(Attributes);

			Result = HidD_GetAttributes(DeviceHandle, &Attributes);
			
			if (Attributes.VendorID == WIIMOTE_VENDOR_ID && Attributes.ProductID == WIIMOTE_PRODUCT_ID)
			{
				// If the vendor and product IDs match, we've found a wiimote 
				T device;

				device.m_device_path_name = detailData->DevicePath;
				device.m_device_handle = DeviceHandle;

				// Register to receive device notifications.
				// RegisterForDeviceNotifications();

				GetDeviceCapabilities(device);

				device.m_write_handle = CreateFile(detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
					(LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);

				PrepareForOverlappedTransfer(device, detailData);

				device.m_controller_id = devices.size() + 1;
				devices.push_back(device);
			}
			else
			{
				CloseHandle(DeviceHandle);
			}

			free(detailData);

			MemberIndex = MemberIndex + 1;
		} 
		while (true);

		SetupDiDestroyDeviceInfoList(hDevInfo);

		return devices;
	}
};
