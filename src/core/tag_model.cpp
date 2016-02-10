#include <core/tag_model.h>
#include <core/tag_item.h>

#include <QStandardItemModel>

TagModel::TagModel(
        QObject *parent
    )
{
    tag_model_ = new QStandardItemModel( parent );
}

TagModel::~TagModel()
{
}


