#ifndef TAG_MODEL_H
#define TAG_MODEL_H

#include <core/tag_item.h>

#include <QAbstractItemModel>
#include <QList>


// Tree model for image tag labeling.
// Example:
// - label_1
// .... image1.png
// .... image2.png
// - label_2
// .... image1.png
// - label_3
// .... image_6.jpeg
class TagModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    // does nothing
    TagModel(
        QObject* parent = 0
    );

    // removes the data associated to the model
    virtual ~TagModel();

// re-implementing virtual pure functions from QAbstractItemModel
public:

    // returns 1
    virtual int columnCount(
        const QModelIndex& parent = QModelIndex()
    ) const Q_DECL_OVERRIDE;

    // returns empty string - no header
    virtual QVariant headerData(
        int section,
        Qt::Orientation orientation,
        int role = Qt::DisplayRole
    ) const Q_DECL_OVERRIDE;

    // index.row() is only taken into account since there is only 1 column
    // Returns:
    // - tag color for the decoration role if tag label
    // - tag name for the display role and tooltip role if tag label
    // - image name for the display role if image
    // - image full path for tooltip role if image
    // - nothing for all other roles
    virtual QVariant data(
        const QModelIndex& index,
        int role = Qt::DisplayRole
    ) const Q_DECL_OVERRIDE;

    // returns the row index following tree model/view design
    virtual QModelIndex index(
        int row,
        int column,
        const QModelIndex& parent = QModelIndex()
    ) const Q_DECL_OVERRIDE;

    // returns QModelIndex() for tag label
    // return QModelIndex based on row for image
    virtual QModelIndex parent(
        const QModelIndex& index
    ) const Q_DECL_OVERRIDE;

    // returns the total count of tags (images + labels)
    virtual int rowCount(
        const QModelIndex& parent = QModelIndex()
    ) const Q_DECL_OVERRIDE;

private:
    // this tree has only one level with unique tag names
    // no need for fancy parent/child design
    // scalability: it is not expected to have
    // a lot of tags (<10), so searching through
    // list is perfectly acceptable (no overkill)
    typedef QList<TagItem> TagData;
    TagData tag_data_;

};

#endif // TAG_MODEL_H
