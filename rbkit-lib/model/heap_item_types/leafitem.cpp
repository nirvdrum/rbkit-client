#include "leafitem.h"
#include <QSqlQuery>
#include <QDebug>
#include <QFileInfo>
#include "stringutil.h"
#include "heapitem.h"

namespace RBKit {

LeafItem::LeafItem(const QString _className, quint32 _count, quint32 _referenceCount,
                   quint32 _totalSize, const QString _filename, int _snapShotVersion)
    : BaseHeapItem(_className, _count, _referenceCount, _totalSize, _snapShotVersion)
{
    filename = _filename;
}

QVariant LeafItem::getClassOrFile() const
{
    if (filename.isEmpty())
        return QVariant(QString("<compiled>"));
    else
        return QVariant(filename);
}


// returns references for given object
BaseHeapItem *LeafItem::getSelectedReferences()
{
    QString queryString;
    QString viewName = QString("view_").append(RBKit::StringUtil::randomSHA());
    queryString = QString("create view %0 AS select * from %1 where %1.id in "
                          "(select %2.child_id from %2 "
                          " INNER JOIN %1 ON %1.id = %2.object_id "
                          " where %1.class_name = '%3' and %1.file='%4')").
            arg(viewName).arg(objectsTableName).
            arg(referenceTableName).arg(className).arg(filename);

    QSqlQuery query;

    if (!query.exec(queryString)) {
        qDebug() << "Error creating view with";
        qDebug() << query.lastError();
    }

    HeapItem *rootItem = new HeapItem(-1);
    rootItem->setObjectsTableName(viewName);
    rootItem->setIsSnapshot(false);
    rootItem->findImmediateChildren();
    rootItem->childrenFetched = true;
    rootItem->computePercentage();
    return rootItem;
}

QString LeafItem::shortLeadingIdentifier()
{
    if (filename.isEmpty()) {
        return className;
    } else {
        QString shortFileName = QFileInfo(filename).fileName();
        return QString("%0 - %1").arg(className).arg(shortFileName);
    }
}

} // namespace RBKit
