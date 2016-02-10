#ifndef TAG_ITEM_H
#define TAG_ITEM_H

#include <QList>
#include <QHash>
#include <QRect>
#include <QColor>

// A TagItem is tied to a tag label+color and has:
// a list of bounding box corresponding for each image
class TagItem
{
public:
    TagItem(
        const QString& tag_label,
        const QColor& tag_color
    );

    TagItem(
        const TagItem& tag_item
    );

    TagItem& operator = (const TagItem& tag_item );

    // returns true if tag labels are the same
    inline bool operator == (const TagItem& tag_item );

    virtual ~TagItem();

    // adds a bounding box rectangle for the given image
    void add_tag(
        const QString& image_path,
        const QRect& bbox
    );

    // returns the filename with extension at the given index
    QString filename( int i ) const;

    // returns the full file path at the given index
    QString fullpath( int i ) const;

    // returns the tag label name
    inline const QString& label() const;

    // returns the tag color
    inline const QColor& color() const;

    // returns the number of image/bbox associations
    inline int tagCount() const;

private:
    QString tag_label_;
    QColor tag_color_;

    QList<QString> index_;
    QHash< QString, QList<QRect> > bbox_;
};


/************************* inline *************************/

bool TagItem::operator == (const TagItem& tag_item )
{
    return ( tag_label_ == tag_item.tag_label_ );
}

const QString& TagItem::label() const
{
    return tag_label_;
}

const QColor& TagItem::color() const
{
    return tag_color_;
}

int TagItem::tagCount() const
{
    return index_.count();
}

#endif // TAG_ITEM_H
