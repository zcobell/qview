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
//  File: qjob.cpp
//
//------------------------------------------------------------------------------

#include "qjob.h"
#include <QStringList>

/**
 * @brief Qjob::Qjob Default constructor
 * @param parent
 */
Qjob::Qjob(QObject *parent) : QObject(parent) {
  this->_mJobNumber = 0;
  this->_mPriority = 0.0;
  this->_mJobName = "none";
  this->_mUser = "none";
  this->_mStatus = -1;
  this->_mTime = QDateTime();
  this->_mNcpus = 0;
  this->_mNode = "none";
  this->_mIsOnQueue = false;
}

/**
 * @brief Qjob::fromQueueLine generates a job object from a queue line
 * @param line text from the queue line
 * @return job object
 */
int Qjob::fromQueueLine(QString line) {
  int tempInt;
  bool ok;

  line = line.simplified();
  QStringList lineData = line.split(" ");
  this->_mJobNumber = lineData.value(0).toInt();
  this->_mPriority = lineData.value(1).toDouble();
  this->_mJobName = lineData.value(2);
  this->_mUser = lineData.value(3);
  this->_mStatus = this->_getJobStatus(lineData.value(4));
  this->_mTime =
      QDateTime::fromString(lineData.value(5), "mm/dd/yyyy hh:MM:ss");
  this->_mTime.setDate(QDate::fromString(lineData.value(5), "mm/dd/yyyy"));
  this->_mTime.setTime(QTime::fromString(lineData.value(6), "hh:MM:ss"));
  this->_mTime.setTimeSpec(Qt::UTC);
  this->_mNode = lineData.value(7);
  this->_mNode = this->_mNode.split("@").value(1);
  this->_mCore = this->_mNode.split(".").value(0);
  tempInt = this->_mCore.right(3).toInt(&ok);
  if (ok)
    this->_mCoreNumber = tempInt;
  else
    this->_mCoreNumber = -1;
  this->_mCore = this->_mCore.left(this->_mCore.length() - 3);
  return 0;
}

/**
 * @brief Qjob::_getJobStatus converts the textual status to an internal code
 * @param stat text status identifier
 * @return SGE code used internally
 */
int Qjob::_getJobStatus(QString stat) {
  if (stat == "r" || stat == "t" || stat == "Rr" || stat == "Rt")
    return SGE_STATUS_RUNNING;
  else if (stat == "qw" || stat == "wq")
    return SGE_STATUS_PENDING;
  else if (stat == "hRqw" || stat == "hqw" || stat == "hRwq" || stat == "hwq")
    return SGE_STATUS_HELD;
  else if (stat == "s" || stat == "S" || stat == "ts" || stat == "tS" ||
           stat == "T" || stat == "tT")
    return SGE_STATUS_SUSPENDED;
  else if (stat == "Eqw" || stat == "Ehqw" || stat == "EhRqw")
    return SGE_STATUS_ERROR;
  else if (stat == "dr" || stat == "dt" || stat == "dRr" || stat == "dRt" ||
           stat == "ds" || stat == "dT" || stat == "dRs" || stat == "dRS" ||
           stat == "dRT")
    return SGE_STATUS_DELETED;
  else
    return SGE_STATUS_UNKNOWN;
}

/**
 * @brief Qjob::statusString Converts an internal status code to a text status
 * @return text status for the current job
 */
QString Qjob::statusString() {
  if (this->_mStatus == SGE_STATUS_RUNNING)
    return QStringLiteral("r");
  else if (this->_mStatus == SGE_STATUS_PENDING)
    return QStringLiteral("qw");
  else if (this->_mStatus == SGE_STATUS_HELD)
    return QStringLiteral("h");
  else if (this->_mStatus == SGE_STATUS_SUSPENDED)
    return QStringLiteral("s");
  else if (this->_mStatus == SGE_STATUS_DELETED)
    return QStringLiteral("d");
  else if (this->_mStatus == SGE_STATUS_ERROR)
    return QStringLiteral("e");
  else if (this->_mStatus == SGE_STATUS_UNKNOWN)
    return QStringLiteral("?");
  return QStringLiteral("?");
}

/**
 * @brief Qjob::core Gets the number of cores for the current job
 * @return number of cores used in this job
 */
QString Qjob::core() { return this->_mCore; }

/**
 * @brief Qjob::coreNumber Returns the main core number in the queue line
 * @return core number from the queue line
 */
int Qjob::coreNumber() { return this->_mCoreNumber; }

/**
 * @brief Qjob::jobName returns the job name for this job
 * @return job name
 */
QString Qjob::jobName() { return this->_mJobName; }

/**
 * @brief Qjob::setJobName sets the current job name
 * @param name new job name
 */
void Qjob::setJobName(QString name) { this->_mJobName = name; }

/**
 * @brief Qjob::addCoreList Adds the cores listed in the xml to the list for
 * this job
 * @param core code ID to add to the list
 */
void Qjob::addCoreList(int core) {
  if (!this->_mNodeIds.contains(core))
    this->_mNodeIds.append(core);
  return;
}

/**
 * @brief Qjob::containsCore Checks if the core specified is found in the list
 * for this job
 * @param core core ID to check
 * @return true or false if contained (true) or not contained (false)
 */
bool Qjob::containsCore(int core) { return this->_mNodeIds.contains(core); }

/**
 * @brief Qjob::jobNumber returns the job number for this job
 * @return job number
 */
int Qjob::jobNumber() { return this->_mJobNumber; }

/**
 * @brief Qjob::ncpu Returns the number of CPUs used by this job
 * @return number of CPUs used
 */
int Qjob::ncpu() { return this->_mNcpus; }

/**
 * @brief Qjob::priority Returns the current priority for this job
 * @return priority for this job
 */
qreal Qjob::priority() { return this->_mPriority; }

/**
 * @brief Qjob::status Returns the current status for the job
 * @return status integer
 */
int Qjob::status() { return this->_mStatus; }

/**
 * @brief Qjob::node Returns the main node from the queue
 * @return String containing the core listed in the queue
 */
QString Qjob::node() { return this->_mNode; }

/**
 * @brief Qjob::user Returns the user for this job
 * @return user name for this job
 */
QString Qjob::user() { return this->_mUser; }

/**
 * @brief Qjob::time Returns the submit/start time for this job
 * @return Submit or start time for this job
 */
QDateTime Qjob::time() { return this->_mTime; }

/**
 * @brief Qjob::addQueueHash Adds the input queue to the list of queues this job
 * uses resources from
 * @param hash Hash for the queue to add to the list
 */
void Qjob::addQueueHash(QByteArray hash) {
  if (!this->_mQueueHash.contains(hash))
    this->_mQueueHash.append(hash);
  return;
}

/**
 * @brief Qjob::containsQueueHash Check if the hash for the input queue is
 * contained in the list for this job
 * @param hash hash to check
 * @return true if the hash is contained, false if it is not
 */
bool Qjob::containsQueueHash(QByteArray hash) {
  return this->_mQueueHash.contains(hash);
}

/**
 * @brief Qjob::setNcpu Set the number of CPUs used by this job
 * @param n number of CPUs
 */
void Qjob::setNcpu(int n) { this->_mNcpus = n; }

/**
 * @brief Qjob::setIsOnQueue Boolean value denoting if the job is part of the
 * queue of user interest
 * @param q boolean value determining if the job is on the queue of user
 * interest
 */
void Qjob::setIsOnQueue(bool q) { this->_mIsOnQueue = q; }

/**
 * @brief Qjob::isOnQueue Returns true if the job is on the queue of user
 * interest
 * @return boolean if the job is on the queue of user interest
 */
bool Qjob::isOnQueue() { return this->_mIsOnQueue; }
