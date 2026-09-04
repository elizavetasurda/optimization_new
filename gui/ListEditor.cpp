#include "ListEditor.h"

#include <QVBoxLayout>//вертик раскладка
#include <QHBoxLayout>//гориз раскладка
#include <QGroupBox>//рамка с заголовками
#include <QLineEdit>//поле ввода
#include <QListWidget>//список
#include <QPushButton>//кнопки

ListEditor::ListEditor(const QString& title, const QString& placeholder, QWidget* parent)
    : QWidget(parent)   //вызываем конструктор родителя QWidget, передавая ему "родительский" виджет
{
    //QGroupBox — рамка с заголовком сверху
    //this - этот QGroupBox принадлежит текущему ListEditor
    //те как только удалится ListEditor — Qt автоматически удалит и group и все что внутри
    auto* group = new QGroupBox(title, this);

    //раскладка внутри рамки: сверху строка ввода+кнопки, снизу список.
    auto* groupLayout = new QVBoxLayout(group);

    //горизонтальная раскладка для строки "поле ввода + кнопка Добавить + кнопка Удалить"
    auto* inputLayout = new QHBoxLayout();
//QGroupBox — это фундамент с надписью
//QVBoxLayout — элементы клади друг под другом
//QHBoxLayout — элементы в ряд

    //поле для вводы текста
    input = new QLineEdit(this);
    input->setPlaceholderText(placeholder);   //подсказка
    //делаем кнопки
    auto* addBtn = new QPushButton("Добавить", this);
    auto* delBtn = new QPushButton("Удалить", this);

    //addWidget добавляет виджет в раскладку слева направо (для QHBoxLayout)
    inputLayout->addWidget(input);
    inputLayout->addWidget(addBtn);
    inputLayout->addWidget(delBtn);

    //QListWidget — готовый список Qt
    list = new QListWidget(this);

    //собираем layout рамки: сверху строка ввода, снизу список
    groupLayout->addLayout(inputLayout); //сначала строка ввода
    groupLayout->addWidget(list); //потом список

    //внешняя раскладка самого ListEditor — просто вставляем рамку целиком.
    //setContentsMargins(0,0,0,0) убирает лишние отступы
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(group);

    //подключение сигналов к слотам
    //синтаксис: connect(отправитель, &Класс::сигнал, получатель, &Класс::слот)
    //те когда нажимают кнопку "Добавить" - вызывается addItem()
    //когда нажимают "Удалить" - вызывается removeSelected()
    //когда нажимают enter в поле ввода - вызывается addItem()
    connect(addBtn, &QPushButton::clicked, this, &ListEditor::addItem);
    connect(delBtn, &QPushButton::clicked, this, &ListEditor::removeSelected);
    connect(input, &QLineEdit::returnPressed, this, &ListEditor::addItem);
}

void ListEditor::addItem() {
    //trimmed() убирает пробелы по краям строки
    const QString text = input->text().trimmed();
    if (!text.isEmpty()) {     //если что-то ввели
        list->addItem(text);   //добавляем строку в конец списка
        input->clear();        //очищаем поле ввода для следующего значения
    }
}

void ListEditor::removeSelected() {
    //selectedItems() — возвращает список всех строк, которые пользователь выделил мышкой
    //qDeleteAll удаляет каждый элемент; QListWidget сам корректно уберёт их из списка при удалении.
    qDeleteAll(list->selectedItems());
}

QStringList ListEditor::items() const { //этот метод используется MainWindow, чтобы получить все методы или функции, которые добавил пользователь
    QStringList result; //создаём пустой список строк
    //проходим по всем строкам списка и собираем их текст в обычный QStringList
    for (int i = 0; i < list->count(); ++i)//проходим по элементам
        result << list->item(i)->text();//добавляем в список
    return result; //возвращем список строк
}
