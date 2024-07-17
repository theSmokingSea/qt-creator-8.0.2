﻿/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include <QAbstractListModel>
#include <QPointer>

namespace QmlDesigner {
namespace Experimental {

class StatesEditorView;

class StatesEditorModel : public QAbstractListModel
{
    Q_OBJECT

    enum {
        StateNameRole = Qt::DisplayRole,
        StateImageSourceRole = Qt::UserRole,
        InternalNodeId,
        HasWhenCondition,
        WhenConditionString,
        IsDefault,
        ModelHasDefaultState,
        HasExtend,
        ExtendString
    };

public:
    StatesEditorModel(StatesEditorView *view);

    Q_INVOKABLE int count() const;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void insertState(int stateIndex);
    void removeState(int stateIndex);
    void updateState(int beginIndex, int endIndex);
    Q_INVOKABLE void renameState(int internalNodeId, const QString &newName);
    Q_INVOKABLE void setWhenCondition(int internalNodeId, const QString &condition);
    Q_INVOKABLE void resetWhenCondition(int internalNodeId);
    Q_INVOKABLE QStringList autoComplete(const QString &text, int pos, bool explicitComplete);
    Q_INVOKABLE QVariant stateModelNode(int internalNodeId);

    Q_INVOKABLE void setStateAsDefault(int internalNodeId);
    Q_INVOKABLE void resetDefaultState();
    Q_INVOKABLE bool hasDefaultState() const;
    Q_INVOKABLE void setAnnotation(int internalNodeId);
    Q_INVOKABLE void removeAnnotation(int internalNodeId);
    Q_INVOKABLE bool hasAnnotation(int internalNodeId) const;

    QStringList stateGroups() const;
    QString activeStateGroup() const;
    void setActiveStateGroup(const QString &name);
    int activeStateGroupIndex() const;
    void setActiveStateGroupIndex(int index);

    Q_INVOKABLE bool renameActiveStateGroup(const QString &name);

    Q_INVOKABLE void addStateGroup(const QString &name);
    Q_INVOKABLE void removeStateGroup();

    Q_INVOKABLE QVariantMap get(int idx) const;

    QVariantMap baseState() const;
    bool hasExtend() const;
    QStringList extendedStates() const;

    Q_PROPERTY(QVariantMap baseState READ baseState NOTIFY baseStateChanged)
    Q_PROPERTY(QStringList extendedStates READ extendedStates NOTIFY extendedStatesChanged)

    Q_PROPERTY(bool hasExtend READ hasExtend NOTIFY hasExtendChanged)

    Q_PROPERTY(QString activeStateGroup READ activeStateGroup WRITE setActiveStateGroup NOTIFY
                   activeStateGroupChanged)
    Q_PROPERTY(int activeStateGroupIndex READ activeStateGroupIndex WRITE setActiveStateGroupIndex
                   NOTIFY activeStateGroupIndexChanged)
    Q_PROPERTY(QStringList stateGroups READ stateGroups NOTIFY stateGroupsChanged)

    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE void drop(int from, int to);

    void reset();
    void evaluateExtend();

signals:
    void changedToState(int n);
    void baseStateChanged();
    void hasExtendChanged();
    void extendedStatesChanged();
    void activeStateGroupChanged();
    void activeStateGroupIndexChanged();
    void stateGroupsChanged();

private:
    QPointer<StatesEditorView> m_statesEditorView;
    bool m_hasExtend;
    QStringList m_extendedStates;
};

} // namespace Experimental
} // namespace QmlDesigner
