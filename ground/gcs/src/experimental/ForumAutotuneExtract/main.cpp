/**
 ******************************************************************************
 * @file       main.cpp
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @addtogroup [Group]
 * @{
 * @addtogroup
 * @{
 * @brief [Brief]
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <QCoreApplication>
#include "../../libs/utils/phpbb.h"
#include "forumextract.h"

#include <QDebug>

#define FORUM_FORUM     24
#define FORUM_THREAD    255

#define USERNAME    ""
#define PASSWORD    ""

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Utils::PHPBB php("http://forum.taulabs.org", &a);
    forumextract ext;
    qDebug() << "LOGIN SUCCESS:" << php.login(USERNAME, PASSWORD);
    QList<Utils::PHPBB::forumPost> list = php.getAllPosts(FORUM_FORUM, FORUM_THREAD);
    QList<forumextract::autotune> alist = ext.parsePosts(list);
    foreach (forumextract::autotune tune, alist) {
        //DO WHAT IS NEEDED
        qDebug()<< tune.computed_values.pitch.rateKi;
    }
    return a.exec();
}
