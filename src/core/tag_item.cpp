#include <core/tag_item.h>


TagItem::TagItem(
        const QColor& tag_color,
        const QString& tag_label
    ) : QStandardItem()
{
    tag_color_ = tag_color;
    tag_label_ = tag_label;
}

TagItem::TagItem(
        TagItem* tag_item,
        const QString& image_file
    )
{
    if( !tag_item ) {
        return;
    }

    fullpath_ = image_file;
    tag_color_ = tag_item->tag_color_;
    tag_label_ = tag_item->tag_label_;
    bbox_ = tag_item->bbox_;
    tag_item->appendRow( this );
}

TagItem::TagItem(
        const TagItem& tag_item
    )
{
    tag_color_ = tag_item.tag_color_;
    tag_label_ = tag_item.tag_label_;
    fullpath_ = tag_item.fullpath_;
    bbox_ = tag_item.bbox_;

    QStandardItem* tag_parent = tag_item.QStandardItem::parent();
    if( tag_parent ) {
        tag_parent->appendRow( this );
    }
}

TagItem& TagItem::operator = (
        const TagItem& tag_item
    )
{
    if( this != &tag_item ) {
        tag_color_ = tag_item.tag_color_;
        tag_label_ = tag_item.tag_label_;
        fullpath_ = tag_item.fullpath_;
        bbox_ = tag_item.bbox_;

        // remove itself first
        QStandardItem* current_parent = QStandardItem::parent();
        if( current_parent ) {
            current_parent->removeRow( row() );
        }

        // then reparent
        QStandardItem* tag_parent = tag_item.QStandardItem::parent();
        if( tag_parent ) {
            tag_parent->appendRow( this );
        }
    }

    return *this;
}

TagItem::~TagItem()
{
}

QVariant TagItem::data(
        int role
    ) const
{
    if( fullpath_.isEmpty() ) { // tag label
        if( role == Qt::DecorationRole ) {
            return QVariant( tag_color_ );

        } else if( role == Qt::DisplayRole || role == Qt::ToolTipRole ) {
            return QVariant( tag_label_ );
        }

    } else { // image item
        if( role == Qt::DisplayRole ) {
            return QVariant( filename() );

        } else if( role == Qt::ToolTipRole ) {
            return QVariant( fullpath_ );

        } else if( role == Qt::DecorationRole ) {
            return QVariant( QColor( Qt::transparent ) );
        }
    }

    return QVariant();
}

TagItem* TagItem::find_item(
        const QString& fullpath
    ) const
{
    for( int r = 0; r < rowCount(); ++r ) {
        TagItem* kid = dynamic_cast<TagItem*>(child( r ));
        if( !kid ) {
            continue;
        }

        if( fullpath == kid->fullpath() ) {
            return kid;
        }
    }

    return 0;
}

