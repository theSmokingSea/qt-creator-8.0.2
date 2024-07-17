/****************************************************************************
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

#include <model.h>
#include <modelnode.h>

#include <QObject>
#include <QUrl>
#include <QQmlPropertyMap>
#include <QQmlComponent>
#include <QColor>
#include <QPoint>
#include <QMouseEvent>

namespace QmlDesigner {

class MaterialEditorContextObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl specificsUrl READ specificsUrl WRITE setSpecificsUrl NOTIFY specificsUrlChanged)
    Q_PROPERTY(QString specificQmlData READ specificQmlData WRITE setSpecificQmlData NOTIFY specificQmlDataChanged)
    Q_PROPERTY(QQmlComponent *specificQmlComponent READ specificQmlComponent NOTIFY specificQmlComponentChanged)

    Q_PROPERTY(QString stateName READ stateName WRITE setStateName NOTIFY stateNameChanged)
    Q_PROPERTY(QStringList allStateNames READ allStateNames WRITE setAllStateNames NOTIFY allStateNamesChanged)
    Q_PROPERTY(QStringList possibleTypes READ possibleTypes WRITE setPossibleTypes NOTIFY possibleTypesChanged)
    Q_PROPERTY(int possibleTypeIndex READ possibleTypeIndex NOTIFY possibleTypeIndexChanged)

    Q_PROPERTY(bool isBaseState READ isBaseState WRITE setIsBaseState NOTIFY isBaseStateChanged)
    Q_PROPERTY(bool selectionChanged READ selectionChanged WRITE setSelectionChanged NOTIFY selectionChangedChanged)

    Q_PROPERTY(int majorVersion READ majorVersion WRITE setMajorVersion NOTIFY majorVersionChanged)

    Q_PROPERTY(bool hasAliasExport READ hasAliasExport NOTIFY hasAliasExportChanged)
    Q_PROPERTY(bool hasActiveTimeline READ hasActiveTimeline NOTIFY hasActiveTimelineChanged)
    Q_PROPERTY(bool hasQuick3DImport READ hasQuick3DImport WRITE setHasQuick3DImport NOTIFY hasQuick3DImportChanged)
    Q_PROPERTY(bool hasModelSelection READ hasModelSelection WRITE setHasModelSelection NOTIFY hasModelSelectionChanged)
    Q_PROPERTY(bool hasMaterialRoot READ hasMaterialRoot WRITE setHasMaterialRoot NOTIFY hasMaterialRootChanged)

    Q_PROPERTY(QQmlPropertyMap *backendValues READ backendValues WRITE setBackendValues NOTIFY backendValuesChanged)

public:
    MaterialEditorContextObject(QQmlContext *context, QObject *parent = nullptr);

    QUrl specificsUrl() const { return m_specificsUrl; }
    QString specificQmlData() const {return m_specificQmlData; }
    QQmlComponent *specificQmlComponent();
    QString stateName() const { return m_stateName; }
    QStringList allStateNames() const { return m_allStateNames; }
    QStringList possibleTypes() const { return m_possibleTypes; }
    int possibleTypeIndex() const { return m_possibleTypeIndex; }

    bool isBaseState() const { return m_isBaseState; }
    bool selectionChanged() const { return m_selectionChanged; }

    QQmlPropertyMap *backendValues() const { return m_backendValues; }

    Q_INVOKABLE QString convertColorToString(const QVariant &color);
    Q_INVOKABLE QColor colorFromString(const QString &colorString);

    Q_INVOKABLE void changeTypeName(const QString &typeName);
    Q_INVOKABLE void insertKeyframe(const QString &propertyName);

    Q_INVOKABLE void hideCursor();
    Q_INVOKABLE void restoreCursor();
    Q_INVOKABLE void holdCursorInPlace();

    Q_INVOKABLE int devicePixelRatio();

    Q_INVOKABLE QStringList allStatesForId(const QString &id);

    Q_INVOKABLE bool isBlocked(const QString &propName) const;
    Q_INVOKABLE void goIntoComponent();

    enum ToolBarAction {
        ApplyToSelected = 0,
        ApplyToSelectedAdd,
        AddNewMaterial,
        DeleteCurrentMaterial,
        OpenMaterialBrowser
    };
    Q_ENUM(ToolBarAction)

    int majorVersion() const;
    void setMajorVersion(int majorVersion);

    bool hasActiveTimeline() const;
    void setHasActiveTimeline(bool b);

    bool hasQuick3DImport() const;
    void setHasQuick3DImport(bool b);

    bool hasMaterialRoot() const;
    void setHasMaterialRoot(bool b);

    bool hasModelSelection() const;
    void setHasModelSelection(bool b);

    bool hasAliasExport() const { return m_aliasExport; }

    void setSelectedMaterial(const ModelNode &matNode);

    void setSpecificsUrl(const QUrl &newSpecificsUrl);
    void setSpecificQmlData(const QString &newSpecificQmlData);
    void setStateName(const QString &newStateName);
    void setAllStateNames(const QStringList &allStates);
    void setPossibleTypes(const QStringList &types);
    void setCurrentType(const QString &type);
    void setIsBaseState(bool newIsBaseState);
    void setSelectionChanged(bool newSelectionChanged);
    void setBackendValues(QQmlPropertyMap *newBackendValues);
    void setModel(QmlDesigner::Model *model);

    void triggerSelectionChanged();
    void setHasAliasExport(bool hasAliasExport);

signals:
    void specificsUrlChanged();
    void specificQmlDataChanged();
    void specificQmlComponentChanged();
    void stateNameChanged();
    void allStateNamesChanged();
    void possibleTypesChanged();
    void possibleTypeIndexChanged();
    void isBaseStateChanged();
    void selectionChangedChanged();
    void backendValuesChanged();
    void majorVersionChanged();
    void hasAliasExportChanged();
    void hasActiveTimelineChanged();
    void hasQuick3DImportChanged();
    void hasMaterialRootChanged();
    void hasModelSelectionChanged();

private:
    void updatePossibleTypeIndex();

    QUrl m_specificsUrl;
    QString m_specificQmlData;
    QQmlComponent *m_specificQmlComponent = nullptr;
    QQmlContext *m_qmlContext = nullptr;

    QString m_stateName;
    QStringList m_allStateNames;
    QStringList m_possibleTypes;
    int m_possibleTypeIndex = -1;
    QString m_currentType;

    int m_majorVersion = 1;

    QQmlPropertyMap *m_backendValues = nullptr;
    Model *m_model = nullptr;

    QPoint m_lastPos;

    bool m_isBaseState = false;
    bool m_selectionChanged = false;
    bool m_aliasExport = false;
    bool m_hasActiveTimeline = false;
    bool m_hasQuick3DImport = false;
    bool m_hasMaterialRoot = false;
    bool m_hasModelSelection = false;

    ModelNode m_selectedMaterial;
};

} // QmlDesigner
