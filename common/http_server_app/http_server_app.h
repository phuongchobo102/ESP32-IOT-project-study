#ifndef _HTTP_SERVER_APP_H
#define _HTTP_SERVER_APP_H

typedef void ( *http_post_callback_t) (char* dta, int len);
typedef void ( *http_get_callback_t) (void);
void start_webserver(void);
void stop_webserver(void);
void http_set_callback_switch1(void *cb1);
void http_set_callback_switch2(void *cb2);
void http_set_callback_ahihi(void *cb);
void dht11_response(char *data, int len);
void http_set_callback_slider(void *cb);
#endif