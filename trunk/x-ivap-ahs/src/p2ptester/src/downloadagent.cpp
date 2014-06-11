/***************************************************************************
 *   Copyright (C) 2006 by Martin Domig                                    *
 *   martin@domig.net                                                      *
 *                                                                         *
 *   For license information see LICENSE.xivap in the root folder of the   *
 *   source code.                                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/

#include "downloadagent.h"
#include <QMessageBox>

// global object
DownloadAgent* downloadAgent = NULL;

DownloadAgent::DownloadAgent()
{
	http = new QHttp;

	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));

	if(downloadAgent == NULL)
		downloadAgent = this;
}

DownloadAgent::~DownloadAgent()
{
	if(http != 0)
		delete http;
	downloadAgent = NULL;
}

void DownloadAgent::download(QString urlstr, DownloadCallbackFunc *func, void *context)
{
	QUrl url(urlstr);
	QFileInfo fileInfo(url.path());
	QString fileName = fileInfo.fileName();

	DownloadInfo* info = new DownloadInfo;
	info->buffer = new QBuffer;
	info->buffer->open(QBuffer::ReadWrite);
	info->callback = func;
	info->context = context;

	http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
	if (!url.userName().isEmpty())
		http->setUser(url.userName(), url.password());

	QString querystr = url.path() + "?" + url.encodedQuery();
	info->getId = http->get(querystr, info->buffer);

	downloadMap[info->getId] = info;
}

// SLOTS --------------------------------------

void DownloadAgent::httpRequestFinished(int requestId, bool error)
{
	QMap<int, DownloadInfo*>::iterator it = downloadMap.find(requestId);
	if(it == downloadMap.end())
		return;

	// rewind buffer
	it.value()->buffer->seek(0);
	// call users callback function
	if(it.value()->callback != NULL)
		it.value()->callback(it.value()->context, it.value()->buffer, error, http->errorString());
	
	// clean up
	delete it.value()->buffer;
	downloadMap.erase(it);
}

void DownloadAgent::setProxyInformation(QString server, int port, QString user, QString password)
{
	if(server.length() > 3 && port > 0) {
		http->abort();
		http->setProxy(server, port, user, password);
	}
}
