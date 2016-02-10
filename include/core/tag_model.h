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

    // attach the internal model to the given view
    inline void attach( QAbstractItemView* view );

private:
    QStandardItemModel* tag_model_;

};


/************************* inline *************************/

void TagModel::attach(
        QAbstractItemView* view
    )
{
    view->setModel( tag_model_ );
}

#endif // TAG_MODEL_H
