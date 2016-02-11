#include <core/tag_model.h>
#include <core/tag_item.h>

#include <QStandardItemModel>


TagModel::TagModel(
        QObject *parent
    )
{
    QStandardItem* header = new QStandardItem( "Tag Labels" );
    header->setTextAlignment( Qt::AlignCenter );

    model_ = new QStandardItemModel( parent );
    model_->setHorizontalHeaderItem( 0, header );

    QStandardItem* root = model_->invisibleRootItem();

    // default tree has 2 labels:
    // - untagged: images that have not been tagged at all
    // - all: all images imported regardless of their tag status
    untagged_item_ = new TagItem( Qt::transparent, "<UNTAGGED>" );
    all_item_ = new TagItem( Qt::transparent, "<ALL>" );

    root->appendRow( untagged_item_ );
    root->appendRow( all_item_ );
}

TagModel::~TagModel()
{
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

bool TagModel::add_new_label(
        const QColor& color,
        const QString& label
    )
{
    // check if label is already taken
    if( model_->findItems( label, Qt::MatchFixedString, 0 ).count() > 0 ) {
        return false;
    }

    // adds the new item
    QStandardItem* root = model_->invisibleRootItem();
    TagItem* new_item = new TagItem( color, label );
    root->appendRow( new_item );

    return true;
}

void TagModel::remove_items(
        const QModelIndexList& index_list
    )
{
    // sort indices from last to first
    // otherwise selection indexing becomes invalid
    // --> using STL - qSort and other QtAlgo are obsolete
    QModelIndexList sorted_index_list = index_list;
    std::sort( sorted_index_list.begin(), sorted_index_list.end() );

    for( int idx_itr = sorted_index_list.count() - 1; idx_itr >= 0; --idx_itr ) {
        const QModelIndex& idx = sorted_index_list.at( idx_itr );
        if( !idx.isValid() ) {
            continue;
        }

        QStandardItem* item = model_->itemFromIndex( idx );
        if( !item || item == untagged_item_ || item == all_item_ ) {
            continue;
        }

        QModelIndex parent_index = item->parent()? item->parent()->index() : QModelIndex();
        model_->removeRow( item->row(), parent_index );
    }

    // to do: re-assign removed items to ALL and UNTAGGED if applicable
    // to do: implies removing the bbox, etc.
}
