#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QItemSelection>

class QTreeView;
class QFileSystemModel;
class QComboBox;
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

protected slots:
    // internal slot for updating the viewer
    // with the current selected image
    void update_viewer();

    // internal slot for updating the viewer tagging options
    // based on the current tag selection
    void set_viewer_tag_options();

protected:
    // returns the list of supported image format files
    static QStringList valid_image_format();

    // update the tag selector to be sync'ed with label list
    void update_tag_selector();

private:
    QFileSystemModel* dir_model_;
    QTreeView* dir_view_;

    TagModel* tag_model_;
    QTreeView* tag_view_;

    QComboBox* label_selector_;
    TagViewer* tag_viewer_;
    TagScrollView* tag_scroll_view_;
};

#endif // MAIN_WINDOW_H
