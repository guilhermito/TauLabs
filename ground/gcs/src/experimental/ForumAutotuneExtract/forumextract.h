/**
 ******************************************************************************
 * @file       forumextract.h
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @addtogroup [Group]
 * @{
 * @addtogroup forumextract
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

#ifndef FORUMEXTRACT_H
#define FORUMEXTRACT_H

#include "../../libs/utils/phpbb.h"

class forumextract
{
public:
    struct autotune
    {
        Utils::PHPBB::forumPost post;
        QString description;
        QString observations;
        struct _measured_properties
        {
          struct _roll
          {
                double gain;
                double bias;
                double tau;
                double noise;
          } roll;
          struct _pitch
          {
                double gain;
                double bias;
                double tau;
                double noise;
          } pitch;
        }measured_properties;
        struct _tuning_aggressiveness
        {
            double damping;
            double noise_sensitivity;
            double natural_frequency;
        }tuning_aggressiveness;
        struct _computed_values
        {
            struct _roll
            {
                double rateKp;
                double rateKi;
                double rateKd;
            } roll;
            struct _pitch
            {
                double rateKp;
                double rateKi;
                double rateKd;
            } pitch;
            double outerKp;
            double derivativeCutOff;
        }computed_values;
    };

    forumextract();
    QList<autotune> parsePosts(QList<Utils::PHPBB::forumPost> postList);
private:
    int indexOfAdjusted(QString source, int from, QString strToMatch);
};

#endif // FORUMEXTRACT_H
