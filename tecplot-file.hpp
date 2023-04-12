#ifndef __TECPLOT_FILE__
#define __TECPLOT_FILE__

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>

#include "tecplot-zone.hpp"

enum{
    __TITLE__,
    __VARIABLES_INIT__,
    __VARIABLES__,
    __ZONE_HEADER__,
    __DATA__,
    __FOOTER__,
    __DATA_TYPE__,
    __UKNOWN__,
};

class tecplot_file{
    public:
        tecplot_file(std::string filename);
        void loadFile();
        void setName(std::string name);
        void setTitle(std::string title);
        std::vector<double> getNode(size_t zoneID, size_t node);
        std::vector<double> getNode(size_t zoneID, size_t node, std::vector<size_t> columns);
        tecplot_zone getZone(size_t zoneID);
        size_t size();
        size_t firstZoneID();
        size_t lastZoneID();
        tecplot_zone firstZone();
        tecplot_zone lastZone();
        void setCoordColumns(std::vector<size_t> columns);
        std::vector<double> findNodeCoords(std::vector<double> coords, double epsilon);
        size_t findNode(std::vector<double> coords, double epsilon);
    private:
        void addVariable(std::string variable_name);
        char decodeDataType(std::string line);
        bool variables_flag;
        bool zone_header_flag;
        bool data_type_flag;
        bool data_flag;
        bool footer_flag;
        bool new_file;
        void unpack_header(tecplot_zone* zone, std::string line);
        std::string filename;
        std::string title;
        std::vector<std::string> variables;
        std::vector<tecplot_zone> zone;
        void switchAllFlags();
};

#endif