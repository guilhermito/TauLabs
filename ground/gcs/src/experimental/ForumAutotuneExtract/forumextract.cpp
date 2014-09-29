/**
 ******************************************************************************
 * @file       forumextract.cpp
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

#include "forumextract.h"

forumextract::forumextract()
{
}

QList<forumextract::autotune> forumextract::parsePosts(QList<Utils::PHPBB::forumPost> postList)
{
    int start_index;
    int end_index;
    QList<forumextract::autotune> list;
    foreach (Utils::PHPBB::forumPost post, postList) {
        start_index = 0;
        end_index = 0;
        forumextract::autotune item;
        item.post = post;
        start_index = indexOfAdjusted(post.text, start_index, "Aircraft description</span>");
        end_index = post.text.indexOf("<br />", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.description = post.text.mid(start_index, end_index - start_index);
        start_index = indexOfAdjusted(post.text, start_index, "Observations</span>");
        end_index = post.text.indexOf("<br />", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.observations = post.text.mid(start_index, end_index - start_index);
        start_index = indexOfAdjusted(post.text, start_index, "Roll</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.measured_properties.roll.gain = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.measured_properties.roll.bias = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.measured_properties.roll.tau = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.measured_properties.roll.noise = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "Pitch</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.measured_properties.pitch.gain = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.measured_properties.pitch.bias = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.measured_properties.pitch.tau = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.measured_properties.pitch.noise = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "Damping</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.tuning_aggressiveness.damping = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "Noise sensitivity</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.tuning_aggressiveness.noise_sensitivity = post.text.mid(start_index, end_index - start_index - 2).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "Natural frequency</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.tuning_aggressiveness.natural_frequency = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "Roll</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.computed_values.roll.rateKp = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.computed_values.roll.rateKi = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.computed_values.roll.rateKd = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "Pitch</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.computed_values.pitch.rateKp = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.computed_values.pitch.rateKi = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "<td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.computed_values.pitch.rateKd = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "Outer Kp</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.computed_values.outerKp = post.text.mid(start_index, end_index - start_index).toDouble();
        start_index = indexOfAdjusted(post.text, start_index, "Derivative cutoff</span></td> <!-- tdtd --> <td>");
        end_index = post.text.indexOf("</td>", start_index);
        if(start_index == -1 || end_index == -1) continue;
        item.computed_values.derivativeCutOff = post.text.mid(start_index, end_index - start_index).toDouble();
        list.append(item);
    }
    return list;
}

int forumextract::indexOfAdjusted(QString source, int from, QString strToMatch)
{
    return source.indexOf(strToMatch, from) + strToMatch.length();
}
