#-----GPL----------------------------------------------------------------------
#
# This file is part of the arcadis-util library
# Copyright (C) 2010-2016  Arcadis
#
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#  contact: Zachary Cobell, zachary.cobell@arcadis.com
#  Arcadis
#  11001 W. 120th Ave, Suite 200
#  Broomfield, CO 80021
#
#  All indications and logos of, and references to, "Arcadis" 
#  are registered trademarks of Arcadis, and remain the property of
#  Arcadis. All rights reserved.
#
#------------------------------------------------------------------------------
#
#  File: qview.pro
#
#------------------------------------------------------------------------------

QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += qview.cpp \
    viewqueue.cpp \
    qstat.cpp \
    queue.cpp \
    qjob.cpp

HEADERS += \
    viewqueue.h \
    qstat.h \
    queue.h \
    qjob.h
