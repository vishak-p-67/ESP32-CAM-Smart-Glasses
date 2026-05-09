#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ================= WIFI =================
const char* sta_ssid = "YOUR_WIFI_SSID"";
const char* sta_password = "YOUR_WIFI_PASSWORD";
const char* device_name = "YOUR_ESP32_WIFI_CAM"; 

// ================= CAMERA PINS (AI Thinker) =================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

httpd_handle_t stream_httpd = NULL;

// ================= STREAM HANDLER =================
static esp_err_t stream_handler(httpd_req_t *req){

  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;

  static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";
  static const char* _STREAM_BOUNDARY = "\r\n--frame\r\n";
  static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

  char part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK) return res;

  while(true){

    if (WiFi.status() != WL_CONNECTED){
      Serial.println("WiFi Lost");
      break;
    }

    fb = esp_camera_fb_get();
    if (!fb){
      Serial.println("Camera capture failed");
      vTaskDelay(30 / portTICK_PERIOD_MS);
      continue;
    }

    res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    if(res != ESP_OK) break;

    size_t hlen = snprintf(part_buf, 64, _STREAM_PART, fb->len);
    res = httpd_resp_send_chunk(req, part_buf, hlen);
    if(res != ESP_OK) break;

    res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
    if(res != ESP_OK) break;

    esp_camera_fb_return(fb);

    vTaskDelay(30 / portTICK_PERIOD_MS);   // stable FPS
  }

  if(fb) esp_camera_fb_return(fb);
  return res;
}

// ================= CAPTURE HANDLER =================
static esp_err_t capture_handler(httpd_req_t *req){

  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb){
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  esp_err_t res = httpd_resp_send(req, (const char *)fb->buf, fb->len);

  esp_camera_fb_return(fb);
  return res;
}

// ================= START SERVER =================
void startCameraServer(){

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  config.max_open_sockets = 2;   // stable

  httpd_uri_t stream_uri = {
    .uri = "/stream",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL
  };

  httpd_uri_t capture_uri = {
    .uri = "/capture",
    .method = HTTP_GET,
    .handler = capture_handler,
    .user_ctx = NULL
  };

  if(httpd_start(&stream_httpd, &config) == ESP_OK){
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    httpd_register_uri_handler(stream_httpd, &capture_uri);
  }
}

// ================= WIFI CONNECT =================
void connectWiFi(){

  WiFi.setSleep(false);
  WiFi.begin(sta_ssid, sta_password);

  Serial.print("Connecting WiFi");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= SETUP =================
void setup(){

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  Serial.setDebugOutput(false);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_QVGA;   // faster
    config.jpeg_quality = 18;
    config.fb_count = 2;
  }else{
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 22;
    config.fb_count = 1;
  }

  if(esp_camera_init(&config) != ESP_OK){
    Serial.println("Camera init failed");
    return;
  }

  connectWiFi();

  Serial.print("Device Name: ");
  Serial.println(device_name);

  startCameraServer();

  Serial.print("Stream URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/stream");
}

// ================= LOOP =================
void loop(){

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Reconnecting WiFi...");
    WiFi.disconnect();
    connectWiFi();
  }

  delay(1000);
}