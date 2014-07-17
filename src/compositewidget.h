#ifndef COMPOSITEWIDGET_H
#define COMPOSITEWIDGET_H

#include "projectmodel.h"

#include <QMdiSubWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>

class CompositeWidget;
class CompositeView: public QGraphicsView {
    Q_OBJECT
    friend class PartWidget;
public:
    CompositeView(CompositeWidget* parent, QGraphicsScene* scene);
protected:
    // Forwards these to parent
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    void wheelEvent(QWheelEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void keyPressEvent(QKeyEvent *event);

protected:
    CompositeWidget* cw;
};

class CompositeWidget : public QMdiSubWindow
{
    Q_OBJECT
    friend class CompositeView;
public:
    explicit CompositeWidget(const QString& compName, QWidget *parent = 0);
    void updateCompFrames(); // loads the comp frames etc
    void updateCompFramesMinorChanges();

    void updatePropertiesOverlays(); // update overlays
    void updateFrame(); // updates the visible parts
    void updateChildRecursively(int index, const QPointF& parentPivotPos);

    // part updates..
    void partNameChanged(const QString& oldPartName, const QString& newPartName);
    void partFrameUpdated(const QString& part, const QString& mode, int frame);
    void partFramesUpdated(const QString& part, const QString& mode);
    void partNumPivotsUpdated(const QString& part, const QString& mode);

    // TODO: comp updates
    void compNameChanged(const QString& name);
    void compPropertiesChanged(const QString& name);
    // void compChanged(...) -> updateCompFrames()

    void setChildMode(const QString& child, const QString& mode);
    void setChildLoop(const QString& child, bool loop);
    void setChildVisible(const QString& child, bool loop);
    void setPlaybackSpeedMultiplier(int index, float value);

    void updateDropShadow();

    // query
    QString compName() const {return mCompName;}
    int zoom() const {return (int)mZoom;}

    QString modeForCurrentSet(const QString& child) const;
    bool loopForCurrentSet(const QString& child) const;
    bool visibleForCurrentSet(const QString& child) const;

    bool isPlaying() const {return mIsPlaying;}
    int playbackSpeedMultiplierIndex() const {return mPlaybackSpeedMultiplierIndex;}

    QString properties() const {return mProperties;}

signals:
    void zoomChanged();
    void closed(CompositeWidget*);
    void playActivated(bool);

public slots:
    void setZoom(int);
    // void setPenSize(int);
    // void setPenColour(QColor);
    // void fitToWindow();
    // void updateDropShadow();
    //void updateOnionSkinning();
    //void updatePivots();
    void setPosition(QPointF);

    void play(bool);
    void updateAnimation();

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *resizeEvent);

    void compViewMousePressEvent(QMouseEvent *mouseEvent);
    void compViewMouseMoveEvent(QMouseEvent *mouseEvent);
    void compViewMouseReleaseEvent(QMouseEvent *mouseEvent);
    void compViewWheelEvent(QWheelEvent *event);
    void compViewKeyPressEvent(QKeyEvent *event);




protected:
    QString mCompName;
    Composite* mComp;
    CompositeView* mCompView;
    QString mProperties;

    float mZoom;
    QPointF mPosition;
    QTimer* mAnimationTimer;
    double mSecondsPassedSinceLastFrame; // TODO: change to seconds since frame?
    bool mIsPlaying;
    QRectF mBoundsRect;

    struct ChildDriver {
        bool valid; // Is this child valid?

        // Part stuff
        QString part;
        struct Mode {
            int width, height;
            int numFrames;
            int numPivots;
            QVector<QPoint> anchors;
            QVector<QPoint> pivots[MAX_PIVOTS];
            QVector<QGraphicsPixmapItem*> pixmapItems;
            int fps;
            float spf;
            QGraphicsRectItem* boundsItem;
        };
        QMap<QString, Mode> modes;
        QString mode;
        bool loop;
        bool visible;
        int frame;
        float accumulator; // seconds since last frame

        // children data
        int index; // in children
        int parent; // index of parent
        int parentPivot; // pivot index connected to
        int z;
        QList<int> children; // list of children (indices)

        // derived data..
        bool updated;
        QPointF parentPivotOffset;

        // TODO: Also show anchors and pivots as QGraphicsSimpleTextItem* or just dots
    };

    QMap<QString, ChildDriver> mChildrenMap;
    QList<QString> mChildren;
    int mRoot;

    int mPlaybackSpeedMultiplierIndex;
    float mPlaybackSpeedMultiplier;

    QPoint mLastPoint;
    QPoint mMousePos;
    QPointF mLastCanvasPosition;
    bool mMovingCanvas;

    float mOnionSkinningTransparency;
    bool mOnionSkinningEnabled;
    bool mOnionSkinningEnabledDuringPlayback;

    QColor mPivotColour;
    bool mPivotsEnabled;
    bool mPivotsEnabledDuringPlayback;

    QVector<QGraphicsRectItem*> mRectItems;
};

#endif // COMPOSITEWIDGET_H
