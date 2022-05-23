#pragma once

#ifndef CHDMANPROCESSDATA_H
#define CHDMANPROCESSDATA_H

#include <wx/wx.h>
#include <wx/filename.h>

struct CHDManItemData
{
    public:
        enum CHDManItemStatus
        {
            Pending,
            Processing,
            Complete,
            ErroredOut
        };

        CHDManItemStatus status = CHDManItemStatus::Pending;
        int listIndex = -1;
        int processId = 0;
        CHDManProgressThread *progressThread = NULL;
        int progressPercentage = 0;
        wxFileName inputPath;
        wxFileName outputPath;
};

#endif // CHDMANPROCESSDATA_H