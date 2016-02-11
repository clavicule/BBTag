#include <core/tag_model.h>
#include <core/tag_item.h>

#include <QStandardItemModel>


TagModel::TagModel(
        QObject *parent
    )
{
    QStandardItem* header = new QStandardItem( "Tag Labels" );
    header->setTextAlignment( Qt::AlignCenter );

    tag_model_ = new QStandardItemModel( parent );
    tag_model_->setHorizontalHeaderItem( 0, header );

    QStandardItem* root = tag_model_->invisibleRootItem();

    // default tree has 2 labels:
    // - untagged: images that have not been tagged at all
    // - all: all images imported regardless of their tag status
    untagged_item_ = new TagItem( Qt::transparent, "**untagged**" );
    all_item_ = new TagItem( Qt::transparent, "**all**" );

    root->appendRow( untagged_item_ );
    root->appendRow( all_item_ );
}

TagModel::~TagModel()
{
}

bool TagModel::add_new_label(
        const QColor& color,
        const QString& label
    )
{
    // check if label is already taken
    if( tag_model_->findItems( label, Qt::MatchFixedString, 0 ).count() > 0 ) {
        return false;
    }

    // adds the new item
    QStandardItem* root = tag_model_->invisibleRootItem();
    TagItem* new_item = new TagItem( color, label );
    root->appendRow( new_item );

    return true;
}

void TagModel::import_images(
        const QStringList& image_list
    )
{
    for( QStringList::const_iterator img_itr = image_list.begin(); img_itr != image_list.end(); ++img_itr ) {
        TagItem* new_untagged_item = new TagItem( untagged_item_, *img_itr );
        TagItem* new_all_item = new TagItem( all_item_, *img_itr );
        Q_UNUSED( new_untagged_item );
        Q_UNUSED( new_all_item );
    }
}
