#include <http/HTTPClient.hpp>

#include <curl/curl.h>

namespace {
	struct CallbackData {
		std::string& Response;
		jarvis::http::Client::ResponseCallback Callback;
	};

	std::size_t WriteCallback(
		char* data,
		std::size_t size, std::size_t nmemb,
		void* userdata
	) {
		const std::size_t total = size * nmemb;

		auto* response = static_cast<std::string*>(userdata);
		response->append(data, total);

		return total;
	}

	std::size_t WriteCallbackWithData(
		char* data,
		std::size_t size, std::size_t nmemb,
		void* userdata
	) {
		const std::size_t total = size * nmemb;

		auto* response = static_cast<CallbackData*>(userdata);
		response->Response.append(data, total);
		response->Callback(std::string(data, total));
		
		return total;
	}
}

namespace jarvis::http {
	Client::Client() {
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}

	Client::~Client() {
		curl_global_cleanup();
	}

	std::string Client::Get(const std::string& url) {
		CURL* curl = curl_easy_init();
		if (!curl) return {};

		std::string response;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);	// 1L = Verdadeiro
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		const CURLcode result = curl_easy_perform(curl);
		if (result != CURLE_OK) {
			curl_easy_cleanup(curl);
			return {};
		}

		curl_easy_cleanup(curl);
		return response;
	}

	std::string Client::Post(
		const std::string& url, 
		const std::string& body,
		const std::vector<std::string>& headers,
		ResponseCallback callback
	) {
		// SESSÃO CURL
		CURL* curl = curl_easy_init();
		if (!curl) return {};

		std::string response;

		// CRIANDO OS HEADERS
		curl_slist* pHeaders = nullptr;

		for (const auto& header : headers) {
			pHeaders = curl_slist_append(pHeaders, header.c_str());
		}

		CallbackData callbackData {
			.Response = response,
			.Callback = callback
		};

		// CONFIGURANDO REQUISIÇÃO
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaders);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);	// 1L = Verdadeiro
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackWithData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &callbackData);

		// EXECUTANDO A REQUISIÇÃO
		const CURLcode result = curl_easy_perform(curl);
		curl_slist_free_all(pHeaders);
		if (result != CURLE_OK) {
			curl_easy_cleanup(curl);
			return {};
		}

		curl_easy_cleanup(curl);
		return response;
	}
}