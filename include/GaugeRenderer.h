#pragma once

#ifndef GAUGERENDERER_H
#define GAUGERENDERER_H

#include <wx/wx.h>
#include <wx/dataview.h>

class GaugeRenderer : public wxDataViewCustomRenderer
{
    public:
        explicit GaugeRenderer(wxDataViewListCtrl *listView, int progressColumnIndex)
            : wxDataViewCustomRenderer("long", wxDATAVIEW_CELL_INERT, wxALIGN_LEFT)
        {
        }

        virtual bool Render(wxRect rect, wxDC *dc, int state) wxOVERRIDE;
        virtual wxSize GetSize() const wxOVERRIDE;
        virtual bool SetValue(const wxVariant &value) wxOVERRIDE;
        virtual bool GetValue(wxVariant &WXUNUSED(value)) const wxOVERRIDE;

    private:
        long m_value;
};

#endif // GAUGERENDERER_H