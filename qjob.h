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
//  File: qjob.h
//
//------------------------------------------------------------------------------

#ifndef QJOB_H
#define QJOB_H

#include <QDateTime>
#include <QList>
#include <QObject>

class Qjob : public QObject {
  Q_OBJECT
public:
  explicit Qjob(QObject *parent = nullptr);

  /// Enum with various status codes for jobs
  enum _qStatus {
    SGE_STATUS_PENDING,
    SGE_STATUS_RUNNING,
    SGE_STATUS_SUSPENDED,
    SGE_STATUS_HELD,
    SGE_STATUS_DELETED,
    SGE_STATUS_ERROR,
    SGE_STATUS_UNKNOWN
  };

  int fromQueueLine(QString line);

  int jobNumber();

  int ncpu();

  qreal priority();

  int status();

  QString statusString();

  QString node();

  QString jobName();

  void setJobName(QString name);

  QString user();

  QDateTime time();

  void addQueueHash(QByteArray hash);

  bool containsQueueHash(QByteArray hash);

  QString core();

  int coreNumber();

  void setNcpu(int n);

  bool isOnQueue();

  void setIsOnQueue(bool q);

  void addCoreList(int core);

  bool containsCore(int core);

private:
  int _getJobStatus(QString stat);

  /// Job number from SGE
  int _mJobNumber;

  /// Number of CPUs for this job
  int _mNcpus;

  /// Displayed core in the queue information
  int _mCoreNumber;

  /// Job priority real
  qreal _mPriority;

  /// Status code for the job
  int _mStatus;

  /// Node name for this job
  QString _mNode;

  /// Core string for the job
  QString _mCore;

  /// User string for the job
  QString _mUser;

  /// Job name
  QString _mJobName;

  /// Job submit time
  QDateTime _mTime;

  /// List of queues this job participates in
  QList<QByteArray> _mQueueHash;

  /// Logical value denoting if this job is involved in the queue of interest
  bool _mIsOnQueue;

  /// List of nodes that are used for this job
  QList<int> _mNodeIds;
};

#endif // QJOB_H
