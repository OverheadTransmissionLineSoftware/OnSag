// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_ONSAGPRINTOUT_H_
#define OTLS_ONSAG_ONSAGPRINTOUT_H_

#include "wx/print.h"

#include "onsag/on_sag_view.h"

/// \par OVERVIEW
///
/// This is class lays out the print format, to be used for previewing or
/// printing.
///
/// \par PAGE LAYOUT
///
/// The printout consists of a single page, which should replicate the graphics
/// being displayed in the plot pane.
class OnSagPrintout : public wxPrintout {
 public:
  /// \brief Constructor.
  /// \param[in] view
  ///   The view.
  OnSagPrintout(OnSagView* view);

  /// \brief Destructor.
  virtual ~OnSagPrintout();

  /// \brief Gets the page information.
  /// \param[in,out] min
  ///   The minimum page.
  /// \param[in,out] max
  ///   The maximum page.
  /// \param[in] from
  ///   The start of the required print range.
  /// \param[in] to
  ///   The end of the required print range.
  virtual void GetPageInfo(int* min, int* max, int* from, int* to) override;

  /// \brief Determines if the page number is present.
  /// \param[in] number
  ///   The page number.
  /// \return If the page number is present.
  /// The page numbers start at 1 (as opposed to 0).
  virtual bool HasPage(int number) override;

  /// \brief Prints the page.
  /// \param[in] number
  ///   The page number.
  /// \return If the print job should continue.
  virtual bool OnPrintPage(int number) override;

 protected:
  /// \var view_
  ///   The view.
  OnSagView* view_;
};

#endif  // OTLS_ONSAG_ONSAGPRINTOUT_H_
