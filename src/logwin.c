/***************************************************************************
 *   Copyright (C) 2011 by levin                                           *
 *   levin108@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.            *
 ***************************************************************************/

#include "logwin.h"

//not sure if there will be not_main log window
static GtkWidget *main_log_win;

GtkWidget *hybrid_log_win_new()
{
    GtkWidget *logwin;
    GtkWidget *vbox;
    GtkWidget *button_box;
    GtkWidget *paned;

    logwin = hybrid_create_window(_("Chat Log"), NULL,
                                  GTK_WIN_POS_CENTER, TRUE);

    gtk_window_set_role(logwin, "chat-log");
    vbox = gtk_vbox_new(FALSE, 0);
    paned = gtk_hpaned_new();
    button_box = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CANTAINER(vbox), button_box);
    gtk_container_add(GTK_CANTAINER(vbox), paned);


    return logwin;
}
