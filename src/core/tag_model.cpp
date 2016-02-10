#include <core/tag_model.h>
#include <core/tag_item.h>

#include <QFileInfo>

TagModel::TagModel(
        QObject *parent
    ) : QAbstractItemModel( parent )
{
}

TagModel::~TagModel()
{
}

int TagModel::columnCount(
        const QModelIndex& /*parent*/
    ) const
{
    return 1;
}

QVariant TagModel::headerData(
        int /*section*/,
        Qt::Orientation /*orientation*/,
        int /*role*/
    ) const
{
    return QVariant();
}

QVariant TagModel::data(
        const QModelIndex& index,
        int role
    ) const
{
    if( !index.isValid() ) {
        return QVariant();
    }

    if( index.column() > 0 ) {
        return QVariant();
    }

    if( !index.parent().isValid() ) { // tag label
        int row = index.row();

        // check for out of bound index
        if( row < 0 || row >= tag_data_.count() ) {
            return QVariant();
        }

        const TagItem& tag_item = tag_data_.at( row );

        if( role == Qt::DecorationRole ) {
            return QVariant( tag_item.color() );

        } else if( role == Qt::DisplayRole || role == Qt::ToolTipRole ) {
            return QVariant( tag_item.label() );
        }

    } else { // tag item
        int row = index.row();
        int label_row = index.parent().row();

        // check for out of bound index
        if( label_row < 0 || label_row >= tag_data_.count() ) {
            return QVariant();
        }

        const TagItem& tag_item = tag_data_.at( label_row );

        if( role == Qt::DisplayRole ) {
            return QVariant( tag_item.filename( row ) );

        } else if( role == Qt::ToolTipRole ) {
            return QVariant( tag_item.fullpath( row ) );
        }
    }

    return QVariant();
}


QModelIndex TagModel::index(
        int row,
        int column,
        const QModelIndex& parent
    ) const
{
    if( !hasIndex( row, column, parent ) ) {
        return QModelIndex();
    }

    if( !parent.isValid() ) {
        return createIndex( row, 0 );

    } else {
        return parent.child( row, 0 );
    }

    return QModelIndex();
}

QModelIndex TagModel::parent(
        const QModelIndex& index
    ) const
{
    if( !index.isValid() ) {
        return QModelIndex();
    }

    if( !index.parent().isValid() ) {
        return QModelIndex();
    }

    return index.parent();
}

int TagModel::rowCount(
        const QModelIndex& parent
    ) const
{
    int no_rows = 0;

    if( !parent.isValid() ) {
        no_rows = tag_data_.count();

    } else {
        int row = parent.row();

        // check for out of bound index
        if( row >= 0 && row < tag_data_.count() ) {
            no_rows = tag_data_.at( row ).tagCount();
        }
    }

    return no_rows;
}
