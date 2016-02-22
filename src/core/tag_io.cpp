#include <core/tag_io.h>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTextCodec>


const QString TagIO::DATASET = "dataset";
const QString TagIO::NAME = "name";
const QString TagIO::COMMENT = "comment";


void TagIO::write(
        QIODevice* out,
        const QHash< QString, QList<TagItem::Elements> >& elts
    )
{
    if( !out ) {
        return;
    }

    QXmlStreamWriter xml;
    xml.setAutoFormatting( true );
    xml.setDevice( out );

    xml.writeStartDocument();
    xml.writeStartElement( DATASET );
    xml.writeTextElement( NAME, "dataset containing bounding box labels on images" );
    xml.writeTextElement( COMMENT, "created by BBTag" );
    xml.writeStartElement( "images" );

    for( QHash< QString, QList<TagItem::Elements> >::const_iterator img_itr = elts.begin(); img_itr != elts.end(); ++img_itr ) {
        QString fullpath = img_itr.key();
        const QList<TagItem::Elements>& tags = img_itr.value();

        xml.writeStartElement( "image" );
        xml.writeAttribute( "path", fullpath );

        for( QList<TagItem::Elements>::const_iterator tag_itr = tags.begin(); tag_itr != tags.end(); ++tag_itr ) {
            const TagItem::Elements& elt = *tag_itr;
            const QList<QRect>& bboxes = elt._bbox;

            for( QList<QRect>::const_iterator bbox_itr = bboxes.begin(); bbox_itr != bboxes.end(); ++bbox_itr ) {
                const QRect& bbox = *bbox_itr;

                xml.writeStartElement( "box" );
                xml.writeAttribute( "top", QString::number( bbox.top() ) );
                xml.writeAttribute( "left", QString::number( bbox.left() ) );
                xml.writeAttribute( "width", QString::number( bbox.width() ) );
                xml.writeAttribute( "height", QString::number( bbox.height() ) );
                xml.writeTextElement( "label", elt._label );
                xml.writeEndElement();
            }
        }

        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();
}

bool TagIO::read(
        QIODevice* in,
        QHash< QString, QList<TagItem::Elements> >& elts
    )
{
    if( !in ) {
        return false;
    }

    QXmlStreamReader xml;
    xml.setDevice( in );

    if( xml.readNext() == QXmlStreamReader::StartDocument && !xml.isStartDocument() ) {
        return false;
    }

    if( !xml.readNextStartElement() ) {
        return false;
    }

    if( xml.name() != DATASET ) {
        return false;
    }

    // skip info for user
    while( xml.readNextStartElement() && xml.name() != "images" ) {
        xml.skipCurrentElement();
    }
    xml.readNextStartElement();

    // within the "images" element
    while( !xml.atEnd() ) {
        if( xml.name() == "image" ) {
            QString fullpath = xml.attributes().value( "path" ).toString();
            if( fullpath.isEmpty() ) {
                xml.skipCurrentElement();
                continue;
            }

            if( !xml.readNextStartElement() ) {
                xml.skipCurrentElement();
                continue;
            }

            while( xml.name() == "box" ) {
                QXmlStreamAttributes att =  xml.attributes();
                QStringRef top = att.value( "top" );
                QStringRef left = att.value( "left" );
                QStringRef width = att.value( "width" );
                QStringRef height = att.value( "height" );

                bool skip = ( top.isEmpty() || left.isEmpty() || width.isEmpty() || height.isEmpty() );
                xml.readNextStartElement();
                skip = skip || ( xml.name() != "label" );
                QString label = xml.readElementText();
                skip = skip || ( label.isEmpty() );

                if( skip ) {
                    xml.skipCurrentElement();
                    xml.readNextStartElement();
                    continue;
                }

                TagItem::Elements elt;
                elt._fullpath = fullpath;
                elt._label = label;
                elt._bbox.append( QRect( left.toInt(), top.toInt(), width.toInt(), height.toInt() ) );
                elts[ fullpath ].append( elt );

                xml.readNextStartElement();
                while( xml.isEndElement() ) {
                    xml.readNextStartElement();
                }
            }
        }
    }

    return true;
}
