#ifndef LOGGER_H
#define LOGGER_H

#include <QtGlobal>
#include <QDate>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

const QString DATETIME_FORMAT = "dd.MM.yyyy hh:mm:ss";
const QString LOG_FILENAME = "log.txt";
const QString LOG_STR_FORMAT = "%1, %2: %3";

void logMessagesInFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
//    QByteArray localMsg = msg.toLocal8Bit();
    QFile file(LOG_FILENAME);

    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        QString datetime = QDateTime::currentDateTime().toString(DATETIME_FORMAT);
        switch (type) {
        case QtDebugMsg:
            stream << LOG_STR_FORMAT.arg(datetime, "Info", msg) << endl;
            break;
        case QtWarningMsg:
            stream << LOG_STR_FORMAT.arg(datetime, "Warning", msg) << endl;
            break;
        case QtCriticalMsg:
            stream << LOG_STR_FORMAT.arg(datetime, "Critical", msg) << endl;
            break;
        case QtFatalMsg:
            stream << LOG_STR_FORMAT.arg(datetime, "Fatal", msg) << endl;
            QCoreApplication::exit(-1);
        }
    }

}

#endif // LOGGER_H
