#pragma once
#include <QObject> //баз класс для работы с сигналами и слотами
#include <QProcess> //класс для запуска внешних программ
#include <QStringList>//список строк

//OptimizationRunner — класс, который отвечает за запуск вашей консольной программы optimization и чтение файла app.log. он ничего не знает про кнопки,
//окна и тд — наследуется от QObject (не от QWidget!), потому что ему не нужно ничего рисовать, ему нужны только сигналы/слоты.
//это для универсальности те при каких-либо изменений,этот класс можно оставить так
class OptimizationRunner : public QObject {
    Q_OBJECT //для сигналов и слотов
public:
    explicit OptimizationRunner(QObject* parent = nullptr);

    //запускает optimization с нужными аргументами
    //exePath     — путь к исполняемому файлу
    //methods     — список имён методов (то, что после -m)
    //functions   — список имён функций (то, что после -f)
    //noAnalysis  — если true, добавит флаг -s (без анализа производительности)
    void run(const QString& exePath, const QStringList& methods,
              const QStringList& functions, bool noAnalysis);

    //читает файл app.log (он создаётся вашим Logger'ом рядом с exe-файлом) и возвращает его содержимое одной строкой
    QString readAppLog(const QString& exePath) const;

signals:
    // signals — это события, которые класс может делать
    // сам OptimizationRunner не решает, что делать с этими данными —он просто оповещает MainWindow
    void outputLine(const QString& text);      // пришла новая строка из stdout программы
    void errorLine(const QString& text);       // пришла новая строка из stderr программы
    void processFinished(int exitCode);        // программа завершилась, вот код возврата
    void failedToStart(const QString& reason); // не получилось запустить (например, файла нет)

private:
    QProcess* process;//для запуска внешней проги

    //приватный вспомогательный метод: собирает QStringList аргументов вида ["-m", "method1", "method2", "-f", "func1", "-s"]
    QStringList buildArguments(const QStringList& methods, const QStringList& functions,
                                bool noAnalysis) const;
};
