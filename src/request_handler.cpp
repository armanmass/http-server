#include "request_handler.hpp"

#include <string>
#include <sstream>
#include <fstream>

// file extension to MIME type
// generated function
std::string mime_type(const std::string& extension) {
    if (extension == ".html" || extension == ".htm") return "text/html";
    if (extension == ".css") return "text/css";
    if (extension == ".js") return "application/javascript";
    if (extension == ".json") return "application/json";
    if (extension == ".png") return "image/png";
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".gif") return "image/gif";
    return "application/octet-stream";
}

RequestHandler::RequestHandler(const std::string& doc_root)
    : doc_root_(doc_root) {}

void RequestHandler::handle_request(const HttpRequest& req, HttpResponse& res){
    std::string request_path;
    if(!url_decode(req.uri, request_path)){
        res = HttpResponse::stock_response(HttpResponse::bad_request);
        return;
    }

    //check for root query redirect to index.html
    if(request_path.empty() || request_path[request_path.size()-1] == '/')
        request_path += "index.html";

    size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if(last_dot_pos != std::string::npos)
        extension = request_path.substr(last_dot_pos);
    
        std::string full_path = doc_root_ + request_path;
        if(full_path.find("..") != std::string::npos){
            res = HttpResponse::stock_response(HttpResponse::bad_request);
            return;
        }

    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if(!is){
        res = HttpResponse::stock_response(HttpResponse::not_found);
        return;
    }

    res.status = HttpResponse::ok;
    char buf[512];
    while(is.read(buf, sizeof(buf)).gcount() > 0)
        res.content.append(buf, is.gcount());

    res.headers.resize(3);
    res.headers[0].first = "Content-Length";
    res.headers[0].second = std::to_string(res.content.size());
    res.headers[1].first = "Content-Type";
    res.headers[1].second = mime_type(extension);
    res.headers[2].first = "Connection";
    res.headers[2].second = "keep-alive";
}

bool RequestHandler::url_decode(const std::string& in, std::string& out){
    out.clear();
    out.reserve(in.size());

    for(std::size_t i=0; i<in.size(); i++){
        if(in[i] == '%'){
            if(i+3 <= in.size()){
                int val = 0;
                std::istringstream is(in.substr(i+1,2));
                if(is >> std::hex >> val){
                    out += static_cast<char>(val);
                    i += 2;
                }else{ return false; }
            }else{ return false; }
        }else if(in[i] == '+'){
            out += ' ';
        }else{
            out += in[i];
        }
    }
    return true;
}