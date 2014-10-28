#ifndef RBKIT_EVENTS_H
#define RBKIT_EVENTS_H

#include <QVariantMap>
#include <QSharedPointer>
#include <QVariantList>
#include <QDateTime>
#include <QString>
#include "stringutil.h"
#include "model/objectdetail.h"
#include <msgpack.hpp>


class Subscriber;               // this acts as processor also atm.

namespace RBKit
{
    class EventDataBase
    {
    public:
        EventDataBase(QDateTime ts, QString eventName);
        virtual void process(Subscriber& processor) const = 0;
        virtual ~EventDataBase() {}

        QDateTime timestamp;
        QString eventName;
    };

    // event pointer.
    typedef QSharedPointer<EventDataBase> EventPtr;

    class EvtNewObject : public EventDataBase
    {
    public:
        EvtNewObject(QDateTime ts, QString eventName, RBKit::ObjectDetailPtr object);
        void process(Subscriber& processor) const;

        RBKit::ObjectDetailPtr object;
    };

    class EvtDelObject : public EventDataBase
    {
    public:
        EvtDelObject(QDateTime ts, QString eventName, QVariantMap payload);
        void process(Subscriber& processor) const;

        quint64 objectId;
    };

    class EvtGcStats : public EventDataBase
    {
    public:
        EvtGcStats(QDateTime ts, QString eventName, QVariantMap payload);
        void process(Subscriber& processor) const;

        QVariantMap payload;
    };

    class EvtGcStart : public EventDataBase
    {
    public:
        EvtGcStart(QDateTime ts, QString eventName);
        void process(Subscriber &processor) const;
    };

    class EvtGcStop : public EventDataBase
    {
    public:
        EvtGcStop(QDateTime ts, QString eventName);
        void process(Subscriber &processor) const;
    };

    class EvtObjectDump : public EventDataBase
    {
    public:
        EvtObjectDump(QDateTime ts, QString eventName, msgpack::unpacked heapDump);
        void process(Subscriber& processor) const;

        msgpack::unpacked dump;
    };

    class EvtCollection : public EventDataBase
    {
    public:
        EvtCollection(QDateTime ts, QString eventName, QList<RBKit::EventPtr>);
        void process(Subscriber& process) const;

        QList<RBKit::EventPtr> events;
    };

    class EventParser
    {
    public:
        EventParser(msgpack::unpacked& unpacked_)
            : unpacked(unpacked_)
        {}

    public:
        QString guessEvent(msgpack::object&);
        RBKit::EventDataBase* makeEventFromObject(msgpack::object&);
        RBKit::EventDataBase* makeEventFromUnpacked(msgpack::unpacked&);

    private:
        msgpack::unpacked& unpacked;
    };

    EventDataBase* parseEvent(const QByteArray& rawMessage);
    EventDataBase* makeEventFromQVariantMap(const QVariantMap& map);
}


#endif // RBKIT_EVENTS_H
