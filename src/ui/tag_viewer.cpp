#include <ui/tag_viewer.h>

#include <QPainter>
#include <QMouseEvent>

TagViewer::TagViewer(
        QWidget* parent
    ) : QWidget( parent ), tagging_( true )
{
    setBackgroundRole( QPalette::Base );
    setAutoFillBackground( true );
}

TagViewer::~TagViewer()
{
}

void TagViewer::updateTagToolStatus(
        bool /*activate*/
    )
{
//    tagging_ = activate;
    emit( tagFinished() );
}

void TagViewer::enforceBoundaryConditions(
        QPoint& p
    )
{
    int x = p.x();
    int y = p.y();

    if( x < 0 ) {
        p.setX( 0 );
    }

    if( y < 0 ) {
        p.setY( 0 );
    }

    if( x >= width() ) {
        p.setX( width() - 3 );
    }

    if( y >= height() ) {
        p.setY( height() - 3 );
    }
}

void TagViewer::paintEvent(
        QPaintEvent* /*e*/
    )
{
    QPainter p( this );
    p.setPen( Qt::blue );
    p.drawRect( QRect( tag_start_, tag_end_ ) );
}

void TagViewer::mousePressEvent(
        QMouseEvent* e
    )
{
    if( !tagging_ ) {
        return;
    }

    if( e->button() != Qt::LeftButton ) {
        return;
    }

    tag_start_ = e->pos();
}

void TagViewer::mouseMoveEvent(
        QMouseEvent* e
    )
{
    if( !tagging_ ) {
        return;
    }

    tag_end_ = e->pos();
    enforceBoundaryConditions( tag_end_ );
    repaint();

}

void TagViewer::mouseReleaseEvent(
        QMouseEvent* e
     )
{
    if( !tagging_ ) {
        return;
    }

    tag_end_ = e->pos();
    enforceBoundaryConditions( tag_end_ );
    updateTagToolStatus( false );
}
