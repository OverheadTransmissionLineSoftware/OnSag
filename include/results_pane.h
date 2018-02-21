// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_RESULTSPANE_H_
#define OTLS_ONSAG_RESULTSPANE_H_

#include "appcommon/widgets/report_table.h"
#include "wx/docview.h"
#include "wx/wx.h"

#include "sagging_analysis_result.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that holds all of the results display/controls.
class ResultsPane : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] view
  ///   The view.
  ResultsPane(wxWindow* parent, wxView* view);

  /// \brief Destructor.
  ~ResultsPane();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void Update(wxObject* hint = nullptr);

 private:
  /// \brief Handles the listctrl select event.
  /// \param[in] event
  ///   The event.
  void OnListCtrlSelect(wxListEvent& event);

  /// \brief Updates the report data.
  void UpdateReportData();

  /// \brief Updates the report data with dyno results.
  /// \param[in] results
  ///   The results from the document.
  void UpdateReportDataDyno(
      const std::list<const SaggingAnalysisResult*>* results);

  /// \brief Updates the report data with stopwatch results.
  /// \param[in] results
  ///   The results from the document.
  void UpdateReportDataStopwatch(
      const std::list<const SaggingAnalysisResult*>* results);

  /// \brief Updates the report data with transit results.
  /// \param[in] results
  ///   The results from the document.
  void UpdateReportDataTransit(
      const std::list<const SaggingAnalysisResult*>* results);

  /// \var data_
  ///   The report data.
  ReportData data_;

  /// \var table_
  ///   The report table used to display the results.
  ReportTable* table_;

  /// \var view_
  ///   The view.
  wxView* view_;

  DECLARE_EVENT_TABLE()
};

# endif  // OTLS_ONSAG_RESULTSPANE_H_
