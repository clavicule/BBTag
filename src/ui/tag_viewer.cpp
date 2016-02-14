#include <ui/tag_viewer.h>

#include <QPainter>
#include <QMouseEvent>

TagViewer::TagViewer(
        QWidget* parent
    ) : QWidget( parent ), tagging_( false )
{
    setBackgroundRole( QPalette::Base );
    setAutoFillBackground( true );
}

TagViewer::~TagViewer()
{
}

void TagViewer::set_tagging_status(
        bool activate
    )
{
    tagging_ = activate;
}

void TagViewer::enforce_boundary_conditions(
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

    if( !pix_.isNull() ) {
        p.drawPixmap( 0, 0, pix_ );

    } else {
        QTextOption options( Qt::AlignLeft );
        options.setWrapMode( QTextOption::WordWrap );
        p.setPen( QPen( Qt::red, 4 ) );
        p.drawText(
            QRectF( rect() ),
            "Image cannot be displayed. Check:\n"
            " - only one image is selected\n"
            " - the image file is still at the same disk location when imported\n"
            " - the image format is valid and/or the file is not corrupted ",
            options
        );

        return;
    }

    QFont font;
    font.setPointSize( 8 );
    p.setFont( font );

    // draw bounding boxes
    for( QList<TagDisplayElement>::iterator tag_itr = elts_.begin(); tag_itr != elts_.end(); ++tag_itr ) {
        const TagDisplayElement& tag = *tag_itr;
        const QList<QRect> bbox = tag._bbox;

        p.setPen( QPen( tag._color, 2 ) );

        for( QList<QRect>::const_iterator bbox_itr = bbox.begin(); bbox_itr != bbox.end(); ++bbox_itr ) {
            p.drawRect( *bbox_itr );
            p.drawText( (*bbox_itr).x(), (*bbox_itr).y(), tag._label );
        }
    }

    // draw current box being tagged
    p.setPen( QPen( current_color_, 2 ) );
    if( tagging_ && tag_start_ != tag_end_ ) {
        QRect current_rect( tag_start_, tag_end_ );
        p.drawRect( current_rect );
        p.drawText( current_rect.x(), current_rect.y(), current_label_ );
    }

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
    enforce_boundary_conditions( tag_end_ );
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
    enforce_boundary_conditions( tag_end_ );
}
