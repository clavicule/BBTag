#ifndef TAG_ITEM_H
#define TAG_ITEM_H

#include <QList>
#include <QRect>
#include <QColor>
#include <QStandardItem>
#include <QFileInfo>

// A TagItem is tied to a tag label+color and has:
// a list of bounding box corresponding for each image
class TagItem : public QObject, public QStandardItem
{
    Q_OBJECT

public:
    // structure provided for convenience
    struct Elements {
        QColor _color;
        QString _label;
        QString _fullpath;
        QList<QRect> _bbox;
    };

public:
    // use this constructor to make it a tag label
    // (no image reference)
    TagItem(
        const QColor& tag_color,
        const QString& tag_label
    );

    // use this constructor to make it a image item
    // image_file must be a valid path
    // add itself as children of tag_item
    TagItem(
        TagItem* tag_item,
        const QString& image_file
    );

    // copy internal data and add itself as sibling
    TagItem(
        const TagItem& tag_item
    );

    // copy internal data and add itself as sibling
    TagItem& operator = (const TagItem& tag_item );

    virtual ~TagItem();

    // Returns:
    // - tag color for the decoration role if tag label
    // - tag name for the display role and tooltip role if tag label
    // - image name for the display role if image item
    // - image full path for tooltip role if image item
    // - nothing for all other roles
    virtual QVariant data(
        int role = Qt::UserRole + 1
    ) const Q_DECL_OVERRIDE;

    // adds a bounding box rectangle corresponding to a tag
    inline void add_tag(
        const QRect& bbox
    );

    inline void remove_tag(
        const QRect& bbox
    );

    // returns the filename with extension
    inline QString filename() const;

    // returns the full file path
    inline const QString& fullpath() const;

    // returns the tag label name
    inline const QString& label() const;

    // returns the tag color
    inline const QColor& color() const;

    // sets the tag label name
    inline void set_label(
        const QString& name
    );

    // sets the tag color
    inline void set_color(
        const QColor& color
    );

    // returns the list of tags
    inline const QList<QRect>& tags() const;

    // returns all the item parameters
    // under one structured element
    // for convenience
    inline Elements elements() const;

private:
    QColor tag_color_;
    QString tag_label_;
    QString fullpath_;
    QList<QRect> bbox_;
};


/************************* inline *************************/

void TagItem::add_tag(
        const QRect& bbox
    )
{
    bbox_.append( bbox );
}

void TagItem::remove_tag(
        const QRect& bbox
    )
{
    bbox_.removeOne( bbox );
}

QString TagItem::filename() const
{
    return QFileInfo( fullpath_ ).fileName();
}

const QString& TagItem::fullpath() const
{
    return fullpath_;
}

const QString& TagItem::label() const
{
    return tag_label_;
}

const QColor& TagItem::color() const
{
    return tag_color_;
}

void TagItem::set_label(
        const QString& name
    )
{
    if( !name.isEmpty() ) {
        tag_label_ = name;
    }
}

void TagItem::set_color(
        const QColor& color
    )
{
    if( color.isValid() ) {
        tag_color_ = color;
    }
}

const QList<QRect>& TagItem::tags() const
{
    return bbox_;
}

TagItem::Elements TagItem::elements() const
{
    Elements elt;
    elt._color = tag_color_;
    elt._label = tag_label_;
    elt._fullpath = fullpath_;
    elt._bbox = bbox_;

    return elt;
}

#endif // TAG_ITEM_H
