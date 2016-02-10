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

};

#endif // TAG_VIEWER_H
