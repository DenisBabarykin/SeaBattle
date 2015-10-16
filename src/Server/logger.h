#ifndef LOGGER_H
#define LOGGER_H

#include <QtGlobal>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

void logMessagesInFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QFile file("log.txt");

    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        switch (type) {
        case QtDebugMsg:
            stream << "Debug: " << localMsg.constData() << ", " << context.file << "," << context.line << "," << context.function << endl;
            break;
            /*
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;*/
        case QtWarningMsg:
            stream << "Warning: " << localMsg.constData() << ", " << context.file << ", " << context.line << ", " << context.function << endl;
            break;
        case QtCriticalMsg:
            stream << "Critical: " << localMsg.constData() << ", " << context.file << ", " << context.line << ", " << context.function << endl;
            break;
        case QtFatalMsg:
            stream << "Fatal: " << localMsg.constData() << ", " << context.file << ", " << context.line << ", " << context.function << endl;
            QCoreApplication::exit(-1);
        }
    }

}

#endif // LOGGER_H
