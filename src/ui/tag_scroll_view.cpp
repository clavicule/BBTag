#include <ui/tag_scroll_view.h>

#include <QWheelEvent>
#include <QScrollBar>

TagScrollView::TagScrollView(
        QWidget* parent
    ) : QScrollArea( parent )
{
    setBackgroundRole( QPalette::Dark );
    setAutoFillBackground( true );
}

TagScrollView::~TagScrollView()
{
}

void TagScrollView::zoom_in()
{
    // safeguard
    QSize new_size = 1.25 * widget()->size();
    if( qMax( new_size.width(), new_size.height() ) > 10 * qMax( size().width(), size().height() ) ) {
        return;
    }

    scale_by( 1.25 );
}

void TagScrollView::zoom_out()
{
    // safeguard
    QSize new_size = 0.8 * widget()->size();
    if( qMin( new_size.width(), new_size.height() ) < 10 ) {
        return;
    }

    scale_by( 0.8 ); // = 1 / 1.25
}

void TagScrollView::fit_to_view()
{
    QSize widget_size = widget()->size();
    widget_size.scale( size(), Qt::KeepAspectRatio );
    widget()->resize( widget_size );
}

void TagScrollView::scale_by(
        float factor
    )
{
    if( factor <= 0. ) {
        return;
    }

    widget()->resize( widget()->size() * factor );

    adjust_scrollbars( factor );
}

void TagScrollView::adjust_scrollbars(
        float factor
    )
{
    horizontalScrollBar()->setValue( int( factor * horizontalScrollBar()->value() + ( ( factor - 1 ) * horizontalScrollBar()->pageStep() / 2 ) ) );
    verticalScrollBar()->setValue( int( factor * verticalScrollBar()->value() + ( ( factor - 1 ) * verticalScrollBar()->pageStep() / 2 ) ) );
}

void TagScrollView::wheelEvent(
        QWheelEvent* e
     )
{
    if( e->modifiers() == Qt::CTRL ) {
        QPoint angle = e->angleDelta();
        if( angle.isNull() ) {
            return;
        }

        if( angle.y() > 0 ) {
            zoom_in();
        } else if( angle.y() < 0 ) {
            zoom_out();
        }
    } else {
        QScrollArea::wheelEvent(e);
    }
}
