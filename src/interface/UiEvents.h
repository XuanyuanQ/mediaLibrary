#pragma once

#include <QObject>
#include <QVariant>

#include "FromUi.h"

/*
 * Single entry point for messages from the UI to C++.
 *
 *   QML: UiEvents.post(FromUi.ItemClicked, mediaId)
 *
 * main.cpp connects post() once per module; each module's handleUiEvent()
 * handles only its own message range. Registered as a QML singleton.
 */
class UiEvents : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

signals:
    void post(FromUi::Message message, const QVariant &data);
};
