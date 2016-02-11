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

    inline void setImage(
        const QPixmap& pix
    );

signals:
    void tagFinished();

public slots:
    void updateTagToolStatus(
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

};


/************************* inline *************************/

void TagViewer::setImage(
        const QPixmap& pix
    )
{
    pix_ = pix;

    if( pix_.isNull() ) {
        resize( 400, 150 );
    } else {
        resize( pix.size() );
    }
    repaint();
}

#endif // TAG_VIEWER_H
