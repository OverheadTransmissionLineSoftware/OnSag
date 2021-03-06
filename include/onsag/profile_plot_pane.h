// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef ONSAG_PROFILE_PLOT_PANE_H_
#define ONSAG_PROFILE_PLOT_PANE_H_

#include "appcommon/graphics/line_data_set_2d.h"
#include "appcommon/graphics/plot_pane_2d.h"
#include "appcommon/graphics/text_data_set_2d.h"
#include "models/transmissionline/catenary.h"
#include "wx/docview.h"

#include "onsag/on_sag_config.h"
#include "onsag/sagging_analysis_result.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that displays a profile plot of the activated
/// span.
class ProfilePlotPane : public PlotPane2d {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] view
  ///   The view.
  ProfilePlotPane(wxWindow* parent, wxView* view);

  /// \brief Destructor.
  ~ProfilePlotPane();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void Update(wxObject* hint = nullptr);

 protected:
  /// \brief Clears the datasets.
  void ClearDataSets();

  /// \brief Handles the context menu select event.
  /// \param[in] event
  ///   The event.
  void OnContextMenuSelect(wxCommandEvent& event);

  /// \brief Handles the mouse events.
  /// \param[in] event
  ///   The event.
  void OnMouse(wxMouseEvent& event);

  /// \brief Updates a catenary dataset.
  /// \param[in] result
  ///   The analysis result.
  void UpdateDatasetCatenary(const SaggingAnalysisResult& result);

  /// \brief Updates the dimension dataset.
  /// \param[in] result
  ///   The analysis result.
  void UpdateDatasetDimensions(const SaggingAnalysisResult& result);

  /// \brief Updates the sag method dataset.
  /// \param[in] result
  ///   The analysis result.
  void UpdateDatasetMethod(const SaggingAnalysisResult& result);

  /// \brief Updates the plot datasets.
  void UpdatePlotDatasets() override;

  /// \brief Updates the plot renderers.
  void UpdatePlotRenderers() override;

  /// \var dataset_catenary_
  ///   The catenary dataset.
  LineDataSet2d dataset_catenary_;

  /// \var dataset_dimension_lines_
  ///   The dimension line dataset.
  LineDataSet2d dataset_dimension_lines_;

  /// \var dataset_dimension_text_
  ///   The dimension text dataset.
  TextDataSet2d dataset_dimension_text_;

  /// \var dataset_method_lines_
  ///   The sag method line dataset.
  LineDataSet2d dataset_method_lines_;

  /// \var options_
  ///   The plot options, which are stored in the application config.
  ProfilePlotOptions* options_;

  /// \var view_
  ///   The view.
  wxView* view_;

  DECLARE_EVENT_TABLE()
};

#endif  // ONSAG_PROFILE_PLOT_PANE_H_
