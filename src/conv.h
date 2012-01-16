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

#ifndef HYBRID_CONV_H
#define HYBRID_CONV_H

#include <gtk/gtk.h>
#include "logs.h"

#include "account.h"
#include "blist.h"
#include "chat.h"

typedef struct _HybridConversation HybridConversation;
typedef struct _HybridChatWindow   HybridChatWindow;
typedef struct _HybridChatTextOps  HybridChatTextOps;
typedef struct _HybridChatTheme    HybridChatTheme;

// typedef void (*ChatCallback)(HybridAccount *, const gchar *);

typedef GtkWidget* (*text_create)(void);
typedef void (*text_append)(GtkWidget *, HybridAccount *,
                            HybridBuddy *,	const gchar *, time_t);
typedef void (*text_notify)(GtkWidget *, const gchar *, gint);
typedef void (*theme_set_ops_func)(void);

struct _HybridChatTheme {
    const gchar        *name;
    theme_set_ops_func  func;
};

struct _HybridChatTextOps{
    text_create create;
    text_append append;
    text_notify notify;
};

struct _HybridConversation {
    GtkWidget *window;
    GtkWidget *notebook;
    GSList    *chat_buddies;
};

struct _HybridChatWindow {
    HybridConversation   *parent;
    HybridChatSession    *session;
    GdkPixbuf *icon;      /**< only used when it's user-defined window. */

    /* Also use signals instead. add hook to signal new of session */
    /* and connect to signal new-message and destroy for every new object */
    //HybridLogs *logs;           /* log context. */

    GtkWidget *pagelabel;


    GtkWidget *textview;
    GtkWidget *toolbar;
    GtkWidget *sendtext;
    GtkWidget *vbox;            /**< The Notebook child widget */

    /* label to show how many words left that can be input. */
    GtkWidget *words_left_label;

    /* tab label */
    GtkWidget   *tablabel;
    GtkTreeIter  tabiter;

    /* tip label */
    GtkWidget   *tiplabel;
    GtkTreeIter  tipiter;

    /* Use signals instead. */
    //ChatCallback callback;

    guint    typing_source;

    /* event source of the inputing timeout event. */
    guint input_source;

    //TODO: to be removed
    gpointer data;
};

/**
 * Notebook tab columns.
 */
enum {
    TAB_STATUS_ICON_COLUMN = 0,
    TAB_NAME_COLUMN,
    TAB_COLUMNS
};

/**
 * Notebook tips columns.
 */
enum {
    BUDDY_ICON_COLUMN = 0,
    BUDDY_NAME_COLUMN,
    BUDDY_STATUS_ICON_COLUMN,
    BUDDY_PROTO_ICON_COLUMN,
    LABEL_COLUMNS
};

enum {
    MSG_NOTIFICATION_INPUT,
    MSG_NOTIFICATION_ERROR
};

#ifdef __cplusplus
extern "C" {
#endif

    void hybrid_chat_window_init();

/**
 * Set the icon of the chat window, it's only used when
 * the window is a user-defined window, otherwise this function
 * will be ignored.
 *
 * @param window The user-defined chat window.
 * @param pixbuf The icon.
 */
    //TODO: probably move to session~~~ or sth similar
    // void hybrid_chat_window_set_icon(HybridChatWindow *window,
    //                                  GdkPixbuf *pixbuf);

/**
 * Find a chat window for buddy with the given buddy id.
 *
 * @param buddy_id ID of the buddy.
 *
 * @param chat NULL if not found.
 */
    /* AFAIK, the only use of this function (after using signals to handle *
     * messages etc.) is to find the right panel to activate from system   *
     * tray icon (because all others can be done just by the session       *
     * itself), and this one can also be done by using the data arguement  *
     * when connecting to the unread signal.                               */
    // HybridChatWindow *hybrid_conv_find_chat(const gchar *buddy_id);

    void hybrid_conv_got_message(HybridAccount *account,
                                 const gchar *buddy_id, const gchar *message,
                                 time_t time);

/**
 * Set the chat text ops.
 *
 * @param ops The ops methods.
 */
    void hybrid_conv_set_chat_text_ops(HybridChatTextOps *ops);

/**
 * Got a status message to display in the receiving window.
 *
 * @param account  The account.
 * @param buddy_id ID of the buddy to which the chat window belongs.
 * @param text     Content of the status message.
 * @param type     Tyep of the status.
 */
    void hybrid_conv_got_status(HybridAccount *account, const gchar *buddy_id,
                                const gchar *text, gint type);

/**
 * Got an buddy's inputing message.
 *
 * @param account   The account.
 * @param buddy_id  ID of the buddy to which the chat window belongs.
 * @param auto_stop If TRUE it will set the inputing state to be stoped automaticly.
 */
    void hybrid_conv_got_input(HybridAccount *account, const gchar *buddy_id,
                               gboolean auto_stop);

/**
 * Got an buddy's stopping inputing message.
 *
 * @param account   The account.
 * @param buddy_id  ID of the buddy to which the chat window belongs.
 */
    void hybrid_conv_stop_input(HybridAccount *account, const gchar *buddy_id);

/**
 * Clear the inputing message.
 *
 * @param account The account.
 * @param buddy_id ID of the buddy to which the chat window belongs.
 */
    void hybrid_conv_clear_input(HybridAccount *account, const gchar *buddy_id);

/**
 * Get the supported theme list.
 *
 * @return The theme list terminated with the name set to NULL.
 */
    HybridChatTheme *hybrid_chat_window_get_themes(void);

/**
 * Update the tips title of the chat window, usually used when
 * a new message is received, or messages are being read.
 */
    void hybrid_chat_window_update_tips(HybridChatWindow *window);

#ifdef __cplusplus
}
#endif

#endif
