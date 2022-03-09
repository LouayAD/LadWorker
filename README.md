# LadWorker
#### A helper class to make network requests in Qt
LadWorker simplify your workflow when trying to make network requests within Qt framework. It contains most of what you need to make a request and get a response.

### How does it works:
#### 1) Post Data
To post data use PostData() function that takes 3 parameters as follows:

```c++
PostData(data, url, responseType);
```

#### 2) Get Data
To get data use GetData() function that takes 2 parameters as follows:

```c++
GetData(url, responseType);
```

- data = QUrlQuery
- url = QUrl
- responseType is an enum where you can choose one of the following types { JSON_ARRAY, BYTE_ARRAY, NO_REPLY }

### Notes:
- Both of PostData & GetData are booleans. They return true if the connection is successsfull and false otherwise.

- Alternatively we could also call CurrentState() function which returns an enum called WorkerState. The two important states for this scenario are CONNECTION_SUCCESSFUL & CONNECTION_FAILED

- If the request is successful then depending on the ResponseType we should call either the Json() or ByteArray() function to get the correct response that was returned from the server

### Compatibility:
- LadWorker is tested on Qt5.12+ & Qt6.2+ for windows. Connects fine with Siteground and Hostgator-China shared servers.

- I have been using it since 2020 with no issues for my personal use. However, it is possible to encounter some problems depending on your environment and use case. If so, pls reach out to me and explain the problem in details.

- Pls keep in mind that some isses are due to server's configuration and require a fix on the server.

 
### Examples:

#### 1. Get Data

```c++
QUrl *url = new QUrl("http://exampleurl.com/data.php");

LadWorker worker;
//check if the connection is successful
if(worker.GetData(url, ResponseType::BYTE_ARRAY))
{
   //we cast the response we received into a string
   QString DataAsString = QString(worker.ByteArray());
   
   //that's it, do what ever u want with the response
}
else
{
   //failed to connect
}
```
#### 2. Post Data

```c++
QUrl *url = new QUrl("http://exampleurl.com/data.php");

//the values that we want to post
QString value1= "1";
QString value2= "2";

//convert to utf8
QByteArray const value1Data = value1.toUtf8();
QByteArray const value2Data = value2.toUtf8();

//finally we add them to the query
QUrlQuery postData;
postData.addQueryItem("key1", value1Data );
postData.addQueryItem("key2", value2Data );

LadWorker worker;
//check if the connection is successful
if(worker.PostData(postData, url, ResponseType::NO_REPLY))
{
    //posted successfully
}
else
{
    //something went wrong
}
```

### Implementation:
First of all make sure network is added to .pro as follows

```c++
QT       += core gui network
```

Now simply create a classs called 'LadWorker' then copy-paste the following .h & .cpp code

#### .h

```c++
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

    QJsonArray _PlJson;
    QByteArray _ByteArray;
    WorkerState _WorkerState;
};

#endif // LADWORKER_H
```

#### .cpp

```c++
#include "ladworker.h"

LadWorker::LadWorker(){}

LadWorker::~LadWorker(){}

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

    //this part not always needed but it is recommended to keep
    //so far it was only needed for some hosting proders like hostgator.
    req.setHeader(QNetworkRequest::UserAgentHeader, "LAD Worker");

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

    //this part not always needed but it is recommended to keep
    //so far it was only needed for some hosting proders like hostgator.
    req.setHeader(QNetworkRequest::UserAgentHeader, "LAD Worker");

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

    //this part not always needed but it is recommended to keep
    //so far it was only needed for some hosting proders like hostgator.
    req.setHeader(QNetworkRequest::UserAgentHeader, "LAD Worker");

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

    //this part not always needed but it is recommended to keep
    //so far it was only needed for some hosting proders like hostgator.
    req.setHeader(QNetworkRequest::UserAgentHeader, "LAD Worker");

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

    //this part not always needed but it is recommended to keep
    //so far it was only needed for some hosting proders like hostgator.
    req.setHeader(QNetworkRequest::UserAgentHeader, "LAD Worker");

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
```

- 📫 How to reach me: louayad@ladlab.net 





