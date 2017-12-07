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
//  File: queue.h
//
//------------------------------------------------------------------------------

#ifndef QUEUE_H
#define QUEUE_H

#include "qjob.h"
#include <QCryptographicHash>
#include <QObject>

class Queue : public QObject {
  Q_OBJECT
public:
  explicit Queue(QString machineName, QString queueName, QString queueCore,
                 int queueStart, int queueEnd, int coreSize, int nameFormat,
                 QObject *parent = nullptr);
  explicit Queue(QString machineName, QString queueName, QString queueCore,
                 int queueStart, int queueEnd, int queueStart2, int queueEnd2,
                 int coreSize, int nameFormat, QObject *parent = nullptr);

  bool isInQueue(Qjob *job, QString queueName);
  bool isOnNodes(Qjob *testJob);

  QByteArray hash();

  QString queueName();
  QString machine();

  void getQueueHealth();

  int queueTotalNodes();
  int queueUpNodes();
  int queueDownNodes();
  int queueIdleNodes();
  int queueRunningNodes();
  int queueTotalCores();
  int queueFreeCores();
  int queueRunningCores();
  int nameFormat();

private:
  /// Name of the nodes
  QString _mNodeName;

  /// Name of the queue
  QString _mQueueName;

  /// Name of the machine
  QString _mMachineName;

  /// Start index for nodes in this queue
  int _mNodeStart;

  /// End index for nodes in this queue
  int _mNodeEnd;

  /// Second start index for nodes in this queue
  int _mNodeStart2;

  /// Second end index for nodes in this queue
  int _mNodeEnd2;

  /// Size of the cores in this queue
  int _mCoreSize;

  /// Number of ranges of cores (1 or 2)
  int _mNRange;

  /// Number of cores in the queue
  int _mNcore;

  /// Number of down nodes in the queue
  int _mDownNodes;

  /// Number of up nodes in the queue
  int _mUpNodes;

  /// Number of idle nodes in the queue
  int _mIdleNodes;

  /// Number of nodes currently used in jobs
  int _mRunningNodes;

  /// Number of cores currently used in jobs
  int _mRunningCores;

  /// Number of cores currently idle
  int _mIdleCores;

  /// Number of digits used in the specification of node numbers
  int _mNameFormat;

  /// A unique hash for the queue
  QByteArray _mHash;

  void _hash();
  void _calculateSize();
};

#endif // QUEUE_H
