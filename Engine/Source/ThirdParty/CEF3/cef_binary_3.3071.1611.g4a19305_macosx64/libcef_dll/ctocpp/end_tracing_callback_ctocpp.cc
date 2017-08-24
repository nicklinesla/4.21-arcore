// Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//

#include "libcef_dll/ctocpp/end_tracing_callback_ctocpp.h"


// VIRTUAL METHODS - Body may be edited by hand.

void CefEndTracingCallbackCToCpp::OnEndTracingComplete(
    const CefString& tracing_file) {
  cef_end_tracing_callback_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, on_end_tracing_complete))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: tracing_file; type: string_byref_const
  DCHECK(!tracing_file.empty());
  if (tracing_file.empty())
    return;

  // Execute
  _struct->on_end_tracing_complete(_struct,
      tracing_file.GetStruct());
}


// CONSTRUCTOR - Do not edit by hand.

CefEndTracingCallbackCToCpp::CefEndTracingCallbackCToCpp() {
}

template<> cef_end_tracing_callback_t* CefCToCppRefCounted<CefEndTracingCallbackCToCpp,
    CefEndTracingCallback, cef_end_tracing_callback_t>::UnwrapDerived(
    CefWrapperType type, CefEndTracingCallback* c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

#if DCHECK_IS_ON()
template<> base::AtomicRefCount CefCToCppRefCounted<CefEndTracingCallbackCToCpp,
    CefEndTracingCallback, cef_end_tracing_callback_t>::DebugObjCt = 0;
#endif

template<> CefWrapperType CefCToCppRefCounted<CefEndTracingCallbackCToCpp,
    CefEndTracingCallback, cef_end_tracing_callback_t>::kWrapperType =
    WT_END_TRACING_CALLBACK;
