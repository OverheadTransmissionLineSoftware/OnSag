// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "on_sag_view.h"

#include "appcommon/widgets/status_bar_log.h"

#include "on_sag_app.h"

OnSagView::OnSagView() {
}

OnSagView::~OnSagView() {
}

IMPLEMENT_DYNAMIC_CLASS(OnSagView, wxView)

bool OnSagView::OnClose(bool WXUNUSED(delete_window)) {
  if (!GetDocument()->Close()) {
    return false;
  }

  // gets aui manager from main frame
  wxAuiManager* manager = wxAuiManager::GetManager(GetFrame());

  // saves AUI perspective
  wxGetApp().config()->perspective = manager->SavePerspective();

  // adjusts log if the pane was docked
  wxAuiPaneInfo& info = manager->GetPane("Log");
  if (info.IsDocked() == true) {
    info.Float();
    info.Hide();
  }

  // updates manager to show changes
  manager->Update();

  // resets frame to document-less state
  OnSagFrame* frame = wxGetApp().frame();
  frame->Refresh();
  frame->SetTitle(wxGetApp().GetAppDisplayName());

  // resets statusbar
  status_bar_log::SetText("Ready", 0);
  status_bar_log::SetText("", 1);

  // resets menubar
  const int index_menu = frame->GetMenuBar()->FindMenu("Edit");
  wxMenu* menu = frame->GetMenuBar()->GetMenu(index_menu);
  menu->SetLabel(wxID_UNDO, "Undo \tCtrl+Z");
  menu->SetLabel(wxID_REDO, "Redo \tCtrl+Y");

  return true;
}

bool OnSagView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate(doc, flags)) {
    return false;
  }

  // initializes cached references
  target_render_ = RenderTarget::kScreen;

  // gets main application frame
  wxFrame* frame = dynamic_cast<wxFrame*>(wxGetApp().GetTopWindow());

  // notify application of new view
  SetFrame(frame);
  Activate(true);

  /// \todo add details here

  // resets statusbar
  status_bar_log::SetText("Ready", 0);

  return true;
}

void OnSagView::OnDraw(wxDC *dc) {
  /// \todo add details here
}

void OnSagView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);

  // updates statusbar
  status_bar_log::PushText("Updating view", 0);

  /// \todo add details here

  // resets status bar
  status_bar_log::PopText(0);
}

void OnSagView::set_target_render(
    const OnSagView::RenderTarget& target_render) {
  target_render_ = target_render;
}

OnSagView::RenderTarget OnSagView::target_render() const {
  return target_render_;
}
