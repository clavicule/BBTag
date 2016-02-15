#ifndef TAG_VIEWER_H
#define TAG_VIEWER_H

#include <QWidget>

class TagViewer : public QWidget
{
    Q_OBJECT

public:
    // very similar structure to TagItem::Elements
    // we could use it however that would make TagViewer
    // no longer portable in case it could be
    // used for other structures than TagItem
    // viewer should always be independent of the model
    // I know it seems redundant...
    struct TagDisplayElement {
        QColor _color;
        QList<QRect> _bbox;
        QString _label;
    };

public:
    TagViewer(
        QWidget* parent = 0
    );

    virtual ~TagViewer();

    // set the background image
    // if image cannot be loaded
    // error message will be displayed instead
    inline void set_image(
        const QPixmap& pix
    );

    inline void set_overlay_elements(
        const QList<TagDisplayElement>& elements
    );

    // set the label and color for the current tag being drawn
    inline void set_tag_options(
        const QString& current_label,
        const QColor& current_color
    );

signals:
    // emitted when left mouse button is released
    // while tagging was in progress
    void tagged(
        const QRect& bbox
    );

public slots:
    // activate or deactivate the tagging tool
    void set_tagging_status(
       bool activate
    );

protected:
    void enforce_boundary_conditions(
        QPoint& p
    );

// re-implementation from QWidget
protected:
    virtual void paintEvent(
        QPaintEvent* e
    ) Q_DECL_OVERRIDE;

    virtual void mousePressEvent(
        QMouseEvent* e
    ) Q_DECL_OVERRIDE;

    virtual void mouseMoveEvent(
        QMouseEvent* e
    ) Q_DECL_OVERRIDE;

    virtual void mouseReleaseEvent(
        QMouseEvent* e
    ) Q_DECL_OVERRIDE;

private:
    bool tagging_;
    QPoint tag_start_;
    QPoint tag_end_;

    QPixmap pix_;
    QList<TagDisplayElement> elts_;

    QString current_label_;
    QColor current_color_;
};


/************************* inline *************************/

void TagViewer::set_image(
        const QPixmap& pix
    )
{
    pix_ = pix;

    if( pix_.isNull() ) {
        resize( 400, 100 );
    } else {
        resize( pix.size() );
    }
}

void TagViewer::set_overlay_elements(
        const QList<TagViewer::TagDisplayElement>& elements
    )
{
    elts_ = elements;
}

void TagViewer::set_tag_options(
        const QString& current_label,
        const QColor& current_color
    )
{
    current_label_ = current_label;
    current_color_ = current_color;
}

#endif // TAG_VIEWER_H
