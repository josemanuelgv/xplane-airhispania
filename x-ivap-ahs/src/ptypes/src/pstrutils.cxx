/*
 *
 *  C++ Portable Types Library (PTypes)
 *  Version 2.0.2  Released 17-May-2004
 *
 *  Copyright (C) 2001-2004 Hovik Melikyan
 *
 *  http://www.melikyan.com/ptypes/
 *
 */

#include <string.h>

#include "ptypes.h"


PTYPES_BEGIN


string ptdecl fill(int width, char pad)
{
    string res;
    if (width > 0) {
        setlength(res, width);
        memset(pchar(pconst(res)), pad, length(res));
    }
    return res;
}


string ptdecl pad(const string& s, int width, char c, bool left)
{
    int len = length(s);
    if (len < width && width > 0)
    {
        string res;
        setlength(res, width);
        if (left)
        {
            if (len > 0)
                memcpy(pchar(pconst(res)), pconst(s), len);
            memset(pchar(pconst(res)) + len, c, width - len);
        }
        else
        {
            memset(pchar(pconst(res)), c, width - len);
            if (len > 0)
                memcpy(pchar(pconst(res)) + width - len, pconst(s), len);
        }
        return res;
    }
    else
        return s;
}


PTYPES_END
