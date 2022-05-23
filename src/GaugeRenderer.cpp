#include <GaugeRenderer.h>

bool GaugeRenderer::Render(wxRect rect, wxDC *dc, int state)
{
    auto textColor = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    auto fillColor = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);

    auto percentage = (float)m_value / 100;
    auto label = wxString::Format("%d%", m_value);

    auto left = rect.GetLeft();
    auto top = rect.GetTop();
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();

    auto textSize = dc->GetTextExtent(label);
    auto textX = width / 2 - textSize.GetWidth() / 2;
    auto textY = height / 2 - textSize.GetHeight() / 2;
    auto filledExtent = wxRound(width * percentage);

    dc->SetBrush(wxBrush(fillColor));
    dc->SetTextForeground(textColor);
    dc->SetClippingRegion(left, top, filledExtent, height);
    dc->DrawRectangle(left, top, filledExtent, height);
    dc->DrawText(label, left + textX, top + textY);
    dc->DestroyClippingRegion();

    dc->SetBrush(wxBrush(textColor));
    dc->SetTextForeground(fillColor);
    dc->SetClippingRegion(left + filledExtent - 1, top, width - filledExtent + 1, height);
    dc->DrawRectangle(left + filledExtent - 1, top, width - filledExtent + 1, height);
    dc->DrawText(label, left + textX, top + textY);
    dc->DestroyClippingRegion();

    return true;
}

wxSize GaugeRenderer::GetSize() const
{
    return GetView()->FromDIP(wxDefaultSize);
}

bool GaugeRenderer::SetValue(const wxVariant &value)
{
    m_value = value;
    return true;
}

bool GaugeRenderer::GetValue(wxVariant &value) const
{
    return true;
}