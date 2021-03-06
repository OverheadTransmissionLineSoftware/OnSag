// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/on_sag_view.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/cmdproc.h"

#include "onsag/on_sag_app.h"
#include "onsag/on_sag_printout.h"

IMPLEMENT_DYNAMIC_CLASS(OnSagView, wxView)

BEGIN_EVENT_TABLE(OnSagView, wxView)
  EVT_MENU(wxID_PREVIEW, OnSagView::OnPrintPreview)
  EVT_MENU(wxID_PRINT, OnSagView::OnPrint)
END_EVENT_TABLE()

OnSagView::OnSagView() {
}

OnSagView::~OnSagView() {
}

wxRect OnSagView::GraphicsPlotRect() const {
  // gets active plot pane rect
  return pane_plot_->GetClientRect();
}

bool OnSagView::OnClose(bool WXUNUSED(delete_window)) {
  if (!GetDocument()->Close()) {
    return false;
  }

  // gets aui manager from main frame
  wxAuiManager* manager = wxAuiManager::GetManager(GetFrame());

  // saves AUI perspective
  wxGetApp().config()->perspective = manager->SavePerspective();

  // detaches panes
  manager->DetachPane(pane_results_);
  manager->DetachPane(pane_edit_);
  manager->DetachPane(pane_plot_);

  // adjusts log if the pane was docked
  wxAuiPaneInfo& info = manager->GetPane("Log");
  if (info.IsDocked() == true) {
    info.Float();
    info.Hide();
  }

  // updates manager to show changes
  manager->Update();

  // destroys panes
  pane_edit_->Destroy();
  pane_plot_->Destroy();
  pane_results_->Destroy();

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
  index_result_ = -1;
  target_render_ = RenderTarget::kScreen;

  // gets main application frame
  wxFrame* frame = dynamic_cast<wxFrame*>(wxGetApp().GetTopWindow());

  // notify application of new view
  SetFrame(frame);
  Activate(true);

  // gets aui manager
  wxAuiManager* manager = wxAuiManager::GetManager(frame);

  // creates AUI windows and adds to manager
  wxAuiPaneInfo info;

  info = wxAuiPaneInfo();
  info.Name("Plot");
  info.CenterPane();
  pane_plot_ = new ProfilePlotPane(frame, this);
  manager->AddPane(pane_plot_, info);

  info = wxAuiPaneInfo();
  info.Name("Results");
  info.Caption("Results");
  info.CloseButton(false);
  info.Floatable(false);
  info.Top();
  pane_results_ = new ResultsPane(frame, this);
  manager->AddPane(pane_results_, info);

  info = wxAuiPaneInfo();
  info.Name("Edit");
  info.BestSize(300, 700);
  info.Caption("Edit");
  info.CloseButton(false);
  info.Floatable(false);
  info.Layer(1);
  info.Left();
  info.MinSize(150, 350);
  pane_edit_ = new EditPane(frame, this);
  manager->AddPane(pane_edit_, info);

  // loads perspective and updates
  std::string perspective = wxGetApp().config()->perspective;
  if (perspective == "") {
    manager->Update();
  } else {
    manager->LoadPerspective(wxGetApp().config()->perspective);
  }

  // resets statusbar
  status_bar_log::SetText("Ready", 0);

  // links command processor to edit menu
  const int index_menu = frame->GetMenuBar()->FindMenu("Edit");
  wxMenu* menu = frame->GetMenuBar()->GetMenu(index_menu);

  wxCommandProcessor* processor = GetDocument()->GetCommandProcessor();
  processor->SetEditMenu(menu);

  return true;
}

wxPrintout* OnSagView::OnCreatePrintout() {
  return new OnSagPrintout(this);
}

void OnSagView::OnDraw(wxDC *dc) {
  // caches the current background
  // alters the background to white if view is rendering to print
  const wxBrush brush = pane_plot_->background();
  if (target_render_ == RenderTarget::kPrint) {
    pane_plot_->set_background(*wxWHITE_BRUSH);
  }

  // draws the active plot pane
  pane_plot_->RenderPlot(*dc);

  // resets to the original background
  pane_plot_->set_background(brush);
}

void OnSagView::OnPrint(wxCommandEvent& event) {
  // gets printout
  wxPrintout* printout = OnCreatePrintout();

  // sets up printer and prints
  wxPrintDialogData data_print(wxGetApp().config()->data_page->GetPrintData());
  wxPrinter printer(&data_print);
  if (printer.Print(wxGetApp().frame(), printout) == false) {
    if (wxPrinter::GetLastError() == wxPRINTER_ERROR) {
      wxLogError("There was a problem printing.");
    }
  }

  delete printout;
}

void OnSagView::OnPrintPreview(wxCommandEvent& event) {
  // gets printouts
  // one is for previewing, the other is for printing
  wxPrintout* printout_view = OnCreatePrintout();
  wxPrintout* printout_print = OnCreatePrintout();

  // creates a print dialog data using app data
  wxPrintDialogData data(wxGetApp().config()->data_page->GetPrintData());

  // generates a print preview
  wxPrintPreview* preview = new wxPrintPreview(printout_view, printout_print,
                                               &data);
  if (preview->IsOk() == false) {
    delete preview;
    wxLogError("There was a problem previewing.");
    return;
  }

  // creates and shows a preview frame
  wxPreviewFrame* frame = new wxPreviewFrame(preview, wxGetApp().frame(),
                                             "Print Preview",
                                             wxPoint(100, 100),
                                             wxSize(850, 750));
  frame->Centre(wxBOTH);
  frame->Initialize();
  frame->Show();
}

void OnSagView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);

  // updates statusbar
  status_bar_log::PushText("Updating view", 0);

  // don't need to distinguish sender - all frames are grouped under one view
  pane_edit_->Update(hint);
  pane_results_->Update(hint);
  pane_plot_->Update(hint);

  // resets status bar
  status_bar_log::PopText(0);
}

const int OnSagView::index_result() const {
  return index_result_;
}

EditPane* OnSagView::pane_edit() {
  return pane_edit_;
}

ProfilePlotPane* OnSagView::pane_plot() {
  return pane_plot_;
}

ResultsPane* OnSagView::pane_results() {
  return pane_results_;
}

void OnSagView::set_index_result(const int& index) {
  index_result_ = index;
}

void OnSagView::set_target_render(
    const OnSagView::RenderTarget& target_render) {
  target_render_ = target_render;
}

OnSagView::RenderTarget OnSagView::target_render() const {
  return target_render_;
}
