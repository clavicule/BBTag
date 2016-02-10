#include <ui/tag_scroll_view.h>

#include <QWheelEvent>

TagScrollView::TagScrollView(
        QWidget* parent
    ) : QScrollArea( parent ), scale_factor_( 1. )
{
    setBackgroundRole( QPalette::Dark );
    setAutoFillBackground( true );
}

TagScrollView::~TagScrollView()
{
}

void TagScrollView::wheelEvent(
        QWheelEvent* e
     )
{

}
