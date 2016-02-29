//-----------------------------------------------------------------------------
//
//    ** DO NOT EDIT THIS FILE! **
//    This file was generated by a tool
//    re-running the tool will overwrite this file.
//
//-----------------------------------------------------------------------------


#include "RawIO.h"
#include "RawIO_Microsoft_SPOT_Hardware_AddressSpaceUInt32.h"

using namespace Microsoft::SPOT::Hardware;


HRESULT Library_RawIO_Microsoft_SPOT_Hardware_AddressSpaceUInt32::get_Item___U4__U4( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        UINT32 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 1, param0 ) );

        UINT32 retVal = AddressSpaceUInt32::get_Item( pMngObj,  param0, hr );
        TINYCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retVal );

    }
    TINYCLR_NOCLEANUP();
}

HRESULT Library_RawIO_Microsoft_SPOT_Hardware_AddressSpaceUInt32::set_Item___VOID__U4__U4( CLR_RT_StackFrame& stack )
{
    TINYCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        UINT32 param0;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 1, param0 ) );

        UINT32 param1;
        TINYCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 2, param1 ) );

        AddressSpaceUInt32::set_Item( pMngObj,  param0, param1, hr );
        TINYCLR_CHECK_HRESULT( hr );
    }
    TINYCLR_NOCLEANUP();
}