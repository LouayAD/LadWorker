#ifndef LADWORKER_H
#define LADWORKER_H

#include <QObject>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

enum class ResponseType
{
  JSON_ARRAY,
  BYTE_ARRAY,
  NO_REPLY,
};

enum class WorkerState
{
  CONNECTING,
  CONNECTION_SUCCESSFUL,
  CONNECTION_FAILED,
  POSITIVE_RESPONSE,
  NEGATIVE_RESPONSE,
  ERROR,
  VALIDATING,
  INVALID_TOKEN,
};

class LadWorker : public QObject
{
    Q_OBJECT
public:
    LadWorker();
    ~LadWorker();

    bool PostData(QUrlQuery postData, QUrl *pipeUrl, ResponseType responseType);
    bool GetData(QUrl *pipeUrl, ResponseType responseType);
    QJsonArray Json();
    QByteArray ByteArray();
    WorkerState CurrentState();

private:
    bool PostJsonRequest(QUrlQuery postData, QUrl *pipeUrl);
    bool PostByteArrayRequest(QUrlQuery postData, QUrl *pipeUrl);
    bool PostNoReplyRequest(QUrlQuery postData, QUrl *pipeUrl);
    bool GetJsonRequest(QUrl *pipeUrl);
    bool GetByteArrayRequest(QUrl *pipeUrl);
    bool Validate_Integrity();

    QJsonArray _PlJson;
    QByteArray _ByteArray;
    WorkerState _WorkerState;
};

#endif // LADWORKER_H
