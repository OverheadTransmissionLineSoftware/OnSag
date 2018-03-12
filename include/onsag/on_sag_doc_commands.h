// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_ONSAGDOCCOMMANDS_H_
#define OTLS_ONSAG_ONSAGDOCCOMMANDS_H_

#include <string>

#include "wx/cmdproc.h"
#include "wx/xml/xml.h"

#include "onsag/on_sag_doc.h"

/// \par OVERVIEW
///
/// This class is a command that modifies spans in the document.
///
/// \par NAME
///
/// The command must be initialized with one of the declared names, as it will
/// determine what kind of action is to be performed.
///
/// \par SPAN XML NODES
///
/// This command stores sag span xml nodes. Up to two nodes are stored. One of
/// the nodes will be committed to the document on a 'do' method, while the
/// other will be committed to the document on an 'undo' method. These nodes
/// may not be needed for all command types, and will be kept as a nullptr if
/// not used.
///
/// \par COMMAND INFORMATION
///
/// The following information is needed for each command name/type:
/// - Delete = index
/// - Insert = index + span node
/// - Modify = index + span node
/// - Move Down = index
/// - Move Up = index
class SagSpanCommand : public wxCommand {
 public:
  /// \var kNameDelete
  ///   The command string to use for deleting a span.
  static const std::string kNameDelete;

  /// \var kNameInsert
  ///   The command name to use for inserting a span.
  static const std::string kNameInsert;

  /// \var kNameModify
  ///   The command string to use for modifying a span.
  static const std::string kNameModify;

  /// \var kNameMoveDown
  ///   The command string to use for moving a span down.
  static const std::string kNameMoveDown;

  /// \var kNameMoveUp
  ///   The command string to use for moving a span up.
  static const std::string kNameMoveUp;

  /// \brief Constructor.
  /// \param[in] name
  ///   The command name, which should match one of the defined strings.
  SagSpanCommand(const std::string& name);

  /// \brief Destructor.
  virtual ~SagSpanCommand();

  /// \brief Creates a sag span from an xml node.
  /// \param[in] node
  ///   The xml node.
  /// \param[out] span
  ///   The sag span to populate.
  /// \return Success status.
  static bool CreateSpanFromXml(const wxXmlNode* node, SagSpan& span);

  /// \brief Does the command.
  /// \return If the action completes successfully.
  virtual bool Do();

  /// \brief Saves the sag span to an xml node.
  /// \param[in] span
  ///   The sag span.
  /// \return An xml node for the sag span.
  static wxXmlNode* SaveSpanToXml(const SagSpan& span);

  /// \brief Undoes the command.
  /// \return True if the action completes successfully.
  virtual bool Undo();

  /// \brief Gets the index.
  /// \return The index.
  int index() const;

  /// \brief Gets the span xml node.
  /// \return The span xml node.
  const wxXmlNode* node_span() const;

  /// \brief Sets the index.
  /// \param[in] index
  ///   The index.
  void set_index(const int& index);

  /// \brief Sets the span xml node.
  /// \param[in] node_span
  ///   The span xml node.
  void set_node_span(const wxXmlNode* node_span);

 private:
  /// \brief Does the delete span command.
  /// \return The success status.
  bool DoDelete();

  /// \brief Does the insert span command.
  /// \param[in] node
  ///   The xml node of the span.
  /// \return The success status.
  bool DoInsert(const wxXmlNode* node);

  /// \brief Does the modify span command.
  /// \param[in] node
  ///   The xml node of the span.
  /// \return The success status.
  bool DoModify(const wxXmlNode* node);

  /// \brief Does the move span down command.
  /// \return The success status.
  bool DoMoveDown();

  /// \brief Does the move span up command.
  /// \return The success status.
  bool DoMoveUp();

  /// \var doc_
  ///   The document.
  OnSagDoc* doc_;

  /// \var index_
  ///   The index to the spans list in the document. The command will be applied
  ///   at this index.
  int index_;

  /// \var node_do_
  ///   The span xml node that is committed to the document on a do operation.
  ///   This command owns the node.
  const wxXmlNode* node_do_;

  /// \var node_undo_
  ///   The span xml node that is committed to the document on an undo
  ///   operation. It contains the state of the span in the document before any
  /// edits occur. This command owns the node.
  const wxXmlNode* node_undo_;
};

#endif  // OTLS_ONSAG_ONSAGDOCCOMMANDS_H_
