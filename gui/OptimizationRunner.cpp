#include "OptimizationRunner.h"

#include <QFile> //работа с файлами
#include <QFileInfo>//инфа о пути имени файла
#include <QTextStream>//чтение текста из файлв

OptimizationRunner::OptimizationRunner(QObject* parent) : QObject(parent) {
    //создаём QProcess. this-автоматич уничтожение, когда уничтожится OptimizationRunner (система родитель-потомок Qt, как и с виджетами).
    process = new QProcess(this);

    //SeparateChannels — разделяем stdout и stderr, чтобы можно было по-разному их подсвечивать/обрабатывать при желании.
    process->setProcessChannelMode(QProcess::SeparateChannels);

    //4 подключения сигналов QProcess
    connect(process, &QProcess::readyReadStandardOutput, this, [this] {//когда появляется вывод в stdout
        //readAllStandardOutput() — читает весь накопившийся вывод программы с последнего раза.
        //fromLocal8Bit — превращает байты из консоли в правильную QString из-за кирилицы
        emit outputLine(QString::fromLocal8Bit(process->readAllStandardOutput()));
        // emit — ключевое слово Qt, которым мы "испускаем" сигнал. После этого все, кто подписан (через connect) на outputLine, получат вызов своего слота
    });

    connect(process, &QProcess::readyReadStandardError, this, [this] { //когда появляется вывод в stderr
        emit errorLine(QString::fromLocal8Bit(process->readAllStandardError()));
    });
    //QString::fromLocal8Bit нужен, чтобы правильно обрабатывать русские буквы в выводе программы.

    //у finished() есть перегрузка (два варианта параметров), поэтому нужно явно
    //указать компилятору, какую именно версию сигнала мы имеем в виду — для этого используется QOverload.
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int code, QProcess::ExitStatus) { emit processFinished(code); });//испускаем сигнал с кодом завершения

    //errorOccurred срабатывает, если процесс вообще не удалось запустить
    //(например, неверный путь к файлу, нет прав на выполнение)
    connect(process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {//испускаем сигнал с ошибкой
        emit failedToStart(process->errorString());
    });
}

//собирает аргументы командной строки
QStringList OptimizationRunner::buildArguments(const QStringList& methods,
                                                const QStringList& functions,
                                                bool noAnalysis) const {
    QStringList args;
    //если есть методы, добавляем -m и список методов
    if (!methods.isEmpty()) args << "-m" << methods;
    if (!functions.isEmpty()) args << "-f" << functions;
    if (noAnalysis) args << "-s";
    return args;
    //если methods = ["gradient_descent", "newton"], functions = ["rosenbrock"], noAnalysis = true, то вернётся:
    //["-m", "gradient_descent", "newton", "-f", "rosenbrock", "-s"]
}

//запускает программу
void OptimizationRunner::run(const QString& exePath, const QStringList& methods,
                              const QStringList& functions, bool noAnalysis) {
    //проверяем, что указанный файл вообще существует, до того как пытаться его запустить —
    //так пользователь сразу поймёт причину ошибки, а не получит невнятный сбой QProcess
    if (!QFile::exists(exePath)) {
        emit failedToStart("Исполняемый файл не найден: " + exePath);
        return;
    }

    //рабочая директория процесса = папка, где лежит exe.
    //важно, потому что ваша программа создаёт app.log рядом с собой, а также ищет файлы через относительные пути — если не выставить рабочую
    //директорию правильно, программа может искать файлы не там
    process->setWorkingDirectory(QFileInfo(exePath).absolutePath());

    //start() запускает процесс АСИНХРОННО — то есть эта функция сразу возвращает управление, а сама программа работает "в фоне". Мы узнаём о её выводе
    //и завершении через сигналы, которые подключили в конструкторе выше.
    process->start(exePath, buildArguments(methods, functions, noAnalysis));
}

//читает файл app.log
QString OptimizationRunner::readAppLog(const QString& exePath) const {
    QString logPath = QFileInfo(exePath).absolutePath() + "/app.log";//путь к логу папка с exe + "/app.log"
    QFile file(logPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))//открываем файл
        return QString("Не удалось открыть app.log: %1").arg(logPath);
    QTextStream in(&file);//читаем содержимое
    return in.readAll();
}
