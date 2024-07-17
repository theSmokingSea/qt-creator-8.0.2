/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
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

#include "../common/themeselector.h"

#include <QApplication>
#include <QFormLayout>
#include <QLayout>
#include <QMessageBox>
#include <QSettings>

#include <utils/infolabel.h>

#include <coreplugin/manhattanstyle.h>

using namespace Utils;

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif // Qt < 6

    QApplication app(argc, argv);

    auto widget = new QWidget;
    auto mainLayout = new QVBoxLayout(widget);
    mainLayout->addWidget(new ManualTest::ThemeSelector);
    auto variationsLayout = new QGridLayout;
    mainLayout->addLayout(variationsLayout);

    const static struct {
        const InfoLabel::InfoType type;
        const char *text;
        const char *tooltip;
        const char *tooltipSeparator;
    } labels[] = {
        {InfoLabel::Information, "Information", "This is an informative Tooltip for you", "\n\n"},
        {InfoLabel::Warning, "Warning", "This is a warning Tooltip for you", " - "},
        {InfoLabel::Error, "Error", "This is an erroneous Tooltip for you", " | "},
        {InfoLabel::Ok, "Ok", "This is an ok Tooltip for you", " :) "},
        {InfoLabel::NotOk, "NotOk", "This Tooltip is just not ok", ""},
        {InfoLabel::None, "None", "", "----"},
    };

    int gridRow = 0;
    for (auto filled : {false, true}) {
        for (auto label : labels) {
            for (auto enabled : {true, false}) {
                auto infoLabel = new Utils::InfoLabel(
                            label.text + QLatin1String(filled ? " (filled)" : "")
                            + QLatin1String(enabled ? "" : " (disabled)"), label.type);
                infoLabel->setEnabled(enabled);
                infoLabel->setFilled(filled);
                infoLabel->setAdditionalToolTip(label.tooltip);
                infoLabel->setAdditionalToolTipSeparator(label.tooltipSeparator);
                variationsLayout->addWidget(infoLabel, gridRow, enabled ? 0 : 1);
            }
            gridRow++;
        }
        variationsLayout->addItem(new QSpacerItem(0, 10), gridRow++, 0);
    }

    auto withLink = new Utils::InfoLabel("With <a href=\"link clicked!\">link</a>", InfoLabel::Error);
    withLink->setElideMode(Qt::ElideNone);
    QObject::connect(withLink, &QLabel::linkActivated, [widget](const QString& link){
        QMessageBox::information(widget, {}, link);
    });
    mainLayout->addWidget(withLink);

    auto stretching = new Utils::InfoLabel("Stretching and centering vertically", InfoLabel::Warning);
    stretching->setFilled(true);
    mainLayout->addWidget(stretching, 2);

    auto formLayout = new QFormLayout;
    auto multiLine = new Utils::InfoLabel("Multi line<br/>in<br/>QFormLayout");
    multiLine->setElideMode(Qt::ElideNone);
    multiLine->setFilled(true);
    formLayout->addRow("Label:", multiLine);
    mainLayout->addLayout(formLayout);

    const QString lorem =
            "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt "
            "ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
            "ullamco laboris nisi ut aliquid ex ea commodi consequat.";

    auto elideRight = new Utils::InfoLabel("Qt::ElideRight: " + lorem, InfoLabel::Information);
    elideRight->setAdditionalToolTip("This control will elide the right side and show an Information Icon to its left. The Elided text will be separated from this text by two \\n");
    mainLayout->addWidget(elideRight);

    auto elideLeft = new Utils::InfoLabel("Qt::ElideLeft: " + lorem, InfoLabel::Warning);
    elideLeft->setElideMode(Qt::ElideLeft);
    elideLeft->setAdditionalToolTip("This control will elide the left side and show a Warning Icon to its left. The Elided text will be separated from this text by \" :) \"");
    elideLeft->setAdditionalToolTipSeparator(" :) ");
    mainLayout->addWidget(elideLeft);

    auto elideMid= new Utils::InfoLabel("Qt::ElideMiddle: " + lorem, InfoLabel::Ok);
    elideMid->setElideMode(Qt::ElideMiddle);
    elideMid->setAdditionalToolTip("This control will elide the middle and show an Ok Icon to its left. The Elided text will be separated from this text by \" -> \"");
    elideMid->setAdditionalToolTipSeparator(" -> ");
    mainLayout->addWidget(elideMid);


    auto elideNone = new Utils::InfoLabel("Qt::ElideNone: " + lorem, InfoLabel::Information);
    elideNone->setElideMode(Qt::ElideNone);
    elideNone->setWordWrap(true);
    elideNone->setAdditionalToolTip("This control is never elided due to setElideMode(Qt::ElideNone) being used");
    mainLayout->addWidget(elideNone);

    widget->resize(350, 500);
    widget->show();

    return app.exec();
}
