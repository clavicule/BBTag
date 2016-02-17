#ifndef TAG_SCROLL_VIEW_H
#define TAG_SCROLL_VIEW_H

#include <QScrollArea>

class TagScrollView : public QScrollArea
{
    Q_OBJECT

public:
    TagScrollView(
        QWidget* parent = 0
    );

    virtual ~TagScrollView();

public slots:
    void zoom_in();
    void zoom_out();
    void fit_to_view();

protected:
    void scale_by(
        float factor
    );

    void adjust_scrollbars(
        float factor
    );

// overloaded functions
protected:
    // overload as shortcut for zooming in and out
    virtual void wheelEvent(
        QWheelEvent* e
    ) Q_DECL_OVERRIDE;

};

#endif // TAG_SCROLL_VIEW_H
