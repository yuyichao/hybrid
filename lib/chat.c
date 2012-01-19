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

#include "chat.h"

//half an hour
//TODO: preference
#define HYBRID_CHAT_SESSION_TIMEOUT 1800

enum {
    HYBRID_CHAT_SESSION_NEW,
    HYBRID_CHAT_SESSION_PRESENT,
    HYBRID_CHAT_SESSION_NEW_MESSAGE,
    HYBRID_CHAT_SESSION_DESTROY,
    HYBRID_CHAT_SESSION_LAST_SIGNAL
};

enum {
    HYBRID_CHAT_SESSION_PROP_0,
    HYBRID_CHAT_SESSION_PROP_UNREAD,
    HYBRID_CHAT_SESSION_PROP_SELF_STATE,
    HYBRID_CHAT_SESSION_PROP_BUDDY_STATE,
    HYBRID_CHAT_SESSION_PROP_TITLE
};

static gint hybrid_chat_session_signals[HYBRID_CHAT_SESSION_LAST_SIGNAL] = {0};

static void _hybrid_chat_session_init(HybridChatSession *session);
static void _hybrid_chat_session_class_init(HybridChatSessionClass *klass);
static void _hybrid_chat_session_set_property(GObject *obj, guint prop_id,
                                              const GValue *value,
                                              GParamSpec *pspec);
static void _hybrid_chat_session_get_property(GObject *obj, guint prop_id,
                                              GValue *value, GParamSpec *pspec);
static void _hybrid_chat_session_dispose(GObject *obj);
static void _hybrid_chat_session_finalize(GObject *obj);

static GList *session_list = NULL;

GType
hybrid_chat_session_get_type()
{
    static GType hybrid_chat_session_type = 0;
    if(!hybrid_chat_session_type) {
        static const GTypeInfo hybrid_chat_session_info = {
            sizeof(HybridChatSessionClass), NULL,NULL,
            (GClassInitFunc)_hybrid_chat_session_class_init, NULL,NULL,
            sizeof(HybridChatSession), 0,
            (GInstanceInitFunc)_hybrid_chat_session_init
        };
        hybrid_chat_session_type = g_type_register_static(
            G_TYPE_OBJECT, "HybridChatSession", &hybrid_chat_session_info, 0);
    }
    return hybrid_chat_session_type;
}

static gpointer _chat_session_class = NULL;

void
hybrid_chat_session_init()
{
    if (!_chat_session_class)
        _chat_session_class = g_type_class_ref(HYBRID_TYPE_CHAT_SESSION);
}

static void
_hybrid_chat_session_init(HybridChatSession *session)
{
    session->account = NULL;
    session->id = NULL;
    session->title = NULL;
    session->messages = NULL;
    session->timeout = 0;
}

static void
_hybrid_chat_session_class_init(HybridChatSessionClass *sessionclass)
{
    GObjectClass *g_class = G_OBJECT_CLASS(sessionclass);
    GParamSpec *pspec;

    g_class->set_property = _hybrid_chat_session_set_property;
    g_class->get_property = _hybrid_chat_session_get_property;
    g_class->dispose = _hybrid_chat_session_dispose;
    g_class->finalize = _hybrid_chat_session_finalize;

    /* Notification system register to the notify signal of this property, */
    /* and also of buddy-state property if necessary. */
    pspec = g_param_spec_boolean("unread", "Unread",
                                 "Whether there is unread message.",
                                 FALSE, G_PARAM_READABLE | G_PARAM_WRITABLE |
                                 G_PARAM_CONSTRUCT);
    g_object_class_install_property(g_class,
                                    HYBRID_CHAT_SESSION_PROP_UNREAD,
                                    pspec);

    pspec = g_param_spec_int("self-state", "Self Typing State",
                             "Whether the user is typing.",
                             HYBRID_TYPE_NONE, HYBRID_TYPE_STOPPED,
                             HYBRID_TYPE_NONE,
                             G_PARAM_READABLE | G_PARAM_WRITABLE |
                             G_PARAM_CONSTRUCT);
    g_object_class_install_property(g_class,
                                    HYBRID_CHAT_SESSION_PROP_SELF_STATE,
                                    pspec);

    pspec = g_param_spec_int("buddy-state", "Buddy Typing State",
                             "Whether the buddy is typing.",
                             HYBRID_TYPE_NONE, HYBRID_TYPE_STOPPED,
                             HYBRID_TYPE_NONE,
                             G_PARAM_READABLE | G_PARAM_WRITABLE |
                             G_PARAM_CONSTRUCT);
    g_object_class_install_property(g_class,
                                    HYBRID_CHAT_SESSION_PROP_BUDDY_STATE,
                                    pspec);

    pspec = g_param_spec_string("title", "Title",
                                 "The title of the session",
                                 "", G_PARAM_READABLE | G_PARAM_WRITABLE |
                                 G_PARAM_CONSTRUCT);
    g_object_class_install_property(g_class,
                                    HYBRID_CHAT_SESSION_PROP_TITLE,
                                    pspec);

    /* for hook only */
    hybrid_chat_session_signals[HYBRID_CHAT_SESSION_NEW] =
        g_signal_new("new", HYBRID_TYPE_CHAT_SESSION,
                     G_SIGNAL_RUN_FIRST, 0, NULL,NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL);
    hybrid_chat_session_signals[HYBRID_CHAT_SESSION_PRESENT] =
        g_signal_new("present", HYBRID_TYPE_CHAT_SESSION,
                     G_SIGNAL_RUN_FIRST | G_SIGNAL_DETAILED, 0, NULL,NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL);
    hybrid_chat_session_signals[HYBRID_CHAT_SESSION_NEW_MESSAGE] =
        g_signal_new("new-message", HYBRID_TYPE_CHAT_SESSION,
                     G_SIGNAL_RUN_FIRST | G_SIGNAL_DETAILED, 0, NULL,NULL,
                     g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE,
                     1, G_TYPE_POINTER, NULL);
    hybrid_chat_session_signals[HYBRID_CHAT_SESSION_DESTROY] =
        g_signal_new("destroy", HYBRID_TYPE_CHAT_SESSION, G_SIGNAL_RUN_FIRST, 0,
                     NULL,NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
                     0, NULL);
}

static void
_hybrid_chat_session_dispose(GObject *obj)
{
    HybridChatSession *session = HYBRID_CHAT_SESSION(obj);
    g_list_free_full(session->messages, (GDestroyNotify)hybrid_message_free);
    session->messages = NULL;
    g_signal_emit(session,
                  hybrid_chat_session_signals[HYBRID_CHAT_SESSION_DESTROY], 0);
}

static void
_hybrid_chat_session_finalize(GObject *obj)
{
    HybridChatSession *session = HYBRID_CHAT_SESSION(obj);
    if (session->id)
        g_free(session->id);
    if (session->title)
        g_free(session->title);
}

static HybridChatSession*
_hybrid_chat_session_new(HybridAccount *account, const gchar *id)
{
    HybridChatSession *session;
    session = g_object_new(HYBRID_TYPE_CHAT_SESSION, NULL);
    session->account = account;
    session->id = g_strdup(id);
    return session;
}

//Remove after standardize hybrid buddy
typedef struct {
    HybridAccount *account;
    const gchar *id;
} HybridBuddyTemp;

static gint
hybrid_buddy_comp(HybridChatSession *session, HybridBuddyTemp *buddy)
{
    if (session->account == buddy->account &&
        !(g_strcmp0(session->id, buddy->id)))
        return 0;
    return 1;
}

static HybridChatSession*
_hybrid_find_chat_session(HybridAccount *account, const gchar *id)
{
    HybridBuddyTemp buddy = {
        .account = account,
        .id = id
    };
    GList *res;
    res = g_list_find_custom(session_list, &buddy,
                             (GCompareFunc)hybrid_buddy_comp);

    return res ? (HybridChatSession*)res->data : NULL;
}

static gboolean
_hybrid_chat_session_unref_cb(HybridChatSession *session)
{
    g_return_val_if_fail(HYBRID_IS_CHAT_SESSION(session), FALSE);

    session->timeout = 0;
    g_object_unref(session);

    return FALSE;
}

static gboolean
_hybrid_chat_session_add_to(HybridChatSession *session)
{
    //LOCK?
    if (session->to)
        return FALSE;

    session->to = g_timeout_add_second(
        HYBRID_CHAT_SESSION_TIMEOUT,
        (GSourceFunc)_hybrid_chat_session_unref_cb, session);

    return TRUE;
}

static gboolean
_hybrid_chat_session_rm_to(HybridChatSession *session)
{
    if (!session->to)
        return FALSE;

    g_source_remove(session->to);

    session->to = 0;

    return TRUE;
}

HybridChatSession*
hybrid_chat_session_newv(HybridAccount *account, const gchar *id,
                         const gchar *first_prop_name, va_list var_args)
{
    HybridChatSession *session;
    //LOCK?
    session = _hybrid_find_chat_session(account, id);

    if (!session) {
        session = _hybrid_chat_session_new(account, id);
    } else {
        g_object_ref(session);
    }

    g_object_set_valist(G_OBJECT(session), first_prop_name, var_args);
    g_signal_emit(session,
                  hybrid_chat_session_signals[HYBRID_CHAT_SESSION_NEW], NULL);
    _hybrid_chat_session_add_to(session);
    return session;
}

HybridChatSession*
hybrid_chat_session_new(HybridAccount *account, const gchar *id,
                        const gchar *first_prop_name, ...)
{
    HybridChatSession *session;
    va_list var_args;

    va_start(var_args, first_prop_name);
    session = hybrid_chat_session_newv(account, id, first_prop_name,
                                       var_args);
    va_end(var_args);

    return session;
}

static void
default_send_cb(HybridChatSession *session, HybridMessage *msg, gpointer data)
{
    //TODO: Call the default send method if data is NULL
}

static void
default_self_state_cb(HybridChatSession *session, GParamSpec *pspec,
                      gpointer data)
{
    //TODO: Call the default state-change callback if data is NULL
}

void
hybrid_chat_session_connect(HybridChatSession *session, ...)
{
    va_list var_args;
    HybridChatSessionCb type;
    gpointer funp;

    va_start(var_args, session);

    type = va_arg(var_args, HybridChatSessionCb);
    for (;type;type = va_arg(var_args, HybridChatSessionCb)) {
        funp = va_arg(var_args, gpointer);
        if (type == HYBRID_CHAT_SESSION_CB_MSG) {
            g_signal_connect(session, "new-message::out",
                             G_CALLBACK(default_send_cb), funp);
        } else if (type == HYBRID_CHAT_SESSION_CB_STATE) {
            g_signal_connect(session, "notify::self-state",
                             G_CALLBACK(default_self_state_cb), funp);
        } else {
            hybrid_debug_error(__func__, "Unknown callback type %d.", type);
            break;
        }
    }

    va_end(var_args);
}


HybridChatSession *
hybrid_chat_session_new_default(HybridAccount *account, const gchar *id,
                                const gchar *hint,
                                const gchar *first_prop_name, ...)
{
    HybridChatSession *session;
    va_list var_args;

    va_start(var_args, first_prop_name);
    session = hybrid_chat_session_newv(account, id, first_prop_name, var_args);
    va_end(var_args);
    hybrid_chat_session_connect_defaults(session);
    hybrid_chat_session_present(session, hint);
    return session;
}

void
hybrid_chat_session_present(HybridChatSession *session, const gchar *hint)
{
    g_signal_emit(session,
                  hybrid_chat_session_signals[HYBRID_CHAT_SESSION_PRESENT],
                  g_quark_from_string(hint));
}


static void
_hybrid_chat_session_set_property(GObject *obj, guint prop_id,
                                  const GValue *value, GParamSpec *pspec)
{
    HybridChatSession *self = HYBRID_CHAT_SESSION(obj);

    switch (prop_id) {
    case HYBRID_CHAT_SESSION_PROP_UNREAD:
        hybrid_chat_session_set_unread(self, g_value_get_boolean(value));
        break;
    case HYBRID_CHAT_SESSION_PROP_SELF_STATE:
        hybrid_chat_session_set_self_state(self, g_value_get_int(value));
        break;
    case HYBRID_CHAT_SESSION_PROP_BUDDY_STATE:
        hybrid_chat_session_set_buddy_state(self, g_value_get_int(value));
        break;
    case HYBRID_CHAT_SESSION_PROP_TITLE:
        hybrid_chat_session_set_title(self, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
        break;
    }
}

static void
_hybrid_chat_session_get_property(GObject *obj, guint prop_id,
                                  GValue *value, GParamSpec *pspec)
{
    HybridChatSession *self = HYBRID_CHAT_SESSION(obj);

    switch (prop_id) {
    case HYBRID_CHAT_SESSION_PROP_UNREAD:
        g_value_set_boolean(value, self->unread);
        break;
    case HYBRID_CHAT_SESSION_PROP_SELF_STATE:
        g_value_set_int(value, self->self_state);
        break;
    case HYBRID_CHAT_SESSION_PROP_BUDDY_STATE:
        g_value_set_int(value, self->buddy_state);
        break;
    case HYBRID_CHAT_SESSION_PROP_TITLE:
        if (self->title)
            g_value_set_string(value, self->title);
        else
            g_value_set_string(value, "");
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
        break;
    }
}


gboolean
hybrid_chat_session_get_unread(HybridChatSession *session)
{
    return session->unread;
}

void
hybrid_chat_session_set_unread(HybridChatSession *session, gboolean unread)
{
    session->unread = unread;
    g_object_notify(G_OBJECT(session), "unread");
}

char *hybrid_chat_session_get_title(HybridChatSession *session)
{
    return g_strdup(session->title);
}

void hybrid_chat_session_set_title(HybridChatSession *session,
                                   const char *title)
{
    if (session->title)
            g_free(session->title);
    session->title = g_strdup(title);
    g_object_notify(G_OBJECT(session), "title");
}

HybridTypeState
hybrid_chat_session_get_self_state(HybridChatSession *session)
{
    return session->self_state;
}

void
hybrid_chat_session_set_self_state(HybridChatSession *session,
                                   HybridTypeState state)
{
    session->self_state = state;
    g_object_notify(G_OBJECT(session), "self-state");
}

HybridTypeState
hybrid_chat_session_get_buddy_state(HybridChatSession *session)
{
    return session->buddy_state;
}

void
hybrid_chat_session_set_buddy_state(HybridChatSession *session,
                                    HybridTypeState state)
{
    session->buddy_state = state;
    g_object_notify(G_OBJECT(session), "buddy-state");
}

void
hybrid_chat_session_got_message(HybridChatSession *session,
                                HybridMessage *msg)
{
    /* LOCK ? */
    session->messages = g_list_append(session->messages, msg);
    g_signal_emit(session,
                  hybrid_chat_session_signals[HYBRID_CHAT_SESSION_NEW_MESSAGE],
                  msg->in ? g_quark_from_static_string("in") :
                  g_quark_from_static_string("out"), msg);
}

void
hybrid_chat_session_set_filter(HybridChatSession *session,
                               const gchat* name, gint value)
{
    guint old = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(session), name));
    g_object_set_data(G_OBJECT(session), name, GINT_TO_POINTER(value));

    if (!((!old) ^ (!value)))
        return;

    if (value) {
        g_object_ref(session);
    } else {
        g_object_unref(session);
    }
}

gint
hybrid_chat_session_get_filter(HybridChatSession *session, const gchat* name)
{
    return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(session), name));
}

HybridMessage*
hybrid_message_new(time_t time, gboolean in)
{
    HybridMessage *msg = g_new0(HybridMessage, 1);
    msg->content = NULL;
    msg->time = time;
    msg->in = in;
    return msg;
}

void
hybrid_message_free(HybridMessage *msg)
{
    if (msg->content)
        g_free(msg->content);
    g_free(msg);
}

gchar*
hybrid_message_get_content(HybridMessage *msg)
{
    if (msg->content)
        return g_strdup(msg->content);
    return NULL;
}

const gchar*
hybrid_message_content(HybridMessage *msg)
{
    return msg->content;
}

void
hybrid_message_set_content(HybridMessage *msg, const gchar *content)
{
    if (msg->content)
        g_free(msg->content);
    if (content) {
        msg->content = NULL;
    } else {
        msg->content = g_strdup(content);
    }
}
