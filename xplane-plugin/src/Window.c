/*
 *  Window.c
 *  Wi-Fly-plugin
 *
 *  Created by Daniel Dickison on 5/11/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 * 
 *  This file is part of Wi-Fly-plugin.
 *  
 *  Wi-Fly-plugin is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  Wi-Fly-plugin is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with Wi-Fly-plugin.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "iX-Yoke-plugin.h"


int preset_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber);
int axis_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber);
int window_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int textfield_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int pause_checkbox_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int refresh_button_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int delete_button_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int save_button_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

void get_preset_menu_str(char *outStr);
void update_settings_display();

XPWidgetID window_id = 0;
XPWidgetID preset_popup_id = 0;
XPWidgetID host_label_id = 0;
XPWidgetID ip_label_id = 0;
XPWidgetID connection_label_id = 0;
XPWidgetID auto_pause_checkbox_id = 0;
XPWidgetID auto_resume_checkbox_id = 0;
XPWidgetID save_button_id = 0;
XPWidgetID delete_button_id = 0;

void show_save_preset_window();
void destroy_save_preset_window();
int get_save_preset_menu_str(char *outStr);

int save_preset_confirm_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int save_preset_cancel_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

XPWidgetID save_preset_window_id = 0;
XPWidgetID save_preset_textfield_id = 0;
XPWidgetID save_preset_popup_id = 0;



void destroy_window()
{
    if (window_id != 0)
    {
        iXDebug("Destroying config window...");
        XPDestroyWidget(window_id, 1);
        window_id = 0;
    }
}


void show_window()
{
    if (window_id == 0)
    {
        // Create config window.
        iXDebug("Creating config window...");
        
        int x1=200, y1=530, x2=500, y2=40;
        window_id = XPCreateWidget(x1, y1, x2, y2, 0, "Wi-Fly Remote", 1, NULL, xpWidgetClass_MainWindow);
        XPSetWidgetProperty(window_id, xpProperty_MainWindowHasCloseBoxes, 1);
        XPAddWidgetCallback(window_id, window_callback);
        
        x1 += 10; x2 -= 10;
        y1 -= 30; y2 += 10;
        
        // Add server info labels.
        host_label_id = XPCreateWidget(x1, y1, x2, y1-20, 1, "Host Name:", 0, window_id, xpWidgetClass_Caption);
        y1 -= 17;
        ip_label_id = XPCreateWidget(x1, y1, x2, y1-20, 1, "Host IP(s):", 0, window_id, xpWidgetClass_Caption);
        y1 -= 17;
        char port_str[64];
        snprintf(port_str, 64, "Host Port: %d", kServerPort);
        XPCreateWidget(x1, y1, x2, y1-20, 1, port_str, 0, window_id, xpWidgetClass_Caption);
        y1 -= 17;
        connection_label_id = XPCreateWidget(x1, y1, x2, y1-20, 1, "Connection:", 0, window_id, xpWidgetClass_Caption);
        y1 -= 23;
        
        // Add refresh button.
        XPWidgetID refresh_button = XPCreateWidget(x1, y1, x1+100, y1-20, 1, "Refresh", 0, window_id, xpWidgetClass_Button);
        XPSetWidgetProperty(refresh_button, xpProperty_ButtonType, xpButtonBehaviorPushButton);
        XPSetWidgetProperty(refresh_button, xpProperty_ButtonBehavior, xpButtonBehaviorPushButton);
        XPAddWidgetCallback(refresh_button, refresh_button_callback);
        y1 -= 30;
        
        // Add check boxes.
        XPCreateWidget(x1+20, y1, x2, y1-20, 1, "Auto-pause when disconnected", 0, window_id, xpWidgetClass_Caption);
        auto_pause_checkbox_id = XPCreateWidget(x1, y1, x1+20, y1-20, 1, "", 0, window_id, xpWidgetClass_Button);
        XPSetWidgetProperty(auto_pause_checkbox_id, xpProperty_ButtonType, xpButtonBehaviorCheckBox);
        XPSetWidgetProperty(auto_pause_checkbox_id, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
        XPAddWidgetCallback(auto_pause_checkbox_id, pause_checkbox_callback);
        y1 -= 17;
        
        XPCreateWidget(x1+20, y1, x2, y1-20, 1, "Auto-resume and hide window", 0, window_id, xpWidgetClass_Caption);
        auto_resume_checkbox_id = XPCreateWidget(x1, y1, x1+20, y1-20, 1, "", 0, window_id, xpWidgetClass_Button);
        XPSetWidgetProperty(auto_resume_checkbox_id, xpProperty_ButtonType, xpButtonBehaviorCheckBox);
        XPSetWidgetProperty(auto_resume_checkbox_id, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
        XPAddWidgetCallback(auto_resume_checkbox_id, pause_checkbox_callback);
        
        // Add subwindow and controls for each axis.
        y1 -= 30;
        XPWidgetID subwin = XPCreateWidget(x1, y1, x2, y2, 1, "", 0, window_id, xpWidgetClass_SubWindow);
        XPSetWidgetProperty(subwin, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);
        XPAddWidgetCallback(subwin, XPUFixedLayout);
        
        x1 += 10; x2 -= 10; y1 -= 10; y2 = y1-25;
        
        // Add preset controls.
        XPCreateWidget(x1, y1, x1+50, y1-20, 1, "Preset:", 0, subwin, xpWidgetClass_Caption);
        preset_popup_id = XPCreatePopup(x1+55, y1, x2, y2, 1, "Presets", subwin);
        XPAddWidgetCallback(preset_popup_id, preset_popup_callback);
        y1 -= 30;
        
        // Add preset save/delete buttons.
        delete_button_id = XPCreateWidget(x2-170, y1, x2-90, y1-20, 1, "Delete", 0, window_id, xpWidgetClass_Button);
        XPSetWidgetProperty(delete_button_id, xpProperty_ButtonType, xpButtonBehaviorPushButton);
        XPSetWidgetProperty(delete_button_id, xpProperty_ButtonBehavior, xpButtonBehaviorPushButton);
        XPAddWidgetCallback(delete_button_id, delete_button_callback);
        save_button_id = XPCreateWidget(x2-80, y1, x2, y1-20, 1, "Save...", 0, window_id, xpWidgetClass_Button);
        XPSetWidgetProperty(save_button_id, xpProperty_ButtonType, xpButtonBehaviorPushButton);
        XPSetWidgetProperty(save_button_id, xpProperty_ButtonBehavior, xpButtonBehaviorPushButton);
        XPAddWidgetCallback(save_button_id, save_button_callback);
        
        y1 -= 30; y2 = y1;
        
        for (int i = 0; i < kNumAxes; i++)
        {
            y1 = y2-10; y2 = y1-20;
            iXControlAxisRef control = get_axis((iXControlAxisID)i);
            
            // Axis title.
            XPCreateWidget(x1, y1, x1+100, y2, 1, control->title, 0, subwin, xpWidgetClass_Caption);
            
            // Popup
            control->popup_widget = XPCreatePopup(x1+120, y1, x2, y2, 1, axis_choices, subwin);
            XPAddWidgetCallback(control->popup_widget, axis_popup_callback);
            
            // Min, Max, progress
            y1 = y2-5; y2 = y1-20;
            control->min_widget = XPCreateWidget(x1, y1, x1+50, y2, 1, "-1", 0, subwin, xpWidgetClass_TextField);
            control->max_widget = XPCreateWidget(x2-50, y1, x2, y2, 1, "1", 0, subwin, xpWidgetClass_TextField);
            XPAddWidgetCallback(control->min_widget, textfield_callback);
            XPAddWidgetCallback(control->max_widget, textfield_callback);
            
            control->progress_widget = XPCreateWidget(x1+60, y1, x2-60, y2, 1, "", 0, subwin, xpWidgetClass_Progress);
            XPSetWidgetProperty(control->progress_widget, xpProperty_ProgressMin, 0);
            XPSetWidgetProperty(control->progress_widget, xpProperty_ProgressMax, 1024);
        }
    }
    
    update_settings_display();
    
    XPShowWidget(window_id);
}



int update_window()
{
    if (!window_id) return 0;
    
    if (currently_connected())
    {
        char label[256];
        snprintf(label, 256, "Connection: %d packets/second", get_packet_rate());
        XPSetWidgetDescriptor(connection_label_id, label);
    }
    else
    {
        char *msg = get_server_error_string();
        if (msg == NULL)
        {
            msg = "Connection: No signal";
        }
        XPSetWidgetDescriptor(connection_label_id, msg);
    }
    
    for (int i = 0; i < kNumAxes; i++)
    {
        iXControlAxisRef control = get_axis((iXControlAxisID)i);
        XPWidgetID progress = control->progress_widget;
        float value = control->value;
        long intValue = (long)(1024.0f * value);
        XPSetWidgetProperty(progress, xpProperty_ProgressPosition, intValue);
    }
    
    return 1;
}



int axis_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber)
{
    if (inMessage != xpMessage_PopupNewItemPicked)
    {
        return 0;
    }
    
    iXControlAxisRef control = NULL;
    for (int i = 0; i < kNumAxes; i++)
    {
        if (get_axis((iXControlAxisID)i)->popup_widget == (void*)inPopupID)
        {
            control = get_axis((iXControlAxisID)i);
            break;
        }
    }
    if (!control) return 0;
    
    if (control->type != inItemNumber)
    {
        control->type = (iXControlType)inItemNumber;
        set_current_preset(-1);
        update_settings_display();
        update_overrides();
    }
    
    return 1;
}


int window_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if (inMessage == xpMessage_CloseButtonPushed)
	{
        destroy_window();
        return 1;
    }
    
    return 0;
}



int textfield_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
    if (inMessage == xpMsg_KeyPress)
    {
        // Only allow numeric, decimal point and minus characters.
        XPKeyState_t *key_state = (XPKeyState_t *)inParam1;
        char key = key_state->key;
        char vkey = key_state->vkey;
        if (key == '\n' || key == '\r')
        {
            XPLoseKeyboardFocus(inWidget);
            return 1;
        }
        if ((key_state->flags & (xplm_ControlFlag | xplm_ShiftFlag | xplm_OptionAltFlag)) ||
            ((key < '0' || key > '9') && (key != '.') && (key != '-') &&
             (key != 0x7f) && (key != '\b') &&
             (vkey != XPLM_VK_LEFT) && (vkey != XPLM_VK_RIGHT) &&
             (vkey != XPLM_VK_ESCAPE)))
        {
            return 1;
        }
    }
    else if (inMessage == xpMsg_KeyLoseFocus)
    {
        XPWidgetID textfield = inWidget;
        iXControlAxisRef control = NULL;
        int isMin = 0;
        for (int i = 0; i < kNumAxes; i++)
        {
            iXControlAxisRef this_control = get_axis((iXControlAxisID)i);
            if (this_control->min_widget == textfield)
            {
                isMin = 1;
                control = this_control;
                break;
            }
            else if (this_control->max_widget == textfield)
            {
                isMin = 0;
                control = this_control;
                break;
            }
        }
        if (!control) return 0;
        
        char str[32];
        XPGetWidgetDescriptor(textfield, str, 32);
        str[31] = '\0'; // Null-terminate in case user typed >31 chars.
        float value = atof(str);
        int value_changed;
        if (isMin)
        {
            value_changed = control->min != value;
            control->min = value;
        }
        else
        {
            value_changed = control->max != value;
            control->max = value;
        }
        
        if (value_changed)
        {
            set_current_preset(-1);
            update_settings_display();
        }
        
        return 1;
    }
    return 0;
}


int preset_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber)
{
    if (inMessage == xpMessage_PopupNewItemPicked)
    {
        set_current_preset(inItemNumber - 1);
        save_prefs();
        update_settings_display();
        return 1;
    }
    return 0;
}


int pause_checkbox_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
    if (inMessage == xpMsg_ButtonStateChanged)
    {
        if (inWidget == auto_pause_checkbox_id)
        {
            set_pref_int(kPrefAutoPause, inParam2);
        }
        else if (inWidget == auto_resume_checkbox_id)
        {
            set_pref_int(kPrefAutoResume, inParam2);
        }
        save_prefs();
        return 1;
    }
    return 0;
}


int refresh_button_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
    if (inMessage == xpMsg_PushButtonPressed)
    {
        update_settings_display();
        return 1;
    }
    return 0;
}


int delete_button_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
    if (inMessage == xpMsg_PushButtonPressed)
    {
        delete_preset(current_preset());
        update_settings_display();
        return 1;
    }
    return 0;
}


int save_button_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
    if (inMessage == xpMsg_PushButtonPressed)
    {
        show_save_preset_window();
        return 1;
    }
    return 0;
}



void update_settings_display()
{
    char hostname[256];
    char ips[256];
    get_server_info(hostname, 256, ips, 256);
    
    char label[256];

    strcpy(label, "Host Name: ");
    strlcat(label, hostname, 256);
    XPSetWidgetDescriptor(host_label_id, label);
    
    strcpy(label, "Host IP(s): ");
    strlcat(label, ips, 256);
    XPSetWidgetDescriptor(ip_label_id, label);
    
    char preset_menu_str[65*48];
    get_preset_menu_str(preset_menu_str);
    XPSetWidgetDescriptor(preset_popup_id, preset_menu_str);
    XPSetWidgetProperty(preset_popup_id, xpProperty_PopupCurrentItem, 1+current_preset());
    
    XPSetWidgetProperty(auto_pause_checkbox_id, xpProperty_ButtonState, get_pref_int(kPrefAutoPause));
    XPSetWidgetProperty(auto_resume_checkbox_id, xpProperty_ButtonState, get_pref_int(kPrefAutoResume));
    
    
    // Set all the popups and text fields' values.
    for (int i = 0; i < kNumAxes; i++)
    {
        iXControlAxisRef control = get_axis((iXControlAxisID)i);
        
        XPSetWidgetProperty(control->popup_widget, xpProperty_PopupCurrentItem, control->type);
        
        char str[7];
        snprintf(str, 7, "%f", (double)control->min);
        XPSetWidgetDescriptor(control->min_widget, str);
        snprintf(str, 7, "%f", (double)control->max);
        XPSetWidgetDescriptor(control->max_widget, str);
    }
}


void get_preset_menu_str(char *outStr)
{
    char *names[48];
    int n = get_preset_names(kPresetTypeBoth, names);
    strcpy(outStr, "Custom");
    for (int i = 0; i < n; i++)
    {
        strcat(outStr, ";");
        strcat(outStr, names[i]);
    }
}


////// Save Preset Window //////

void show_save_preset_window()
{
    if (save_preset_window_id == 0)
    {
        // Create config window.
        iXDebug("Creating save preset window...");
        
        int x1, y1, x2, y2;
        XPGetWidgetGeometry(window_id, &x1, &y1, &x2, &y2);
        x1 += 20; x2 += 40;
        y1 -= 20; y2 = y1 - 130;
        
        save_preset_window_id = XPCreateWidget(x1, y1, x2, y2, 0, "Wi-Fly - Save Preset", 1, NULL, xpWidgetClass_MainWindow);
        
        x1 += 10; x2 -= 10;
        y1 -= 30; y2 = y1-20;
        
        // Add name text field.
        XPCreateWidget(x1, y1, x1+50, y2, 1, "Save As:", 0, save_preset_window_id, xpWidgetClass_Caption);
        save_preset_textfield_id = XPCreateWidget(x1+60, y1, x2, y2, 1, "Untitled", 0, save_preset_window_id, xpWidgetClass_TextField);
        
        y1 -= 30; y2 -= 30;
        
        // Add replace preset popup.
        XPCreateWidget(x1, y1, x1+90, y2, 1, "Replace Preset:", 0, save_preset_window_id, xpWidgetClass_Caption);
        char preset_menu_str[65*48];
        int num_items = get_save_preset_menu_str(preset_menu_str);
        save_preset_popup_id = XPCreatePopup(x1+100, y1, x2, y2, 1, preset_menu_str, save_preset_window_id);
        XPSetWidgetProperty(save_preset_popup_id, xpProperty_PopupCurrentItem, num_items);
        
        y1 -= 30; y2 -= 30;
        
        // Add cancel and save buttons.
        XPWidgetID cancel_button = XPCreateWidget(x2-210, y1, x2-110, y2, 1, "Cancel", 0, save_preset_window_id, xpWidgetClass_Button);
        XPSetWidgetProperty(cancel_button, xpProperty_ButtonType, xpButtonBehaviorPushButton);
        XPSetWidgetProperty(cancel_button, xpProperty_ButtonBehavior, xpButtonBehaviorPushButton);
        XPAddWidgetCallback(cancel_button, save_preset_cancel_callback);
        XPWidgetID save_button = XPCreateWidget(x2-100, y1, x2, y2, 1, "Save", 0, save_preset_window_id, xpWidgetClass_Button);
        XPSetWidgetProperty(save_button, xpProperty_ButtonType, xpButtonBehaviorPushButton);
        XPSetWidgetProperty(save_button, xpProperty_ButtonBehavior, xpButtonBehaviorPushButton);
        XPAddWidgetCallback(save_button, save_preset_confirm_callback);
    }
    
    XPHideWidget(save_button_id);
    XPHideWidget(delete_button_id);
    
    XPShowWidget(save_preset_window_id);
    XPSetKeyboardFocus(save_preset_textfield_id);
}

void destroy_save_preset_window()
{
    if (save_preset_window_id != 0)
    {
        iXDebug("Destroying save preset window...");
        XPDestroyWidget(save_preset_window_id, 1);
        save_preset_window_id = 0;
        
        if (window_id != 0)
        {
            XPShowWidget(save_button_id);
            XPShowWidget(delete_button_id);
        }
    }
}

int save_preset_confirm_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
    if (inMessage == xpMsg_PushButtonPressed)
    {
        int index = (int)XPGetWidgetProperty(save_preset_popup_id, xpProperty_PopupCurrentItem, NULL);
        char name[64];
        XPGetWidgetDescriptor(save_preset_textfield_id, name, 64);
        name[63] = '\0';
        save_preset(index, name);
        destroy_save_preset_window();
        update_settings_display();
        return 1;
    }
    return 0;
}

int save_preset_cancel_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
    if (inMessage == xpMsg_PushButtonPressed)
    {
        destroy_save_preset_window();
        return 1;
    }
    return 0;
}


int get_save_preset_menu_str(char *outStr)
{
    char *names[MAX_USER_PRESETS];
    int n = get_preset_names(kPresetTypeUser, names);
    if (n > 0) strcpy(outStr, names[0]);
    for (int i = 1; i < n; i++)
    {
        strcat(outStr, ";");
        strcat(outStr, names[i]);
    }
    if (n < MAX_USER_PRESETS-1)
    {
        strcat(outStr, ";[Create New Preset]");
    }
    return n;
}
