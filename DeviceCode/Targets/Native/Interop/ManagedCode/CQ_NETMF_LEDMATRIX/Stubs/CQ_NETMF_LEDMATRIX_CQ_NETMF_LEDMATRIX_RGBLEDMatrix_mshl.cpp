//-----------------------------------------------------------------------------
//
//    ** DO NOT EDIT THIS FILE! **
//    This file was generated by a tool
//    re-running the tool will overwrite this file.
//
//-----------------------------------------------------------------------------


#include "CQ_NETMF_LEDMATRIX.h"
#include "CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix.h"

using namespace CQ::NETMF::LEDMATRIX;


HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_Initialize___STATIC__VOID__SZARRAY_U4( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_TypedArray_UINT32 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT32_ARRAY( stack, 0, param0 ) );

        RGBLEDMatrix::RGBLEDMatrix_Initialize( param0, hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_SelectLine___STATIC__VOID__U1( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        UINT8 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 0, param0 ) );

        RGBLEDMatrix::RGBLEDMatrix_SelectLine( param0, hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_TurnOn___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        RGBLEDMatrix::RGBLEDMatrix_TurnOn( hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_TurnOff___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        RGBLEDMatrix::RGBLEDMatrix_TurnOff( hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_DrawLine8___STATIC__VOID__U1__U1__U1( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        UINT8 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 0, param0 ) );

        UINT8 param1;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 1, param1 ) );

        UINT8 param2;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 2, param2 ) );

        RGBLEDMatrix::RGBLEDMatrix_DrawLine8( param0, param1, param2, hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_DrawLine64___STATIC__VOID__U1__U1__U1( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        UINT8 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 0, param0 ) );

        UINT8 param1;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 1, param1 ) );

        UINT8 param2;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 2, param2 ) );

        RGBLEDMatrix::RGBLEDMatrix_DrawLine64( param0, param1, param2, hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_DrawPat___STATIC__VOID__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_TypedArray_UINT8 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8_ARRAY( stack, 0, param0 ) );

        RGBLEDMatrix::RGBLEDMatrix_DrawPat( param0, hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_DrawFont8x8___STATIC__VOID__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_TypedArray_UINT8 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8_ARRAY( stack, 0, param0 ) );

        RGBLEDMatrix::RGBLEDMatrix_DrawFont8x8( param0, hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_CQ_NETMF_LEDMATRIX_CQ_NETMF_LEDMATRIX_RGBLEDMatrix::RGBLEDMatrix_GetFontBitmap___STATIC__VOID__U2__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        UINT16 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 0, param0 ) );

        CLR_RT_TypedArray_UINT8 param1;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT8_ARRAY( stack, 1, param1 ) );

        RGBLEDMatrix::RGBLEDMatrix_GetFontBitmap( param0, param1, hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}
