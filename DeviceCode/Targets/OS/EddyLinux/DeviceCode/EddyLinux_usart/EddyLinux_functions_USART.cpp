////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Modified for Linux by Device Drivers Limited.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "tinyhal.h"
#include "LinuxAPI.h"

//--//


BOOL USART_Initialize( int ComPortNum, int BaudRate, int Parity, int DataBits, int StopBits, int FlowValue )
{
    return TRUE;
}

BOOL USART_Uninitialize( int ComPortNum )
{
    return TRUE;
}

int USART_Write( int ComPortNum, const char* Data, size_t size )
{
    return(
		linux_USART_Write(ComPortNum, Data, size )
	);
}

int USART_Read( int ComPortNum, char* Data, size_t size )
{
    return(
		linux_USART_Read(ComPortNum, Data, size )
	);
}

BOOL USART_Flush( int ComPortNum )
{
    return TRUE;
}

BOOL USART_AddCharToRxBuffer( int ComPortNum, char c )
{
    return TRUE;
}

BOOL USART_RemoveCharFromTxBuffer( int ComPortNum, char& c )
{
    return TRUE;
}

INT8 USART_PowerSave( int ComPortNum, INT8 Enable )
{
    return 0;
}

void USART_PrepareForClockStop()
{
}

void USART_ClockStopFinished()
{
}

void USART_ForceXON(int ComPortNum) 
{
}

void USART_CloseAllPorts()
{
}

int  USART_BytesInBuffer( int ComPortNum, BOOL fRx )
{
    return 0;
}

void USART_DiscardBuffer( int ComPortNum, BOOL fRx )
{
}

BOOL USART_ConnectEventSink( int ComPortNum, int EventType, void* pContext, PFNUsartEvent pfnUsartEvtHandler, void** ppArg )
{
    return TRUE;
}

void USART_SetEvent( int ComPortNum, unsigned int event )
{
}

