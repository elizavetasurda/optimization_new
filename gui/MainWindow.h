#pragma once
#include <QMainWindow>
//QMainWindow — это готовый каркас окна в Qt: у него уже есть место под меню,
//панель инструментов и центральную область (setCentralWidget), куда мы вставим свой интерфейс

#include <QtCharts/QChartView>   //виджет для отображения графика
#include <QtCharts/QLineSeries>  //линия графика (точки, соединённые линией)

class ListEditor;
class OptimizationRunner;
class QLineEdit;           //поле ввода
class QPlainTextEdit;      //многострочное поле для вывода
class QCheckBox;           //галочка
class QPushButton;         //кнопки

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void browseExecutable();  //открыть диалог выбора файла
    void runOptimization();   //собрать аргументы и запустить OptimizationRunner
    void showAppLog();        //показать содержимое app.log в текстовом поле

private:
    //все виджеты, которые используются в окне
    //поле ввода пути к exe-файлу
    QLineEdit* exePathEdit;

    //два наших переиспользуемых компонента: один для методов, другой для функций
    ListEditor* methodEditor;
    ListEditor* functionEditor;

    QCheckBox* noAnalysisCheck;   //галочка "-s"
    QPlainTextEdit* outputView;   //большое текстовое поле с выводом программы
    QPushButton* runButton;

    //основа запуска вынесена сюда, MainWindow только подписывается на его сигналы и обновляет интерфейс
    OptimizationRunner* runner;

    //ДЛЯ ГРАФИКОВ (Qt Charts)
    QChartView* chartView;   //виджет, который отображает график (область с осями)
    QLineSeries* series;     //линия графика (точки, соединённые линией)
};
