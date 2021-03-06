// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice-E version 1.3.0
// Generated from file `Identity.ice'

#ifndef __C__Users_gongcheng_Documents_Visual_Studio_2008_Projects_Slice_Slice__Identity_h__
#define __C__Users_gongcheng_Documents_Visual_Studio_2008_Projects_Slice_Slice__Identity_h__

#include <IceE/ProxyF.h>
#include <IceE/ObjectF.h>
#include <IceE/Exception.h>
#include <IceE/ScopedArray.h>
#include <IceE/UndefSysMacros.h>

#ifndef ICEE_IGNORE_VERSION
#   if ICEE_INT_VERSION / 100 != 103
#       error IceE version mismatch!
#   endif
#   if ICEE_INT_VERSION % 100 < 0
#       error IceE patch level mismatch!
#   endif
#endif

namespace Ice
{

struct Identity
{
    ::std::string name;
    ::std::string category;

    bool operator==(const Identity&) const;
    bool operator<(const Identity&) const;
    bool operator!=(const Identity& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const Identity& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const Identity& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const Identity& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

}

#endif
