#include "MainWindow.h"
#include "ListEditor.h"
#include "OptimizationRunner.h"

#include <QVBoxLayout>      //вертикальная раскладка
#include <QHBoxLayout>      //горизонтальная раскладка
#include <QLineEdit>        //поле ввода
#include <QPlainTextEdit>   //поле вывода
#include <QCheckBox>        //галочка
#include <QPushButton>      //кнопки
#include <QLabel>           //текстовая метка
#include <QFileDialog>      //диалог выбора файла
#include <QMessageBox>      //всплывающее сообщение
#include <QDir>             //работа с директориями

//ДЛЯ ГРАФИКОВ (Qt Charts)
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) { //конструктор — создание всего интерфейса
    setWindowTitle("Optimization — графический интерфейс");
    resize(820, 700);   //чуть увеличила высоту для графика

    //central — виджет, который займёт всю центральную область окна.
    //у QMainWindow должен быть central widget, иначе некуда класть элементы
    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);   //устанавливаем вертикальную раскладку

    // строка с путём к exe
    auto* exeLayout = new QHBoxLayout();
    //QDir::current().filePath("optimization") — по умолчанию подставляем
    //"optimization" из текущей папки запуска, чтобы не пришлось всегда искать вручную
    exePathEdit = new QLineEdit(QDir::current().filePath("optimization"), this);
    auto* browseBtn = new QPushButton("Обзор...", this);
    exeLayout->addWidget(new QLabel("Исполняемый файл:"));
    exeLayout->addWidget(exePathEdit);
    exeLayout->addWidget(browseBtn);
    mainLayout->addLayout(exeLayout);

    //два блока списков: методы и функции
    //создаём два экземпляра одного класса ListEditor с разными подписями
    methodEditor = new ListEditor("Методы (-m)", "например: gradient_descent", this);
    functionEditor = new ListEditor("Функции (-f)", "например: rosenbrock", this);
    mainLayout->addWidget(methodEditor);
    mainLayout->addWidget(functionEditor);

    //строка с галочкой и кнопками действий
    auto* runLayout = new QHBoxLayout();
    noAnalysisCheck = new QCheckBox("Без анализа (-s)", this);
    auto* logBtn = new QPushButton("Показать app.log", this);
    runButton = new QPushButton("Запустить", this);
    runLayout->addWidget(noAnalysisCheck);
    runLayout->addStretch();      //прижимает кнопки вправо
    runLayout->addWidget(logBtn);
    runLayout->addWidget(runButton);
    mainLayout->addLayout(runLayout);

    //область вывода
    outputView = new QPlainTextEdit(this);
    outputView->setReadOnly(true);   //пользователь не должен редактировать вывод программы
    mainLayout->addWidget(outputView, 1);
    //второй параметр 1 у addWidget — это растяжимость (stretch factor):
    //говорит layout эта область должна занимать всё оставшееся свободное место,
    //в отличие от полей ввода выше, у которых фиксированная высота

    //ОБЛАСТЬ ДЛЯ ГРАФИКА (Qt Charts)
    //создаём линию для данных
    series = new QLineSeries();
    series->setName("Значение функции");

    //создаём сам график
    auto* chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("График сходимости");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);

    //создаём виджет для отображения графика
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(250);                    //минимальная высота, чтобы график был виден

    //добавляем график в основной layout (тоже растягивается)
    mainLayout->addWidget(chartView, 1);

    setCentralWidget(central);   //регистрируем central как содержимое окна

    //создаём главную часть — OptimizationRunner. "this" делает MainWindow его владельцем
    runner = new OptimizationRunner(this);

    //подключаем кнопки к нашим слотам
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::browseExecutable);
    connect(runButton, &QPushButton::clicked, this, &MainWindow::runOptimization);
    connect(logBtn, &QPushButton::clicked, this, &MainWindow::showAppLog);

    //подключаем сигналы OptimizationRunner к обновлению интерфейса
    //здесь мы используем лямбды прямо в MainWindow
    connect(runner, &OptimizationRunner::outputLine, outputView, [this](const QString& t) {
        outputView->appendPlainText(t);
    });
    connect(runner, &OptimizationRunner::errorLine, outputView, [this](const QString& t) {
        outputView->appendPlainText(t);
    });
    connect(runner, &OptimizationRunner::failedToStart, this, [this](const QString& reason) {
        QMessageBox::warning(this, "Ошибка", reason);
        runButton->setEnabled(true);   //разблокируем кнопку "Запустить" обратно
    });
    connect(runner, &OptimizationRunner::processFinished, this, [this](int code) {
        outputView->appendPlainText(QString("\n[Процесс завершён, код: %1]").arg(code));
        runButton->setEnabled(true);
    });

    //ПОДКЛЮЧАЕМ ДАННЫЕ ДЛЯ ГРАФИКА
    //когда приходят данные с итерациями — добавляем точки на график
    connect(runner, &OptimizationRunner::iterationData, this, [this](int iter, double value) {
        //добавляем точку (итерация, значение)
        series->append(iter, value);

        //обновляем график
        chartView->chart()->update();
    });
}

void MainWindow::browseExecutable() {
    //системный диалог "Открыть файл" — тот же самый, что вы видите в любых
    //других программах вашей ОС.
    QString path = QFileDialog::getOpenFileName(this, "Выберите исполняемый файл optimization");
    if (!path.isEmpty()) exePathEdit->setText(path);
}

void MainWindow::runOptimization() {
    QStringList functions = functionEditor->items();
    if (functions.isEmpty()) {
        //проверяем на стороне GUI то же самое, что проверяет  ParserTerminal
        //в main.cpp (без -f программа тоже откажется работать) — так пользователь
        //узнаёт об ошибке сразу, а не после запуска процесса.
        QMessageBox::warning(this, "Ошибка", "Добавьте хотя бы одну функцию (-f обязателен).");
        return;
    }
    outputView->clear();//очищаем окно вывода

    //ОЧИЩАЕМ ГРАФИК ПЕРЕД НОВЫМ ЗАПУСКОМ
    series->clear();              //удаляем все старые точки
    chartView->chart()->update(); //перерисовываем пустой график

    runButton->setEnabled(false);   //блокируем повторный клик, пока идёт выполнение
    runner->run(exePathEdit->text().trimmed(), methodEditor->items(), functions,
                noAnalysisCheck->isChecked());
}

//показать app.log
void MainWindow::showAppLog() {
    outputView->appendPlainText("\n--- app.log ---\n" +
                                 runner->readAppLog(exePathEdit->text().trimmed()));
}
