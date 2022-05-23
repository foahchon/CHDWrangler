#pragma once

#ifndef CHDMANPROGRESSEVENT_H
#define CHDMANPROGRESSEVENT_H

#include <wx/wx.h>

struct CHDManProgressEvent
{
    int processId;
    wxString message;
};

#endif // CHDMANPROGRESSEVENT_H