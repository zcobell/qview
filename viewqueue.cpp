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
//  File: viewqueue.cpp
//
//------------------------------------------------------------------------------

#include "viewqueue.h"
#include <QTextStream>

/**
 * @brief ViewQueue::ViewQueue Default constructor
 * @param parent Pointer to parent object
 */
ViewQueue::ViewQueue(QObject *parent) : QObject(parent) {
  this->_mQueueStat = new Qstat(this);
}

/**
 * @brief ViewQueue::run Run the qview code
 */
void ViewQueue::run() {
  int index;
  QTextStream output(stdout);
  QTextStream input(stdin);

  output << "Select CRC Subsystem:\n";
  for (int i = 0; i < this->_mQueueStat->numQueues(); i++)
    output << "(" << i + 1 << ") " << this->_mQueueStat->queue(i)->machine()
           << " - " << this->_mQueueStat->queue(i)->queueName() << "\n";
  output << "==> ";
  output.flush();
  input >> index;

  this->_mQueueStat->run(this->_mQueueStat->queue(index - 1)->hash());
  emit finished();

  return;
}
