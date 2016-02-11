#ifndef TAG_MODEL_H
#define TAG_MODEL_H

#include <core/tag_item.h>

#include <QAbstractItemView>
#include <QHash>

class QStandardItemModel;

// TagModel has a tree model but is not one
// so that only the required metods are exposed
// Data is hold by the TagItem belonging to the
// tree model.
// Example of model:
// - label_1
// .... image1.png
// .... image2.png
// - label_2
// .... image1.png
// - label_3
// .... image_6.jpeg
class TagModel
{
public:
    // does nothing
    TagModel(
        QObject* parent = 0
    );

    // removes the data associated to the model
    virtual ~TagModel();

    // attaches the internal model to the given view
    inline void attach(
        QAbstractItemView* view
    );

    // initial import of images
    // they will be added to <UNTAGGED and <ALL>
    void import_images(
        const QFileInfoList& image_list
    );

    // adds a new tag label name in the tree
    // with the given color
    // returns false if item cannot be added
    // (e.g. the label is already listed)
    bool add_new_label(
        const QColor& color,
        const QString& label
    );

    // removes the items with the given indexes
    // if a tag label is selected, all the item images are removed
    // as well as the label
    // if removed images no longer have tag, they become available
    // in the <UNTAGGED> area
    // UNTAGGED and ALL labels cannot be removed but their image can
    void remove_items(
        const QModelIndexList& index_list
    );

    // returns the image path file corresponding to the given index
    // returns null if index does not correspond to an image item
    QString get_fullpath(
        const QModelIndex& index
    );

protected:
    // internal use: adds a new image item in the model
    // and reference the label associated with it
    void add_image_to_label(
        TagItem* label_item,
        const QFileInfo& image_file
    );

private:
    QStandardItemModel* model_;
    TagItem* untagged_item_;
    TagItem* all_item_;

    // cross-reference image path with tag name
    // to make it easier and avoid costly search in children items
    typedef QList<TagItem*> TagItemList;
    QHash<QString, TagItemList> image_path_ref_;
};


/************************* inline *************************/

void TagModel::attach(
        QAbstractItemView* view
    )
{
    view->setModel( model_ );
}

#endif // TAG_MODEL_H
