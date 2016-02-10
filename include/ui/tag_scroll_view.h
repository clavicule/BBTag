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

protected:
    virtual void wheelEvent(
        QWheelEvent* e
    ) Q_DECL_OVERRIDE;

private:
    float scale_factor_;

};

#endif // TAG_SCROLL_VIEW_H
