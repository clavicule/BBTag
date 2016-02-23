#include <ui/tag_viewer.h>

#include <QPainter>
#include <QMouseEvent>

TagViewer::TagViewer(
        QWidget* parent
    ) : QWidget( parent ), tagging_( false ), untagging_( false )
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
    tag_start_ = QPoint( 0, 0 );
    tag_end_ = QPoint( 0, 0 );

    QCursor cursor = activate? QCursor( Qt::CrossCursor ) : QCursor( Qt::ArrowCursor );
    setCursor( cursor );
}

void TagViewer::set_untagging_status(
        bool activate
    )
{
    untagging_ = activate;

    QCursor cursor = activate? QCursor( Qt::ForbiddenCursor ) : QCursor( Qt::ArrowCursor );
    setCursor( cursor );
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

float TagViewer::scale_factor() const
{
    float scale_factor = 1.;

    if( !pix_.isNull() && !size().isNull() ) {
        scale_factor = (float)(size().width()) / (float)(pix_.width());
    }

    return scale_factor;
}

int TagViewer::shortest_distance(
        const QPoint& p,
        const QRect& rect
    )
{    
    int top = rect.top();
    int left = rect.left();
    int bottom = rect.y() + rect.height(); // note from Qt doc --> retrieve the true y-coordinate
    int right = rect.x() + rect.width(); // note from Qt doc --> retrieve the true x-coordinate

    int x = p.x();
    int y = p.y();

    // if x (resp. y ) in between rectangle left & right (resp. top and bottom)
    // --> count a distance of 0
    int dx = ( x < right && x > left )? 0 : qMin( abs( left - x ), abs( right - x ) );
    int dy = ( y < bottom && y > top )? 0 : qMin( abs( bottom - y ), abs( top - y ) );

    return ( dx * dx + dy * dy );
}

void TagViewer::paintEvent(
        QPaintEvent* /*e*/
    )
{
    QPainter p( this );

    float scale_f = scale_factor();
    QRect drawing_area( 0, 0, size().width(), size().height() );

    if( !pix_.isNull() && !size().isNull() ) {
        p.drawPixmap( drawing_area, pix_ );

    } else {
        QTextOption options( Qt::AlignLeft );
        options.setWrapMode( QTextOption::WordWrap );
        p.setPen( QPen( Qt::red, 4 ) );
        p.drawText(
            QRectF( drawing_area ),
            "Image cannot be displayed. Check:\n"
            " - the same image is selected among the multiple selection (it's ok to select labels)\n"
            " - the image file is still at the same disk location when imported\n"
            " - the image format is valid and/or the file is not corrupted ",
            options
        );

        return;
    }

    QFont font;
    font.setPointSize( 10 );
    p.setFont( font );

    // draw bounding boxes
    for( QList<TagDisplayElement>::iterator tag_itr = elts_.begin(); tag_itr != elts_.end(); ++tag_itr ) {
        const TagDisplayElement& tag = *tag_itr;
        const QList<QRect>& bbox = tag._bbox;

        p.setPen( QPen( tag._color, 2 ) );

        for( QList<QRect>::const_iterator bbox_itr = bbox.begin(); bbox_itr != bbox.end(); ++bbox_itr ) {
            const QRect& box_rect = *bbox_itr;
            QRect scaled_box( scale_f * box_rect.topLeft(), scale_f * box_rect.bottomRight() );
            p.drawRect( scaled_box );
            p.drawText( scaled_box.x(), scaled_box.y(), tag._label );
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
    if( e->button() != Qt::LeftButton ) {
        return;
    }

    if( tagging_ ) {
        tag_start_ = e->pos();
        enforce_boundary_conditions( tag_start_ );
        tag_end_ = tag_start_;

    } else if( untagging_ ) {
        // searches the closest rectangle to the picked point
        // removes it only if point is deemed close enough
        // there won't be tons of label per image, so a brute force search
        // is perfectly acceptable, no need to go into quad-tree
        float scale_f = scale_factor();
        QRect rect_found;
        QString label_found;
        int distance_min = 200. / scale_f;
        QPoint p = e->pos();
        enforce_boundary_conditions( p );
        p /= scale_f;

        for( QList<TagDisplayElement>::iterator tag_itr = elts_.begin(); tag_itr != elts_.end(); ++tag_itr ) {
            const TagDisplayElement& tag = *tag_itr;
            const QList<QRect>& bbox = tag._bbox;

            for( QList<QRect>::const_iterator bbox_itr = bbox.begin(); bbox_itr != bbox.end(); ++bbox_itr ) {
                int d = shortest_distance( p, *bbox_itr );
                if( d < distance_min ) {
                    distance_min = d;
                    label_found = tag._label;
                    rect_found = *bbox_itr;
                }
            }
        }
        if( !label_found.isEmpty() && rect_found.isValid() ) {
            emit( untagged( label_found, rect_found ) );
        }
    }
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
    if( !tagging_ || e->button() != Qt::LeftButton ) {
        return;
    }

    tag_end_ = e->pos();
    enforce_boundary_conditions( tag_end_ );

    float scale_f = scale_factor();

    // make a valid rectangle
    QRect tag( tag_start_ / scale_f, tag_end_ / scale_f );
    int left = tag.left();
    int top = tag.top();
    if( left > tag.right() ) {
        tag.setLeft( tag.right() );
        tag.setRight( left );
    }
    if( top > tag.bottom() ) {
        tag.setTop( tag.bottom() );
        tag.setBottom( top );
    }

    emit( tagged( tag ) );

    tag_start_ = QPoint( 0, 0 );
    tag_end_ = QPoint( 0, 0 );
}
