/***************************************************************************
                               command.h
                              -------------
    copyright            : (C) 2018 Felix Salfelder
    email                : yes
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_COMMMAND_H
#define QUCS_COMMMAND_H

#include "components/component.h"

/*!
 * this is a bug. commands are not components, maybe elements?
 * decide later. we only need this base class.
 */
class Command : public Component {
};

#endif
