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

#ifndef HYBRID_CHAT_H
#define HYBRID_CHAT_H

typedef struct _HybridSession HybridSession;

/* Do not put Multi-sender/receiver support here, */
/* since plugins shouldn't deal with or be aware of this. */
typedef struct {
    HybridSession *session;
    const gchar *message;
    time_t time;
    gboolean in;
} HybridMessage;

typedef enum {
    HYBRID_NONE,
    HYBRID_TYPING,
    HYBRID_STOPPED
} HybridTypingState;

/* Abstract structure for a chat session */
/* signals: notify get-message send-message destroy */
struct _HybridSession {
    /* For properties and signals. */
    GObject parent;
    HybridAccount *account;

    gchar *buddy;
    gpointer data;

    /* timeout id, to remove session after certain amount of time. */
    gint timeout;

    /* Properties. */
    gboolean unread;
    HybridTypingState type_state;
    gchar *title;
};


#endif /* HYBRID_CHAT_H */
