#include <ui/bbtag_main_window.h>
#include <QApplication>

int main(
        int argc,
        char *argv[]
    )
{
    QApplication a(argc, argv);
    BBTagMainWindow w;
    w.show();

    return a.exec();
}
