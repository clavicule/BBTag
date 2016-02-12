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
    all_item_ = new TagItem( Qt::transparent, "<ALL>" );
    untagged_item_ = new TagItem( Qt::transparent, "<UNTAGGED>" );

    root->appendRow( untagged_item_ );
    root->appendRow( all_item_ );
}

TagModel::~TagModel()
{
}

void TagModel::import_images(
        const QFileInfoList& image_list
    )
{
    for( QFileInfoList::const_iterator img_itr = image_list.begin(); img_itr != image_list.end(); ++img_itr ) {
        const QFileInfo& fi = *img_itr;

        add_image_to_label( untagged_item_, fi );
        add_image_to_label( all_item_, fi );
    }
}

bool TagModel::add_new_label(
        const QColor& color,
        const QString& label
    )
{
    // check if label is already taken
    if( model_->findItems( label, Qt::MatchCaseSensitive, 0 ).count() > 0 ) {
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

    QModelIndexList index_to_remove;
    for( QModelIndexList::const_iterator idx_itr = index_list.begin(); idx_itr != index_list.end(); ++idx_itr ) {
        const QModelIndex& idx = *idx_itr;
        if( !idx.isValid() ) {
            continue;
        }

        TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( idx ));
        if( !item || item == untagged_item_ || item == all_item_ ) {
            continue;
        }

        TagItem* parent_item = dynamic_cast<TagItem*>(item->QStandardItem::parent());
        QModelIndex parent_index = parent_item? parent_item->index() : QModelIndex();

        // unreferences relevant labels associated the item
        // if parent is ALL --> completely removes item i-e from all label it belongs to
        // if parent is UNTAGGED --> does nothing, item cannot be removed from UNTAGGED
        // if parent is another label --> unref and removes from that label only
        // if parent is root (i-e we're dealing with an image item)
        //   --> remove all items belonging to that label
        if( parent_index.isValid() ) {
            const QString& fullpath = item->fullpath();
            TagItemList& ref_list = image_path_ref_[ fullpath ];

            if( parent_item == all_item_ ) {
                for( TagItemList::iterator ref_itr = ref_list.begin(); ref_itr != ref_list.end(); ++ref_itr ) {

                    // find the reference to the same image in other labels
                    TagItem* ref = (*ref_itr)->find_item( fullpath );
                    if( !ref ) {
                        // it should not happen but we never know...
                        continue;
                    }
                    index_to_remove.append( ref->index() );
                }
                image_path_ref_.remove( fullpath );

            } else if( parent_item == untagged_item_ ) {
                // the only way to remove from UNTAGGED is when the image
                // is removed from ALL
                continue;

            } else {
                // unref itself
                index_to_remove.append( item->index() );
                ref_list.removeAll( parent_item );

                // check if image is not reference is any other tag
                // in that case, it is added back to UNTAGGED
                if( ref_list.isEmpty() ) {
                    add_image_to_label( untagged_item_, QFileInfo( item->fullpath() ) );
                }
            }
        } else {
            // we only unref all items from the label
            // no need to destroy individually all the items as they will be removed
            // when the tag label is removed
            for( int r = 0; r < item->rowCount(); ++r ) {
                TagItem* image_item = dynamic_cast<TagItem*>(item->child( r ));
                if( !image_item ) {
                    // it should not happen but we never know...
                    continue;
                }
                image_path_ref_[ image_item->fullpath() ].removeAll( item );
            }
            index_to_remove.append( item->index() );

        }
    }

    // sort indices from last to first
    // otherwise selection indexing becomes invalid
    // --> using STL - qSort and other QtAlgo are obsolete

    std::sort( index_to_remove.begin(), index_to_remove.end() );

    for( int idx_itr = index_to_remove.count() - 1; idx_itr >= 0; --idx_itr ) {
        const QModelIndex& idx = index_to_remove.at( idx_itr );

        QStandardItem* item = model_->itemFromIndex( idx );
        if( !item ) {
            continue;
        }

        QModelIndex parent_index = item->parent()? item->parent()->index() : QModelIndex();
        model_->removeRow( idx.row(), parent_index );
    }
}

void TagModel::add_image_to_label(
        TagItem* label_item,
        const QFileInfo& image_file
    )
{
    if( !label_item || !image_file.exists() ) {
        return;
    }

    QString image_filename = image_file.absoluteFilePath();

    // ensure the image is not already imported
    if( image_path_ref_[ image_filename ].contains( label_item ) ) {
        return;
    }
    image_path_ref_[ image_filename ].append( label_item );

    TagItem* new_item = new TagItem( label_item, image_filename );
    Q_UNUSED( new_item );
}

QString TagModel::get_fullpath(
        const QModelIndex& index
    ) const
{
    if( !index.isValid() ) {
        return QString::null;
    }

    TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( index ));
    if( !item ) {
        return QString::null;
    }

    return item->fullpath();
}

QList<QRect> TagModel::get_tags(
        const QModelIndex& index
    ) const
{
    if( !index.isValid() ) {
        return QList<QRect>();
    }

    TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( index ));
    if( !item ) {
        return QList<QRect>();
    }

    return item->tags();
}

QColor TagModel::get_color(
        const QModelIndex& index
    ) const
{
    if( !index.isValid() ) {
        return QColor( Qt::transparent );
    }

    TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( index ));
    if( !item ) {
        return QColor( Qt::transparent );
    }

    return item->color();
}

QString TagModel::get_label(
        const QModelIndex& index
    ) const
{
    if( !index.isValid() ) {
        return QString::null;
    }

    TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( index ));
    if( !item ) {
        return QString::null;
    }

    return item->label();
}

QList< QPair<QString, QColor> > TagModel::get_all_tags() const
{
    QList< QPair<QString, QColor> > tags;
    for( int r = 0; r < model_->rowCount(); ++r ) {
        TagItem* item = dynamic_cast<TagItem*>(model_->item( r ));
        if( !item || item == untagged_item_ || item == all_item_ ) {
            continue;
        }

        tags.append( QPair<QString, QColor>( item->label(), item->color() ) );
    }

    return tags;
}
