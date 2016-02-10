#include <core/tag_item.h>

#include <QFileInfo>


TagItem::TagItem(
        const QString& tag_label,
        const QColor& tag_color
    )
{
    tag_label_ = tag_label;
    tag_color_ = tag_color;
}

TagItem::TagItem(
        const TagItem& tag_item
    )
{
    tag_label_ = tag_item.tag_label_;
    tag_color_ = tag_item.tag_color_;

    index_ = tag_item.index_;
    bbox_ = tag_item.bbox_;
}

TagItem& TagItem::operator = (
        const TagItem& tag_item
    )
{
    if( this != &tag_item ) {
        tag_label_ = tag_item.tag_label_;
        tag_color_ = tag_item.tag_color_;

        index_ = tag_item.index_;
        bbox_ = tag_item.bbox_;
    }

    return *this;
}

TagItem::~TagItem()
{
}

void TagItem::add_tag(
        const QString& image_path,
        const QRect& bbox
    )
{
    QFileInfo fi( image_path );
    if( !fi.exists() ) {
        return;
    }

    QString key = fi.absoluteFilePath();
    index_.append( key );
    bbox_[ key ].append( bbox );
}

QString TagItem::filename(
        int i
    ) const
{
    if( i < 0 || i > index_.count() ) {
        return QString::null;
    }

    return QFileInfo( index_.at(i) ).fileName();
}

// returns the full file path at the given index
QString TagItem::fullpath(
        int i
    ) const
{
    if( i < 0 || i > index_.count() ) {
        return QString::null;
    }

    return index_.at(i);
}

