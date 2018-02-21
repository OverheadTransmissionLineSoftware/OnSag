// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "on_sag_doc.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/xml/xml.h"

#include "on_sag_app.h"

IMPLEMENT_DYNAMIC_CLASS(OnSagDoc, wxDocument)

OnSagDoc::OnSagDoc() {
}

OnSagDoc::~OnSagDoc() {
}

wxInputStream& OnSagDoc::LoadObject(wxInputStream& stream) {
  wxBusyCursor cursor;

  std::string message;

  message = "Loading document file: " + this->GetFilename();
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // attempts to load an xml document from the input stream
  wxXmlDocument doc_xml;
  if (doc_xml.Load(stream) == false) {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains an invalid xml structure. The document "
              "will close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    status_bar_log::PopText(0);

    // sets stream to invalid state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  /// \todo add details here

  // resets modified status to false because the xml parser uses functions
  // that mark it as modified
  Modify(false);

  status_bar_log::PopText(0);

  return stream;
}

bool OnSagDoc::OnCreate(const wxString& path, long flags) {
  // calls base class function
  return wxDocument::OnCreate(path, flags);
}

wxOutputStream& OnSagDoc::SaveObject(wxOutputStream& stream) {
  wxBusyCursor cursor;

  // logs
  std::string message = "Saving document file: " + GetFilename();
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  /// \todo add details here

  status_bar_log::PopText(0);

  return stream;
}
