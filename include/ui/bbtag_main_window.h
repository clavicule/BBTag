#ifndef BBTAG_MAIN_WINDOW_H
#define BBTAG_MAIN_WINDOW_H

#include <QMainWindow>

class BBTagMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    BBTagMainWindow(QWidget *parent = 0);
    ~BBTagMainWindow();
};

#endif // BBTAG_MAIN_WINDOW_H
