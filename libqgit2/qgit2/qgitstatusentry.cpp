/******************************************************************************
 * This file is part of the libqgit2 library
 * Copyright (C) 2013 Leonardo Giordani
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <QtCore/QDir>
#include <QtCore/QFile>

#include "qgitstatusentry.h"
#include "qgitstatus.h"
#include "qgitdiffdelta.h"

using namespace LibQGit2;

StatusEntry::StatusEntry(const git_status_entry *entry)
    : d(entry)
{
}

StatusEntry::StatusEntry(const StatusEntry &other)
    : d(other.d)
{
}

StatusEntry::~StatusEntry()
{
}

Status StatusEntry::status() const
{
    return Status(d->status);
}

DiffDelta StatusEntry::headToIndex() const
{
    return DiffDelta(d->head_to_index);
}

DiffDelta StatusEntry::indexToWorkdir() const
{
    return DiffDelta(d->index_to_workdir);
}
