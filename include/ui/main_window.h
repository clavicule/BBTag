#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

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

private:
    TagScrollView* tag_scroll_view_;
    TagModel* tag_model_;
    TagViewer* tag_viewer_;
};

#endif // MAIN_WINDOW_H
