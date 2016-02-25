#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <QMenu>
#include <QModelIndex>
#include <QFileDialog>

class QTreeView;
class QFileSystemModel;
class QComboBox;
class QPushButton;
class TagScrollView;
class TagViewer;
class TagModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(
        QWidget* parent = 0
    );

    virtual ~MainWindow();

public slots:
    // import images in the tag tree
    void import_images();

    // add a new label in the tag tree
    void add_label();

    // remove selected image items from
    // the tag tree
    void remove_images();

    // show the contextual menu only for label
    // ability to change label color and name
    void show_context_menu(
        const QPoint& pos
    );

    // pop-up color selector for the label that
    // was right-clicked
    void change_selected_label_color();

    // pop-up line edit for the label that
    // was right-clicked
    void change_selected_label_name();

    // open a XML file containing the image tags
    void open_xml();

    // open a XML file and merge it to the current tree
    void open_xml_and_merge();

    // save tags as XML file
    void save_as_xml();

    // save selected tags as XML file
    void save_selection_as_xml();

    // crop images per label and save them individually
    void save_as_images();

    // crop selected images per selected label and save them individually
    void save_selection_as_images();

    // show the help contents
    void show_help();

    // show the credits
    void show_credits();

protected slots:
    // internal slot for updating the viewer
    // with the current selected image
    void update_viewer();

    // internal slot for updating the viewer tagging options
    // based on the current tag selection
    void set_viewer_tag_options();

    // update the viewer with the proper tag status
    void enable_tag(
        bool activate
    );

    // update the viewer with the proper untag status
    void enable_untag(
        bool activate
    );

    // tag the current image (if valid selection)
    // with the given bounding box
    void tag_image(
        const QRect& bbox
    );

    // untag the current image (if valid selection)
    // with the given bounding box and label
    // label is used to know which tag to remove
    // (because we can have multi-label selection)
    void untag_image(
        const QString& label,
        const QRect& bbox
    );

    // loads the given XML file
    // if merge is off, clears the current tree first
    void load_xml(
        const QString& filename,
        const QString& relative_dir,
        bool merge
    );

    // saves tag as XML file
    // if no selection is provided, save all items
    void save_xml(
        const QString& filename,
        const QString& relative_dir,
        const QModelIndexList& selection
    );

protected:
    // returns the list of supported image format files
    static QStringList valid_image_format();

    // update the tag selector to be sync'ed with label list
    void update_tag_selector();

    // return the fullpath of the selected image.
    // if more than one, returns null.
    QString get_image_from_index_list(
        const QModelIndexList& index_list
    ) const;

    // build and popup the custom XML file dialog
    // use mode to choose whether it's a save or open file dialog
    void pop_up_file_dialog(
        QString& filename,
        QString& relative_dir,
        QFileDialog::AcceptMode mode
    );

    void pop_up_html_dialog(
        const QUrl& url
    );

private:
    QFileSystemModel* dir_model_;
    QTreeView* dir_view_;

    QPushButton* tag_button_;
    QPushButton* untag_button_;

    TagModel* tag_model_;
    QTreeView* tag_view_;

    QMenu* context_menu_;
    QModelIndex selected_for_context_;

    QComboBox* label_selector_;
    TagViewer* tag_viewer_;
    TagScrollView* tag_scroll_view_;
};

#endif // MAIN_WINDOW_H
