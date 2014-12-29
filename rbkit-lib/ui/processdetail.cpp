#include "processdetail.h"

#include <QPushButton>
#include <QLabel>
#include "../model/appstate.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <iostream>
#include <QHeaderView>

ProcessDetail::ProcessDetail()
{
    processInfoLabel = new QLabel();
    processInfoLabel->setStyleSheet("border-bottom: 2px; border-bottom-color: black");
    addWidget(processInfoLabel);
    displayProcessDetail();
    createGenerationTable();
}

ProcessDetail::~ProcessDetail()
{

}

void ProcessDetail::displayProcessDetail()
{
    if (RBKit::AppState::getInstance()->getState("process_name").isValid()) {
        processInfoLabel->setText("<b> Process Name </b> :" + RBKit::AppState::getInstance()->getState("process_name").toString() + "<br />" +
                                  "<b> Process pid </b>: " + RBKit::AppState::getInstance()->getState("pid").toString() + "<br />" +
                                  "<b> Process directory </b>: " + RBKit::AppState::getInstance()->getState("pwd").toString()
                              );
    } else {
        processInfoLabel->setText("Currently not profiling any Application");
    }
}

void ProcessDetail::createGenerationTable()
{
    QWidget *widget = new QWidget();
    horizontalTables = new QHBoxLayout();
    horizontalTables->setMargin(0);

    youngView = createTableView("Young Generation");
    youngModel = createModel(*youngView);

    fixTableDisplay(youngView);
    secondGenView = createTableView("Second Generation");
    secondGenModel = createModel(*secondGenView);
    fixTableDisplay(secondGenView);


    oldView = createTableView("Old Generation");
    oldGenModel = createModel(*oldView);
    fixTableDisplay(oldView);

    widget->setLayout(horizontalTables);

    addWidget(widget);
}

QTableView *ProcessDetail::createTableView(const QString &label)
{
    QWidget *tableViewContainer = new QWidget();

    QVBoxLayout *viewLayout = new QVBoxLayout();
    viewLayout->setMargin(0);
    tableViewContainer->setLayout(viewLayout);

    QLabel *gcLabel = new QLabel(label);
    viewLayout->addWidget(gcLabel);
    QTableView *tableView = new QTableView();
    viewLayout->addWidget(tableView);
    horizontalTables->addWidget(tableViewContainer);
    return tableView;
}

void ProcessDetail::receiveGenerationStats(int genType, QVariantMap map)
{
    switch (genType) {
    case 0:
        updateModel(youngModel, map);
        break;
    case 1:
        updateModel(secondGenModel, map);
        break;
    case 2:
        updateModel(oldGenModel, map);
        break;
    default:
        break;
    }
}

void ProcessDetail::createTableHeader(QStandardItemModel &model)
{
    model.setHorizontalHeaderItem(0, new QStandardItem(QString("Class")));
    model.setHorizontalHeaderItem(1, new QStandardItem(QString("Count")));
}

QStandardItemModel *ProcessDetail::createModel(QTableView &tableView)
{
    QStandardItemModel *model = new QStandardItemModel(this);
    createTableHeader(*model);

    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    tableView.setModel(proxyModel);
    return model;

}

void ProcessDetail::updateModel(QStandardItemModel *model, QVariantMap &map)
{
        model->removeRows(0, model->rowCount());
        int counter = 0;
        for(auto row : map.toStdMap() ) {
            QStandardItem *className = new QStandardItem(row.first);
            model->setItem(counter, 0, className);

            QStandardItem *count = new QStandardItem(QString("%0").arg(row.second.toUInt()));
            model->setItem(counter, 1, count);
            counter += 1;
        }
}

void ProcessDetail::fixTableDisplay(QTableView *view)
{
    view->setSortingEnabled(true);
    view->setAlternatingRowColors(true);
    view->sortByColumn(1, Qt::DescendingOrder);
    view->setColumnWidth(0, 240);
    view->setColumnWidth(1, 80);
    view->horizontalHeader()->setStretchLastSection(true);
    view->verticalHeader()->setVisible(false);
}
