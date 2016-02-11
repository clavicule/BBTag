#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class QTreeView;
class QFileSystemModel;
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
    void import_images();
    void add_label();
    void remove_images();

protected:
    static QStringList valid_image_format();

private:
    QFileSystemModel* dir_model_;
    QTreeView* dir_view_;

    TagModel* tag_model_;
    QTreeView* tag_view_;

    TagViewer* tag_viewer_;
    TagScrollView* tag_scroll_view_;
};

#endif // MAIN_WINDOW_H
