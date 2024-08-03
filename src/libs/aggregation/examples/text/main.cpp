#include "main.h"

#include <QApplication>

MyMain::MyMain(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    ui.setupUi(this);
    connect(ui.comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MyMain::select);
}

void MyMain::add(IComboEntry *obj)
{
    m_entries.append(obj);
    ui.comboBox->addItem(obj->title());
}

void MyMain::select(int index)
{
    IComboEntry *entry = m_entries.at(index);
    // 如果使用多重继承，我们会在这里使用qobject_cast，但我们使用query来获取组件（如果它们存在的话）
    IText1 *t1 = Aggregation::query<IText1>(entry);
    IText2 *t2 = Aggregation::query<IText2>(entry);
    IText3 *t3 = Aggregation::query<IText3>(entry);
    // 根据各自的接口实现是否存在，设置标签文本并启用/禁用
    ui.text1->setText(t1 ? t1->text() : tr("N/A"));
    ui.text2->setText(t2 ? t2->text() : tr("N/A"));
    ui.text3->setText(t3 ? t3->text() : tr("N/A"));
    ui.text1->setEnabled(t1);
    ui.text2->setEnabled(t2);
    ui.text3->setEnabled(t3);
}

MyMain::~MyMain()
{
    // 以下操作将删除所有的Aggregate、IComboEntry和ITextX对象，以及我们可能添加的任何其他组件
    qDeleteAll(m_entries);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyMain w;
    //创建并设置一些对象

    //第一个对象只实现了必需的IComboEntry，我们不需要为此创建聚合体
    IComboEntry *obj1 = new IComboEntry("Entry without text");

    //  第二个对象额外提供了IText2实现，通过将聚合体设置为组件的父对象来将组件添加到聚合体中
    Aggregation::Aggregate *obj2 = new Aggregation::Aggregate;
    obj2->add(new IComboEntry("Entry with text 2"));
    obj2->add(new IText2("This is a text for label 2"));

    // 依此类推...再创建两个对象...
    Aggregation::Aggregate *obj3 = new Aggregation::Aggregate;
    obj3->add(new IComboEntry("Entry with text 1 and 2"));
    obj3->add(new IText1("I love Qt!"));
    obj3->add(new IText2("There are software companies..."));
    Aggregation::Aggregate *obj4 = new Aggregation::Aggregate;
    obj4->add(new IComboEntry("Entry with text 1 and 3"));
    obj4->add(new IText1("Some text written here."));
    obj4->add(new IText3("I'm a troll."));

    // API接受IComboEntry类型，所以我们将它们转换为该类型MyMain对象接管整个聚合体的所有权
    w.add(Aggregation::query<IComboEntry>(obj1));
    w.add(Aggregation::query<IComboEntry>(obj2));
    w.add(Aggregation::query<IComboEntry>(obj3));
    w.add(Aggregation::query<IComboEntry>(obj4));
    w.show();

    return app.exec();
}
