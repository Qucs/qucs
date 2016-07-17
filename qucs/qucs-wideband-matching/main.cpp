/*
 * main.cpp - Wideband matching network synthesis tool
 *
 * copyright (C) 2016 Andres Martinez-Mera <andresmartinezmera@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */
#include "ui.h"
#include <QApplication>
#include "mat.h"
#include "MathOperations.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ui *WB_MatchingTool = new ui();
    WB_MatchingTool->raise();
    WB_MatchingTool->resize(350, 350);
    WB_MatchingTool->show();
    return app.exec();
}
