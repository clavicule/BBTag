#include <core/tag_model.h>
#include <core/tag_item.h>

#include <QStandardItemModel>

QString TagModel::ALL = "<ALL>";
QString TagModel::UNTAGGED = "<UNTAGGED>";

TagModel::TagModel(
        QObject *parent
    )
{
    model_ = new QStandardItemModel( parent );
    init();
}

TagModel::~TagModel()
{
}

void TagModel::init()
{
    model_->clear();
    image_image_ref_.clear();
    image_label_ref_.clear();

    QStandardItem* header = new QStandardItem( "Tag Labels" );
    header->setTextAlignment( Qt::AlignCenter );

    model_->setHorizontalHeaderItem( 0, header );

    QStandardItem* root = model_->invisibleRootItem();

    // default tree has 2 labels:
    // - untagged: images that have not been tagged at all
    // - all: all images imported regardless of their tag status
    all_item_ = new TagItem( Qt::transparent, ALL );
    untagged_item_ = new TagItem( Qt::transparent, UNTAGGED );

    root->appendRow( untagged_item_ );
    root->appendRow( all_item_ );
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
    if( label.isEmpty() ) {
        return false;
    }

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
    QSet<QString> images_processed;
    QSet<QModelIndex> parent_index_to_remove;
    QSet<QModelIndex> child_index_to_remove;

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
            TagItemList& ref_label_list = image_label_ref_[ fullpath ];
            TagItemList& ref_image_list = image_image_ref_[ fullpath ];
            images_processed.insert( fullpath );

            if( parent_item == all_item_ ) {
                while( !ref_image_list.isEmpty() ) {
                    TagItem* ref = ref_image_list.takeLast();
                    child_index_to_remove.insert( ref->index() );
                }
                image_label_ref_.remove( fullpath );
                image_image_ref_.remove( fullpath );

            } else if( parent_item == untagged_item_ ) {
                // the only way to remove from UNTAGGED is when the image
                // is removed from ALL
                continue;

            } else {
                // unref itself
                child_index_to_remove.insert( item->index() );
                ref_label_list.removeAll( parent_item );
                ref_image_list.removeAll( item );
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

                QString image_fullpath = image_item->fullpath();
                image_label_ref_[ image_fullpath ].removeAll( item );
                image_image_ref_[ image_fullpath ].removeAll( image_item );
                images_processed.insert( image_fullpath );
            }
            parent_index_to_remove.insert( item->index() );
        }
    }

    // sort indices from last to first
    // otherwise selection indexing becomes invalid
    // --> using STL - qSort and other QtAlgo are obsolete
    // however sorting QModelIndex does not take parenting into account
    // therefore no need to sort child items
    // but parents still do
    QModelIndexList list_to_process = parent_index_to_remove.toList();
    std::sort( list_to_process.begin(), list_to_process.end() );
    list_to_process.append( child_index_to_remove.toList() );

    for( int idx_itr = list_to_process.count() - 1; idx_itr >= 0; --idx_itr ) {
        const QModelIndex& idx = list_to_process.at( idx_itr );

        QStandardItem* item = model_->itemFromIndex( idx );
        if( !item ) {
            continue;
        }

        QModelIndex parent_index = item->parent()? item->parent()->index() : QModelIndex();
        model_->removeRow( idx.row(), parent_index );
    }

    // check if image is not reference is any other tag
    // in that case, it is added back to UNTAGGED
    for( QSet<QString>::iterator img_itr = images_processed.begin(); img_itr != images_processed.end(); ++img_itr ) {
        QString img = *img_itr;
        TagItem* item_as_untagged = get_tag_item( img, UNTAGGED );
        if( !item_as_untagged && image_label_ref_[ img ].count() == 1 ) {
            add_image_to_label( untagged_item_, QFileInfo( img ) );
        }
    }
}

TagItem* TagModel::add_image_to_label(
        TagItem* label_item,
        const QFileInfo& image_file
    )
{
    if( !label_item || !image_file.exists() ) {
        return 0;
    }

    QString image_filename = image_file.absoluteFilePath();

    // ensure the image is not already imported
    if( image_label_ref_[ image_filename ].contains( label_item ) ) {
        return 0;
    }
    TagItem* new_item = new TagItem( label_item, image_filename );

    image_label_ref_[ image_filename ].append( label_item );
    image_image_ref_[ image_filename ].append( new_item );

    return new_item;
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

QColor TagModel::get_color(
        const QModelIndex& index
    ) const
{
    if( !index.isValid() ) {
        return QColor();
    }

    TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( index ));
    if( !item ) {
        return QColor();
    }

    return item->color();
}

void TagModel::set_label(
        const QModelIndex &index,
        const QString& name
    )
{
    if( !index.isValid() || name.isEmpty() ) {
        return;
    }

    TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( index ));
    if( !item || item == untagged_item_ || item == all_item_ ) {
        return;
    }

    item->set_label( name );
    for( int r = 0; r < item->rowCount(); ++r ) {
        TagItem* tag_item = dynamic_cast<TagItem*>(item->child(r));
        if( !tag_item ) {
            continue;
        }

        tag_item->set_label( name );
    }
}

void TagModel::set_color(
        const QModelIndex& index,
        const QColor& color
    )
{
    if( !index.isValid() || !color.isValid() ) {
        return;
    }

    TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( index ));
    if( !item || item == untagged_item_ || item == all_item_ ) {
        return;
    }

    item->set_color( color );
    for( int r = 0; r < item->rowCount(); ++r ) {
        TagItem* tag_item = dynamic_cast<TagItem*>(item->child(r));
        if( !tag_item ) {
            continue;
        }

        tag_item->set_color( color );
    }
}

TagItem* TagModel::get_tag_item(
        const QString& fullpath,
        const QString& label
    )
{
    if( fullpath.isEmpty() || label.isEmpty() ) {
        return 0;
    }

    if( !image_image_ref_.contains( fullpath ) ) {
        return 0;
    }

    const TagItemList items = image_image_ref_[ fullpath ];
    for( TagItemList::const_iterator item_itr = items.begin(); item_itr != items.end(); ++item_itr ) {
        TagItem* image = *item_itr;

        if( image->label() == label ) {
            return image;
        }
    }

    return 0;
}

TagItem::Elements TagModel::get_element(
        const QString& fullpath,
        const QString& label
    )
{
    TagItem* image = get_tag_item( fullpath, label );
    if( !image ) {
        return TagItem::Elements();
    }

    return image->elements();
}

QList<TagItem::Elements> TagModel::get_elements(
        const QModelIndexList& index_list
    ) const
{
    QList<TagItem::Elements> elts;

    for( QModelIndexList::const_iterator idx_itr = index_list.begin(); idx_itr != index_list.end(); ++idx_itr ) {
        const QModelIndex& index = *idx_itr;
        if( !index.isValid() ) {
            continue;
        }

        TagItem* item = dynamic_cast<TagItem*>(model_->itemFromIndex( index ));
        if( !item ) {
            continue;
        }

        // if item is in ALL, returns all the elements of all
        // labels it belongs to
        if( item->QStandardItem::parent() == all_item_ ) {
            const TagItemList& all_items = image_image_ref_[ item->fullpath() ];

            // clears the list to avoid duplicates
            elts.clear();
            for( TagItemList::const_iterator tag_itr = all_items.begin(); tag_itr != all_items.end(); ++tag_itr ) {
                elts.append( (*tag_itr)->elements() );
            }
            // returns the list: it contains all the elements
            return elts;

        } else {
            elts.append( item->elements() );
        }
    }

    return elts;
}

QHash< QString, QList<TagItem::Elements> > TagModel::get_all_elements() const
{
    QHash< QString, QList<TagItem::Elements> > elts;

    for( QHash<QString, TagItemList>::const_iterator elt_itr = image_image_ref_.begin(); elt_itr != image_image_ref_.end(); ++elt_itr ) {
        const QString& fullpath = elt_itr.key();
        const TagItemList& tags = elt_itr.value();

        for( TagItemList::const_iterator tag_itr = tags.begin(); tag_itr != tags.end(); ++tag_itr ) {
            TagItem* item = *tag_itr;
            if( item->QStandardItem::parent() == all_item_ || item->QStandardItem::parent() == untagged_item_ ) {
                continue;
            }
            elts[ fullpath ].append( item->elements() );
        }
    }

    return elts;
}

void TagModel::init_from_elements(
        const QHash< QString, QList<TagItem::Elements> >& elts
    )
{
    init();
    for( QHash< QString, QList<TagItem::Elements> >::const_iterator elt_itr = elts.begin(); elt_itr != elts.end(); ++elt_itr ) {
        const QString& fullpath = elt_itr.key();
        const QList<TagItem::Elements>& tags = elt_itr.value();

        QFileInfoList fi;
        fi.append( QFileInfo( fullpath ) );
        import_images( fi );

        for( QList<TagItem::Elements>::const_iterator tag_itr = tags.begin(); tag_itr != tags.end(); ++tag_itr ) {
            const TagItem::Elements& elt = *tag_itr;
            const QList<QRect>& bbox = elt._bbox;

            QColor color = elt._color;
            if( !color.isValid() ) {
                int r = qrand() % 255;
                int g = qrand() % 255;
                int b = qrand() % 255;

                color = QColor::fromRgb( r, g, b );
            }
            add_new_label( color, elt._label );

            for( QList<QRect>::const_iterator bbox_itr = bbox.begin(); bbox_itr != bbox.end(); ++bbox_itr ) {
                add_tag_to_label( fullpath, elt._label, *bbox_itr );
            }
        }
    }
}

QHash<QString, QColor> TagModel::get_all_tags() const
{
    QHash<QString, QColor> tags;
    for( int r = 0; r < model_->rowCount(); ++r ) {
        TagItem* item = dynamic_cast<TagItem*>(model_->item( r ));
        if( !item || item == untagged_item_ || item == all_item_ ) {
            continue;
        }

        tags[ item->label() ] = item->color();
    }

    return tags;
}

QModelIndex TagModel::add_tag_to_label(
        const QString& fullpath,
        const QString& label,
        const QRect& tag
    )
{
    TagItem* image = get_tag_item( fullpath, label );

    // it is the first tag for this label
    if( !image ) {

        QList<QStandardItem*> labels = model_->findItems( label, Qt::MatchCaseSensitive, 0 );
        if( labels.count() != 1 ) {
            // it shouldn't be possible but we never know...
            return QModelIndex();
        }

        image = add_image_to_label( dynamic_cast<TagItem*>(labels.first()), QFileInfo( fullpath ) );
        if( !image ) {
            return QModelIndex();
        }
    }

    image->add_tag( tag );

    // first tag for any label --> remove it from untagged
    TagItem* item_as_untagged = get_tag_item( fullpath, UNTAGGED );
    if( item_as_untagged ) {
        image_label_ref_[ fullpath ].removeAll( untagged_item_ );
        image_image_ref_[ fullpath ].removeAll( item_as_untagged );
        model_->removeRow( item_as_untagged->index().row(), untagged_item_->index() );
    }

    return image->index();
}

QModelIndex TagModel::remove_tag_from_label(
        const QString& fullpath,
        const QString& label,
        const QRect& tag
    )
{
    TagItem* image = get_tag_item( fullpath, label );
    if( !image ) {
        return QModelIndex();
    }

    QModelIndex index = image->index();
    image->remove_tag( tag );

    // it is the last tag for this label
    if( image->tags().count() == 0 ) {
        QModelIndexList item;
        item.append( index );
        remove_items( item );

        TagItem* untagged_item = get_tag_item( fullpath, UNTAGGED );
        index = untagged_item? untagged_item->index() : QModelIndex();
    }

    return index;
}
