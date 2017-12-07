//-----GPL----------------------------------------------------------------------
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------
//
//  File: qstat.h
//
//------------------------------------------------------------------------------

#ifndef QSTAT_H
#define QSTAT_H

#include "qjob.h"
#include "queue.h"
#include <QMap>
#include <QObject>
#include <QVector>

class Qstat : public QObject {
  Q_OBJECT
public:
  explicit Qstat(QObject *parent = nullptr);

  void run(QByteArray hash);

  int numQueues();
  Queue *queue(int index);

private:
  //...Color codes for unix terminal display
  const QString _cyan = "\E[36m";
  const QString _black = "\E[30;47m";
  const QString _red = "\E[31m";
  const QString _green = "\E[32m";
  const QString _yellow = "\E[33m";
  const QString _blue = "\E[34m";
  const QString _magenta = "\E[35m";
  const QString _white = "\E[37m";
  const QString _reset = "\E[0m";

  int _parseQstat();
  int _getJobInfo();
  int _getQueue();
  void _displayQueue(QByteArray hash);
  void _displayQueueHealth(Queue *q);
  void _initializeQueues();
  int _getXML(Qjob *testJob);
  int _findQueue(Qjob *testJob, QString queueName);
  QString _formatJobOutputLine(Qjob *job);

  /// List of queues that the user can select from
  QVector<Queue *> _mQueues;

  /// Mapping from a queue hash to a pointer to the queue
  QMap<QByteArray, Queue *> _mQueueMap;

  /// Vector of the jobs in each queue
  QVector<Qjob *> _mJobs;
};

#endif // QSTAT_H
