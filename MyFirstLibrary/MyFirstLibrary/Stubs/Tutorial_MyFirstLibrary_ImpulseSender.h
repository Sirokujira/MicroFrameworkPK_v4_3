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


#ifndef _TUTORIAL_MYFIRSTLIBRARY_IMPULSESENDER_H_
#define _TUTORIAL_MYFIRSTLIBRARY_IMPULSESENDER_H_

namespace MyFirstLibrary
{
    struct ImpulseSender
    {
        // Helper Functions to access fields of managed object
        static UINT32& Get__pin( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UINT32( pMngObj, Library_Tutorial_MyFirstLibrary_ImpulseSender::FIELD___pin ); }

        // Declaration of stubs. These functions are implemented by Interop code developers
        static void WritePulse( CLR_RT_HeapBlock* pMngObj, INT8 param0, INT32 param1, HRESULT &hr );
    };
}
#endif  //_TUTORIAL_MYFIRSTLIBRARY_IMPULSESENDER_H_
