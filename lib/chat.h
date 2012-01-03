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

#include <glib-object.h>
#include "account.h"

typedef struct _HybridChatSession HybridChatSession;
typedef struct _HybridChatSessionClass HybridChatSessionClass;

typedef struct {
    /* HybridChatSession *session; //Add back if necessary. */
    gchar *content;
    time_t time;
    gboolean in;
} HybridMessage;

HybridMessage *hybrid_message_new(time_t time, gboolean in);
void hybrid_message_free(HybridMessage *msg);
gchar *hybrid_message_get_content(HybridMessage *msg);
void hybrid_message_set_content(HybridMessage *msg, const gchar *content);

typedef enum {
    HYBRID_TYPE_NONE,
    HYBRID_TYPE_TYPING,
    HYBRID_TYPE_STOPPED
} HybridTypeState;

#define HYBRID_TYPE_CHAT_SESSION (hybrid_chat_session_get_type())
#define HYBRID_CHAT_SESSION(obj)                                        \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), HYBRID_TYPE_CHAT_SESSION,        \
                                HybridChatSession))
#define HYBRID_IS_CHAT_SESSION(obj)                                     \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), HYBRID_TYPE_CHAT_SESSION))
#define HYBRID_CHAT_SESSION_CLASS(klass)                                \
    (G_TYPE_CHECK_CLASS_CAST((klass), HYBRID_TYPE_CHAT_SESSION,         \
                             HybridChatSessionClass))
#define HYBRID_IS_CHAT_SESSION_CLASS(klass)                             \
    (G_TYPE_CHECK_CLASS_TYPE((klass), HYBRID_TYPE_CHAT_SESSION))
#define HYBRID_CHAT_SESSION_GET_CLASS(obj)                              \
    (G_TYPE_INSTANCE_GET_CLASS((obj), HYBRID_TYPE_CHAT_SESSION,         \
                               HybridChatSessionClass))


/* Abstract structure for a chat session */
/* signals: notify get-message send-message destroy */
/*  new(for showing dialog, with para in/out) */
struct _HybridChatSession {
    /* For properties and signals. */
    GObject parent;
    HybridAccount *account;

    gchar *id;
    gpointer data;

    GList *messages;

    /* Properties. */
    gboolean unread;
    HybridTypeState self_state;
    HybridTypeState buddy_state;
    gchar *title;
};

struct _HybridChatSessionClass {
    GObjectClass parent_class;
};

void hybrid_chat_session_init();

HybridChatSession *hybrid_chat_session_new(HybridAccount *account,
                                           const gchar *id, gboolean in,
                                           const gchar *first_prop_name, ...);

void hybrid_chat_session_set_unread(HybridChatSession *session,
                                    gboolean unread);

gboolean hybrid_chat_session_get_unread(HybridChatSession *session);

void hybrid_chat_session_set_self_state(HybridChatSession *session,
                                        HybridTypeState state);

HybridTypeState hybrid_chat_session_get_self_state(HybridChatSession *session);

void hybrid_chat_session_set_buddy_state(HybridChatSession *session,
                                         HybridTypeState state);

HybridTypeState hybrid_chat_session_get_buddy_state(HybridChatSession *session);

void hybrid_chat_session_set_title(HybridChatSession *session,
                                   const gchar *title);

gchar *hybrid_chat_session_get_title(HybridChatSession *session);

void hybrid_chat_session_got_message(HybridChatSession *session,
                                     HybridMessage *msg);

#endif /* HYBRID_CHAT_H */
