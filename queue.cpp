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
//  File: queue.cpp
//
//------------------------------------------------------------------------------

#include "queue.h"
#include <QEventLoop>
#include <QProcess>
#include <QStringList>

/**
 * @brief Queue::Queue Default constructor
 * @param machineName Name of machine
 * @param queueName Name of queue
 * @param queueCore Name of cores
 * @param queueStart Start index for queue
 * @param queueEnd End index for queue
 * @param coreSize Number of processors on each core
 * @param nameFormat Number of digits in each core
 * @param parent Pointer to parent object
 */
Queue::Queue(QString machineName, QString queueName, QString queueCore,
             int queueStart, int queueEnd, int coreSize, int nameFormat,
             QObject *parent)
    : QObject(parent) {
  this->_mMachineName = machineName;
  this->_mQueueName = queueName;
  this->_mNodeName = queueCore;
  this->_mNodeStart = queueStart;
  this->_mNodeEnd = queueEnd;
  this->_mCoreSize = coreSize;
  this->_mNRange = 1;
  this->_mNodeStart2 = -1;
  this->_mNodeEnd2 = -1;
  this->_mDownNodes = 0;
  this->_mUpNodes = 0;
  this->_mIdleNodes = 0;
  this->_mRunningNodes = 0;
  this->_mIdleCores = 0;
  this->_mRunningCores = 0;
  this->_mNameFormat = nameFormat;
  this->_hash();
  this->_calculateSize();
}

/**
 * @brief Queue::Queue Default constructor
 * @param machineName Name of machine
 * @param queueName Name of queue
 * @param queueCore Name of cores
 * @param queueStart Start index for first set of nodes
 * @param queueEnd End index for first set of nodes
 * @param queueStart2 Start index for second set of nodes
 * @param queueEnd2 End index for second set of nodes
 * @param coreSize Number of processors on each core
 * @param nameFormat Number of digits in each core
 * @param parent Pointer to parent object
 */
Queue::Queue(QString machineName, QString queueName, QString queueCore,
             int queueStart, int queueEnd, int queueStart2, int queueEnd2,
             int coreSize, int nameFormat, QObject *parent)
    : QObject(parent) {
  this->_mMachineName = machineName;
  this->_mQueueName = queueName;
  this->_mNodeName = queueCore;
  this->_mNodeStart = queueStart;
  this->_mNodeEnd = queueEnd;
  this->_mNodeStart2 = queueStart2;
  this->_mNodeEnd2 = queueEnd2;
  this->_mCoreSize = coreSize;
  this->_mNRange = 2;
  this->_mDownNodes = 0;
  this->_mUpNodes = 0;
  this->_mIdleNodes = 0;
  this->_mRunningNodes = 0;
  this->_mIdleCores = 0;
  this->_mRunningCores = 0;
  this->_mNameFormat = nameFormat;
  this->_hash();
  this->_calculateSize();
}

/**
 * @brief Queue::_hash Generates a unique hash for the queue for searching later
 */
void Queue::_hash() {
  QCryptographicHash hash(QCryptographicHash::Sha1);
  hash.reset();
  hash.addData(this->_mMachineName.toUtf8());
  hash.addData(this->_mQueueName.toUtf8());
  hash.addData(this->_mNodeName.toUtf8());
  hash.addData(QString::number(this->_mNodeStart).toUtf8());
  hash.addData(QString::number(this->_mNodeEnd).toUtf8());
  hash.addData(QString::number(this->_mNodeStart2).toUtf8());
  hash.addData(QString::number(this->_mNodeEnd2).toUtf8());
  hash.addData(QString::number(this->_mCoreSize).toUtf8());
  this->_mHash = hash.result().toHex();
}

/**
 * @brief Queue::nameFormat Returns the format of the name for this queue
 * @return Integer with the number of digits used in this queue
 */
int Queue::nameFormat() { return this->_mNameFormat; }

/**
 * @brief Queue::_calculateSize Calculates the number of processors in this
 * queue
 */
void Queue::_calculateSize() {
  this->_mNcore = (this->_mNodeEnd - this->_mNodeStart + 1) * this->_mCoreSize;
  if (this->_mNRange == 2)
    this->_mNcore =
        this->_mNcore +
        (this->_mNodeEnd2 - this->_mNodeStart2 + 1) * this->_mCoreSize;
  return;
}

/**
 * @brief Queue::queueName Returns the name of this queue
 * @return name of this queue
 */
QString Queue::queueName() { return this->_mQueueName; }

/**
 * @brief Queue::machine Returns the name of this machine
 * @return name of this machine
 */
QString Queue::machine() { return this->_mMachineName; }

/**
 * @brief Queue::hash Returns the hash for this queue
 * @return queue hash
 */
QByteArray Queue::hash() { return this->_mHash; }

/**
 * @brief Queue::isInQueue Checks if the job specified is in the named queue
 * @param job pointer to a job
 * @param queueName name of a queue
 * @return boolean value for if the job is in the named queue
 */
bool Queue::isInQueue(Qjob *job, QString queueName) {

  if (job->status() == Qjob::SGE_STATUS_RUNNING) {
    if (job->core() == this->_mNodeName) {
      for (int i = this->_mNodeStart; i <= this->_mNodeEnd; i++) {
        if (job->containsCore(i))
          return true;
      }
      if (this->_mNRange == 2) {
        for (int i = this->_mNodeStart2; i <= this->_mNodeEnd2; i++) {
          if (job->containsCore(i))
            return true;
        }
      }
    } else
      return false;
    return false;
  } else {
    if (this->_mQueueName == queueName)
      return true;
    else
      return false;
  }
}

/**
 * @brief Queue::isOnNodes Checks if the job pointer is on this queue
 * @param testJob pointer to job
 * @return boolean if the job is in this queue
 */
bool Queue::isOnNodes(Qjob *testJob) {
  if (testJob->core() == this->_mNodeName) {
    if (this->_mNRange == 1) {
      if (testJob->coreNumber() >= this->_mNodeStart &&
          testJob->coreNumber() <= this->_mNodeEnd)
        return true;
      else
        return false;
    } else if (this->_mNRange == 2) {
      if (testJob->coreNumber() >= this->_mNodeStart &&
          testJob->coreNumber() <= this->_mNodeEnd)
        return true;
      else if (testJob->coreNumber() >= this->_mNodeStart2 &&
               testJob->coreNumber() <= this->_mNodeEnd2)
        return true;
      else
        return false;
    } else
      return false;
  } else
    return false;
}

/**
 * @brief Queue::getQueueHealth Gets the current health status of the queue
 */
void Queue::getQueueHealth() {
  QString cmd = "qstat -f";
  QEventLoop loop;
  QProcess command(this);
  QStringList splitString;
  QString node, load, tempString;
  int id, c1, c2, c3;
  bool isProcessor;
  bool ok;

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("IFS", "");

  command.start(cmd);
  command.setEnvironment(env.toStringList());
  connect(&command, SIGNAL(finished(int, QProcess::ExitStatus)), &loop,
          SLOT(quit()));
  loop.exec();

  QStringList queueData = QString(command.readAllStandardOutput()).split("\n");
  queueData = queueData.filter(this->_mNodeName);

  this->_mDownNodes = 0;
  this->_mUpNodes = 0;

  for (int i = 0; i < queueData.length(); i++) {
    splitString = queueData.value(i).simplified().split(" ");

    node = splitString.value(0);
    load = splitString.value(2);
    tempString = node.split(".").value(0);
    id = tempString.right(this->nameFormat()).toInt(&ok);

    isProcessor = false;
    if (id >= this->_mNodeStart && id <= this->_mNodeEnd)
      isProcessor = true;
    else if (this->_mNRange == 2 && id >= this->_mNodeStart2 &&
             id <= this->_mNodeEnd2)
      isProcessor = true;

    if (isProcessor) {
      if (splitString.length() == 6)
        this->_mDownNodes = this->_mDownNodes + 1;
      else {
        this->_mUpNodes = this->_mUpNodes + 1;

        c1 = load.split("/").value(0).toInt();
        c2 = load.split("/").value(1).toInt();
        c3 = load.split("/").value(2).toInt();

        if (c2 > 0) {
          this->_mRunningNodes = this->_mRunningNodes + 1;
          this->_mRunningCores = this->_mRunningCores + c2;
          this->_mIdleCores = this->_mIdleCores + (this->_mCoreSize - c2);
        } else if (c2 == 0) {
          this->_mIdleNodes = this->_mIdleNodes + 1;
          this->_mIdleCores = this->_mIdleCores + this->_mCoreSize;
        }
      }
    }
  }
}

/**
 * @brief Queue::queueDownNodes Gets the number of nodes that are considered
 * down
 * @return number of down nodes
 */
int Queue::queueDownNodes() { return this->_mDownNodes; }

/**
 * @brief Queue::queueIdleNodes Gets the number of nodes considered idle
 * @return number of idle nodes
 */
int Queue::queueIdleNodes() { return this->_mIdleNodes; }

/**
 * @brief Queue::queueRunningNodes Gets the number of nodes running jobs
 * @return Number of running nodes
 */
int Queue::queueRunningNodes() { return this->_mRunningNodes; }

/**
 * @brief Queue::queueTotalNodes Gets the total number of nodes in the queue
 * @return
 */
int Queue::queueTotalNodes() { return this->_mUpNodes + this->_mDownNodes; }

/**
 * @brief Queue::queueUpNodes Gets the number of up nodes
 * @return number of up nodes
 */
int Queue::queueUpNodes() { return this->_mUpNodes; }

/**
 * @brief Queue::queueTotalCores Gets the number of total cores in the queue
 * @return number of total cores
 */
int Queue::queueTotalCores() {
  return this->queueTotalNodes() * this->_mCoreSize;
}

/**
 * @brief Queue::queueFreeCores Gets the number of free cores
 * @return number of free cores
 */
int Queue::queueFreeCores() { return this->_mIdleCores; }

/**
 * @brief Queue::queueRunningCores Gets the number of running cores
 * @return number of running cores
 */
int Queue::queueRunningCores() { return this->_mRunningCores; }
