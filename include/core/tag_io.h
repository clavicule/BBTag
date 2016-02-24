#ifndef TAG_IO_H
#define TAG_IO_H

#include <core/tag_item.h>

#include <QIODevice>
#include <QDir>

// class for writing/reading XML files
// containing tag information
class TagIO
{
public:
    static const QString DATASET;
    static const QString NAME;
    static const QString COMMENT;
    static const QString TAGS;
    static const QString SINGLE_TAG;
    static const QString LABEL;
    static const QString COLOR;
    static const QString IMAGES;
    static const QString SINGLE_IMAGE;
    static const QString PATH;
    static const QString BOX;
    static const QString TOP;
    static const QString LEFT;
    static const QString WIDTH;
    static const QString HEIGHT;

public:
    // write to XML file the given elements
    // label colors (optional) are provided by tag_color_dict
    static void write_xml(
        QIODevice* out,
        const QString& relative_dir,
        const QHash<QString, QColor>& tag_color_dict,
        const QHash< QString, QList<TagItem::Elements> >& elts
    );

    // read XML file
    // if no label colors were provided, colors are chosen randomly
    static bool read_xml(
        QIODevice* in,
        const QString& relative_dir,
        QHash< QString, QList<TagItem::Elements> >& elts
    );

    static void write_images(
        const QDir& output_dir,
        const QHash< QString, QList<TagItem::Elements> >& elts
    );

};


#endif // TAG_IO_H
