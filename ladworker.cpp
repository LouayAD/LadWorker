#include "ladworker.h"

LadWorker::LadWorker()
{

}

LadWorker::~LadWorker()
{

}

bool LadWorker::PostData(QUrlQuery postData, QUrl *pipeUrl, ResponseType responseType)
{
    switch (responseType)
    {
    case ResponseType::JSON_ARRAY:
        if(PostJsonRequest(postData, pipeUrl)) {return true;}
        else return false;
    case ResponseType::BYTE_ARRAY:
        if(PostByteArrayRequest(postData, pipeUrl)) {return true;}
        else return false;
    case ResponseType::NO_REPLY:
        if(PostNoReplyRequest(postData, pipeUrl)) {return true;}
        else return false;
    }
    return false;
}

bool LadWorker::GetData(QUrl *pipeUrl, ResponseType responseType)
{
    switch (responseType)
    {
    case ResponseType::JSON_ARRAY:
        if(GetJsonRequest(pipeUrl)) {return true;}
        else return false;
    case ResponseType::BYTE_ARRAY:
        if(GetByteArrayRequest(pipeUrl)) {return true;}
        else return false;
    case ResponseType::NO_REPLY:
        return false;
    }
    return false;
}

QJsonArray LadWorker::Json()
{
    return _PlJson;
}

QByteArray LadWorker::ByteArray()
{
    return _ByteArray;
}

WorkerState LadWorker::CurrentState()
{
    return _WorkerState;
}

bool LadWorker::PostJsonRequest(QUrlQuery postData, QUrl *pipeUrl)
{
    _WorkerState = WorkerState::CONNECTING;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest req(*pipeUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    //this part not always needed | so far I only needed the following line of code to connect to Hostgator-China.
    req.setHeader(QNetworkRequest::UserAgentHeader, "STRONEX Worker");

    // maybe save the pointer in a class attribute
    QNetworkReply *reply = manager->post(req, postData.toString(QUrl::FullyEncoded).toUtf8());

    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response_data = reply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(response_data);
        _PlJson = json.array();
        delete reply;

        _WorkerState = WorkerState::CONNECTION_SUCCESSFUL;
        return true;
    }
    else
    {
        _WorkerState = WorkerState::CONNECTION_FAILED;
        return false;
    }
}

bool LadWorker::PostByteArrayRequest(QUrlQuery postData, QUrl *pipeUrl)
{
    _WorkerState = WorkerState::CONNECTING;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest req(*pipeUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    //this part not always needed | so far I only needed the following line of code to connect to Hostgator-China.
    req.setHeader(QNetworkRequest::UserAgentHeader, "STRONEX Worker");

    // maybe save the pointer in a class attribute
    QNetworkReply *reply = manager->post(req, postData.toString(QUrl::FullyEncoded).toUtf8());

    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        _ByteArray = reply->readAll();
        delete reply;

        _WorkerState = WorkerState::CONNECTION_SUCCESSFUL;
        return true;
    }
    else
    {
        _WorkerState = WorkerState::CONNECTION_FAILED;
        return false;
    }
}

bool LadWorker::PostNoReplyRequest(QUrlQuery postData, QUrl *pipeUrl)
{
    _WorkerState = WorkerState::CONNECTING;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest req(*pipeUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    //this part not always needed | so far I only needed the following line of code to connect to Hostgator-China.
    req.setHeader(QNetworkRequest::UserAgentHeader, "STRONEX Worker");

    // maybe save the pointer in a class attribute
    QNetworkReply *reply = manager->post(req, postData.toString(QUrl::FullyEncoded).toUtf8());

    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        delete reply;

        _WorkerState = WorkerState::CONNECTION_SUCCESSFUL;
        return true;
    }
    else
    {
        _WorkerState = WorkerState::CONNECTION_FAILED;
        return false;
    }
}

bool LadWorker::GetJsonRequest(QUrl *pipeUrl)
{
    _WorkerState = WorkerState::CONNECTING;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest req(*pipeUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    //this part not always needed | so far I only needed the following line of code to connect to Hostgator-China.
    req.setHeader(QNetworkRequest::UserAgentHeader, "STRONEX Worker");

    // maybe save the pointer in a class attribute
    QNetworkReply *reply = manager->get(req);

    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response_data = reply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(response_data);
        _PlJson = json.array();
        delete reply;

        _WorkerState = WorkerState::CONNECTION_SUCCESSFUL;
        return true;
    }
    else
    {
        _WorkerState = WorkerState::CONNECTION_FAILED;
        return false;
    }
}

bool LadWorker::GetByteArrayRequest(QUrl *pipeUrl)
{
    _WorkerState = WorkerState::CONNECTING;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest req(*pipeUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    //this part not always needed | so far I only needed the following line of code to connect to Hostgator-China.
    req.setHeader(QNetworkRequest::UserAgentHeader, "STRONEX Worker");

    // maybe save the pointer in a class attribute
    QNetworkReply *reply = manager->get(req);

    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        _ByteArray = reply->readAll();
        delete reply;

        _WorkerState = WorkerState::CONNECTION_SUCCESSFUL;
        return true;
    }
    else
    {
        _WorkerState = WorkerState::CONNECTION_FAILED;
        return false;
    }
}
