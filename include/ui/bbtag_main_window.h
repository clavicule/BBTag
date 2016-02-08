#ifndef BBTAG_MAIN_WINDOW_H
#define BBTAG_MAIN_WINDOW_H

#include <QMainWindow>

class QScrollArea;

class BBTagMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    BBTagMainWindow(
        QWidget* parent = 0
    );

    virtual ~BBTagMainWindow();

private:
    QScrollArea* image_viewer_;
};

#endif // BBTAG_MAIN_WINDOW_H
