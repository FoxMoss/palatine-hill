
#include "curlutil.h"

size_t write_string(char* ptr, size_t /*unused*/, size_t nmemb,
                    void* userdata) {
  auto* user_str = (std::string*)userdata;
  user_str->insert(user_str->end(), ptr, ptr + nmemb);
  return nmemb;
}

