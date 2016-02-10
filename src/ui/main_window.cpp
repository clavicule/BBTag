#include <ui/main_window.h>
#include <ui/tag_viewer.h>
#include <ui/tag_scroll_view.h>
#include <core/tag_model.h>

#include <QLayout>
#include <QWidget>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSplitter>
#include <QPushButton>
#include <QScrollArea>

MainWindow::MainWindow(
        QWidget *parent
    ) : QMainWindow( parent )
{
    // splitter is the central widget of the main window
    // it is made of 3 widgets:
    // - directory parser
    // - image tag list
    // - image viewer
    QSplitter* splitter = new QSplitter( this );
    splitter->setOrientation( Qt::Horizontal );


    // --------
    // widget #1
    // get the path to the working directory
    QString root_path = QDir::rootPath();

    // set the tree view to follow a directory parser model
    QFileSystemModel* dir_model = new QFileSystemModel( this );
    dir_model->setRootPath( root_path );

    // left-most widget in splitter is a directory parser
    QTreeView* dir_view = new QTreeView( splitter );
    dir_view->setModel( dir_model );
    dir_view->setSelectionBehavior( QAbstractItemView::SelectRows );
    dir_view->setSelectionMode( QAbstractItemView::ExtendedSelection );

    // initialize directory tree view
    const QModelIndex root_index = dir_model->index( root_path );
    if( root_index.isValid() ) {
        dir_view->setRootIndex( root_index );
    }


    // --------
    // widget #2
    // middle widget in splitter is image tag tree
    QWidget* image_tag_widget = new QWidget( splitter );
    QHBoxLayout* image_tag_layout = new QHBoxLayout();

    // setup add/remove buttons to add/remove image files from the list of images to tag
    // Icons unmodified under CreativeCommon Attribution-NonCommercial 3.0 Unported license
    // http://creativecommons.org/licenses/by-nc/3.0/
    // From Snip Master --> http://www.snipicons.com/
    QPushButton* add_button = new QPushButton( QIcon( ":/pixmaps/add.png" ), "", image_tag_widget );
    QPushButton* remove_button = new QPushButton( QIcon( ":/pixmaps/remove.png" ), "", image_tag_widget );
    add_button->setFixedSize( 32, 32 );
    remove_button->setFixedSize( 32, 32 );
    add_button->setIconSize( QSize( 32, 32 ) );
    remove_button->setIconSize( QSize( 32, 32 ) );

    QVBoxLayout* image_tag_button_layout = new QVBoxLayout();
    image_tag_button_layout->addWidget( add_button );
    image_tag_button_layout->addWidget( remove_button );

    tag_model_ = new TagModel( this );
    QTreeView* tag_tree_view = new QTreeView( image_tag_widget );
    tag_tree_view->setModel( tag_model_ );

    image_tag_layout->addLayout( image_tag_button_layout );
    image_tag_layout->addWidget( tag_tree_view );
    image_tag_widget->setLayout( image_tag_layout );


    // --------
    // widget #3
    // right-most widget in
    // on OSX: don't mind the debug messages when mouse hover scrollbar:
    // bug report as of Feb 6th 2016.
    // --> https://bugreports.qt.io/browse/QTBUG-49899
    tag_viewer_ = new TagViewer( splitter );
    tag_scroll_view_ = new TagScrollView( splitter );
    tag_scroll_view_->setWidget( tag_viewer_ );

    // --------
    // Main Widget
    // arrange widgets inside the splitter widget
    splitter->addWidget( dir_view );
    splitter->addWidget( image_tag_widget );
    splitter->addWidget( tag_scroll_view_ );
    splitter->setStretchFactor( 0, 1 );
    splitter->setStretchFactor( 1, 1 );
    splitter->setStretchFactor( 2, 3 );

    QList<int> sizes;
    sizes << 100 << 100 << 300;
    splitter->setSizes( sizes );

    setCentralWidget( splitter );
    resize( 800, 500 );
}

MainWindow::~MainWindow()
{

}
