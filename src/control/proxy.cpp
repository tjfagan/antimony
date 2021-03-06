#include <Python.h>

#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include "control/proxy.h"
#include "control/control.h"
#include "ui/viewport/viewport.h"

ControlProxy::ControlProxy(Control* control, Viewport* viewport,
                           QGraphicsItem* parent)
    : QGraphicsObject(parent), control(control),
      viewport(viewport), hover(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

    connect(control, &Control::destroyed,
            this, &ControlProxy::deleteLater);
    connect(viewport, &Viewport::destroyed,
            this, &ControlProxy::deleteLater);
    connect(control, &Control::redraw,
            this, &ControlProxy::redraw);
}

void ControlProxy::redraw()
{
    prepareGeometryChange();
}

QRectF ControlProxy::boundingRect() const
{
    return (control && control->getNode())
        ? control->bounds(getMatrix(), viewport->getTransformMatrix())
        : QRectF();
}

QPainterPath ControlProxy::shape() const
{
    return (control && control->getNode())
        ? control->shape(getMatrix(), viewport->getTransformMatrix())
        : QPainterPath();
}

void ControlProxy::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (control && control->getNode())
        control->paint(getMatrix(), viewport->getTransformMatrix(),
                       isSelected() || hover, painter);
}

void ControlProxy::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (!hover)
    {
        hover = true;
        update();
    }
    emit(control->glowChanged(control->getNode(), true));
}

void ControlProxy::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (hover)
    {
        hover = false;
        update();
    }
    emit(control->glowChanged(control->getNode(), false));
}

void ControlProxy::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    click_pos = event->pos();
    control->beginDrag();

    if (event->button() != Qt::LeftButton || !control->onClick())
        QGraphicsObject::mousePressEvent(event);
}

void ControlProxy::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    if (scene()->mouseGrabberItem() == this)
        ungrabMouse();
    control->endDrag();
}

void ControlProxy::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseMoveEvent(event);

    QMatrix4x4 mi = getMatrix().inverted();
    QVector3D p0 = mi * QVector3D(click_pos);
    QVector3D p1 = mi * QVector3D(event->pos());

    control->drag(p1, p1 - p0);
    click_pos = event->pos();
}

void ControlProxy::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete ||
        event->key() == Qt::Key_Backspace)
        control->deleteNode();
    else
        event->ignore();
}

QMatrix4x4 ControlProxy::getMatrix() const
{
    return viewport->getMatrix();
}


Node* ControlProxy::getNode() const
{
    return control ? control->getNode() : NULL;
}

Control* ControlProxy::getControl() const
{
    return control;
}
