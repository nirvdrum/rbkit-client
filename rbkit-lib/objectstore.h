#ifndef OBJECTSTORE_H
#define OBJECTSTORE_H

#include <QHash>
#include <QVariant>
#include <QVariantMap>
#include <QMap>
#include <QMultiMap>

#include "objectdetail.h"
#include "objecttyperow.h"

namespace RBKit {
    class ObjectStore
    {
        class Sorter {
            const ObjectStore *objectStore;
        public:
            Sorter(const ObjectStore *_objectStore): objectStore(_objectStore) {};
            bool operator() (QString key1, QString key2) {
                return objectStore->objectTypeCount[key1] > objectStore->objectTypeCount[key2];
            }
        };

    public:
        ObjectStore();
        ObjectStore(const ObjectStore &);
        // Store mapping between object-id and detail
        QHash<quint64, RBKit::ObjectDetail*> objectStore;
        // mapping between object class and its count
        QHash<QString, quint32> objectTypeCount;
        // Mapping between object class and the ids
        QMultiMap<QString, quint64> objectTypeIdMap;

        void addObject(RBKit::ObjectDetail *objectDetails);
        void removeObject(quint64 key);
        void reset();
        void updateObjectGeneration();
        ObjectDetail *getObject(quint64 key);
        quint32 getObjectTypeCount(const QString& className);
        const quint32 liveObjectCount() const;
        const QVariantMap getObjectTypeCountMap();
        std::list<QString> sort(int critirea) const;
    };
}

Q_DECLARE_METATYPE(RBKit::ObjectStore)


#endif // OBJECTSTORE_H