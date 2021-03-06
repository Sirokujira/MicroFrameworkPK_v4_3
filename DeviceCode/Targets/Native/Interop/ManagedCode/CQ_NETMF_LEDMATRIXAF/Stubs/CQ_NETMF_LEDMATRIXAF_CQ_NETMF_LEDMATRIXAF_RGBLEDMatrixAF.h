//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------


#ifndef _CQ_NETMF_LEDMATRIXAF_CQ_NETMF_LEDMATRIXAF_RGBLEDMATRIXAF_H_
#define _CQ_NETMF_LEDMATRIXAF_CQ_NETMF_LEDMATRIXAF_RGBLEDMATRIXAF_H_

namespace CQ
{
    namespace NETMF
    {
        namespace LEDMATRIXAF
        {
            struct RGBLEDMatrixAF
            {
                // Helper Functions to access fields of managed object
                // Declaration of stubs. These functions are implemented by Interop code developers
                static void RGBLEDMatrixAF_Initialize( CLR_RT_TypedArray_UINT32 param0, HRESULT &hr );
                static void RGBLEDMatrixAF_SelectLine( UINT8 param0, HRESULT &hr );
                static void RGBLEDMatrixAF_TurnOn( HRESULT &hr );
                static void RGBLEDMatrixAF_TurnOff( HRESULT &hr );
                static void RGBLEDMatrixAF_DrawLineColor2( UINT32 param0, CLR_RT_TypedArray_UINT8 param1, CLR_RT_TypedArray_UINT8 param2, UINT32 param3, CLR_RT_TypedArray_UINT8 param4, CLR_RT_TypedArray_UINT8 param5, HRESULT &hr );
                static void RGBLEDMatrixAF_DrawScreenColor2( CLR_RT_TypedArray_UINT32 param0, UINT32 param1, CLR_RT_TypedArray_UINT8 param2, CLR_RT_TypedArray_UINT8 param3, CLR_RT_TypedArray_UINT32 param4, UINT32 param5, CLR_RT_TypedArray_UINT8 param6, CLR_RT_TypedArray_UINT8 param7, UINT32 param8, HRESULT &hr );
                static void RGBLEDMatrixAF_DrawScreenColor2( CLR_RT_TypedArray_UINT8 param0, UINT32 param1, CLR_RT_TypedArray_UINT8 param2, CLR_RT_TypedArray_UINT8 param3, CLR_RT_TypedArray_UINT32 param4, UINT32 param5, CLR_RT_TypedArray_UINT8 param6, CLR_RT_TypedArray_UINT8 param7, UINT32 param8, HRESULT &hr );
                static void RGBLEDMatrixAF_DrawScreenColor64( CLR_RT_TypedArray_UINT8 param0, UINT32 param1, CLR_RT_TypedArray_UINT8 param2, UINT32 param3, UINT32 param4, HRESULT &hr );
                static void RGBLEDMatrixAF_GetFontBitmap( UINT16 param0, CLR_RT_TypedArray_UINT8 param1, HRESULT &hr );
                static void RGBLEDMatrixAF_SetFontsToBuf( CLR_RT_TypedArray_UINT16 param0, CLR_RT_TypedArray_UINT8 param1, HRESULT &hr );
                static void RGBLEDMatrixAF_SetFontsToBuf( CLR_RT_TypedArray_UINT16 param0, CLR_RT_TypedArray_UINT32 param1, HRESULT &hr );
                static void RGBLEDMatrixAF_RotateXBufColor2( CLR_RT_TypedArray_UINT32 param0, INT32 param1, HRESULT &hr );
                static void RGBLEDMatrixAF_Pset( CLR_RT_TypedArray_UINT8 param0, INT32 param1, INT32 param2, UINT8 param3, UINT8 param4, HRESULT &hr );
                static void RGBLEDMatrixAF_Line( CLR_RT_TypedArray_UINT8 param0, INT32 param1, INT32 param2, INT32 param3, INT32 param4, UINT8 param5, UINT8 param6, HRESULT &hr );
                static void RGBLEDMatrixAF_Circle( CLR_RT_TypedArray_UINT8 param0, INT32 param1, INT32 param2, INT32 param3, UINT8 param4, UINT8 param5, HRESULT &hr );
            };
        }
    }
}
#endif  //_CQ_NETMF_LEDMATRIXAF_CQ_NETMF_LEDMATRIXAF_RGBLEDMATRIXAF_H_
