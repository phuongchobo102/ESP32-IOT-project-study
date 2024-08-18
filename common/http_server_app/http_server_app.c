#include "http_server_app.h"
/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"

#include <esp_http_server.h>

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server = NULL;
static httpd_req_t *REG;
/* An HTTP GET handler */
// extern const uint8_t index_html_start[] asm("_binary_ahihi_jpg_start");
// extern const uint8_t index_html_end[] asm("_binary_ahihi_jpg_end");
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

static http_post_callback_t http_post_switch1_callback = NULL;
static http_post_callback_t http_post_switch2_callback = NULL;
static http_get_callback_t http_get_ahihi_callback = NULL;
static http_post_callback_t http_post_slider_callback = NULL;

static esp_err_t data_post_handler(httpd_req_t *req)
{
    char buf[100];
        /* Read the data for the request */
    httpd_req_recv(req, buf,req->content_len);
    printf ("DATA: %s\n", buf);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t post_data = {
    .uri       = "/data",
    .method    = HTTP_POST,
    .handler   = data_post_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

static esp_err_t sw1_post_handler(httpd_req_t *req)
{
    char buf[100];
        /* Read the data for the request */
    httpd_req_recv(req, buf,req->content_len);
    http_post_switch1_callback(buf,req->content_len);
    //printf ("DATA: %s\n", buf);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t sw1_post_data = {
    .uri       = "/switch1",
    .method    = HTTP_POST,
    .handler   = sw1_post_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

////////////////////////////////
static esp_err_t sw2_post_handler(httpd_req_t *req)
{
    char buf[100];
        /* Read the data for the request */
    httpd_req_recv(req, buf,req->content_len);
    http_post_switch2_callback(buf,req->content_len);
    //printf ("DATA: %s\n", buf);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t sw2_post_data = {
    .uri       = "/switch2",
    .method    = HTTP_POST,
    .handler   = sw2_post_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};
///////////////////////////////

static esp_err_t slider_post_handler(httpd_req_t *req)
{
    char buf[100];
        /* Read the data for the request */
    httpd_req_recv(req, buf,req->content_len);
    http_post_slider_callback(buf,req->content_len);
    //printf ("DATA: %s\n", buf);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t slider_post_data = {
    .uri       = "/slider",
    .method    = HTTP_POST,
    .handler   = slider_post_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

static esp_err_t hello_get_handler(httpd_req_t *req)
{
    // const char* resp_str = (const char*) "Hello world!" ;
    httpd_resp_set_type(req,"text/html");
    httpd_resp_send(req,(const char *) index_html_start, index_html_end - index_html_start);
    //httpd_resp_set_type(req,"image/jpg");
   // httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start); 
    return ESP_OK;
}

static const httpd_uri_t get_ahihi = {
    .uri       = "/ahihi",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

void dht11_response(char *data, int len)
{
    httpd_resp_send(REG, data,len);
}

static esp_err_t ahihi_get_handler(httpd_req_t *req)
{
    // const char* resp_str = (const char*) "Hello world!" ;
    // const char* resp_str  = (const char*) "{\"temperature\": \"27.3\", \"humidity\": \"80\"}";
    // httpd_resp_send(req,resp_str, strlen(resp_str)); 
    REG = req;
    http_get_ahihi_callback();
    return ESP_OK;
}

static const httpd_uri_t get_data_ahihi = {
    .uri       = "/getdataahihi",
    .method    = HTTP_GET,
    .handler   = ahihi_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};


esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/ahihi", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/ahihi URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &post_data);
        httpd_register_uri_handler(server, &get_ahihi);
        httpd_register_uri_handler(server, &slider_post_data);
        httpd_register_uri_handler(server, &sw1_post_data);
        httpd_register_uri_handler(server, &sw2_post_data);
        httpd_register_uri_handler(server, &get_data_ahihi);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    else {
        ESP_LOGI(TAG, "Error starting server!");
    }
}

void stop_webserver(void)
{
    // Stop the httpd server
    httpd_stop(server);
}

void http_set_callback_switch1 (void *cb1)
{
    http_post_switch1_callback = cb1; 
}

void http_set_callback_switch2(void *cb2)
{
    http_post_switch2_callback = cb2;
}

void http_set_callback_slider(void *cb)
{
    http_post_slider_callback = cb;
}

void http_set_callback_ahihi(void *cb)
{
    http_get_ahihi_callback = cb;
}
