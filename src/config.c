#include "server.h"

#include "base/string.h"
#include "juson/juson.h"

config_t server_cfg;

#define CFG_ERR_ON(cond, msg)       \
if ((cond)) {                       \
    ju_error("config file: "msg);   \
    return ERROR;                   \
};

static bool load_bool_json(Json::Value const & root, const std::string & key){
    const Json::Value debug = root[key];
    return debug.asBool();
}

static int json_load_int(Json::Value const & root, const std::string & key){
    const Json::Value debug = root[key];
    return debug.asInt();
}

int config_load(config_t* cfg) {
    config_init(cfg);

    const char* cfg_file = INSTALL_DIR "config.json";
    Json::Value root;
    std::ifstream ifs;
    ifs.open(cfg_file);
    Json::CharReaderBuilder builder;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        std::cout << errs << std::endl;
        return EXIT_FAILURE;
    }


    cfg->debug = load_bool_json(root , "debug");
    cfg->daemon = load_bool_json(root , "daemon");
    cfg->timeout = json_load_int(root, "timeout");
    cfg->port = json_load_int(root, "port");



    string path = json_load_string(root, "root");
    cfg->root_fd = open(path.data, O_RDONLY);
    CFG_ERR_ON(cfg->root_fd < 0, "open root failed");


    int  worker_val = json_load_int(root, "worker");
    cfg->workers = vector<Worker>(worker_val);
    CFG_ERR_ON(cfg->workers.size > sysconf(_SC_NPROCESSORS_ONLN),
               "wrokers specified greater than cpu cores");



    auto locations_val = json_load_vec(root, "locations");
    CFG_ERR_ON(root_val == NULL ||
               locations_val->t != JUSON_ARRAY ||
               locations_val->size == 0,
               "parse locations specification failed");

    cfg->locations = vector<location_t>(locations_val->size);

    for (int i = 0; i < locations_val->size; ++i) {
        location_t& loc = locations_val[i];
        string path_val = loc_val[0];
        loc->path = juson_val2str(path_val);

        auto loc_cfg_val = loc_val[1];
        juson_value_t* pass_val = juson_object_get(loc_cfg_val, "pass");
        if (pass_val != NULL) {
            loc->pass = true;
            loc->host = pass_val[0];
            loc->port = pass_val[1];
            string protocol =juson_object_get(loc_cfg_val, "protocol");
            loc->protocol = string2protocol(protocol);
        } else {
            loc->pass = false;
        }
    }

    juson_destroy(&json);
    return OK;
}

void config_destroy(config_t* cfg) {
    free(cfg->text);
    vector_clear(&cfg->locations);
}

int string2protocol(const string & protocol){
    if (protocol == "http") {
        return PROT_HTTP;
    } else if (protocol == "fcgi") {
        return PROT_FCGI;
    } else {
        return PROT_UWSGI; // Default
    }
}
