#ifndef TAG_IO_H
#define TAG_IO_H

#include <core/tag_item.h>

#include <QIODevice>

// class for writing/reading XML files
// containing tag information
class TagIO
{
public:
    static void write(
        QIODevice* out,
        const QHash< QString, QList<TagItem::Elements> >& elts
    );

    static void read(
        QIODevice* in
    );

};


#endif // TAG_IO_H
