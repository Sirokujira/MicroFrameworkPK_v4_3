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


#include "RawIO.h"
#include "RawIO_Microsoft_SPOT_Hardware_AddressSpaceUInt16.h"

using namespace Microsoft::SPOT::Hardware;

UINT16 AddressSpaceUInt16::get_Item( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr )
{
    void* pTemp;
    hr = AddressSpace::GetAddress(pObj, Offset, pTemp);
    if(FAILED(hr))
        return 0;
    return *reinterpret_cast<UINT16*>(pTemp);
}

void AddressSpaceUInt16::set_Item( CLR_RT_HeapBlock* pMngObj, UINT32 param0, UINT16 param1, HRESULT &hr )
{
    void* pTemp;
    hr = AddressSpace::GetAddress(pObj, Offset, pTemp);
    if(FAILED(hr))
        return;
    *reinterpret_cast<UINT16*>(pTemp) = value;
}
