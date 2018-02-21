// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "profile_plot_pane.h"

#include <cmath>

#include "appcommon/graphics/line_renderer_2d.h"
#include "appcommon/graphics/text_renderer_2d.h"
#include "appcommon/widgets/status_bar_log.h"
#include "models/base/helper.h"
#include "wx/dcbuffer.h"

#include "on_sag_app.h"
#include "on_sag_doc.h"
#include "on_sag_view.h"

/// \par OVERVIEW
///
/// This is the enumeration for the context menu.
enum {
  kFitPlotData = 0,
};

BEGIN_EVENT_TABLE(ProfilePlotPane, PlotPane2d)
  EVT_MOTION(ProfilePlotPane::OnMouse)
  EVT_RIGHT_DOWN(ProfilePlotPane::OnMouse)
END_EVENT_TABLE()

ProfilePlotPane::ProfilePlotPane(wxWindow* parent, wxView* view)
    : PlotPane2d(parent) {
  view_ = view;

  // gets config
  OnSagConfig* config = wxGetApp().config();

  // sets plot defaults
  const wxBrush* brush =
      wxTheBrushList->FindOrCreateBrush(config->color_background);
  plot_.set_background(*brush);
  plot_.set_is_fitted(true);
  plot_.set_scale_x(1);
  plot_.set_scale_y(10);
  plot_.set_zoom_factor_fitted(1.0 / 1.2);
}

ProfilePlotPane::~ProfilePlotPane() {
}

void ProfilePlotPane::Update(wxObject* hint) {
  // typically only null on initialization
  if (hint == nullptr) {
    return;
  }

  // gets a buffered dc to prevent flickering
  wxClientDC dc(this);
  wxBufferedDC dc_buf(&dc, bitmap_buffer_);

  // updates plot based on app config
  OnSagConfig* config = wxGetApp().config();
  const wxBrush* brush =
      wxTheBrushList->FindOrCreateBrush(config->color_background);
  plot_.set_background(*brush);

  // interprets hint
  const UpdateHint* hint_update = dynamic_cast<UpdateHint*>(hint);
  if (hint_update == nullptr) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == UpdateHint::Type::kPreferencesEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == UpdateHint::Type::kSpansEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == UpdateHint::Type::kViewSelect) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  }
}

void ProfilePlotPane::ClearDataSets() {
  dataset_catenary_.Clear();
  dataset_dimension_lines_.Clear();
  dataset_dimension_text_.Clear();
  dataset_method_lines_.Clear();
}

void ProfilePlotPane::OnContextMenuSelect(wxCommandEvent& event) {
  // not creating busy cursor to avoid cursor flicker

  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kFitPlotData) {
    // toggles plot fit
    if (plot_.is_fitted() == true) {
      plot_.set_is_fitted(false);
    } else {
      plot_.set_is_fitted(true);
      this->Refresh();
    }
  }
}

void ProfilePlotPane::OnMouse(wxMouseEvent& event) {
  // overrides for right mouse click
  if (event.RightDown() == true) {
    // builds a context menu
    wxMenu menu;

    menu.AppendCheckItem(kFitPlotData, "Fit Plot");
    menu.Check(kFitPlotData, plot_.is_fitted());

    // shows context menu
    // the event is caught by the pane
    PopupMenu(&menu, event.GetPosition());

    // stops processing event (needed to allow pop-up menu to catch its event)
    event.Skip();
  } else {
    // calls base function
    PlotPane2d::OnMouse(event);
  }

  // converts graphics point to data point
  wxPoint point_graphics;
  point_graphics.x = event.GetX();
  point_graphics.y = event.GetY();
  const Point2d<float> point_data = plot_.PointGraphicsToData(point_graphics);

  // logs to status bar
  std::string str = "X="
                    + helper::DoubleToFormattedString(point_data.x, 2)
                    + "   Y="
                    + helper::DoubleToFormattedString(point_data.y, 2);

  status_bar_log::SetText(str, 1);
}

void ProfilePlotPane::UpdateDatasetCatenary(
    const SaggingAnalysisResult& result) {
  // gets catenary
  const Catenary3d& catenary = result.catenary;

  // calculates points
  std::list<Point3d<double>> points;
  const int i_max = 100;
  for (int i = 0; i <= i_max; i++) {
    double pos = static_cast<double>(i) / static_cast<double>(i_max);

    // gets catenary coordinate
    Point3d<double> p = catenary.Coordinate(pos);

    // shifts coordinates from catenary to span
    p.x += result.offset_coordinates.x;
    p.y += 0;
    p.z += result.offset_coordinates.y;

    // adds to list
    points.push_back(p);
  }

  // converts points to lines and adds to dataset
  dataset_catenary_.Clear();
  for (auto iter = points.cbegin(); iter != std::prev(points.cend(), 1);
       iter++) {
    // gets current and next point in the list
    const Point3d<double>& p0 = *iter;
    const Point3d<double>& p1 = *(std::next(iter, 1));

    // creates a line and maps 3d catenary points to 2d points for drawing
    Line2d* line = new Line2d();
    line->p0.x = p0.x;
    line->p0.y = p0.z;
    line->p1.x = p1.x;
    line->p1.y = p1.z;

    dataset_catenary_.Add(line);
  }
}

void ProfilePlotPane::UpdateDatasetDimensions(
    const SaggingAnalysisResult& result) {
  // initializes working variables
  Point3d<double> point_3d(-999999, -999999, -999999);
  Point2d<float> point(-999999, -999999);
  Line2d* line = nullptr;
  Text2d* text = nullptr;

  // gets span from doc
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const SagSpan* span = doc->SpanActivated();
  if (span == nullptr) {
    return;
  }

  // gets catenary
  const Catenary3d& catenary = result.catenary;

  // maps 3d catenary geometry to 2d
  Vector2d spacing;
  spacing.set_x(catenary.spacing_endpoints().x());
  spacing.set_y(catenary.spacing_endpoints().z());

  // defines catenary end points
  const Point2d<float> point_data_start(0 + result.offset_coordinates.x,
                                        0 + result.offset_coordinates.y);
  const Point2d<float> point_data_end(
    spacing.x() + result.offset_coordinates.x,
    spacing.y() + result.offset_coordinates.y);

  // solves for chord line connecting end points
  line = new Line2d();
  line->p0.x = point_data_start.x;
  line->p0.y = point_data_start.y;
  line->p1.x = point_data_end.x;
  line->p1.y = point_data_end.y;
  dataset_dimension_lines_.Add(line);

  // solves for horizontal dimension lines and text
  line = new Line2d();
  line->p0.x = point_data_start.x;
  line->p0.y = std::max(point_data_start.y, point_data_end.y);
  line->p1.x = point_data_end.x;
  line->p1.y = std::max(point_data_start.y, point_data_end.y);
  dataset_dimension_lines_.Add(line);

  point.x = (line->p1.x - line->p0.x) / 2;
  point.y = line->p0.y;

  text = new Text2d();
  text->angle = 0;
  text->message = helper::DoubleToFormattedString(spacing.x(), 2);
  text->offset = Point2d<int>(0, 5);
  text->point = point;
  text->position = Text2d::BoundaryPosition::kCenterLower;
  dataset_dimension_text_.Add(text);

  // solves for vertical dimension lines and text
  if (spacing.y() < 0) {
    // solves for dimension to the right of the catenary
    line = new Line2d();
    line->p0.x = point_data_end.x;
    line->p0.y = point_data_start.y;
    line->p1.x = point_data_end.x;
    line->p1.y = point_data_end.y;
    dataset_dimension_lines_.Add(line);

    point.x = line->p0.x;
    point.y = (line->p1.y + line->p0.y) / 2;

    text = new Text2d();
    text->angle = 0;
    text->message = helper::DoubleToFormattedString(spacing.y(), 2);
    text->offset = Point2d<int>(5, 0);
    text->point = point;
    text->position = Text2d::BoundaryPosition::kLeftCenter;
    dataset_dimension_text_.Add(text);
  } else if (0 < spacing.y()) {
    // solves for dimension to the left of the catenary
    line = new Line2d();
    line->p0.x = point_data_start.x;
    line->p0.y = point_data_start.y;
    line->p1.x = point_data_start.x;
    line->p1.y = point_data_end.y;
    dataset_dimension_lines_.Add(line);

    point.x = line->p0.x;
    point.y = (line->p1.y + line->p0.y) / 2;

    text = new Text2d();
    text->angle = 0;
    text->message = helper::DoubleToFormattedString(spacing.y(), 2);
    text->offset = Point2d<int>(-5, 0);
    text->point = point;
    text->position = Text2d::BoundaryPosition::kRightCenter;
    dataset_dimension_text_.Add(text);
  } else {
    // y = 0, and no dimension is necessary
  }

  // solves for sag dimension lines and text
  const double position_sag = catenary.PositionFractionSagPoint();
  line = new Line2d();

  point_3d = catenary.CoordinateChord(position_sag);
  line->p0.x = point_3d.x + result.offset_coordinates.x;
  line->p0.y = point_3d.z + result.offset_coordinates.y;
  point_3d = catenary.Coordinate(position_sag);
  line->p1.x = point_3d.x + result.offset_coordinates.x;
  line->p1.y = point_3d.z + result.offset_coordinates.y;
  dataset_dimension_lines_.Add(line);

  point.x = line->p0.x + (line->p1.x - line->p0.x) / 2;
  point.y = line->p0.y + (line->p1.y - line->p0.y) / 2;

  text = new Text2d();
  text->angle = 0;
  text->message = helper::DoubleToFormattedString(line->p0.y - line->p1.y, 2);
  text->offset = Point2d<int>(5, 0);
  text->point = point;
  text->position = Text2d::BoundaryPosition::kLeftCenter;
  dataset_dimension_text_.Add(text);

  // draws sagging method dimensions
  if (span->method.type == SagMethod::Type::kDynamometer) {
    // nothing to draw
  } else if (span->method.type == SagMethod::Type::kStopWatch) {
    // nothing to draw
  } else if (span->method.type == SagMethod::Type::kTransit) {
    const SagStructure* structure = nullptr;
    if (result.direction_transit == AxisDirectionType::kPositive) {
      structure = &span->structure_back;
    } else if (result.direction_transit == AxisDirectionType::kNegative) {
      structure = &span->structure_ahead;
    }

    // solves for the horizontal dimension for transit
    if (structure->point_attachment.x != span->method.point_transit.x) {
      line = new Line2d();
      line->p0.x = structure->point_attachment.x;
      line->p0.y = span->method.point_transit.y;
      line->p1.x = span->method.point_transit.x;
      line->p1.y = span->method.point_transit.y;
      dataset_dimension_lines_.Add(line);

      point.x = line->p0.x + (line->p1.x - line->p0.x) / 2;
      point.y = line->p0.y + (line->p1.y - line->p0.y) / 2;

      text = new Text2d();
      text->angle = 0;
      text->message = helper::DoubleToFormattedString(
          std::abs(line->p0.x - line->p1.x), 2);
      text->offset = Point2d<int>(0, 5);
      text->point = point;
      text->position = Text2d::BoundaryPosition::kCenterLower;
      dataset_dimension_text_.Add(text);
    }

    // solves for the vertical dimension for transit
    if (structure->point_attachment.y != span->method.point_transit.y) {
      line = new Line2d();
      line->p0.x = structure->point_attachment.x;
      line->p0.y = structure->point_attachment.y;
      line->p1.x = structure->point_attachment.x;
      line->p1.y = span->method.point_transit.y;
      dataset_dimension_lines_.Add(line);

      point.x = line->p0.x + (line->p1.x - line->p0.x) / 2;
      point.y = line->p0.y + (line->p1.y - line->p0.y) / 2;

      text = new Text2d();
      text->angle = 0;
      text->message = helper::DoubleToFormattedString(line->p0.y - line->p1.y,
                                                      2);
      text->point = point;

      if (result.direction_transit == AxisDirectionType::kPositive) {
        text->offset = Point2d<int>(-5, 0);
        text->position = Text2d::BoundaryPosition::kRightCenter;
      } else if (result.direction_transit == AxisDirectionType::kNegative) {
        text->offset = Point2d<int>(5, 0);
        text->position = Text2d::BoundaryPosition::kLeftCenter;
      }

      dataset_dimension_text_.Add(text);
    }

    // solves for attachment-to-target tangent line and text
    line = new Line2d();
    if (result.direction_transit == AxisDirectionType::kNegative) {
      // dimensions the back structure
      line->p0.x = span->structure_back.point_attachment.x;
      line->p0.y = span->structure_back.point_attachment.y;
      line->p1.x = result.point_target.x;
      line->p1.y = result.point_target.y;
    } else if (result.direction_transit == AxisDirectionType::kPositive) {
      // dimensions the ahead structure
      line->p0.x = span->structure_ahead.point_attachment.x;
      line->p0.y = span->structure_ahead.point_attachment.y;
      line->p1.x = result.point_target.x;
      line->p1.y = result.point_target.y;
    }

    dataset_dimension_lines_.Add(line);

    point.x = line->p0.x + (line->p1.x - line->p0.x) / 2;
    point.y = line->p0.y + (line->p1.y - line->p0.y) / 2;

    text = new Text2d();
    text->angle = 0;
    text->message = helper::DoubleToFormattedString(line->p0.y - line->p1.y, 2);

    if (result.direction_transit == AxisDirectionType::kPositive) {
      text->offset = Point2d<int>(5, 0);
      text->position = Text2d::BoundaryPosition::kLeftCenter;
    } else if (result.direction_transit == AxisDirectionType::kNegative) {
      text->offset = Point2d<int>(-5, 0);
      text->position = Text2d::BoundaryPosition::kRightCenter;
    }

    text->point = point;
    dataset_dimension_text_.Add(text);
  }
}

void ProfilePlotPane::UpdateDatasetMethod(const SaggingAnalysisResult& result) {
  // initializes working variables
  Point3d<double> point_3d(-999999, -999999, -999999);
  Point2d<float> point(-999999, -999999);
  Line2d* line = nullptr;

  // gets span from doc
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const SagSpan* span = doc->SpanActivated();
  if (span == nullptr) {
    return;
  }

  // determines what to draw based on method type
  if (span->method.type == SagMethod::Type::kDynamometer) {
    // solves for a 1:1 line angled downward and away from span center
    line = new Line2d();
    if (span->method.end == SagMethod::SpanEndType::kBackOnLine) {
      line->p0.x = span->structure_back.point_attachment.x;
      line->p0.y = span->structure_back.point_attachment.y;
      line->p1.x = line->p0.x - 0.25 * result.catenary.spacing_endpoints().x();
      line->p1.y = line->p0.y - 0.25 / 10
                   * result.catenary.spacing_endpoints().x();

    } else if (span->method.end == SagMethod::SpanEndType::kAheadOnLine) {
      line->p0.x = span->structure_ahead.point_attachment.x;
      line->p0.y = span->structure_ahead.point_attachment.y;
      line->p1.x = line->p0.x + 0.25 * result.catenary.spacing_endpoints().x();
      line->p1.y = line->p0.y - 0.25 / 10
                   * result.catenary.spacing_endpoints().x();
    }
    dataset_method_lines_.Add(line);
  } else if (span->method.type == SagMethod::Type::kStopWatch) {
    // nothing to draw
  } else if (span->method.type == SagMethod::Type::kTransit) {
    // solves for the horizontal reference line at transit
    line = new Line2d();
    line->p0.x = span->method.point_transit.x;
    line->p0.y = span->method.point_transit.y;
    line->p1.x = result.point_target.x;
    line->p1.y = span->method.point_transit.y;

    // checks if the line will intersect catenary
    // if so, reduces the line to only 10% length for rendering
    if (result.angle_transit < 0) {
      line->p1.x = line->p0.x + 0.1 * (line->p1.x - line->p0.x);
    }

    dataset_method_lines_.Add(line);

    // solves for the transit-to-target tangent line
    line = new Line2d();
    line->p0.x = span->method.point_transit.x;
    line->p0.y = span->method.point_transit.y;
    line->p1.x = result.point_target.x;
    line->p1.y = result.point_target.y;
    dataset_method_lines_.Add(line);
  }
}

void ProfilePlotPane::UpdatePlotDatasets() {
  wxLogVerbose("Updating profile plot dataset.");

  ClearDataSets();

  // gets result index from view
  OnSagView* view = dynamic_cast<OnSagView*>(view_);
  const int index = view->index_result();

  // gets filtered result from doc
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const SaggingAnalysisResult* result = doc->Result(index);
  if (result == nullptr) {
    return;
  }

  // updates datasets
  UpdateDatasetCatenary(*result);
  UpdateDatasetDimensions(*result);
  UpdateDatasetMethod(*result);
}

void ProfilePlotPane::UpdatePlotRenderers() {
  wxLogVerbose("Updating profile plot renderers.");

  // clears existing renderers
  plot_.ClearRenderers();

  // checks if dataset has any data
  if (dataset_catenary_.data()->empty() == true) {
    return;
  }

  // creates renderer
  LineRenderer2d* renderer_line = nullptr;
  TextRenderer2d* renderer_text = nullptr;
  const wxPen* pen = nullptr;

  // adds catenary renderer
  pen = wxThePenList->FindOrCreatePen(*wxGREEN, 1, wxPENSTYLE_SOLID);
  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_catenary_);
  renderer_line->set_pen(pen);
  plot_.AddRenderer(renderer_line);

  // adds dimension line renderer
  pen = wxThePenList->FindOrCreatePen(*wxWHITE, 1, wxPENSTYLE_SHORT_DASH);
  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_dimension_lines_);
  renderer_line->set_pen(pen);
  plot_.AddRenderer(renderer_line);

  // adds dimension text renderer
  renderer_text = new TextRenderer2d();
  renderer_text->set_dataset(&dataset_dimension_text_);
  renderer_text->set_color(wxWHITE);
  plot_.AddRenderer(renderer_text);

  // adds method line renderer
  pen = wxThePenList->FindOrCreatePen(*wxRED, 1, wxPENSTYLE_SOLID);
  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_method_lines_);
  renderer_line->set_pen(pen);
  plot_.AddRenderer(renderer_line);
}
