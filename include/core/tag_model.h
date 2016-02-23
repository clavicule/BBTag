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
    static QString ALL;
    static QString UNTAGGED;

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

    // returns the fullpath corresponding to the given index
    QString get_fullpath(
        const QModelIndex& index
    ) const;

    // returns the label corresponding to the given index
    QString get_label(
        const QModelIndex& index
    ) const;

    // returns the color corresponding to the given index
    QColor get_color(
        const QModelIndex& index
    ) const;

    // sets the label to the given index
    // and all its children will get
    // that color too
    void set_label(
        const QModelIndex& index,
        const QString& name
    );

    // sets the color to the given index
    // and all its children will get
    // that color too
    void set_color(
        const QModelIndex& index,
        const QColor& color
    );

    // get the element corresponding to the given image and label
    // there can be at most one element
    TagItem::Elements get_element(
        const QString& fullpath,
        const QString& label
    );

    // returns the elements corresponding to the given index if not item under ALL
    // returns one element per label if item belongs to ALL and not a specific label
    // returns an empty list if index does not correspond to a valid index
    QList<TagItem::Elements> get_elements(
        const QModelIndexList& index_list
    ) const;

    // returns all the data as unique table of image file associated to tag item
    QHash< QString, QList<TagItem::Elements> > get_all_elements() const;

    // clears the current model and reinitializes it from the given elements
    void init_from_elements(
        const QHash< QString, QList<TagItem::Elements> >& elts
    );

    // returns the list of tags with their elements
    // excluding UNTAGGED and ALL
    QHash<QString, QColor> get_all_tags() const;

    // adds a label tag with the given bounding box to the given image
    QModelIndex add_tag_to_label(
        const QString& fullpath,
        const QString& label,
        const QRect& tag
    );

    // removes the tag corresponding to the given bounding box and label
    QModelIndex remove_tag_from_label(
        const QString& fullpath,
        const QString& label,
        const QRect& tag
    );

protected:
    // internal use: adds a new image item in the model
    // and reference the label associated with it
    TagItem* add_image_to_label(
        TagItem* label_item,
        const QFileInfo& image_file
    );

    // get the item associated to the given tag and fullpath
    // returns null if no match
    TagItem* get_tag_item(
        const QString& fullpath,
        const QString& label
    );

    // clears the model (removes all item)
    // and reinitializes it (ALL, UNTAGGED, etc.)
    void init();

private:
    QStandardItemModel* model_;
    TagItem* untagged_item_;
    TagItem* all_item_;

    // cross-reference:
    // - image path with tag name
    // - image path with all existing image item
    // to make it easier and avoid "costly" search in children items
    typedef QList<TagItem*> TagItemList;
    QHash<QString, TagItemList> image_label_ref_;
    QHash<QString, TagItemList> image_image_ref_;
};


/************************* inline *************************/

void TagModel::attach(
        QAbstractItemView* view
    )
{
    view->setModel( model_ );
}

#endif // TAG_MODEL_H
