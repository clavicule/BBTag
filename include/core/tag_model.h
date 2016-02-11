#ifndef TAG_MODEL_H
#define TAG_MODEL_H

#include <core/tag_item.h>

#include <QAbstractItemView>

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

    // adds a new tag label name in the tree
    // with the given color
    // returns false if item cannot be added
    // (e.g. the label is already listed)
    bool add_new_label(
        const QColor& color,
        const QString& label
    );

    // initial import of images
    // they will be added to **untagged** and *all*
    void import_images(
        const QStringList& image_list
    );

private:
    QStandardItemModel* tag_model_;
    TagItem* untagged_item_;
    TagItem* all_item_;

};


/************************* inline *************************/

void TagModel::attach(
        QAbstractItemView* view
    )
{
    view->setModel( tag_model_ );
}

#endif // TAG_MODEL_H
