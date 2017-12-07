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
//  File: qstat.cpp
//
//------------------------------------------------------------------------------

#include "qstat.h"
#include <QEventLoop>
#include <QProcess>
#include <QTextStream>
#include <QXmlStreamReader>

/**
 * @brief Qstat::Qstat Default constructor
 * @param parent parent object pointer
 */
Qstat::Qstat(QObject *parent) : QObject(parent) { this->_initializeQueues(); }

/**
 * @brief Qstat::_initializeQueues Initializes the list of queues. Add any
 * additional queues here
 */
void Qstat::_initializeQueues() {
  this->_mQueues.push_back(new Queue("Aegaeon", "@@westerink_d12chas_1992",
                                     "d12chas", 20, 102, 24, 3, this));
  this->_mQueues.push_back(new Queue("Aegaeon", "@@westerink_d12chas_1488",
                                     "d12chas", 41, 102, 24, 3, this));
  this->_mQueues.push_back(new Queue("Aegaeon", "@@westerink_d12chas_1008",
                                     "d12chas", 41, 82, 24, 3, this));
  this->_mQueues.push_back(new Queue("Aegaeon", "@@westerink_d12chas_984",
                                     "d12chas", 20, 40, 83, 102, 24, 3, this));
  this->_mQueues.push_back(new Queue("Aegaeon", "@@westerink_d12chas_504",
                                     "d12chas", 20, 40, 24, 3, this));
  this->_mQueues.push_back(
      new Queue("Athos", "@@westerink_d6cneh", "d6cneh", 1, 83, 12, 3, this));
  this->_mQueues.push_back(new Queue("Proteus", "@@westerink_graphics",
                                     "proteus", 1, 2, 12, 1, this));

  for (int i = 0; i < this->_mQueues.size(); i++)
    this->_mQueueMap[this->_mQueues[i]->hash()] = this->_mQueues[i];

  return;
}

/**
 * @brief Qstat::run Runs the code to check the queue
 * @param hash Hash for the queue of user interest
 */
void Qstat::run(QByteArray hash) {
  for (int i = 0; i < this->_mQueues.size(); i++)
    if (hash == this->_mQueues[i]->hash())
      this->_mQueues[i]->getQueueHealth();

  int ierr = this->_getQueue();
  if (ierr == 0)
    this->_displayQueue(hash);
}

/**
 * @brief Qstat::numQueues Gets the number of queues that can be checked
 * @return number of queues that can be checked
 */
int Qstat::numQueues() { return this->_mQueues.size(); }

/**
 * @brief Qstat::queue Returns a pointer to a queue
 * @param index position in list of queues displayed to user
 * @return pointer to a queue
 */
Queue *Qstat::queue(int index) { return this->_mQueues[index]; }

/**
 * @brief Qstat::_displayQueue Displayes the queue status to the user
 * @param hash Queue to display to the user
 */
void Qstat::_displayQueue(QByteArray hash) {
  int nJobs = 0;
  QTextStream output(stdout);
  output << "\n";
  for (int i = 0; i < this->_mQueues.size(); i++) {
    if (this->_mQueues[i]->hash() == hash) {
      output << QString(_cyan + "Machine:" + _reset + " %1 \n " + _cyan +
                        " Queue:" + _reset + " %2")
                    .arg(this->_mQueues[i]->machine())
                    .arg(this->_mQueues[i]->queueName())
             << "\n";
      output << _cyan
             << "|-------------------------------------------------------------"
                "-------------------|\n";
      output << _cyan
             << "|   JID    |            Job Name            |    User    |   "
                "Status  |   Cores   |\n";
      output << _cyan
             << "|-------------------------------------------------------------"
                "-------------------|\n";
      for (int j = 0; j < this->_mJobs.length(); j++) {
        if (this->_mJobs[j]->containsQueueHash(this->_mQueues[i]->hash())) {
          nJobs = nJobs + 1;
          output << this->_formatJobOutputLine(this->_mJobs[j]);
        }
      }
      output << _cyan
             << "|-------------------------------------------------------------"
                "-------------------|\n";
      output.flush();
      output << "\n" << _reset;
      output << "SYSTEM STATUS"
             << "\n";
      output << "   RUNNING JOBS: " << nJobs << "\n\n";
      output.flush();
      this->_displayQueueHealth(this->_mQueues[i]);
      output << "Note: Jobs that fall between multiple queues are shown \n"
                "in each queue they use resources from.\n";
      output.flush();
      return;
    }
  }
  return;
}

/**
 * @brief Qstat::_formatJobOutputLine Formats the job data into a formatted
 * string
 * @param job job pointer to be formatted
 * @return Formatted string
 */
QString Qstat::_formatJobOutputLine(Qjob *job) {
  QString user = QProcessEnvironment::systemEnvironment().value("USER");
  QString jobnum, jobname, username, status, ncpu;

  jobnum.sprintf("%7.7s",
                 QString::number(job->jobNumber()).toStdString().c_str());
  jobname.sprintf("%30.30s", job->jobName().toStdString().c_str());
  username.sprintf("%10.10s", job->user().toStdString().c_str());
  status.sprintf("%9.9s", job->statusString().toStdString().c_str());
  ncpu.sprintf("%9.9s", QString::number(job->ncpu()).toStdString().c_str());

  QString output;
  output = _cyan + "| " + _reset + jobnum;
  output = output + _cyan + "  | " + _reset + jobname;

  if (job->user() == user)
    output = output + _cyan + " | " + _red + username;
  else
    output = output + _cyan + " | " + _reset + username;

  if (job->status() == Qjob::SGE_STATUS_RUNNING)
    output = output + _cyan + " | " + _green + status;
  else if (job->status() == Qjob::SGE_STATUS_PENDING)
    output = output + _cyan + " | " + _yellow + status;
  else if (job->status() == Qjob::SGE_STATUS_HELD)
    output = output + _cyan + " | " + _magenta + status;
  else if (job->status() == Qjob::SGE_STATUS_ERROR)
    output = output + _cyan + " | " + _red + status;
  else
    output = output + _cyan + " | " + _reset + status;

  output = output + _cyan + " | " + _reset + ncpu;
  output = output + _cyan + " | \n";
  return output;
}

/**
 * @brief Qstat::_displayQueueHealth Prints the current health of the queue
 * @param q queue for which to print the health to the screen
 */
void Qstat::_displayQueueHealth(Queue *q) {
  QTextStream output(stdout);
  output << "    TOTAL CORES: " << q->queueTotalCores() << "\n";
  output << "    AVAIL CORES: " << q->queueFreeCores() << "\n";
  output << "  RUNNING CORES: " << q->queueRunningCores() << "\n";
  output << "\n";
  output << "    TOTAL NODES: " << q->queueTotalNodes() << "\n";
  output << "       UP NODES: " << q->queueUpNodes() << "\n";
  output << "     DOWN NODES: " << q->queueDownNodes() << "\n";
  output << "     IDLE NODES: " << q->queueIdleNodes() << "\n";
  output << "\n";
  output.flush();
  return;
}

/**
 * @brief Qstat::_getQueue Runs qstat and parses the XML return data
 * @return status code
 */
int Qstat::_getQueue() {

  QString cmd = "qstat";
  QEventLoop loop;
  QProcess command(this);
  Qjob *tempJob;

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("IFS", "");

  command.start(cmd);
  command.setEnvironment(env.toStringList());
  connect(&command, SIGNAL(finished(int, QProcess::ExitStatus)), &loop,
          SLOT(quit()));
  loop.exec();

  //...Read the output from the blanket qstat command
  QStringList queueData = QString(command.readAllStandardOutput()).split("\n");

  //...Loop over the job list and save the ones
  //   that matter
  for (int i = 2; i < queueData.size() - 1; i++) {
    tempJob = new Qjob(this);
    tempJob->fromQueueLine(queueData.at(i));

    //...If running, check if the job is possibly in one of
    //   our queues of interest. Speeds up code.
    //   Queued jobs are still always checked
    if (tempJob->status() == Qjob::SGE_STATUS_RUNNING) {
      for (int j = 0; j < this->_mQueues.size(); j++) {
        if (this->_mQueues[j]->isOnNodes(tempJob)) {
          this->_getXML(tempJob);
          if (tempJob->isOnQueue()) {
            this->_mJobs.push_back(tempJob);
            break;
          }
        }
      }
    } else {
      this->_getXML(tempJob);
      if (tempJob->isOnQueue()) {
        this->_mJobs.push_back(tempJob);
        continue;
      }
    }
  }

  return 0;
}

/**
 * @brief Qstat::_getXML Runs qstat with xml output for additional job detail
 * @param testJob job to get the output for
 * @return status code
 */
int Qstat::_getXML(Qjob *testJob) {
  QString queueName, jobName, coreName;
  int nCore = 0;
  bool ok, foundCoreCount;

  QString cmd = "qstat -xml -j " + QString::number(testJob->jobNumber());

  QEventLoop loop;
  QProcess command(this);

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("IFS", "");

  command.start(cmd);
  command.setEnvironment(env.toStringList());
  connect(&command, SIGNAL(finished(int, QProcess::ExitStatus)), &loop,
          SLOT(quit()));
  loop.exec();

  QString data = command.readAllStandardOutput();

  QXmlStreamReader xmlParser(data);

  foundCoreCount = false;

  //...Loop over xml elements
  while (!xmlParser.atEnd() && !xmlParser.hasError()) {
    QXmlStreamReader::TokenType token = xmlParser.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (xmlParser.name() == "QR_name") {
        queueName = xmlParser.readElementText();
        if (queueName.left(1) == "*")
          queueName = queueName.right(queueName.length() - 1);
      } else if (xmlParser.name() == "RN_max" && !foundCoreCount) {
        foundCoreCount = true;
        nCore = xmlParser.readElementText().toInt();
      } else if (xmlParser.name() == "JB_job_name")
        jobName = xmlParser.readElementText();
      else if (xmlParser.name() == "PET_id") {
        coreName = xmlParser.readElementText().split(".").value(1);
        testJob->addCoreList(coreName.right(3).toInt(&ok));
        if (!ok)
          testJob->addCoreList(coreName.right(1).toInt(&ok));
      } else if (xmlParser.name() == "JG_qhostname") {
        coreName = xmlParser.readElementText().split(".").value(0);
        testJob->addCoreList(coreName.right(3).toInt(&ok));
        if (!ok)
          testJob->addCoreList(coreName.right(1).toInt(&ok));
      }
    }
  }

  //...Set the job info and locate the queue
  testJob->setNcpu(nCore);
  testJob->setJobName(jobName);
  this->_findQueue(testJob, queueName);

  return 0;
}

/**
 * @brief Qstat::_findQueue Finds the queues that a job participates in
 * @param testJob pointer to a job
 * @param queueName name of the queue to check
 * @return status code
 */
int Qstat::_findQueue(Qjob *testJob, QString queueName) {
  for (int i = 0; i < this->_mQueues.size(); i++) {
    if (this->_mQueues[i]->isInQueue(testJob, queueName)) {
      testJob->addQueueHash(this->_mQueues[i]->hash());
      testJob->setIsOnQueue(true);
    }
  }
  return 0;
}
