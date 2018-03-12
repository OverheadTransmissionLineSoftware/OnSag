// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/on_sag_printout.h"

#include "onsag/on_sag_app.h"

OnSagPrintout::OnSagPrintout(OnSagView* view)
    : wxPrintout(wxGetApp().GetDocument()->GetFilename()) {
  view_ = view;
}

OnSagPrintout::~OnSagPrintout() {
}

void OnSagPrintout::GetPageInfo(int* min, int* max, int* from, int* to) {
  *min = 1;
  *max = 1;
  *from = 1;
  *to = 1;
}

bool OnSagPrintout::HasPage(int number) {
  if (number == 1) {
    return true;
  } else {
    return false;
  }
}

/// This method scales and shifts the printout dc to match the graphics dc that
/// appears on screen. This makes it compatible with the OnDraw() method in the
/// view.
bool OnSagPrintout::OnPrintPage(int number) {
  // checks if page exists
  if (HasPage(number) == false) {
    return false;
  }

  // gets the graphics rect
  wxRect rect_graphics = view_->GraphicsPlotRect();

  // scales the dc associated with this printout to match the graphics dc
  // and adjusts the page size to account for margins
  wxPageSetupDialogData* data = wxGetApp().config()->data_page;
  FitThisSizeToPageMargins(rect_graphics.GetSize(), *data);

  // offsets the origin so the graphics rect is centered in the page rect
  wxRect rect_page = GetLogicalPageMarginsRect(*data);
  wxCoord x_offset = (rect_page.width - rect_graphics.width) / 2;
  wxCoord y_offset = (rect_page.height - rect_graphics.height) / 2;
  OffsetLogicalOrigin(x_offset, y_offset);

  // changes the view render target to print
  view_->set_target_render(OnSagView::RenderTarget::kPrint);

  // renders the plot
  // at this point the plot can be rendered according to graphics parameters
  // (rect size, ppi, etc) on the printout dc and everything will appear right
  wxDC* dc = GetDC();
  view_->OnDraw(dc);

  // changes the view render target back to the screen
  view_->set_target_render(OnSagView::RenderTarget::kScreen);

  return true;
}
