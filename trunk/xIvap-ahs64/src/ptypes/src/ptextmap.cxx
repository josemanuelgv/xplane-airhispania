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

#include "ptypes.h"


PTYPES_BEGIN


typedef _textitem* ptextitem;


textmap::textmap(bool casesens)
    : tobjlist<_textitem>(true)
{
    config.sorted = true;
    config.casesens = casesens;
}


textmap::~textmap()
{
}


int textmap::compare(const void* key, const void* item) const
{
   if (config.casesens)
        return strcmp(pconst(key), ptextitem(item)->key);
    else
        return strcasecmp(pconst(key), ptextitem(item)->key);
}


const string& textmap::get(const char* key) const
{
    int index;
    if (search(key, index))
        return dogetvalue(index);
    else
        return nullstring;
}


int textmap::put(const string& key, const string& value)
{
    int index;
    if (search(pconst(key), index))
    {
        if (isempty(value))
            dodel(index);
        else
            doget(index)->value = value;
    }
    else if (!isempty(value))
        doins(index, new _textitem(key, value));
    return index;
}


int textmap::indexof(const char* key) const
{
    int index;
    if (search(key, index))
        return index;
    else
        return -1;
}


PTYPES_END
