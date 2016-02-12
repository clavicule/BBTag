#ifndef TAG_VIEWER_H
#define TAG_VIEWER_H

#include <QWidget>

class TagViewer : public QWidget
{
    Q_OBJECT

public:
    TagViewer(
        QWidget* parent = 0
    );

    virtual ~TagViewer();

    inline void set_image(
        const QPixmap& pix
    );

    inline void set_tags(
        const QList<QRect>& tags
    );

    inline void set_color(
        const QColor& color
    );

    inline void set_label(
        const QString& label
    );

    inline void set_tag_options(
        const QString& current_label,
        const QColor& current_color
    );

public slots:
    void set_tagging_status(
       bool activate
    );

protected:
    void enforceBoundaryConditions(
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
    QList<QRect> tags_;
    QColor color_;
    QString label_;

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

void TagViewer::set_tags(
        const QList<QRect>& tags
    )
{
    tags_ = tags;
}

void TagViewer::set_color(
        const QColor& color
    )
{
    color_ = color;
}

void TagViewer::set_label(
        const QString& label
    )
{
    label_ = label;
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
